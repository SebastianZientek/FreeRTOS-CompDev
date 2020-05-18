#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"
#include "display.h"

TaskHandle_t SystemInitTaskHandle;
TaskHandle_t BlinkTaskHandle;

void blinkTask(void *param)
{
    (void)param;
    static uint32_t blinksNum = 0;

    while (1)
    {
        setPinValue(13, !getPinValue(13));
        displaySetPos(0, 0);
        displayPrintf("Blinks: %u\nTest", blinksNum++);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void systemInitTask()
{
    uartInit(9600);
    displayInit();
    setPinMode(13, PIN_OUTPUT);
    uartPrint("System initialized\n");

    xTaskCreate(blinkTask, "BlinkTask", 200, NULL, 1, &BlinkTaskHandle);

    // Task done the job, can be removed now.
    vTaskDelete(NULL);
}

void CompDevSDKMain()
{
    xTaskCreate(systemInitTask, "SystemInitTask", 100, NULL, 1, &SystemInitTaskHandle);
}
