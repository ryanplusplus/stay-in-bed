/*!
 * @file
 * @brief
 */

extern "C" {
#include "data_model_key_value_pairs.h"
#include "data_model.h"
#include "led_animation_plugin.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "tiny_time_source_double.h"

TEST_GROUP(led_animation_plugin)
{
  enum {
    step = 50
  };

  tiny_timer_group_t timer_group;
  tiny_time_source_double_t time_source;
  i_tiny_key_value_store_t* kvs;

  void setup()
  {
    tiny_time_source_double_init(&time_source);
    tiny_timer_group_init(&timer_group, &time_source.interface);

    data_model_init();
    kvs = data_model_key_value_store();

    led_animation_plugin_init(&timer_group, kvs);
  }

  void after(tiny_time_source_ticks_t ticks)
  {
    tiny_time_source_double_tick(&time_source, ticks);
    tiny_timer_group_run(&timer_group);
  }

  void after_a_step()
  {
    after(step);
  }

  void given_that_the_requested_state_is(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t state = { red, green, blue, brightness };
    tiny_key_value_store_write(kvs, key_led_requested_state, &state);
  }

  void given_that_the_actual_state_is(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t state = { red, green, blue, brightness };
    tiny_key_value_store_write(kvs, key_led_actual_state, &state);
  }

  void the_actual_state_should_be(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t expected = { red, green, blue, brightness };
    led_state_t actual;
    tiny_key_value_store_read(kvs, key_led_actual_state, &actual);

    CHECK_EQUAL(expected.red, actual.red);
    CHECK_EQUAL(expected.green, actual.green);
    CHECK_EQUAL(expected.blue, actual.blue);
    CHECK_EQUAL(expected.brightness, actual.brightness);
  }
};

TEST(led_animation_plugin, should_update_proportionately)
{
  given_that_the_requested_state_is(100, 10, 50, 200);
  given_that_the_actual_state_is(0, 0, 0, 0);

  after_a_step();
  the_actual_state_should_be(10, 1, 5, 20);

  after_a_step();
  the_actual_state_should_be(19, 2, 9, 38);
}

TEST(led_animation_plugin, should_update_by_at_least_one)
{
  given_that_the_requested_state_is(10, 0, 0, 200);
  given_that_the_actual_state_is(9, 0, 0, 200);

  after_a_step();
  the_actual_state_should_be(10, 0, 0, 200);
}
