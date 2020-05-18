/*!
 * @file
 * @brief
 */

#ifndef led_state_h
#define led_state_h

#include <stdint.h>

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t brightness;
} led_state_t;

#endif
