/*!
 * @file
 * @brief
 */

#include "stm8s_gpio.h"
#include "data_model_key_value_pairs.h"
#include "button_plugin.h"
#include "tiny_utils.h"

#define button_port GPIOD
#define button_pin (1 << 3)

#define ground_port GPIOD
#define ground_pin (1 << 1)

enum {
  poll_period_msec = 15,
  target_debounce_count = 3
};

static tiny_timer_t timer;
static bool previous;
static bool debounced;
static uint8_t debounce_count;

inline bool button_state(void)
{
  // Since we're pulled up we need to read the inverse
  return !(button_port->IDR & button_pin);
}

inline void configure_input(void)
{
  // Input with pull up
  button_port->CR1 |= button_pin;
}

inline void configure_output(void)
{
  // Output low
  ground_port->DDR |= ground_pin;
  ground_port->ODR &= ~ground_pin;
}

static void send_press_signal(i_tiny_key_value_store_t* key_value_store, bool state)
{
  tiny_key_value_store_key_t key = state ? key_button_press_signal : key_button_release_signal;

  event_signal_t signal;
  tiny_key_value_store_read(key_value_store, key, &signal);
  signal++;
  tiny_key_value_store_write(key_value_store, key, &signal);
}

static void poll(tiny_timer_group_t* timer_group, void* context)
{
  reinterpret(key_value_store, context, i_tiny_key_value_store_t*);

  bool current = button_state();

  if(current != previous) {
    debounce_count = 0;
  }
  else if(current != debounced) {
    debounce_count += 1;

    if(debounce_count == target_debounce_count) {
      debounced = current;
      send_press_signal(key_value_store, debounced);
    }
  }

  previous = current;

  tiny_timer_start(timer_group, &timer, poll_period_msec, poll, key_value_store);
}

void button_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  configure_input();
  configure_output();
  poll(timer_group, key_value_store);
}
