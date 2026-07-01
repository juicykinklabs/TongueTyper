#include "ADXL343.h"

// #include <limits.h>

ADXL343::ADXL343(uint8_t cs, uint8_t sck, uint8_t miso, uint8_t mosi)
{
  _cs = cs;
  _sck = sck;
  _miso = miso;
  _mosi = mosi;
  _range = ADXL343_RANGE_2_G;
  _spiclock = 1000000;
}

ADXL343::~ADXL343()
{
  // any cleanup?
}

bool ADXL343::init(uint32_t spi_bus_clock)
{
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  delayMicroseconds(10); // "bring CS high before changing clock polarity and phase"
  _spiclock = spi_bus_clock;
  SPI.end();
  SPI.begin(_sck, _miso, _mosi);
  SPI.setFrequency(_spiclock);

  uint8_t id = readRegister(ADXL343_REG_DEVID);
  writeRegister(ADXL343_REG_DATA_FORMAT, 0x2C); // set up 4-wire spi, active low interrupt, full resolution, left justify
  writeRegister(ADXL343_REG_POWER_CTL, 0x08);   // turn on power

  return (id == ADXL343_DEVICE_ID);
}

void ADXL343::writeRegister(uint8_t reg, uint8_t value)
{
  // caller: you must ensure SPI.begin has been called to claim the clock peripheral
  uint8_t buffer[2] = {reg, value};
  SPI.beginTransaction(SPISettings(_spiclock, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  NOP();
  SPI.transfer(buffer, 2);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

uint8_t ADXL343::readRegister(uint8_t reg)
{
  // caller: you must ensure SPI.begin has been called to claim the clock peripheral
  uint8_t buffer[2] = {(reg) | uint8_t(0x80), uint8_t(0xFF)};
  SPI.beginTransaction(SPISettings(_spiclock, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  NOP();
  SPI.transfer(buffer, 2); // 1 byte address, 1 byte return
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
 
  return buffer[1];
}

void ADXL343::dumpRegisters() {
  SPI.end();
  SPI.begin(_sck, _miso, _mosi);
  for (uint8_t reg = 0x1D; reg <= 0x39; reg++) {
    Serial.printf("%#02x: %#02x \n", reg, readRegister(reg));
  }
}

void ADXL343::setRange(uint8_t range)
{
  uint8_t format = readRegister(ADXL343_REG_DATA_FORMAT);

  format &= ~0x0F;
  format |= range;
  format |= 0x08; // full res

  writeRegister(ADXL343_REG_DATA_FORMAT, format);
  _range = range;
}

void ADXL343::setRate(uint8_t dataRate)
{
  writeRegister(ADXL343_REG_BW_RATE, dataRate);
}

void ADXL343::getAcceleration(double *x, double *y, double *z)
{
  // acceleration in Gs
  // when Vs = 2.5, 256 LSB/g
  // ** reinitializes SPI bus in case another peripheral took its registers **
  // remove this in future!!
  //
  int16_t xraw, yraw, zraw;
  uint8_t buffer[7] = {0};
  buffer[0] = 0x32 | 0x80 | 0x40; // DATAX0, read, multi-byte
  SPI.end();
  SPI.begin(_sck, _miso, _mosi);
  SPI.beginTransaction(SPISettings(_spiclock, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  SPI.transfer(buffer, 7); // address + 6 data bytes
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();

  xraw = (int16_t) ((((uint16_t) buffer[2]) << 8) | ((uint16_t)buffer[1]));
  yraw = (int16_t) ((((uint16_t) buffer[4]) << 8) | ((uint16_t)buffer[3]));
  zraw = (int16_t) ((((uint16_t) buffer[6]) << 8) | ((uint16_t)buffer[5]));

  
  // right shift for signed type is arithmetic
  // not sure why this section is not needed, I expected data to 
  // come in misaligned
  //switch (_range)
  //  {
  //  case ADXL343_RANGE_2_G:
  //    xraw >>= 6;
  //    yraw >>= 6;
  //    zraw >>= 6;
  //    break;
  //  case ADXL343_RANGE_4_G:
  //    xraw >>= 5;
  //    yraw >>= 5;
  //    zraw >>= 5;
  //    break;
  //  case ADXL343_RANGE_8_G:
  //    xraw >>= 4;
  //    yraw >>= 4;
  //    zraw >>= 4;
  //    break;
  //  case ADXL343_RANGE_16_G:
  //    xraw >>= 3;
  //    yraw >>= 3;
  //    zraw >>= 3;
  //    break;
  //  }

  *x = ((double) xraw) * ADXL343_FULLRES_LSB2G;
  *y = ((double) yraw) * ADXL343_FULLRES_LSB2G;
  *z = ((double) zraw) * ADXL343_FULLRES_LSB2G;
}

void ADXL343::getAcceleration3V3(double *x, double *y, double *z) {
  // acceleration in Gs
  // "When operating at
  // a supply voltage of VS = 3.3 V, the x- and y-axis offset is typically
  // 25 mg higher than at Vs = 2.5 V operation. The z-axis is typically
  // 20 mg lower when operating at a supply voltage of 3.3 V than when
  // operating at VS = 2.5 V."
  // "... 265 LSB/g when operating
  //with a supply voltage of 3.3 V..."
  // ~ analog datasheet

  // ** reinitializes SPI bus in case another peripheral took its registers **
  // remove this in future!!

  int16_t xraw, yraw, zraw;
  uint8_t buffer[7] = {0};
  buffer[0] = 0x32 | 0x80 | 0x40; // DATAX0, read, multi-byte
  SPI.end();
  SPI.begin(_sck, _miso, _mosi);
  SPI.beginTransaction(SPISettings(_spiclock, MSBFIRST, SPI_MODE3));
  digitalWrite(_cs, LOW);
  SPI.transfer(buffer, 7); // address + 6 data bytes
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();

  xraw = (int16_t) ((((uint16_t) buffer[2]) << 8) | ((uint16_t)buffer[1]));
  yraw = (int16_t) ((((uint16_t) buffer[4]) << 8) | ((uint16_t)buffer[3]));
  zraw = (int16_t) ((((uint16_t) buffer[6]) << 8) | ((uint16_t)buffer[5]));
  

  *x = ((double) xraw) * ADXL343_FULLRES_LSB2G_XY_3V3 - 0.025;
  *y = ((double) yraw) * ADXL343_FULLRES_LSB2G_XY_3V3 - 0.025;
  *z = ((double) zraw) * ADXL343_FULLRES_LSB2G + 0.20;

}
