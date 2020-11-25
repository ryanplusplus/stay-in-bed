/*!
 * @file
 * @brief
 */

#ifndef comms_plugin_h
#define comms_plugin_h

#include "uart1.h"
#include "tiny_timer.h"
#include "i_tiny_key_value_store.h"

void comms_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store);

#endif
