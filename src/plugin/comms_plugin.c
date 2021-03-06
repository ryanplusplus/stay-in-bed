/*!
 * @file
 * @brief
 */

#include "comms_plugin.h"
#include "comms.h"

static comms_t comms;

void comms_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  comms_init(&comms, uart1_init(), key_value_store, timer_group);
}
