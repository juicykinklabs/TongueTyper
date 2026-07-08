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
        // constexpr uint8_t RST = not yet defined;
    } // namespace TFT
    namespace SD {
        constexpr uint8_t CS = D4;
    }
    namespace XL {
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
    namespace XL {
        constexpr uint8_t CS   = GPIO_NUM_11;
        constexpr uint8_t INT1 = GPIO_NUM_12;
    } // namespace XL
    namespace ADC {
        constexpr uint8_t VBAT = GPIO_NUM_10;
    }
#else
#endif
} // namespace Pins

namespace Buttons {
    // mapping for each button onto a gpio expander port and bit
    // port A: just the bit number
    // port B: bit is &'d with 0x80
    constexpr uint8_t TIP   = 7;
    constexpr uint8_t FREN  = 3;
    constexpr uint8_t SHAFT = 1;
    constexpr uint8_t LNEAR = 4;
    constexpr uint8_t RNEAR = 6;
    constexpr uint8_t LFAR  = 0;
    constexpr uint8_t RFAR  = 2;

// example extension:
#ifdef HAS_REVB_DAUGHTERBOARD
    constexpr uint8_t TOPP = 0 | 0x80; // top button
    constexpr uint8_t SHF2 = 1 | 0x80; // second shaft button
    constexpr uint8_t LMID = 2 | 0x80; // left-middle button
    constexpr uint8_t RMID = 3 | 0x80; // right-middle button
#endif
} // namespace Buttons
// expert zone

// interface speeds

namespace SPISpeed {
    constexpr uint32_t TFT = 40000000;
    constexpr uint32_t SD  = 25000000;
    constexpr uint32_t EXP = 5000000;
    constexpr uint32_t XL  = 1000000;
} // namespace SPISpeed

// hardware values, like resistors, gains

#define VBAT_DIV_HIGH (820000) // ohms
#define VBAT_DIV_LOW  (270000) // ohms
