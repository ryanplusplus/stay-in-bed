/*!
 * @file
 * @brief
 */

#ifndef led_plugin_h
#define led_plugin_h

#include "spi.h"
#include "tiny_timer.h"
#include "i_tiny_key_value_store.h"

void led_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store);

#endif
