/*!
 * @file
 * @brief
 */

#include <stddef.h>
#include "data_model_key_value_pairs.h"
#include "led_plugin.h"
#include "apa102.h"
#include "tiny_utils.h"

enum {
  cpol = 0,
  cpha = 0,
  msb_first = true
};

static tiny_event_subscription_t subscription;

static void data_changed(void* context, const void* _args)
{
  (void)context;
  reinterpret(args, _args, const tiny_key_value_store_on_change_args_t*);

  if(args->key == key_led_actual_state) {
    apa102_write((const led_state_t*)args->value);
  }
}

void led_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  (void)timer_group;

  apa102_init(spi_init(cpol, cpha, msb_first, spi_baud_8mhz));

  tiny_event_subscription_init(&subscription, NULL, data_changed);
  tiny_event_subscribe(tiny_key_value_store_on_change(key_value_store), &subscription);
}
