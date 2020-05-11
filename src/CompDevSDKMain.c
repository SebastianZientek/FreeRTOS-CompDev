#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"
#include "display.h"

TaskHandle_t BlinkTaskHandle;
void BlinkTask(void *param)
{
    (void)param;
    static uint32_t blinksNum = 0;
    while (1)
    {
        setPinValue(13, !getPinValue(13));
        displaySetPos(0, 0);
        displayPrintf("Blinks: %u", blinksNum++);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void systemInit()
{
    uartInit(9600);
    displayInit();
    setPinMode(13, PIN_OUTPUT);

    uartPrint("System initialized\n");
}

void CompDevSDKMain()
{
    systemInit();
    xTaskCreate(BlinkTask, "BlinkTask", 200, NULL, 1, &BlinkTaskHandle);
}
