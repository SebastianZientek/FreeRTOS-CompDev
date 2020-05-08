#include "CompDevSDK.h"
#include "ports.h"

TaskHandle_t BlinkTaskHandle;
void BlinkTask(void *param)
{
    (void)param;
    setPinValue(13, !getPinValue(13));
    vTaskDelay(1000/portTICK_PERIOD_MS);
}

void systemInit()
{
    setPinMode(13, PIN_OUTPUT);
}

void CompDevSDKMain()
{
    systemInit();

    xTaskCreate(BlinkTask, "BlinkTask", 100, NULL, 1, &BlinkTaskHandle);
}
