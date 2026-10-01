#include "modeRouter.h"

#include "config/app_conf.h"
#include "config/keybinds_conf.h"
#include "config/constants.h"
#include "structs/UserInputMessage.h"
#include "structs/AudioMessage.h"
#include "structs/MouseClickMessage.h"
#include "taskglobals.h"

void task_moderouter(void *pv) {

    UserInputMessage someEvent;
    while (1) {
        if (xQueueReceive(q_userinput, (void *) &someEvent, portMAX_DELAY)) {
            switch (DEVICE_MODE) {
                
                case DeviceMode_t::KEYBOARD: {
                    // todo: haptics for all modes
                    if (someEvent.uit == UserInputEnum::PAIR) {
                        // in keyboard mode, we only look at the double-keypresses
                        // the following will probably go in its own function
                        char which_key = 0U;
                        for (uint32_t i = 0; i < KEYBOARDMAP_LENGTH; i++) {
                            if ((keyboard_map[i][0] == someEvent.button1) && (keyboard_map[i][1] == someEvent.button2)) {
                                which_key = keyboard_map[i][2];
                                debugln(i);
                                break;
                            }
                        }
                        if (which_key > 0U) {
                            debugf("'%c'\n", which_key);
                            Keyboard.press(which_key);
                            // <-- we should also write to the display here
                            vTaskDelay(10); // todo make configurable
                            Keyboard.release(which_key);
                        } else {
                            debuglnF("invalid key");
                            debugln(which_key);
                        }
                    }
                    break;
                }

                case DeviceMode_t::JOYSTICK: {
                    static bool lastSinglePressWasDualFirst = false;
                    static uint8_t extraBytes[JOYSTICKMAP_DUAL_LENGTH]; // for tracking toggles

                    // in joystick mode, we look at both types of object in the queue to determine actions
                    char which_key            = 0U;
                    uint8_t which_mousebutton = 0U;
                    bool isMouseButton        = false;

                    switch (someEvent.uit) {
                        case UserInputEnum::SINGLE: {
                            // we should only run the single logic if the previous single press was not precursor to a dual button mapping
                            if (lastSinglePressWasDualFirst) {
                                lastSinglePressWasDualFirst = false;
                            } else {
                                for (uint32_t i = 0; i < JOYSTICKMAP_SINGLEKEY_LENGTH; i++) {
                                    if ((joystick_single_keyMap[i][0] == someEvent.button1)) {
                                        which_key = joystick_single_keyMap[i][1];
                                        break;
                                    }
                                }
                                for (uint32_t i = 0; i < JOYSTICKMAP_SINGLEMOUSE_LENGTH; i++) {
                                    if ((joystick_single_mouseMap[i][0] == someEvent.button1)) {
                                        which_mousebutton = joystick_single_mouseMap[i][1];
                                        isMouseButton     = true;
                                        break;
                                    }
                                }
                                if (isMouseButton) {
                                    MouseclickMessage mc;
                                    mc.state  = MouseButtonState::PRESSED;
                                    mc.button = which_mousebutton;
                                    xQueueSend(q_mouseclicks, (void *) &mc, (TickType_t) 0);
                                } else {
                                    if (which_key > 0U) {
                                        Keyboard.press(which_key);
                                        // release is handled by release switch
                                    } else {
                                        debuglnF("invalid key");
                                        debugln(which_key);
                                    }
                                }
                                // check if it was tip button, perchance
                                for (uint32_t i = 0; i < JOYSTICKMAP_DUAL_LENGTH; i++) {
                                    if ((joystick_dual_map[i][0] == someEvent.button1)) {
                                        lastSinglePressWasDualFirst = true;
                                        debuglnF("tip flag enabled for next");
                                        break;
                                    }
                                }
                            }

                            break;
                        }

                        case UserInputEnum::RELEASE: {
                            for (uint32_t i = 0; i < JOYSTICKMAP_DUAL_LENGTH; i++) {
                                if ((joystick_dual_map[i][1] == someEvent.button1)) {
                                    // we may need to release the associated joystick_dual key
                                    which_key = joystick_dual_map[i][2];
                                    Keyboard.release(which_key);
                                    break;
                                }
                            }
                            for (uint32_t i = 0; i < JOYSTICKMAP_SINGLEMOUSE_LENGTH; i++) {
                                if ((joystick_single_mouseMap[i][0] == someEvent.button1)) {
                                    which_mousebutton = joystick_single_mouseMap[i][1];
                                    isMouseButton     = true;
                                    break;
                                }
                            }
                            if (isMouseButton) {
                                MouseclickMessage mc;
                                mc.state  = MouseButtonState::RELEASED;
                                mc.button = which_mousebutton;
                                xQueueSend(q_mouseclicks, (void *) &mc, (TickType_t) 0);
                            } else {
                                for (uint32_t i = 0; i < JOYSTICKMAP_SINGLEKEY_LENGTH; i++) {
                                    if ((joystick_single_keyMap[i][0] == someEvent.button1)) {
                                        which_key = joystick_single_keyMap[i][1];
                                        break;
                                    }
                                }
                                if (which_key > 0U) {
                                    Keyboard.release(which_key);
                                } else {
                                    debuglnF("invalid key");
                                    debugln(which_key);
                                }
                            }
                            break;
                        }

                        case UserInputEnum::PAIR: {
                            uint32_t i_pair = 0;
                            for (i_pair = 0; i_pair < JOYSTICKMAP_DUAL_LENGTH; i_pair++) {
                                if ((joystick_dual_map[i_pair][0] == someEvent.button1) && ((joystick_dual_map[i_pair][1] == someEvent.button2))) {
                                    which_key = joystick_dual_map[i_pair][2];
                                    break;
                                }
                            }
                            if (which_key > 0U) {
                                switch (joystick_dual_map[i_pair][3]) {
                                    case KeybindMode::STANDARD: {
                                        Keyboard.press(which_key);
                                        vTaskDelay(10); // todo make configurable
                                        Keyboard.release(which_key);
                                        break;
                                    }
                                    case KeybindMode::TOGGLEHOLD: {
                                        if (extraBytes[i_pair] == 0U) {
                                            Keyboard.press(which_key);
                                            extraBytes[i_pair] = 1U;
                                        } else {
                                            Keyboard.press(which_key);
                                            extraBytes[i_pair] = 0U;
                                        }
                                        break;
                                    }
                                    case KeybindMode::INTERACTIVE: {
                                        Keyboard.press(which_key); // release switch must handle this
                                        break;
                                    }
                                }
                                Keyboard.release(which_key);
                            }
                            break;
                        }
                        default:
                            break;
                    }

                    break;
                }

                case DeviceMode_t::AUDIOIMAGE: {

                    if (someEvent.uit == UserInputEnum::TRIPLET) {
                        // may have something. let's optimistically decode the last 2 presses first,
                        // assuming the first button press is valid.
                        String fname;
                        char digit1;
                        char digit2;
                        if (someEvent.button2 > 15 || someEvent.button3 > 15) {
                            break; // cannot proceed; not enough hex digits to represent these buttons
                        }
                        if (someEvent.button2 > 9 || someEvent.button3 > 9) {
                            // use hex value
                            digit1 = someEvent.button2 - 10 + 'a';
                            digit2 = someEvent.button3 - 10 + 'a';
                        } else {
                            digit1 = someEvent.button2 + '0';
                            digit2 = someEvent.button3 + '0';
                        }
                        switch (someEvent.button1) {
                            case MM_BUTTON_AUDIO: {
                                fname = FSPATH::UserSoundsFolder;
                                fname += digit1;
                                fname += digit2;
                                fname += ".mp3";
                                // todo: search SD for a file of any extension matching fname

                                debugln("playing sound");
                                debugln(fname);
                                AudioMessage myAudioEvent;
                                // the queue receiver will deallocate this for us. probably.
                                strcpy(myAudioEvent.data, fname.c_str());
                                myAudioEvent.instruction = AudioEventEnum::SFX;
                                xQueueSend(q_sfx_tts, (void *) &myAudioEvent, (TickType_t) 0);
                                // audio event string could go out of scope here
                                break;
                            }
                            case MM_BUTTON_IMAGE: {
                                fname = FSPATH::UserImagesFolder;
                                fname += digit1;
                                fname += digit2;
                                fname += ".bmp";

                                debugln("showing image");
                                debugln(fname);

                                debugln("not yet implemented");
                                // metaTransaction_showImage(fname_buf);

                                break;
                            }
                            case MM_BUTTON_BOTH: {
                                debugln("not yet implemented");
                                break;
                            }
                            default:
                                break;
                        }
                    }

                    break;
                }
                default: {
                    debugln("no valid mode");
                    break;
                }
            }
        }
    }
    vTaskDelete(NULL);
}