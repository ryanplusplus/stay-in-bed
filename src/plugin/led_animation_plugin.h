/*!
 * @file
 * @brief
 */

#ifndef led_animation_plugin_h
#define led_animation_plugin_h

#include "i_tiny_key_value_store.h"
#include "tiny_timer.h"

void led_animation_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store);

#endif
