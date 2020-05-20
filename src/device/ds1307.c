/*!
 * @file
 * @brief
 */

#include "ds1307.h"

enum {
  address = 0x68,

  register_seconds = 0,
  register_minutes = 1,
  register_hours = 2
};

static i_tiny_i2c_t* i2c;

static uint8_t read_register(uint8_t register_address) {
  while(true) {
    if(!tiny_i2c_write(i2c, address, true, &register_address, sizeof(register_address))) continue;
    uint8_t byte;
    if(!tiny_i2c_read(i2c, address, false, &byte, sizeof(byte))) continue;
    return byte;
  }
}

static void write_register(uint8_t register_address, uint8_t value) {
  uint8_t buffer[] = { register_address, value };
  while(!tiny_i2c_write(i2c, address, false, buffer, sizeof(buffer))) {
  }
}

void ds1307_init(i_tiny_i2c_t* _i2c) {
  i2c = _i2c;
}

void ds1307_get(clock_time_t* time) {
  const uint8_t hours = read_register(register_hours);
  const uint8_t minutes = read_register(register_minutes);
  const uint8_t seconds = read_register(register_seconds);

  time->hours = (hours & 0x0F) + ((hours & 0x30) >> 4) * 10;
  time->minutes = (minutes & 0x0F) + ((minutes & 0x70) >> 4) * 10;
  time->seconds = (seconds & 0x0F) + ((seconds & 0x70) >> 4) * 10;
}

void ds1307_set(const clock_time_t* time) {
  const uint8_t hours = ((time->hours / 10) << 4) + (time->hours % 10);
  const uint8_t minutes = ((time->minutes / 10) << 4) + (time->minutes % 10);
  const uint8_t seconds = ((time->seconds / 10) << 4) + (time->seconds % 10);

  write_register(register_hours, hours);
  write_register(register_minutes, minutes);
  write_register(register_seconds, seconds);
}
