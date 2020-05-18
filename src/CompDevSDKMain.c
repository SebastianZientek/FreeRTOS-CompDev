#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"
#include "display.h"

TaskHandle_t BlinkTaskHandle;
TaskHandle_t DisplayInitTaskHandle;

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

void displayInitTask(void *param)
{
    (void)param;
    displayInit();
    vTaskDelete(NULL);
}

void systemInit()
{
    uartInit(9600);
    setPinMode(13, PIN_OUTPUT);
    xTaskCreate(displayInitTask, "DisplayInitTask", 100, NULL, 1, &DisplayInitTaskHandle);

    uartPrint("System initialized\n");
}

void CompDevSDKMain()
{
    systemInit();

    xTaskCreate(blinkTask, "BlinkTask", 200, NULL, 1, &BlinkTaskHandle);
}
