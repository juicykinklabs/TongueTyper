#pragma once

// project hardware description file

#include <Arduino.h>

// pinout

//          *** XIAO PLUS PINOUT HELPER ***
//                      TOP VIEW
//
//                      USB PORT
//          reset button        boot button (gpio0)
//          led pmic            led (gpio21)
//
//              GPIO1/D0        VBUS
//          GPIO38/D11                 -
//              GPIO2/D1        GND
//          GPIO39/D12                 -
//              GPIO3/D2        3V3 700mA LDO out
//          GPIO40/D13                 -
//              GPIO4/D3        GPIO9/D10
//          GPIO41/D14                 GPIO13/D17
//              GPIO5/D4        GPIO8/D9
//          GPIO42/D15                 GPIO12/D18
//              GPIO6/D5        GPIO7/D8
//          GPIO10/D16                 GPIO11/D19
//              GPIO43/D6       GPIO44/D7
//          uUFL connector

namespace Pins {
#if PCB_REVISION == 0
    // Breadboarding with an esp32-s3 NON plus
    constexpr uint8_t HVIBE = GPIO_NUM_44;
    namespace SPI {
        constexpr uint8_t SCK  = D8;
        constexpr uint8_t MISO = D9;
        constexpr uint8_t MOSI = D10;
    } // namespace SPI
    namespace I2S {
        constexpr uint8_t BCLK = D0; // Bit clock
        constexpr uint8_t LRC  = D3; // Word select (LR clock)
        constexpr uint8_t DOUT = D6; // Data out to DAC
    } // namespace I2S
    namespace TFT {
        constexpr uint8_t DC = D1;
        constexpr uint8_t CS = D2;
        constexpr uint8_t RST = -1; // not defined
    } // namespace TFT
    namespace SD {
        constexpr uint8_t CS = D4;
    }
    namespace ACCEL {
        constexpr uint8_t CS = D5;
    }
#elif PCB_REVISION == 1
    // PCB rev A pinout
    constexpr uint8_t HVIBE = GPIO_NUM_43;
    constexpr uint8_t BMODE = GPIO_NUM_13;

    namespace SPI {
        constexpr uint8_t SCK  = GPIO_NUM_7;
        constexpr uint8_t MISO = GPIO_NUM_8;
        constexpr uint8_t MOSI = GPIO_NUM_9;
    } // namespace SPI
    namespace I2S {
        constexpr uint8_t BCLK = GPIO_NUM_39; // Bit clock
        constexpr uint8_t LRC  = GPIO_NUM_40; // Word select (LR clock)
        constexpr uint8_t DOUT = GPIO_NUM_38; // Data out to DAC
    } // namespace I2S
    namespace TFT {
        constexpr uint8_t DC  = GPIO_NUM_42;
        constexpr uint8_t CS  = GPIO_NUM_2;
        constexpr uint8_t BL  = GPIO_NUM_1;
        constexpr uint8_t RST = GPIO_NUM_41;
    } // namespace TFT
    namespace EXP {
        constexpr uint8_t CS   = GPIO_NUM_3;
        constexpr uint8_t INTA = GPIO_NUM_4;
        constexpr uint8_t RST  = GPIO_NUM_5;
    } // namespace EXP
    namespace SD {
        constexpr uint8_t CS = GPIO_NUM_6;
    }
    namespace ACCEL {
        constexpr uint8_t CS   = GPIO_NUM_11;
        constexpr uint8_t INT1 = GPIO_NUM_12;
    } // namespace ACCEL
    namespace ADC {
        constexpr uint8_t VBAT = GPIO_NUM_10; // aka BATT_VOLT_PIN
    }
#else
#endif
} // namespace Pins

#define HAS_GPIO_EXPANDER (PCB_REVISION >= 1)

// ******************
namespace Buttons {
    // mapping for each button onto a gpio expander port and bit
    // port A: just the bit number. 5 is currently unused
    // port B: bit is &'d with 0x80
    constexpr uint8_t TIP   = 7;
    constexpr uint8_t FREN  = 3;
    constexpr uint8_t SHAFT = 1;
    constexpr uint8_t LNEAR = 4;
    constexpr uint8_t RNEAR = 6;
    constexpr uint8_t LFAR  = 0;
    constexpr uint8_t RFAR  = 2;

// example extension:
// if you add more buttons, you need to update the array below,
// and you may also want to update the keybinds_conf.h!
#ifdef HAS_REVB_DAUGHTERBOARD
    constexpr uint8_t TOPP = 0 | 0x80; // top button
    constexpr uint8_t SHF2 = 1 | 0x80; // second shaft button
    constexpr uint8_t LMID = 2 | 0x80; // left-middle button
    constexpr uint8_t RMID = 3 | 0x80; // right-middle button
#endif
} // namespace Buttons

constexpr uint8_t laexpui_buttons[] = {
    // todo: just use 1<<0..1<<15
    // list of all expander user interfacing buttons
    // they must be listed here or they won't be scanned by the buttons service
    // if we update Buttons:: to include more members, include them here!
    Buttons::TIP,   //
    Buttons::FREN,  //
    Buttons::SHAFT, //
    Buttons::LNEAR, //
    Buttons::RNEAR, //
    Buttons::LFAR,  //
    Buttons::RFAR,  //
};

constexpr uint8_t NUM_EXPUI_BUTTONS = (uint8_t) (sizeof(laexpui_buttons) / sizeof(uint8_t));
// ******************

// expert zone

// interface speeds

namespace SPISpeed {
    constexpr uint32_t TFT = 25000000;
#ifdef SLOW_SD_CARD
    constexpr uint32_t SD = 5000000;
#else
    constexpr uint32_t SD = 25000000;
#endif
    constexpr uint32_t EXP = 5000000;
    constexpr uint32_t ACCEL  = 1000000;
} // namespace SPISpeed

// hardware values, like resistors, gains

#define VBAT_DIV_HIGH (820000) // ohms
#define VBAT_DIV_LOW  (270000) // ohms

#define VBAT_FULL (4.2) // volts

#define V_MOTOR_MIN (0.75)
#define V_MOTOR_MAX (3.00)

#if PCB_REVISION == 0
#define V_MOTOR_INPUT (3.30)
#elif PCB_REVISION == 1
#define V_MOTOR_INPUT (2.50)
#elif PCB_REVISION == 2
#define V_MOTOR_INPUT (VBAT_FULL)
#endif

static_assert(V_MOTOR_MIN < V_MOTOR_INPUT);
static_assert(V_MOTOR_MAX > V_MOTOR_MIN);
// (if V_MOTOR_INPUT < V_MOTOR_MAX, we can handle it at runtime in haptics, just won't be as strong)
