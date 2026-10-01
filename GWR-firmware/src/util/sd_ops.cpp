#include "sd_ops.h"

#include <Arduino.h>
#include <SPI.h>

#include <ArduinoJson.h>

#include "config/app_conf.h"
#include "config/hardware_conf.h"

#include "taskglobals.h" // we need the exact semaphores in use



#ifdef USING_ADAFRUIT_SD_FORK_FOR_FILES

#include "SdFat_Adafruit_Fork.h"

SdFat32 SD_as_FAT32;
SdSpiConfig sdConfig(Pins::SD::CS, SHARED_SPI, SPISpeed::SD, &SPI);

bool sdOpBegin() {
    
    // take up to two mutex semaphores, spi then sd, 
    // waiting as long as it takes. todo: deadlock avoidance...?
    // (do these need to be references or does freertos take care of that?)
    // reconfigures SPI bus
    // and call begin for current pins and settings
    // return true on success, gives back semaphores on fail
    
    if (Mutexes::SPI == NULL || Mutexes::SDCard == NULL) {
        return false;
    }

    if (xSemaphoreTake(Mutexes::SPI, portMAX_DELAY) == pdTRUE) {
        if (xSemaphoreTake(Mutexes::SDCard, portMAX_DELAY) == pdTRUE) {
            SPI.end();
            SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);

            if (SD_as_FAT32.begin(sdConfig)) {  // configures pin hardware
                return true;
            } else {
                SD_as_FAT32.end(); // unsure if we need this line
                digitalWrite(Pins::SD::CS, HIGH); // again, taking no chances!
                xSemaphoreGive(Mutexes::SDCard);
                xSemaphoreGive(Mutexes::SPI);
                return false;
            }
        } else {
            debugln("no SD mutex in time");
            xSemaphoreGive(Mutexes::SPI);
            return false;
        }
    } else {
        debugln("no SPI mutex in time");
        return false;
    }
}

void sdOpEnd() {
    // end of an sd op, call me on any type of failure after sdOpBegin, too

    SD_as_FAT32.end();
    digitalWrite(Pins::SD::CS, HIGH); // taking no chances
    SPI.end();
    xSemaphoreGive(Mutexes::SDCard);
    xSemaphoreGive(Mutexes::SPI);
}

// the following expect op begin/op end calls before/after:

bool fileExists(const char* fspath) {
    // check if a file exists on card
    // does NOT initialize or end card!
    return SD_as_FAT32.exists(fspath);
}

bool fileRemove(const char* fspath) {
    // does NOT initialize or end card!
    return SD_as_FAT32.remove(fspath);
}

bool fileToJdoc(const char* fspath, JsonDocument &jdoc) {
    File32 file = SD_as_FAT32.open(fspath, O_READ);
    if (!file) {
        debugln("file does not exist");
        return false;
    }
    DeserializationError error = deserializeJson(jdoc, file);
    if (error) {
        debugf("deserializeJson error: %s", error.f_str());
    }
    file.close();
    return !error;
}

bool jdocToFile(const char* fspath, const JsonDocument &jdoc) {
    // overwrite a file with the serialized contents of a json document
    fileRemove(fspath);

    File32 file = SD_as_FAT32.open(fspath, O_WRITE | O_CREAT);
    if (!file) {
        debugln("couldn't create file");
        return false;
    }
    bool serializationSuccess = (serializeJsonPretty(jdoc, file) != 0);
    if (not serializationSuccess) {
        debugln("failed to serialize to file");
    }
    file.close();
    return serializationSuccess;
}

#else

#include <SD.h>

#warning "not implemented yet"

#endif