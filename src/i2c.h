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
 * @param addr          device address
 * @param data          data to send
 * @param len           size of data
 * @param repeatStart   send repeated start after sending data
 * @return int8_t       error for < 0 or written data size
 */
int8_t i2cWrite(uint8_t addr, uint8_t *data, uint8_t len, bool repeatStart);

/**
 * @brief Write byte over i2c
 *
 * @param addr          device address
 * @param data          byte to send
 * @param repeatStart   send repeated start after sending data
 * @return int8_t       error for < 0 or written data size
 */
int8_t i2cWriteByte(uint8_t addr, uint8_t data, bool repeatStart);

/**
 * @brief Reads data from i2c
 *
 * @param addr          device address
 * @param buffer        buffer to store data
 * @param len           size of the buffer
 * @param repeatStart   send repeated start after reading data
 * @return int8_t       < 0 for error or read size
 */
int8_t i2cRead(uint8_t addr, uint8_t *buffer, uint8_t len, bool repeatStart);

/**
 * @brief Read one byte over i2c
 *
 * @param addr          device address
 * @param repeatStart   send repeated start after reading data
 * @return uint8_t      received byte
 */
uint8_t i2cReadByte(uint8_t addr, bool repeatStart);

#ifdef __cplusplus
}
#endif

#endif // _I2C_H_
