/*!
 * @file
 * @brief
 */

#include <stddef.h>
#include <string.h>
#include "data_model_key_value_pairs.h"
#include "wake_light_plugin.h"
#include "tiny_utils.h"

#define am(x) (x)
#define pm(x) (x + 12)

static tiny_event_subscription_t subscription;

static const clock_time_t start_night_light = {
  .hours = pm(6),
  .minutes = 0,
  .seconds = 0
};

static const clock_time_t default_wake_time = {
  .hours = am(7),
  .minutes = 0,
  .seconds = 0
};

static const led_state_t off_light = {
  .red = 0,
  .green = 0,
  .blue = 0,
  .brightness = 0
};

static const led_state_t default_night_light = {
  .red = 11,
  .green = 0,
  .blue = 5,
  .brightness = 1
};

static const led_state_t default_wake_light = {
  .red = 2,
  .green = 12,
  .blue = 0,
  .brightness = 1
};

static clock_time_t start_nap_night;
static clock_time_t start_nap_wake;
static clock_time_t end_nap_wake;
static bool nap_active;

static bool in_range_worker(const clock_time_t* lower, const clock_time_t* current, const clock_time_t* upper)
{
  bool lower_good = (current->hours > lower->hours) || (current->hours == lower->hours && current->minutes >= lower->minutes);
  bool upper_good = (current->hours < upper->hours) || (current->hours == upper->hours && current->minutes <= upper->minutes);
  return lower_good && upper_good;
}

static bool in_range(const clock_time_t* start, const clock_time_t* current, const clock_time_t* end)
{
  if((current->hours == start->hours && current->minutes == start->minutes) ||
    (current->hours == end->hours && current->minutes == end->minutes)) {
    return true;
  }
  else if(start->hours > end->hours || (start->hours == end->hours && start->minutes > end->minutes)) {
    return !in_range_worker(end, current, start);
  }
  else {
    return in_range_worker(start, current, end);
  }
}

static void add_time(clock_time_t* time, uint8_t hours, uint8_t minutes)
{
  time->hours += hours;
  time->minutes += minutes;

  if(time->minutes > 60) {
    time->minutes -= 60;
    time->hours += 1;
  }

  time->hours %= 24;
}

static void default_to(led_state_t* led_state, const led_state_t* default_state)
{
  if(memcmp(led_state, &off_light, sizeof(*led_state)) == 0) {
    memcpy(led_state, default_state, sizeof(*led_state));
  }
}

static void update_light(i_tiny_key_value_store_t* kvs)
{
  clock_time_t time;
  tiny_key_value_store_read(kvs, key_current_time, &time);

  clock_time_t start_wake_up;
  tiny_key_value_store_read(kvs, key_wake_time, &start_wake_up);

  clock_time_t end_wake_up;
  tiny_key_value_store_read(kvs, key_wake_time, &end_wake_up);
  add_time(&end_wake_up, 2, 0);

  led_state_t night_light;
  tiny_key_value_store_read(kvs, key_night_light_color, &night_light);
  default_to(&night_light, &default_night_light);

  led_state_t wake_light;
  tiny_key_value_store_read(kvs, key_wake_light_color, &wake_light);
  default_to(&wake_light, &default_wake_light);

  if(nap_active && in_range(&start_nap_wake, &time, &end_nap_wake)) {
    tiny_key_value_store_write(kvs, key_led_requested_state, &wake_light);
  }
  else if(nap_active && in_range(&start_nap_night, &time, &start_nap_wake)) {
    tiny_key_value_store_write(kvs, key_led_requested_state, &night_light);
  }
  else if(in_range(&start_wake_up, &time, &end_wake_up)) {
    tiny_key_value_store_write(kvs, key_led_requested_state, &wake_light);
  }
  else if(in_range(&start_night_light, &time, &start_wake_up)) {
    tiny_key_value_store_write(kvs, key_led_requested_state, &night_light);
  }
  else {
    tiny_key_value_store_write(kvs, key_led_requested_state, &off_light);
  }

  if(nap_active && !in_range(&start_nap_night, &time, &end_nap_wake)) {
    nap_active = false;
  }
}

static void stop_nap(void)
{
  nap_active = false;
}

static void start_nap(i_tiny_key_value_store_t* kvs)
{
  nap_active = true;

  clock_time_t time;
  tiny_key_value_store_read(kvs, key_current_time, &time);

  start_nap_night.hours = time.hours;
  start_nap_night.minutes = time.minutes;

  start_nap_wake.hours = time.hours;
  start_nap_wake.minutes = time.minutes;
  add_time(&start_nap_wake, 2, 30);

  end_nap_wake.hours = time.hours;
  end_nap_wake.minutes = time.minutes;
  add_time(&end_nap_wake, 4, 59);
}

static void data_changed(void* context, const void* _args)
{
  reinterpret(kvs, context, i_tiny_key_value_store_t*);
  reinterpret(args, _args, const tiny_key_value_store_on_change_args_t*);

  if(args->key == key_current_time) {
    update_light(kvs);
  }
  else if(args->key == key_button_press_signal) {
    if(nap_active) {
      stop_nap();
    }
    else {
      start_nap(kvs);
    }

    update_light(kvs);
  }
}

void wake_light_plugin_init(
  tiny_timer_group_t* timer_group,
  i_tiny_key_value_store_t* key_value_store)
{
  (void)timer_group;

  nap_active = false;

  tiny_key_value_store_write(key_value_store, key_wake_time, &default_wake_time);

  tiny_event_subscription_init(&subscription, key_value_store, data_changed);
  tiny_event_subscribe(tiny_key_value_store_on_change(key_value_store), &subscription);
}
