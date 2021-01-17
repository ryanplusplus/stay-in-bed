/*!
 * @file
 * @brief
 */

#ifndef comms_h
#define comms_h

#include "hal/i_tiny_uart.h"
#include "i_tiny_key_value_store.h"
#include "tiny_timer.h"
#include "tiny_fsm.h"

typedef struct
{
  tiny_event_subscription_t byte_received_subscription;
  i_tiny_key_value_store_t* key_value_store;
  tiny_timer_t timer;
  tiny_fsm_t fsm;
  uint8_t buffer[50];
  uint8_t buffer_offset;
  bool message_ready;
} comms_t;

void comms_init(
  comms_t* self,
  i_tiny_uart_t* uart,
  i_tiny_key_value_store_t* key_value_store,
  tiny_timer_group_t* timer_group);

#endif
