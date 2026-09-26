#include "services/buttons.h"

#include <Arduino.h>
#include "MCP23S17.h"

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/UserInputMessage.h"
#include "taskglobals.h"

// TEMP: internal struct, buffer to keep track of things, and some tunable constants
// the variables get a little quirky at night
// todo: does this match header file?

typedef struct RawButtonEvent {
    uint32_t timestamp;
    UserInputEnum alreadySentAs;
    uint8_t button;
    bool valid = false;
    bool isPressed;
    uint8_t _pad;
} RawButtonEvent;

// we'll get these from settings at some point
constexpr uint32_t debounce_ms       = 50;
constexpr uint32_t delete_older_than = 3000;

const size_t RawButtonEventBufSize = NUM_EXPUI_BUTTONS * 3; // should be plenty
static RawButtonEvent buttBuff[RawButtonEventBufSize];

// where to place next element. strictly increasing; use mod RawButtonEventBufSize
static uint32_t RawButtonEventBufIndex = 0;

// helper functions

void showButtBuff(bool validOnly = false, bool showReleases = false) {
    debugln("\tBUTT BUFF:");
    for (int i = 0; i < RawButtonEventBufSize; i++) {
        if ((!validOnly || buttBuff[i].valid) && (showReleases || buttBuff[i].isPressed)) {
            debugf(" %d%c button %d %s at %u sentAs=%d [%s]\n", i, (i + 1) == RawButtonEventBufIndex ? '>' : ' ', buttBuff[i].button, buttBuff[i].isPressed ? "pressed" : "not pressed", buttBuff[i].timestamp, buttBuff[i].alreadySentAs,
                   buttBuff[i].valid ? "valid" : "stale");
        }
    }
}

void makeOldStale() {
    static bool shouldPrint = false;
    int validCounter        = 0;

    for (int i = 0; i < RawButtonEventBufSize; i++) {
        if ((buttBuff[i].valid)) {
            validCounter++;
            if (((buttBuff[i].timestamp + delete_older_than) <= millis())) {
                buttBuff[i].valid = false;
                shouldPrint       = true;
            }
        }
    }

    if (shouldPrint && (validCounter == 0)) {
        debugln("All presses invalidated");
        shouldPrint = false;
    }
}

bool stateChangedTooRecently(uint8_t button, bool onlyCheckPresses = false) {
    // we call millis in here, this is probably bad
    RawButtonEvent *checkAgainst;
    uint32_t function_called_at = millis();

    for (uint32_t i = 0; i < RawButtonEventBufSize; i++) {
        checkAgainst = &(buttBuff[i]);
        if (onlyCheckPresses) {
            if ((checkAgainst->button == button) && (checkAgainst->valid) && (checkAgainst->isPressed)) {
                return checkAgainst->timestamp + debounce_ms >= function_called_at;
            }
        } else {
            if ((checkAgainst->button == button) && (checkAgainst->valid)) {
                return checkAgainst->timestamp + debounce_ms >= function_called_at;
            }
        }
    }
    return false;
}

// ISR stuff

bool flag = false; // todo: use messageFromISR
void isr_gpio_int() { flag = true; }

void task_buttons(void *pv) {
    // todo: SPI mutex
    // hardware setup
    SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);
    digitalWrite(Pins::EXP::RST, HIGH);
    delayMicroseconds(10);
    MCP23S17 MCP = MCP23S17(Pins::EXP::CS, &SPI);
    if (MCP.begin()) {
        debugln("gpio expander started");
    } else {
        // this probably won't happen, even if
        // the expander is completely nonexistant
        debugln("gpio expander failed?");
        vTaskDelete(NULL);
    }

    // MCP.setSPIspeed(SPISpeed::EXP);
    MCP.disableHardwareAddress();
    delayMicroseconds(10);
    // we can now send config commands:

    MCP.pinMode8(0, 0xFF);
    MCP.pinMode8(1, 0xFF);
    MCP.setPullup16(0xFFFF);   // ensure pullups enabled
    MCP.setPolarity16(0xFFFF); // low voltage -> logical '1' -> button is pushed

    MCP.setInterruptPolarity(LOW);

    // get an interrupt on buttons being both pressed and released.
    // interestingly, setting this to FALLING is no different. it acts like RISING
    // because our polarity is inverted from the voltage,
    // and then because there is some bounce, we see two edges on every press anyways.
    // (therefore capturing both rising and falling)
    // we don't care so much about releases
    MCP.enableInterrupt16(0xFFFF, CHANGE);

    MCP.getInterruptFlagRegister(); // clear any interrupts

    attachInterrupt(Pins::EXP::INTA, isr_gpio_int, FALLING);

    // testing loop (example code, to be rewritten)
    static uint16_t previous_capture_state = 0x0000;

    while (1) {
        if (flag) {
            flag = false;
            // ^ interrupt trig'd
            MCP.getInterruptFlagRegister(); // clear any interrupts

            // should contains state of input registers at time of interrupt
            // why is it showing '1' sometimes...
            uint16_t captureRegister = MCP.getInterruptCaptureRegister();

            uint8_t stateA = MCP.read8(0);
            uint8_t stateB = MCP.read8(1);

            // ***
            uint16_t changedBits = captureRegister ^ previous_capture_state;
            uint8_t changedBitsA = changedBits >> 8;
            uint8_t changedBitsB = changedBits & 0xFF;

            previous_capture_state = captureRegister;
            // debugf("Capture/A/B: 0x%x 0x%x 0x%x\n", captureRegister, stateA, stateB);
            // debugf("Changed/A/B: 0x%x 0x%x 0x%x\n", changedBits, changedBitsA, changedBitsB);

            RawButtonEvent rbe;

            for (int button_index = 0; button_index < NUM_EXPUI_BUTTONS; button_index++) {
                uint8_t whichButton = laexpui_buttons[button_index];
                if (whichButton & 0x80) {
                    continue; // todo: handle currently unused B-register buttons
                } else {
                    // check if current bit was changed
                    if (changedBitsA & (uint8_t) (0b1 << whichButton)) {
                        // handle button presses
                        if (stateA & (uint8_t) (0b1 << whichButton)) {
                            debugf("button pressed: %d\n", whichButton);

                            // todo: replace a bunch of ! occurrences with 'not' where it makes sense to
                            if (not stateChangedTooRecently(whichButton)) {
                                rbe.valid                                                = true;
                                rbe.isPressed                                            = true;
                                rbe.button                                               = whichButton;
                                rbe.timestamp                                            = millis();
                                rbe.alreadySentAs                                        = UserInputEnum::NONE;
                                buttBuff[RawButtonEventBufIndex % RawButtonEventBufSize] = rbe;
                                RawButtonEventBufIndex++;
                            } else {
                                debugln("(debounced)");
                            }
                        }

                        else {
                            // button changed but isn't pressed. must've been released.
                            // I don't think we need to debounce these, as a series of releases
                            // wouldn't have a noticeable effect?
                            // todo: clean this up
                            rbe.valid                                                = true;
                            rbe.isPressed                                            = false;
                            rbe.button                                               = whichButton;
                            rbe.timestamp                                            = millis();
                            rbe.alreadySentAs                                        = UserInputEnum::NONE;
                            buttBuff[RawButtonEventBufIndex % RawButtonEventBufSize] = rbe;
                            RawButtonEventBufIndex++;
                        }
                    }
                }
            }
        }

        makeOldStale();
        // evaluate the list and send a bunch of messages for every grouping
        // duplicate button presses matter! we might have 3-button combos!

        UserInputMessage buttonMessage;
        int32_t buttBuffIndexStart = (RawButtonEventBufIndex + 0) % RawButtonEventBufSize; // zero because we already increased it by 1 after last push,

        // signed modulo is weird, i would think -1 mod 21 (signed integers) would be 20, rather than 3?
        // welp,
        int32_t buttBuffIndexEnd = (RawButtonEventBufIndex <= 0) ? (RawButtonEventBufSize - 1) : (RawButtonEventBufIndex - 1) % RawButtonEventBufSize;
        int32_t i                = buttBuffIndexStart;
        int32_t i_pair_idx       = -1;
        int32_t i_triplet_idx1    = -1;
        int32_t i_triplet_idx2    = -1;

        while (1) {
            // a circular iterator thingy, i'm just trying to get this done, ok?
            // todo: we could make a function that takes a function and applies it over the whole loop and stuff

            RawButtonEvent *rbe2 = &(buttBuff[i]);

            if (rbe2->valid && rbe2->isPressed) {
                // send all single presses,
                if (rbe2->alreadySentAs == UserInputEnum::NONE) {
                    buttonMessage.button1 = rbe2->button;
                    buttonMessage.uit     = UserInputEnum::SINGLE;
                    rbe2->alreadySentAs   = UserInputEnum::SINGLE;
                    if (xQueueSend(q_userinput, (void *) &buttonMessage, 0) == pdTRUE) {
                        debugln("yippee");
                    } else {
                        debugln("fuck");
                    }
                }
                // send all double presses. we're chilling since everyone in the stack was just confirmed valid
                // todo: just use a couple pointers and swap them this is too complicated and stupid
                if (rbe2->alreadySentAs == UserInputEnum::SINGLE) {
                    

                    if (i_pair_idx == -1) {
                        // can't set buttonMessage.button1 directly
                        // because the next press will want to send a 'single' packet
                        // and overwrite it, so we store its index
                        i_pair_idx = i;
                    } else {
                        buttonMessage.button1 = buttBuff[i_pair_idx].button;
                        buttonMessage.button2 = rbe2->button;
                        debugf("promoted indices:%d,%d\n", i_pair_idx, i);
                        showButtBuff();
                        // update both to pair status:
                        rbe2->alreadySentAs                = UserInputEnum::PAIR;
                        buttBuff[i_pair_idx].alreadySentAs = UserInputEnum::PAIR;
                        i_pair_idx                         = -1;
                        
                        buttonMessage.uit = UserInputEnum::PAIR;
                        if (xQueueSend(q_userinput, (void *) &buttonMessage, 0) == pdTRUE) {
                            debugf("outgoing pair: %d,%d\n", buttonMessage.button1, buttonMessage.button2);
                        }
                    }
                }
                // triple presses (VERY stupid method)
                if (rbe2->alreadySentAs == UserInputEnum::SINGLE || rbe2->alreadySentAs == UserInputEnum::PAIR) {
                    if (i_triplet_idx1 == -1) {
                        i_triplet_idx1 = i;
                    } else if (i_triplet_idx2 == -1) {
                        i_triplet_idx2 = i;
                    } else {
                        buttonMessage.button1 = buttBuff[i_triplet_idx1].button;
                        buttonMessage.button2 = buttBuff[i_triplet_idx2].button;
                        buttonMessage.button3 = rbe2->button;
                        debugf("promoted indices:%d,%d,%d\n", i_triplet_idx2, i_triplet_idx1, i);
                        showButtBuff();
                        rbe2->alreadySentAs = UserInputEnum::TRIPLET;
                        buttBuff[i_triplet_idx1].alreadySentAs = UserInputEnum::TRIPLET;
                        buttBuff[i_triplet_idx2].alreadySentAs = UserInputEnum::TRIPLET;
                        buttonMessage.uit = UserInputEnum::TRIPLET;
                        if (xQueueSend(q_userinput, (void *) &buttonMessage, 0) == pdTRUE) {
                            debugf("outgoing triplet: %d,%d,%d\n", buttonMessage.button1, buttonMessage.button2, buttonMessage.button3);
                        }
                        i_triplet_idx1 = -1;
                        i_triplet_idx2 = -1;
                    }
                }
            }
            // send releases
            if (rbe2->valid && (not rbe2->isPressed) && (rbe2->alreadySentAs == UserInputEnum::NONE)) {
                buttonMessage.button1 = rbe2->button;
                buttonMessage.uit     = UserInputEnum::RELEASE;
                rbe2->alreadySentAs   = UserInputEnum::RELEASE;
                if (xQueueSend(q_userinput, (void *) &buttonMessage, (TickType_t) 5) == pdTRUE) {
                    debugln("released button");
                }
            }

            if (i == buttBuffIndexEnd) {
                break;
            }
            i++;
            i %= RawButtonEventBufSize;
        }

        // polling slowly will have a 'sorta' debouncing effect
        // but reduces responsiveness
        // all the task hyperperiods need to be tuned...
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
