#pragma once

#include <Arduino.h>
#include <SPI.h>

#define ADXL343_DEVICE_ID (0xE5)

#define ADXL343_I2C_ADDRESS (0x1D)     // Assumes ALT address pin high. 0x3A for write, 0x3B for read
#define ADXL343_I2C_ADDRESS_ALT (0x53) // Assumes ALT address pin low. 0xA6 for write, 0xA7 for read

#define ADXL343_REG_DEVID (0x00)          // Device ID
#define ADXL343_REG_THRESH_TAP (0x1D)     // Tap threshold
#define ADXL343_REG_OFSX (0x1E)           // X-axis offset
#define ADXL343_REG_OFSY (0x1F)           // Y-axis offset
#define ADXL343_REG_OFSZ (0x20)           // Z-axis offset
#define ADXL343_REG_DUR (0x21)            // Tap duration
#define ADXL343_REG_LATENT (0x22)         // Tap latency
#define ADXL343_REG_WINDOW (0x23)         // Tap window
#define ADXL343_REG_THRESH_ACT (0x24)     // Activity threshold
#define ADXL343_REG_THRESH_INACT (0x25)   // Inactivity threshold
#define ADXL343_REG_TIME_INACT (0x26)     // Inactivity time
#define ADXL343_REG_ACT_INACT_CTL (0x27)  // Axis enable control for activity and inactivity detection
#define ADXL343_REG_THRESH_FF (0x28)      // Free-fall threshold
#define ADXL343_REG_TIME_FF (0x29)        // Free-fall time
#define ADXL343_REG_TAP_AXES (0x2A)       // Axis control for single/double tap
#define ADXL343_REG_ACT_TAP_STATUS (0x2B) // Source for single/double tap
#define ADXL343_REG_BW_RATE (0x2C)        // Data rate and power mode control
#define ADXL343_REG_POWER_CTL (0x2D)      // Power-saving features control
#define ADXL343_REG_INT_ENABLE (0x2E)     // Interrupt enable control
#define ADXL343_REG_INT_MAP (0x2F)        // Interrupt mapping control
#define ADXL343_REG_INT_SOURCE (0x30)     // Source of interrupts
#define ADXL343_REG_DATA_FORMAT (0x31)    // Data format control
#define ADXL343_REG_DATAX0 (0x32)         // X-axis data 0
#define ADXL343_REG_DATAX1 (0x33)         // X-axis data 1
#define ADXL343_REG_DATAY0 (0x34)         // Y-axis data 0
#define ADXL343_REG_DATAY1 (0x35)         // Y-axis data 1
#define ADXL343_REG_DATAZ0 (0x36)         // Z-axis data 0
#define ADXL343_REG_DATAZ1 (0x37)         // Z-axis data 1
#define ADXL343_REG_FIFO_CTL (0x38)       // FIFO control
#define ADXL343_REG_FIFO_STATUS (0x39)    // FIFO status

#define ADXL343_DATARATE_3200_HZ (0b1111) // 1600Hz Bandwidth   140uA
#define ADXL343_DATARATE_1600_HZ (0b1110) //  800Hz Bandwidth    90uA
#define ADXL343_DATARATE_800_HZ (0b1101)  //  400Hz Bandwidth   140uA
#define ADXL343_DATARATE_400_HZ (0b1100)  //  200Hz Bandwidth   140uA
#define ADXL343_DATARATE_200_HZ (0b1011)  //  100Hz Bandwidth   140uA
#define ADXL343_DATARATE_100_HZ (0b1010)  //   50Hz Bandwidth   140uA (default value)
#define ADXL343_DATARATE_50_HZ (0b1001)   //   25Hz Bandwidth    90uA
#define ADXL343_DATARATE_25_HZ (0b1000)   // 12.5Hz Bandwidth    60uA
#define ADXL343_DATARATE_12_5_HZ (0b0111) // 6.25Hz Bandwidth    50uA
#define ADXL343_DATARATE_6_25HZ (0b0110)  // 3.13Hz Bandwidth    45uA
#define ADXL343_DATARATE_3_13_HZ (0b0101) // 1.56Hz Bandwidth    40uA
#define ADXL343_DATARATE_1_56_HZ (0b0100) // 0.78Hz Bandwidth    34uA
#define ADXL343_DATARATE_0_78_HZ (0b0011) // 0.39Hz Bandwidth    23uA
#define ADXL343_DATARATE_0_39_HZ (0b0010) // 0.20Hz Bandwidth    23uA
#define ADXL343_DATARATE_0_20_HZ (0b0001) // 0.10Hz Bandwidth    23uA
#define ADXL343_DATARATE_0_10_HZ (0b0000) // 0.05Hz Bandwidth    23uA

#define ADXL343_RANGE_16_G (0b11) // +/- 16g
#define ADXL343_RANGE_8_G (0b10)  // +/- 8g
#define ADXL343_RANGE_4_G (0b01)  // +/- 4g
#define ADXL343_RANGE_2_G (0b00)  // +/- 2g (default value)

#define ADXL343_FULLRES_LSB2G (0.00391) // "4mg" per lsb (Vs=2.5)
#define ADXL343_FULLRES_LSB2G_XY_3V3 (0.00377) // (Vs=3.3)

class ADXL343
{
public:
  ADXL343(uint8_t cs, uint8_t sck, uint8_t miso, uint8_t mosi);
  ~ADXL343();

  bool init(uint32_t spi_bus_clock = 1000000U);
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  void dumpRegisters();
  void setRange(uint8_t range);
  void setRate(uint8_t dataRate);

  void getAcceleration(double* x, double*y, double*z);
  void getAcceleration3V3(double *x, double*y, double*z);
private:
  uint8_t _cs, _sck, _miso, _mosi;
  uint8_t _range;
  uint32_t _spiclock;
};
