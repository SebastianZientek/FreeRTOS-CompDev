#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"
#include "display.h"
#include "i2c.h"
#include "NRF24L01.h"
#include "SPI.h"

#define TRANSMITTER

TaskHandle_t BlinkTaskHandle;
TaskHandle_t RadioTaskHandle;
TaskHandle_t SystemInitTaskHandle;

void transmitterTask(void *param)
{
    (void)param;
    char *address = "abcde";

    nrf24SetForTransmission(address);

    bool on = 0;
    while (1)
    {
        on = !on;

        // if (on)
        //     nrf24SendData("ON", strlen("ON") + 1, 0);
        // else
        //     nrf24SendData("OFF", strlen("OFF") + 1, 0);
        // vTaskDelay(1000/portTICK_PERIOD_MS);


        int val = analogRead(A0);
        if (val > 110 && val < 120)
        {
            // displayPrint("UP    ");
            nrf24SendData("UP", 3, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        if(val > 180 && val < 190)
        {
            // displayPrint("LEFT  ");
            nrf24SendData("LEFT", 5, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        else if(val > 445 && val < 455)
        {
            // displayPrint("RIGHT");
            nrf24SendData("RIGHT", 6, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        else if(val > 1010 && val < 1020)
        {
            // displayPrint("DOWN");
            nrf24SendData("DOWN", 5, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        else if(val > 270 && val < 280)
        {
            // displayPrint("CENTER");
            nrf24SendData("CENTER", 7, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
    }
}

void receiverTask(void *param)
{
    (void)param;
    char *address = "abcde";

    nrf24Listen(0, address);

    while (1)
    {
        if (nrf24IsDataAvailable())
        {
            uint8_t buffer[10];
            nrf24ReadData(buffer, 10, NULL);
            displaySetPos(0, 0);
            displayPrintf("R: %s     ", buffer);
            uartPrintf("R: %s\r\n", buffer);
        }

        displaySetPos(0, 1);
        int val = analogRead(A0);
        if (val > 110 && val < 120)
        {
            displayPrint("UP    ");
        }
        if(val > 180 && val < 190)
        {
            displayPrint("LEFT  ");
        }
        else if(val > 445 && val < 455)
        {
            displayPrint("RIGHT ");
        }
        else if(val > 1010 && val < 1020)
        {
            displayPrint("DOWN  ");
        }
        else if(val > 270 && val < 280)
        {
            displayPrint("CENTER");
        }

        vTaskDelay(120/portTICK_PERIOD_MS);
    }
}

void blinkTask(void *param)
{
    (void)param;
    while (1)
    {
        setPinValue(5, !getPinValue(5));
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void systemInitTask(void *param)
{
    uartInit(9600);
    setPinMode(5, PIN_OUTPUT);

    #ifdef RECEIVER
    i2cInit();
    displayInit();
    #endif

    SPIConfig spiConfig = {
        .bitOrder = SPI_ORDER_MSB_FIRST,
        .clkDivider = SPI_CLKDIV_2,
        .dataMode = SPI_DATA_MODE_0
    };

    SPIInit(spiConfig);

    nrf24Init();
    nrf24SetChannel(20);
    nrf24SetPowerLevel(NRF24_PWR_MAX);

    uartPrint("System initialized\r\nStarting tasks\r\n");

    xTaskCreate(blinkTask, "BlinkTask", 140, NULL, 1, &BlinkTaskHandle);

    #ifdef TRANSMITTER
        xTaskCreate(transmitterTask, "TransmitterRadioTask", 200, NULL, 1, &RadioTaskHandle);
    #endif
    #ifdef RECEIVER
        xTaskCreate(receiverTask, "ReceiverRadioTask", 200, NULL, 1, &RadioTaskHandle);
    #endif

    vTaskDelete(NULL);
}

void CompDevSDKMain()
{
    xTaskCreate(systemInitTask, "SystemInitTask", 200, NULL, 1, &SystemInitTaskHandle);
}
