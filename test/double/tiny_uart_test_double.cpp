/*!
 * @file
 * @brief
 */

#include <cstddef>
#include "tiny_uart_test_double.h"

extern "C" {
#include "tiny_utils.h"
}

static void send(i_tiny_uart_t* self, uint8_t byte)
{
  (void)self;
  (void)byte;
}

static i_tiny_event_t* on_send_complete(i_tiny_uart_t* self)
{
  (void)self;
  return NULL;
}

static i_tiny_event_t* on_receive(i_tiny_uart_t* _self)
{
  reinterpret(self, _self, tiny_uart_test_double_t*);
  return &self->receive_event.interface;
}

static const i_tiny_uart_api_t api = { send, on_send_complete, on_receive };

void tiny_uart_test_double_init(tiny_uart_test_double_t* self)
{
  self->interface.api = &api;
  tiny_event_init(&self->receive_event);
}

void tiny_uart_test_double_trigger_receive(tiny_uart_test_double_t* self, uint8_t byte)
{
  tiny_uart_on_receive_args_t args = { byte };
  tiny_event_publish(&self->receive_event, &args);
}
