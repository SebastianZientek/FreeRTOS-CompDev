#include <stdio.h>

#include "CompDevSDK.h"
#include "ports.h"
#include "uart.h"
#include "display.h"
#include "i2c.h"
#include "NRF24L01.h"
#include "SPI.h"
#include "rtc_ds1703.h"

// #define TRANSMITTER
#define RECEIVER

TaskHandle_t BlinkTaskHandle;
TaskHandle_t RadioTaskHandle;
TaskHandle_t ClockTaskHandle;
TaskHandle_t SystemInitTaskHandle;
TaskHandle_t MapTaskHandle;

ds1703_time_t time;


char gameMap[2][21];
char displayBuffer[2][21];

typedef struct{
    uint8_t x, y;
} Player;
Player player = {0, 0};

enum {
    CLOCK,
    MENU,
    SETTING_CLOCK
} mode;

enum {
    ONGOING,
    GAMEOVER
} gameState = ONGOING;

void transmitterTask(void *param)
{
    (void)param;
    char *address = "abcde";

    nrf24SetForTransmission(address);

    bool on = 0;
    while (1)
    {
        on = !on;

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
        else if(val > 1000 && val < 1050)
        {
            // displayPrint("DOWN");
            nrf24SendData("DOWN", 5, 0);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        else if(val > 270 && val < 280)
        {
            mode = MENU;
            // char clk[9];
            // snprintf(clk, 9, "%02d:%02d:%02d", time.hour, time.min, time.sec);
            // nrf24SendData(clk, 9, 0);

            vTaskDelay(200/portTICK_PERIOD_MS);
        }
        // uartPrintf("val: %d\n\r", val);
        // delayMicroseconds(1000);
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
            // displayPrintf("R: %s     ", buffer);
            // uartPrintf("R: %s\r\n", buffer);

            if (strncmp("DOWN", buffer, 4) == 0)
            {
                player.y = 1;
            }
            else if (strncmp("UP", buffer, 4) == 0)
            {
                player.y = 0;
            }
        }

        displaySetPos(0, 1);
        int val = analogRead(A0);
        if (val > 110 && val < 120)
        {
            // displayPrint("UP    ");
            player.y = 0;
        }
        if(val > 180 && val < 190)
        {
            displayPrint("LEFT  ");
        }
        else if(val > 445 && val < 455)
        {
            displayPrint("RIGHT ");
        }
        else if(val > 1000 && val < 1050)
        {
            // displayPrint("DOWN  ");
            player.y = 1;
        }
        else if(val > 270 && val < 280)
        {
            displayPrint("CENTER");
        }

        vTaskDelay(120/portTICK_PERIOD_MS);
    }
}

void mapTask(void *param)
{
    (void)param;


    while (1)
    {
        if (gameState == ONGOING)
        {
            memcpy(gameMap[0], gameMap[0] + 1, 19);
            memcpy(gameMap[1], gameMap[1] + 1, 19);

            if (gameMap[1][19] == ' ' && gameMap[1][18] == ' ')
            {
                gameMap[0][19] = rand() % 5 ? ' ' : 0x14;
            }
            else
            {
                gameMap[0][19] = ' ';
            }

            if (gameMap[0][19] == ' ' && gameMap[0][18] == ' ')
            {
                gameMap[1][19] = rand() % 5 ? ' ' : 0x14;
            }
            else
            {
                gameMap[1][19] = ' ';
            }
        }


        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


// void drawMap()
// {
//     displaySetPos(0, 0);
//     displayPrint(gameMap[0]);
//     displaySetPos(0, 1);
//     displayPrint(gameMap[1]);
// }

void drawDisplayBuffer()
{
    displaySetPos(0, 0);
    displayPrint(displayBuffer[0]);
    displaySetPos(0, 1);
    displayPrint(displayBuffer[1]);
}

void clearDisplayBuffer()
{
    for (int i = 0; i < 20; ++i)
    {
        displayBuffer[0][i] = ' ';
        displayBuffer[1][i] = ' ';
    }
    displayBuffer[0][20] = '\0';
    displayBuffer[1][20] = '\0';
}

void initialGenerateMap()
{
    for (int i = 0; i < 20; ++i)
    {
        gameMap[0][i] = ' ';
        gameMap[1][i] = ' ';
    }
    gameMap[0][20] = '\0';
    gameMap[1][20] = '\0';
}

void blinkTask(void *param)
{
    (void)param;

    clearDisplayBuffer();
    initialGenerateMap();

    while (1)
    {
        switch (gameState)
        {
            case ONGOING:
                memcpy(displayBuffer, gameMap, sizeof(gameMap));

                if (gameMap[player.y][player.x] != ' ') gameState = GAMEOVER;

                displayBuffer[player.y][player.x] = 0x1D;
                drawDisplayBuffer();
                clearDisplayBuffer();
            break;
            case GAMEOVER:
                displaySetPos(5, 0);
                displayPrint("GAME  OVER");
            break;
        }

        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}

void clockTask(void *param)
{
    (void)param;

    // ds1703_time_t time = {
    //     .sec = 0,
    //     .min = 30,
    //     .hour = 12,
    //     .dow = 5,
    //     .day = 10,
    //     .month = 7,
    //     .year = 2020
    // };

    // ds1703SetTime(time);
    while (1)
    {
        if (mode == CLOCK)
        {
            time = ds1703GetTime();
            // uartPrintf("%d %d %d %d %d %d %d\n\r", time.hour, time.min, time.sec, time.dow, time.day, time.month, time.year);
            displaySetPos(6, 0);
            displayPrintf("%02d:%02d:%02d", time.hour, time.min, time.sec);
            displaySetPos(5, 1);
            displayPrintf("%02d.%02d.%04d", time.day, time.month, time.year);
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        if (mode == MENU)
        {
            displaySetPos(0, 0);
            displayPrint("        MENU        ");
            displaySetPos(0, 1);
            displayPrint("                    ");
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        else
        {
            taskYIELD();
        }
    }
}


void systemInitTask(void *param)
{
    mode = CLOCK;
    uartInit(9600);
    setPinMode(5, PIN_OUTPUT);

    i2cInit();
    displayInit();

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


    #ifdef TRANSMITTER
        xTaskCreate(clockTask, "ClockTask", 140, NULL, 1, &ClockTaskHandle);
        xTaskCreate(transmitterTask, "TransmitterRadioTask", 200, NULL, 1, &RadioTaskHandle);
    #endif
    #ifdef RECEIVER
        xTaskCreate(blinkTask, "BlinkTask", 140, NULL, 1, &BlinkTaskHandle);
        xTaskCreate(mapTask, "MapTask", 100, NULL, 1, &MapTaskHandle);
        xTaskCreate(receiverTask, "ReceiverRadioTask", 200, NULL, 1, &RadioTaskHandle);
    #endif

    vTaskDelete(NULL);
}

void CompDevSDKMain()
{
    xTaskCreate(systemInitTask, "SystemInitTask", 200, NULL, 1, &SystemInitTaskHandle);
}
