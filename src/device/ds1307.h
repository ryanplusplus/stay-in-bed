/*!
 * @file
 * @brief
 */

#ifndef ds1307_h
#define ds1307_h

#include "hal/i_tiny_i2c.h"
#include "clock_time.h"

void ds1307_init(i_tiny_i2c_t* i2c);
void ds1307_get(clock_time_t* time);
void ds1307_set(const clock_time_t* time);

#endif
