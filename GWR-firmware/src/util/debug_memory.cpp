#include "debug_memory.h"

#include <Arduino.h>

#include "config/app_conf.h"

void task_debug_mem(void *pv) {
    while (1) {
        debuglnF("MEMORY DEBUG:");
        
        UBaseType_t task_count = uxTaskGetNumberOfTasks();
        configRUN_TIME_COUNTER_TYPE ulTotalRunTime;
        
        debugf("Tasks: %d\n", task_count);
        debugf("Heap free: %d\n", xPortGetFreeHeapSize());
        
        TaskStatus_t *taskStats;
        taskStats = (TaskStatus_t *) pvPortMalloc(sizeof(TaskStatus_t) * task_count);
        if (taskStats != NULL) {
            uxTaskGetSystemState(taskStats, task_count, &ulTotalRunTime);
            ulTotalRunTime /= 100; // convert to percent
            if (ulTotalRunTime <= 0) {
                ulTotalRunTime = 1;
            }
            for (int x = 0; x < task_count; x++) {
                debugf("Task \"%s\": %d%% cpu, %d bytes free\n", 
                    taskStats[x].pcTaskName, 
                    taskStats[x].ulRunTimeCounter / ulTotalRunTime, 
                    taskStats[x].usStackHighWaterMark
                );
            }
            vPortFree(taskStats);
        }
        vTaskDelay(15000);
    }
    vTaskDelete(NULL);
}