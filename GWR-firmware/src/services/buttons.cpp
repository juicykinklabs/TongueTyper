#include "services/buttons.h"
#include <Arduino.h>
#include "MCP23S17.h"
#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "structs/UserInputMessage.h"
#include "taskglobals.h"
#include "algorithm"
using namespace std;

bool flag = false;

void taskInput(){
    SPI.begin(Pins::SPI::SCK, Pins::SPI::MISO, Pins::SPI::MOSI);
    MCP23S17 MCP = MCP23S17(Pins::XL::CS, &SPI);
    MCP.begin(false);
    
    static long timeout = 1000/portTICK_PERIOD_MS;
    static long debounce = 200/portTICK_PERIOD_MS;

    UserInputMessage messi;
    static int inputValues[2];
    static uint8_t status = 0xdd;
    static long timeoutTimer = -1;
    static long debounceTimer = -1;

    while(1){
        if(flag){//there's new information from IRAM_ATTR
            flag = false;
            
            if((long)millis - debounceTimer > debounce){ //debounce check (refuse input during debounce period)
                debounceTimer = (long)millis;

                if((long)millis - timeoutTimer > timeout){//multi input timed out, empty inputValues array
                    fill(inputValues[0], inputValues[3], -1);
                }
                else{//multi input not timed out, move the button input from previous trigger into the slot for double input, etc
                    inputValues[2] = inputValues[1];
                    inputValues[1] = inputValues[0];
                }

                for (int pin = 0; pin < 7; pin++){
                    status = MCP.read1(pin);
                    switch (status)
                    {
                        case 0:
                            continue;
                        case 1:
                            inputValues[0] = pin;
                        default://TODO error catching?
                            continue;
                    }
                }

                timeoutTimer = (long)millis;

                xQueueSend(q_userinput, (void*)&messi, 0);
            }
        }
        vTaskDelay(30/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
    
}


void IRAM_ATTR gpio_io_io_io_io_io_io(){}