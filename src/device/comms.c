/*!
 * @file
 * @brief
 */

#include "comms.h"
#include "tiny_utils.h"

static void state_wait_for_start(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data)
{
  (void)fsm;
  (void)signal;
  (void)data;
}

static void state_wait_for_end(tiny_fsm_t* fsm, tiny_fsm_signal_t signal, const void* data)
{
  (void)fsm;
  (void)signal;
  (void)data;
}

static void byte_received(void* context, const void* _args)
{
  reinterpret(args, _args, const tiny_uart_on_receive_args_t*);
  (void)context;
  (void)args;
  (void)state_wait_for_end;
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
