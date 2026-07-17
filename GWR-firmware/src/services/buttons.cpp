#include "services/buttons.h"
#include <Arduino.h>
#include "MCP23S17.h"
#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/UserInputMessage.h"
#include "taskglobals.h"


bool flag = false;

void taskInput(){
    SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);
    MCP23S17 MCP = MCP23S17(Pins::XL::CS, &SPI);
    MCP.begin(false);
    
    static long timeout = 180/portTICK_PERIOD_MS;


    UserInputMessage messi;
    static int val[2];
    static int count = 0;
    static uint8_t status = 0xdd;
    static long timer = -1;

    while(1){
        if(flag){
            flag = false;
            for (int pin = 0; pin < 7; pin++){
                status = MCP.read1(pin);
                switch (status)
                {
                    case 0:
                        continue;
                    case 1:
                        val[count] = pin;
                    default://TODO error catching?
                        continue;
                }
                count++;
            }
            count = 0;

            if((long)millis - timer > timeout){
                timer = -1;
            }
            if(timer = -1){
                timer = (long)millis;
            }

            messi.button1 = val[0];
            messi.button2 = val[1];
            messi.button3 = val[2];
            xQueueSend(q_userinput, (void*)&messi, 0);
            
        }
        vTaskDelay(30/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
    
}


void IRAM_ATTR gpio_io_io_io_io_io_io(){}