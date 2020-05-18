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
  time->hours = read_register(register_hours);
  time->minutes = read_register(register_minutes);
  time->seconds = read_register(register_seconds);
}

void ds1307_set(const clock_time_t* time) {
  write_register(register_hours, time->hours);
  write_register(register_minutes, time->minutes);
  write_register(register_seconds, time->seconds);
}
