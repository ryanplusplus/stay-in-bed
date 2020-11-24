/*!
 * @file
 * @brief
 */

#ifndef tiny_uart_test_double_h
#define tiny_uart_test_double_h

extern "C" {
#include "hal/i_tiny_uart.h"
#include "tiny_event.h"
}

typedef struct {
  i_tiny_uart_t interface;
  tiny_event_t receive_event;
} tiny_uart_test_double_t;

void tiny_uart_test_double_init(tiny_uart_test_double_t* self);
void tiny_uart_test_double_trigger_receive(tiny_uart_test_double_t* self, uint8_t byte);

#endif
