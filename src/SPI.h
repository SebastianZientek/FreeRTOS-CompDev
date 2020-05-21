#ifndef _SPI_H_
#define _SPI_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    SPI_ORDER_LSBMSB,
    SPI_ORDER_MSBLSB
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

/**
 * @brief Initialize the SPI
 *
 */
void SPIInit();

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

/**
 * @brief Sets SPI data mode
 *
 * @param mode      data sending mode
 */
void SPISetDataMode(SpiDataMode_t mode);

/**
 * @brief Sets bit order for data transmission
 *
 * @param order     bit order (SPI_ORDER_LSBMSB or SPI_ORDER_MSBLSB)
 */
void SPISetBitOrder(SpiBitOrder_t order);

/**
 * @brief Sets clock divider for SPI clock
 *
 * @param divider     divider for the clock
 */
void SPISetClockDivider(SpiClkDiv_t divider);

#ifdef __cplusplus
}
#endif

#endif // _SPI_H_
