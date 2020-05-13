#ifndef _I2C_H_
#define _I2C_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void i2cInit();
void i2cSend(uint8_t addr, uint8_t data);
uint8_t i2cRead(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif // _I2C_H_
