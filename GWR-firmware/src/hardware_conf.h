#pragma once

// project hardware description file

#include <Arduino.h>


// *** XIAO PINOUT HELPER ***
//          USB PORT
//   reset button  boot0 button
//   led pmic      led gpio21
//     GPIO1/D0     VBUS
//     GPIO2/D1     GND
//     GPIO3/D2     3V3 ldo out
//     GPIO4/D3     GPIO9/D10
//     GPIO5/D4     GPIO8/D9
//     GPIO6/D5     GPIO7/D8
//     GPIO43/D6    GPIO44/D7


#define TFT_DC D1
#define TFT_CS D2
//#define TFT_RST D3

#define XIAO_SCK D8
#define XIAO_MISO D9
#define XIAO_MOSI D10

#define I2S_BCLK D0 // Bit clock
#define I2S_LRC D3  // Word select (LR clock)
#define I2S_DOUT D6 // Data out to DAC

#define SD_CS D4
#define ADXL_CS D5

