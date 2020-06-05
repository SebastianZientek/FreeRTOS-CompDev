#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define R_RX_PL_WID 0x60
#define NOP 0xFF

#define CONFIG 0x00
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0

#define EN_AA 0x01
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define DYNPD 0x1C
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0

#define RX_ADDR_P0 0x0A
#define TX_ADDR 0x10
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS 0x07
#define FEATURE 0x1D
#define EN_DPL 2
#define EN_ACK_PAY 1

#define EN_RXADDR 0x02
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define FIFO_STATUS 0x17
#define RX_EMPTY 0
#define MAX_DATA_IN_PACKAGE 32
#define RX_P_NO_MASK 0x07

#define PWR_MASK 0xF8

typedef enum{
    NRF24_PWR_MIN = 0,
    NRF24_PWR_MID_LOW = 1,
    NRF24_PWR_MID_HIGH = 2,
    NRF24_PWR_MAX = 3
} PowerLevel_t;

void nrf24Init();
uint8_t nrf24IsDataAvailable();
int8_t nrf24ReadData(void *buffer, uint8_t bufferLen, uint8_t *pipeNumber);
void nrf24Listen(uint8_t pipe, uint8_t *addr);
void nrf24SetForTransmission(uint8_t *addr);
void nrf24SendData(const void *data, uint8_t dataSize, uint8_t pipe);
void nrf24SetChannel(uint8_t channel);
void nrf24SetPowerLevel(PowerLevel_t powerLevel);

#ifdef __cplusplus
}
#endif

#endif // _NRF24L01_H_
