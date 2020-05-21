/*!
 * @file
 * @brief
 */

#ifndef data_model_key_value_pairs_h
#define data_model_key_value_pairs_h

#include "tiny_ram_key_value_store_macros.h"
#include "clock_time.h"
#include "led_state.h"
#include "event_signal.h"

// clang-format off
#define data_model_key_value_pairs(pair) \
  pair(key_current_time,                clock_time_t) \
  pair(key_led_requested_state,         led_state_t) \
  pair(key_led_actual_state,            led_state_t) \
  pair(key_button_press_signal,         event_signal_t) \
  pair(key_button_release_signal,       event_signal_t) \
// clang-format on

enumerate_ram_key_value_pairs(data_model_key_value_pairs);

#endif
