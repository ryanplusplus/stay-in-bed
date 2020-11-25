/*!
 * @file
 * @brief
 */

extern "C" {
#include "data_model_key_value_pairs.h"
#include "data_model.h"
#include "comms.h"
#include "tiny_utils.h"
}

#include <cstring>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "tiny_uart_test_double.h"

TEST_GROUP(comms)
{
  enum {
    step = 50
  };

  comms_t self;

  i_tiny_key_value_store_t* kvs;
  tiny_uart_test_double_t uart;
  tiny_event_subscription_t data_changed_subscription;

  void setup()
  {
    data_model_init();
    kvs = data_model_key_value_store();
    tiny_uart_test_double_init(&uart);

    tiny_event_subscription_init(&data_changed_subscription, NULL, data_changed);
    tiny_event_subscribe(tiny_key_value_store_on_change(kvs), &data_changed_subscription);

    comms_init(&self, &uart.interface, kvs);
  }

  static void data_changed(void* context, const void* _args)
  {
    reinterpret(args, _args, const tiny_key_value_store_on_change_args_t*);
    (void)context;

    if(args->key == key_button_press_signal) {
      mock().actualCall("button_press");
    }
  }

  void given_that_the_night_light_color_is(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t state = { red, green, blue, brightness };
    tiny_key_value_store_write(kvs, key_night_light_color, &state);
  }

  void given_that_the_wake_light_color_is(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t state = { red, green, blue, brightness };
    tiny_key_value_store_write(kvs, key_wake_light_color, &state);
  }

  void given_that_the_wake_time_is(uint8_t hours, uint8_t minutes, uint8_t seconds)
  {
    clock_time_t time = { hours, minutes, seconds };
    tiny_key_value_store_write(kvs, key_wake_time, &time);
  }

  void a_button_press_should_be_sent()
  {
    mock().expectOneCall("button_press");
  }

  void the_night_light_color_should_be(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t expected = { red, green, blue, brightness };
    led_state_t actual;
    tiny_key_value_store_read(kvs, key_night_light_color, &actual);

    CHECK_EQUAL(expected.red, actual.red);
    CHECK_EQUAL(expected.green, actual.green);
    CHECK_EQUAL(expected.blue, actual.blue);
    CHECK_EQUAL(expected.brightness, actual.brightness);
  }

  void the_wake_light_color_should_be(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
  {
    led_state_t expected = { red, green, blue, brightness };
    led_state_t actual;
    tiny_key_value_store_read(kvs, key_wake_light_color, &actual);

    CHECK_EQUAL(expected.red, actual.red);
    CHECK_EQUAL(expected.green, actual.green);
    CHECK_EQUAL(expected.blue, actual.blue);
    CHECK_EQUAL(expected.brightness, actual.brightness);
  }

  void the_wake_time_should_be(uint8_t hours, uint8_t minutes, uint8_t seconds)
  {
    clock_time_t expected = { hours, minutes, seconds };
    clock_time_t actual;
    tiny_key_value_store_read(kvs, key_wake_time, &actual);

    CHECK_EQUAL(expected.hours, actual.hours);
    CHECK_EQUAL(expected.minutes, actual.minutes);
    CHECK_EQUAL(expected.seconds, actual.seconds);
  }

  void after_string_is_received(const char* s)
  {
    for(uint8_t i = 0; i < strlen(s); i++) {
      tiny_uart_test_double_trigger_receive(&uart, s[i]);
    }
  }
};

TEST(comms, should_parse_button_press_messages)
{
  a_button_press_should_be_sent();
  after_string_is_received("@button_press()\n");
}

TEST(comms, should_parse_wake_time_messages)
{
  given_that_the_wake_time_is(1, 2, 3);
  after_string_is_received("@wake_time(13,5)\n");
  the_wake_time_should_be(13, 5, 0);
}

TEST(comms, should_parse_night_light_color_messages)
{
  given_that_the_night_light_color_is(0, 0, 0, 0);
  after_string_is_received("@night_light_color(123,7,45)\n");
  the_night_light_color_should_be(123, 7, 45, 1);
}

TEST(comms, should_parse_wake_light_color_messages)
{
  given_that_the_wake_light_color_is(0, 0, 0, 0);
  after_string_is_received("@wake_light_color(123,7,45)\n");
  the_wake_light_color_should_be(123, 7, 45, 1);
}

TEST(comms, should_parse_multiple_commands)
{
  a_button_press_should_be_sent();
  after_string_is_received("@button_press()\n@wake_time(1,2)\n");
  the_wake_time_should_be(1, 2, 0);
}

TEST(comms, should_not_double_parse_a_command)
{
  a_button_press_should_be_sent();
  after_string_is_received("@button_press()\n\n");
}

TEST(comms, should_recover_from_an_interrupted_message)
{
  a_button_press_should_be_sent();
  after_string_is_received("@button_@button_press()\n");
}

TEST(comms, should_not_go_out_of_bounds_when_receiving_a_long_command)
{
  after_string_is_received("@");

  for(uint16_t i = 0; i < 1000; i++) {
    after_string_is_received("1234567890");
  }
}
