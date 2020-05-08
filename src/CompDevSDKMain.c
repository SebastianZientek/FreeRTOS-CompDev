#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"

TaskHandle_t BlinkTaskHandle;
void BlinkTask(void *param)
{
    (void)param;
    while (1)
    {
        setPinValue(13, !getPinValue(13));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void systemInit()
{
    uartInit(9600);
    setPinMode(13, PIN_OUTPUT);

    uartPrint("System initialized\n");
}

void CompDevSDKMain()
{
    systemInit();

    xTaskCreate(BlinkTask, "BlinkTask", 100, NULL, 1, &BlinkTaskHandle);
}
