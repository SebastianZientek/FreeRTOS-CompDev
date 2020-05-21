#ifndef _SPI_H_
#define _SPI_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    SPI_ORDER_LSB_FIRST,
    SPI_ORDER_MSB_FIRST
} SpiBitOrder_t;

typedef enum {
    SPI_CLKDIV_2,
    SPI_CLKDIV_4,
    SPI_CLKDIV_8,
    SPI_CLKDIV_16,
    SPI_CLKDIV_32,
    SPI_CLKDIV_64,
    SPI_CLKDIV_128
} SpiClkDiv_t;

typedef enum {
    SPI_DATA_MODE_0 = 0x00, // CPOL = 0, CPHA = 0
    SPI_DATA_MODE_1 = 0x04, // CPOL = 0, CPHA = 1
    SPI_DATA_MODE_2 = 0x08, // CPOL = 1, CPHA = 0
    SPI_DATA_MODE_3 = 0x0C  // CPOL = 1, CPHA = 1
} SpiDataMode_t;

typedef struct{
    SpiBitOrder_t bitOrder;
    SpiClkDiv_t clkDivider;
    SpiDataMode_t dataMode;
} SPIConfig;

/**
 * @brief Initialize the SPI
 *
 * @param spiConfig     configuration of the SPI
 */
void SPIInit(SPIConfig spiConfig);

/**
 * @brief Disable SPI
 *
 */
void SPIDisable();

/**
 * @brief Transfer and get data from SPI
 *
 * @param data      data to transfer
 * @return uint8_t  received data
 */
uint8_t SPITransfer(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // _SPI_H_
