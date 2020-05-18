/*!
 * @file
 * @brief
 */

#include <stddef.h>
#include "apa102.h"

enum {
  led_count = 60
};

static i_tiny_spi_t* spi;

static const uint8_t start_frame[] = { 0, 0, 0, 0 };
static const uint8_t end_frame[] = { 0xFF, 0xFF, 0xFF, 0xFF };

static const led_state_t all_off = { 0 };

void apa102_init(i_tiny_spi_t* _spi) {
  spi = _spi;
  apa102_write(&all_off);
}

void apa102_write(const led_state_t* state) {
  uint8_t brightness = state->brightness;

  if((state->brightness > 0) && (state->brightness < (1 << 4))) {
    brightness = (1 << 4);
  }

  uint8_t data_frame[] = {
    (brightness >> 3) | 0xE0,
    state->blue,
    state->green,
    state->red
  };

  tiny_spi_transfer(spi, start_frame, NULL, sizeof(start_frame));

  for(uint8_t i = 0; i < led_count; i++) {
    tiny_spi_transfer(spi, data_frame, NULL, sizeof(data_frame));
  }

  tiny_spi_transfer(spi, start_frame, NULL, sizeof(end_frame));
}
