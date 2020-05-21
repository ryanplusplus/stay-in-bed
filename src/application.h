/*!
 * @file
 * @brief
 */

#ifndef application_h
#define application_h

#include "data_model.h"
#include "rtc_plugin.h"
#include "led_plugin.h"
#include "led_animation_plugin.h"
#include "wake_light_plugin.h"
#include "button_plugin.h"

inline void application_init(tiny_timer_group_t* timer_group) {
  data_model_init();
  i_tiny_key_value_store_t* key_value_store = data_model_key_value_store();

  led_plugin_init(timer_group, key_value_store);
  led_animation_plugin_init(timer_group, key_value_store);
  wake_light_plugin_init(timer_group, key_value_store);
  rtc_plugin_init(timer_group, key_value_store);
  button_plugin_init(timer_group, key_value_store);
}

#endif
