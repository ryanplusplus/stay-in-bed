/*!
 * @file
 * @brief
 */

#ifndef apa102_h
#define apa102_h

#include "hal/i_tiny_spi.h"
#include "led_state.h"

void apa102_init(i_tiny_spi_t* spi);
void apa102_write(const led_state_t* state);

#endif
