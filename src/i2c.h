#ifndef _I2C_H_
#define _I2C_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the i2c
 *
 */
void i2cInit();

/**
 * @brief Write data over i2c
 *
 * @param addr      device address
 * @param data      data to send
 * @param size      size of data
 * @return int8_t   error for < 0 or written data size
 */
int8_t i2cWrite(uint8_t addr, uint8_t *data, uint8_t size);

/**
 * @brief Write one byte over i2c
 *
 * @param addr  device address
 * @param data  data to send
 */
void i2cWriteOneByte(uint8_t addr, uint8_t data);

/**
 * @brief Reads data from i2c
 *
 * @param addr          device address
 * @param buffer        buffer to store data
 * @param bufferSize    size of the buffer
 * @return int8_t       < 0 for error or read size
 */
int8_t i2cRead(uint8_t addr, uint8_t *buffer, uint8_t bufferSize);

/**
 * @brief request and read one byte from i2c
 *
 * @param addr      device address
 * @return uint8_t  readed byte
 */
uint8_t i2cReadOneByte(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif // _I2C_H_
