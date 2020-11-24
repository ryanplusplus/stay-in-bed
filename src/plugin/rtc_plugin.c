/*!
 * @file
 * @brief
 */

#include "data_model_key_value_pairs.h"
#include "rtc_plugin.h"
#include "ds1307.h"
#include "tiny_utils.h"

enum {
  poll_period_msec = 500
};

static tiny_timer_t timer;

static void poll(tiny_timer_group_t* timer_group, void* context)
{
  reinterpret(key_value_store, context, i_tiny_key_value_store_t*);

  clock_time_t time;
  ds1307_get(&time);
  tiny_key_value_store_write(key_value_store, key_current_time, &time);

  tiny_timer_start(timer_group, &timer, poll_period_msec, poll, key_value_store);
}

void rtc_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  ds1307_init(i2c_init());

  poll(timer_group, key_value_store);
}
