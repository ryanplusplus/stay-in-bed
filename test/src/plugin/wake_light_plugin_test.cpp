/*!
 * @file
 * @brief
 */

extern "C" {
#include "data_model_key_value_pairs.h"
#include "data_model.h"
#include "wake_light_plugin.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

static const led_state_t night = {
  .red = 40,
  .green = 0,
  .blue = 25,
  .brightness = 1
};

static const led_state_t wake = {
  .red = 10,
  .green = 120,
  .blue = 10,
  .brightness = 1
};

static const led_state_t default_night = {
  .red = 11,
  .green = 0,
  .blue = 5,
  .brightness = 1
};

static const led_state_t default_wake = {
  .red = 2,
  .green = 12,
  .blue = 0,
  .brightness = 1
};

static const led_state_t off = {
  .red = 0,
  .green = 0,
  .blue = 0,
  .brightness = 0
};

TEST_GROUP(wake_light_plugin)
{
  i_tiny_key_value_store_t* kvs;

  void setup()
  {
    data_model_init();
    kvs = data_model_key_value_store();

    wake_light_plugin_init(NULL, kvs);

    given_that_the_night_light_color_is(&night);
    given_that_the_wake_light_color_is(&wake);
    given_that_the_wake_time_is(6, 30);
  }

  void after_the_plugin_is_initialized(void)
  {
    wake_light_plugin_init(NULL, kvs);
  }

  void given_that_the_requested_led_state_is(const led_state_t* state)
  {
    tiny_key_value_store_write(kvs, key_led_requested_state, state);
  }

  void when_the_time_becomes(uint8_t hours, uint8_t minutes)
  {
    clock_time_t time = {
      .hours = hours,
      .minutes = minutes,
      .seconds = 0
    };

    tiny_key_value_store_write(kvs, key_current_time, &time);
  }

  void the_wake_time_should_be(uint8_t hours, uint8_t minutes)
  {
    clock_time_t expected = {
      .hours = hours,
      .minutes = minutes,
      .seconds = 0
    };
    clock_time_t actual;

    tiny_key_value_store_read(kvs, key_wake_time, &actual);

    MEMCMP_EQUAL(&expected, &actual, sizeof(expected));
  }

  void given_that_the_time_is(uint8_t hours, uint8_t minutes)
  {
    when_the_time_becomes(hours, minutes);
  }

  void the_requested_led_state_should_be(const led_state_t* expected)
  {
    led_state_t actual;
    tiny_key_value_store_read(kvs, key_led_requested_state, &actual);
    CHECK_EQUAL(expected->red, actual.red);
    CHECK_EQUAL(expected->blue, actual.blue);
    CHECK_EQUAL(expected->green, actual.green);
    CHECK_EQUAL(expected->brightness, actual.brightness);
  }

  void after_the_button_is_pressed()
  {
    event_signal_t signal;
    tiny_key_value_store_read(kvs, key_button_press_signal, &signal);
    signal++;
    tiny_key_value_store_write(kvs, key_button_press_signal, &signal);
  }

  void given_that_the_night_light_color_is(const led_state_t* state)
  {
    tiny_key_value_store_write(kvs, key_night_light_color, state);
  }

  void given_that_the_wake_light_color_is(const led_state_t* state)
  {
    tiny_key_value_store_write(kvs, key_wake_light_color, state);
  }

  void given_that_the_wake_time_is(uint8_t hours, uint8_t minutes)
  {
    clock_time_t time = { hours, minutes, 0 };
    tiny_key_value_store_write(kvs, key_wake_time, &time);
  }
};

TEST(wake_light_plugin, should_turn_on_night_light_at_8_pm)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(20, 0);
  the_requested_led_state_should_be(&night);
}

TEST(wake_light_plugin, should_turn_on_night_light_between_8_pm_and_629_am)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(22, 0);
  the_requested_led_state_should_be(&night);

  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(3, 0);
  the_requested_led_state_should_be(&night);
}

TEST(wake_light_plugin, should_turn_on_night_light_up_until_629_am)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(6, 29);
  the_requested_led_state_should_be(&night);
}

TEST(wake_light_plugin, should_turn_on_wake_light_at_630_am)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(6, 30);
  the_requested_led_state_should_be(&wake);
}

TEST(wake_light_plugin, should_turn_on_wake_light_between_630_am_and_8_am)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(7, 15);
  the_requested_led_state_should_be(&wake);
}

TEST(wake_light_plugin, should_turn_on_wake_light_up_until_830_am)
{
  given_that_the_requested_led_state_is(&off);
  when_the_time_becomes(8, 30);
  the_requested_led_state_should_be(&wake);
}

TEST(wake_light_plugin, should_turn_off_light_at_831_am)
{
  given_that_the_requested_led_state_is(&wake);
  when_the_time_becomes(8, 31);
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_turn_off_light_between_801_am_and_759_pm)
{
  given_that_the_requested_led_state_is(&wake);
  when_the_time_becomes(11, 0);
  the_requested_led_state_should_be(&off);

  given_that_the_requested_led_state_is(&wake);
  when_the_time_becomes(16, 0);
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_turn_off_light_at_559_pm)
{
  given_that_the_requested_led_state_is(&wake);
  when_the_time_becomes(17, 59);
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_start_a_nap_sequence_when_the_button_is_pressed)
{
  given_that_the_time_is(12, 30);

  after_the_button_is_pressed();
  the_requested_led_state_should_be(&night);

  when_the_time_becomes(14, 59);
  the_requested_led_state_should_be(&night);

  when_the_time_becomes(15, 0);
  the_requested_led_state_should_be(&wake);

  when_the_time_becomes(17, 29);
  the_requested_led_state_should_be(&wake);

  when_the_time_becomes(17, 30);
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_cancel_a_nap_sequence_when_the_button_is_pressed_during_a_nap)
{
  given_that_the_time_is(13, 30);

  after_the_button_is_pressed();
  the_requested_led_state_should_be(&night);

  after_the_button_is_pressed();
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_not_carry_nap_into_the_next_day)
{
  given_that_the_time_is(12, 30);

  after_the_button_is_pressed();
  the_requested_led_state_should_be(&night);

  when_the_time_becomes(17, 30);
  the_requested_led_state_should_be(&off);

  when_the_time_becomes(12, 30);
  the_requested_led_state_should_be(&off);
}

TEST(wake_light_plugin, should_use_default_value_for_night_light_when_it_is_empty)
{
  given_that_the_night_light_color_is(&off);

  when_the_time_becomes(6, 29);
  the_requested_led_state_should_be(&default_night);
}

TEST(wake_light_plugin, should_use_default_value_for_wake_light_when_it_is_empty)
{
  given_that_the_wake_light_color_is(&off);

  when_the_time_becomes(6, 30);
  the_requested_led_state_should_be(&default_wake);
}

TEST(wake_light_plugin, should_set_a_default_wake_time_on_init)
{
  given_that_the_wake_time_is(6, 30);
  after_the_plugin_is_initialized();
  the_wake_time_should_be(7, 0);
}
