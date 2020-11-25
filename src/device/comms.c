/*!
 * @file
 * @brief
 */

#include <string.h>
#include <ctype.h>
#include "data_model_key_value_pairs.h"
#include "comms.h"
#include "tiny_utils.h"

enum {
  signal_byte_received = tiny_fsm_signal_user_start
};

#define starts_with(s, prefix) (0 == strncmp((prefix), (s), strlen(prefix)))

static const char* read_number(const char* s, uint8_t* i)
{
  *i = 0;

  while(isdigit(s[0])) {
    *i *= 10;
    *i += s[0] - '0';
    s++;
  }

  return s;
}

static void handle_message(comms_t* self, const char* command)
{
  if(starts_with(command, "button_press")) {
    event_signal_t signal;
    tiny_key_value_store_read(self->key_value_store, key_button_press_signal, &signal);
    signal++;
    tiny_key_value_store_write(self->key_value_store, key_button_press_signal, &signal);
  }
  else if(starts_with(command, "wake_time")) {
    uint8_t hours;
    uint8_t minutes;

    command += strlen("wake_time(");
    command = read_number(command, &hours);
    command += strlen(",");
    command = read_number(command, &minutes);

    clock_time_t time = { hours, minutes, 0 };
    tiny_key_value_store_write(self->key_value_store, key_wake_time, &time);
  }
  else if(starts_with(command, "night_light_color")) {
    uint8_t r, g, b;

    command += strlen("night_light_color(");
    command = read_number(command, &r);
    command += strlen(",");
    command = read_number(command, &g);
    command += strlen(",");
    command = read_number(command, &b);

    led_state_t state = { r, g, b, 1 };
    tiny_key_value_store_write(self->key_value_store, key_night_light_color, &state);
  }
  else if(starts_with(command, "wake_light_color")) {
    uint8_t r, g, b;

    command += strlen("wake_light_color(");
    command = read_number(command, &r);
    command += strlen(",");
    command = read_number(command, &g);
    command += strlen(",");
    command = read_number(command, &b);

    led_state_t state = { r, g, b, 1 };
    tiny_key_value_store_write(self->key_value_store, key_wake_light_color, &state);
  }
}

static void state_wait_for_start(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data);
static void state_wait_for_end(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data);

static void state_wait_for_start(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data)
{
  reinterpret(byte, data, const uint8_t*);

  switch(signal) {
    case signal_byte_received:
      if(*byte == '@') {
        tiny_fsm_transition(fsm, state_wait_for_end);
      }
  }
}

static void state_wait_for_end(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data)
{
  comms_t* self = container_of(comms_t, fsm, fsm);
  reinterpret(byte, data, const uint8_t*);

  switch(signal) {
    case tiny_fsm_signal_entry:
      self->buffer_offset = 0;
      break;

    case signal_byte_received:
      if(*byte == '@') {
        tiny_fsm_transition(fsm, state_wait_for_end);
      }
      else if(*byte == '\n') {
        handle_message(self, (const char*)self->buffer);
        tiny_fsm_transition(fsm, state_wait_for_start);
      }
      else {
        self->buffer[self->buffer_offset++] = *byte;

        if(self->buffer_offset >= sizeof(self->buffer)) {
          tiny_fsm_transition(fsm, state_wait_for_start);
        }
      }
  }
}

static void byte_received(void* context, const void* args)
{
  reinterpret(self, context, comms_t*);
  (void)context;

  tiny_fsm_send_signal(&self->fsm, signal_byte_received, args);
}

void comms_init(
  comms_t* self,
  i_tiny_uart_t* uart,
  i_tiny_key_value_store_t* key_value_store)
{
  self->key_value_store = key_value_store;

  tiny_event_subscription_init(&self->byte_received_subscription, self, byte_received);
  tiny_event_subscribe(tiny_uart_on_receive(uart), &self->byte_received_subscription);

  tiny_fsm_init(&self->fsm, state_wait_for_start);
}
