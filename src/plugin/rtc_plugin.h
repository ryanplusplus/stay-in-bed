/*!
 * @file
 * @brief
 */

#ifndef rtc_plugin_h
#define rtc_plugin_h

#include "i2c.h"
#include "tiny_timer.h"
#include "i_tiny_key_value_store.h"

void rtc_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store);

#endif
