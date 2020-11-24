/*!
 * @file
 * @brief
 */

#include <stdbool.h>
#include <stddef.h>
#include "stm8s.h"
#include "clock.h"
#include "tim4_system_tick.h"
#include "tiny_timer.h"
#include "watchdog.h"
#include "application.h"

static tiny_timer_group_t timer_group;
static tiny_timer_t timer;

static void kick_watchdog(tiny_timer_group_t* timer_group, void* context)
{
  (void)context;
  watchdog_kick();
  tiny_timer_start(timer_group, &timer, 1, kick_watchdog, NULL);
}

void main(void)
{
  disableInterrupts();
  {
    watchdog_init();
    clock_init();
    tiny_timer_group_init(&timer_group, tim4_system_tick_init());
  }
  enableInterrupts();

  kick_watchdog(&timer_group, NULL);

  application_init(&timer_group);

  while(true) {
    tiny_timer_group_run(&timer_group);
    wfi();
  }
}
