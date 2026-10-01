#include "battery.h"

#include <Arduino.h>

#include "config/app_conf.h"
#include "config/hardware_conf.h"
#include "util/lipo_percentage.h"
#include "taskglobals.h"

void task_batt(void *pv) {
    pinMode(Pins::ADC::VBAT, INPUT); 

    // todo find out if INPUT and OUTPUT differ from ANALOG at all
    // is it used for ADC? PWM? set automatically by periman? hal? idk
    
    while(1) {
        uint32_t mv_adc = analogReadMilliVolts(Pins::ADC::VBAT);
        
        // vadc = v_battery_actual * (lo / (hi + lo))
        g_battery_voltage = (mv_adc / 1000.0) * (float(VBAT_DIV_LOW) / float(VBAT_DIV_LOW + VBAT_DIV_HIGH));
        
        debugf("ADC showing %.2fV\n", mv_adc / 1000.0);
        debugf("Raw battery voltage is %.2fV\n", g_battery_voltage);
        
        g_battery_voltage = constrain(g_battery_voltage, 0.0, VBAT_FULL);
        debugf("Battery percent: %.1f%%\n", lipoPercentFancy(g_battery_voltage));
        
        vTaskDelay(10000);
    }
    
    vTaskDelete(NULL);

}