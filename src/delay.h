#ifndef _DELAY_H_
#define _DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delay microseconds (blocking)
 *
 * @param us        microseconds to wait
 */
void delayUs(unsigned int us);

#ifdef __cplusplus
}
#endif

#endif // _DELAY_H_
