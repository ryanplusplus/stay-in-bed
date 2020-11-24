/*!
 * @file
 * @brief
 */

#include "data_model_key_value_pairs.h"
#include "led_animation_plugin.h"
#include "tiny_utils.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))

enum {
  update_period_msec = 50,
  step = 10,
};

static tiny_timer_t timer;

static uint8_t transform(const uint8_t current, const uint8_t target)
{
  if(current < target) {
    uint16_t diff = target - current;
    uint8_t adjustment = (diff * step) / 100;
    return current + max(adjustment, 1);
  }
  else if(current > target) {
    uint16_t diff = current - target;
    uint8_t adjustment = (diff * step) / 100;
    return current - max(adjustment, 1);
  }
  else {
    return current;
  }
}

static void update(tiny_timer_group_t* timer_group, void* context)
{
  reinterpret(key_value_store, context, i_tiny_key_value_store_t*);
  led_state_t requested;
  led_state_t actual;

  tiny_key_value_store_read(key_value_store, key_led_requested_state, &requested);
  tiny_key_value_store_read(key_value_store, key_led_actual_state, &actual);

  actual.red = transform(actual.red, requested.red);
  actual.green = transform(actual.green, requested.green);
  actual.blue = transform(actual.blue, requested.blue);
  actual.brightness = transform(actual.brightness, requested.brightness);

  tiny_key_value_store_write(key_value_store, key_led_actual_state, &actual);

  tiny_timer_start(timer_group, &timer, update_period_msec, update, key_value_store);
}

void led_animation_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  update(timer_group, key_value_store);
}
