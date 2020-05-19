/*!
 * @file
 * @brief
 */

#ifndef wake_light_plugin_h
#define wake_light_plugin_h

#include "tiny_timer.h"
#include "i_tiny_key_value_store.h"

void wake_light_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store);

#endif
