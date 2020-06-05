#include "platform_config.h"

#include "NRF24L01.h"
#include "ports.h"
#include "SPI.h"
#include "delay.h"

TaskHandle_t NrfEventTaskHandle;

static uint8_t sendCmd(uint8_t cmd, void *data, size_t length)
{
    uint8_t *dataPtr = (uint8_t *)data;
    setPinValue(NRF24_CSN_PIN, PIN_LOW);

    uint8_t status = SPITransfer(cmd);
    while (length--)
    {
        *dataPtr = SPITransfer(*dataPtr);
        dataPtr++;
    }

    setPinValue(NRF24_CSN_PIN, PIN_HIGH);

    return status;
}

static void writeRegister(uint8_t reg, uint8_t value)
{
    sendCmd(W_REGISTER | reg, &value, 1);
}

static uint8_t readRegister(uint8_t reg)
{
    uint8_t value;
    sendCmd(R_REGISTER | reg, &value, 1);

    return value;
}

static void flushTX()
{
    sendCmd(FLUSH_TX, NULL, 0);
}

static void flushRX()
{
    sendCmd(FLUSH_RX, NULL, 0);
}

static int8_t isTransmissionSuccess()
{
    setPinValue(NRF24_CE_PIN, PIN_LOW);
    uint8_t status = sendCmd(NOP, NULL, 0);

    //  0 - Transsmision success
    // -1 - Transsmision failure, maximum number of TX retransmits exceed
    // -2 - Transsmision failure (i.e. when auto retransmission is not enabled)
    int8_t success = -2;
    if (status & (1 << MASK_TX_DS))
    {
        success = 0;
    }
    else if (status & (1 << MASK_MAX_RT))
    {
        success = -1;
    }

    return success;
}

static uint8_t getNrfStatus()
{
    return sendCmd(NOP, NULL, 0);
}

static void nrfEventTask(void *param)
{
    (void)param;
    while (1)
    {
        vTaskSuspend(NULL);

        uint8_t status = getNrfStatus();

        if (status & ((1 << MASK_TX_DS) | (1 << MASK_MAX_RT)))
        {
            int8_t success = isTransmissionSuccess();
            if (success != 0)
            {
                setPinValue(5, !getPinValue(5));
                flushTX();
            }
        }

        // Clear interrupts
        writeRegister(STATUS, (1 << MASK_MAX_RT) | (1 << MASK_RX_DR));
    }
}

ISR(INT0_vect) {
    BaseType_t xYieldRequired = xTaskResumeFromISR(NrfEventTaskHandle);
    if (xYieldRequired == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

void nrf24Init()
{
    setPinMode(NRF24_CSN_PIN, PIN_OUTPUT);
    setPinMode(NRF24_CE_PIN, PIN_OUTPUT);

    setPinValue(NRF24_CSN_PIN, PIN_HIGH);
    setPinValue(NRF24_CE_PIN, PIN_LOW);

    xTaskCreate(nrfEventTask, "nrfEventTask", 150, NULL, 1, &NrfEventTaskHandle);

    // Power up and config
    uint8_t cmdData = (1 << PWR_UP) | (1 << CRCO) | (1 << EN_CRC) | (1 << PRIM_RX);
    writeRegister(CONFIG, (1 << PWR_UP) | (1 << CRCO) | (1 << EN_CRC) | (1 << PRIM_RX));

    delayUs(10);

    // Enable Auto ACK on all pipes
    writeRegister(EN_AA, (1 << ENAA_P0) | (1 << ENAA_P1) | (1 << ENAA_P2)
                        | (1 << ENAA_P3) | (1 << ENAA_P4) | (1 << ENAA_P5));

    // Enable Dynamic Payload Length on all pipes
    writeRegister(DYNPD, (1 << DPL_P0) | (1 << DPL_P1) | (1 << DPL_P2)
                        | (1 << DPL_P3) | (1 << DPL_P4) | (1 << DPL_P5));

    // Disable all RX pipes
    cmdData = 0x00;
    writeRegister(EN_RXADDR, 0x00);

    // Enable Dynamic Payload feature
    cmdData = (1 << EN_DPL);
    writeRegister(FEATURE, (1 << EN_DPL));

    flushTX();
    flushRX();

    // Clear all interrupts
    writeRegister(STATUS,
        (1 << MASK_RX_DR) | (1 << MASK_TX_DS) | (1 << MASK_MAX_RT));

    // Enable interrupt
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
}

void nrf24SetChannel(uint8_t channel)
{
    writeRegister(RF_CH, channel);
}

void nrf24SetPowerLevel(PowerLevel_t powerLevel)
{
    uint8_t rfSetup = readRegister(RF_SETUP) & PWR_MASK;

    if (powerLevel > 3)
    {
        powerLevel = (NRF24_PWR_MAX << 1) + 1;
    } else
    {
        powerLevel = (powerLevel << 1) + 1;
    }

    writeRegister(RF_SETUP, rfSetup | powerLevel);
}

void nrf24Listen(uint8_t pipe, uint8_t *addr)
{
    uint8_t addrTmp[5];
    memcpy(addrTmp, addr, 5);

    uint8_t config = readRegister(CONFIG);
    config |= (1 << PRIM_RX);
    writeRegister(CONFIG, config);

    sendCmd(W_REGISTER | RX_ADDR_P0 + pipe, addrTmp, 5);

    uint8_t currentPipes = readRegister(EN_RXADDR);
    currentPipes |= (1 << pipe);
    writeRegister(EN_RXADDR, currentPipes);

    setPinValue(NRF24_CE_PIN, PIN_HIGH);

    sendCmd(FLUSH_TX, NULL, 0);
}

uint8_t nrf24IsDataAvailable()
{
    uint8_t fifoStatus = readRegister(FIFO_STATUS);

    if (fifoStatus & (1 << RX_EMPTY))
        return 0;

    return 1;
}

int8_t nrf24ReadData(void *buffer, uint8_t bufferLen, uint8_t *pipeNumber)
{
    uint8_t pipe = sendCmd(NOP, NULL, 0);
    if (pipeNumber)
    {
        *pipeNumber = pipe;
    }

    int8_t dataSize = readRegister(R_RX_PL_WID);

    if (dataSize > bufferLen || dataSize == 0)
    {
        return -1;
    }

    sendCmd(R_REGISTER | R_RX_PAYLOAD, buffer, dataSize);

    // Clear all interrupts
    writeRegister(STATUS,
        (1 << MASK_RX_DR) | (1 << MASK_MAX_RT) | (1 << MASK_TX_DS));

    return dataSize;
}

void nrf24SetForTransmission(uint8_t *addr)
{
    uint8_t addrTmp[5];
    memcpy(addrTmp, addr, 5);

    sendCmd(W_REGISTER | TX_ADDR, addrTmp, 5);

    // Set RX_ADDR_P0 equal to TX address to handle automatic acknowledge
    memcpy(addrTmp, addr, 5);
    sendCmd(W_REGISTER | RX_ADDR_P0, addrTmp, 5);

    writeRegister(EN_RXADDR, (1 << ERX_P0));
}

void nrf24SendData(const void *data, uint8_t dataSize, uint8_t pipe)
{
    writeRegister(STATUS, (1 << MASK_TX_DS) | pipe);

    uint8_t *dataCpy = malloc(dataSize);
    memcpy(dataCpy, data, dataSize);
    sendCmd(W_REGISTER | W_TX_PAYLOAD, dataCpy, dataSize);
    free(dataCpy);

    uint8_t config = readRegister(CONFIG);
    config &= ~(1 << PRIM_RX);
    writeRegister(CONFIG, config);

    setPinValue(NRF24_CE_PIN, PIN_HIGH);
}
