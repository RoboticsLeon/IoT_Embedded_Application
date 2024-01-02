/**
 * \file
 *         Ejercicio 4: Parpadeo LEDs
 * \author
 *         Sergio Leon <schoolion01@gmail.com>
 */

#include "contiki.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(parpadeo_1_process, "Blinking led 1 process");
PROCESS(parpadeo_2_process, "Blinking led 2 process");
PROCESS(timer_process, "Periodic process");
AUTOSTART_PROCESSES(&parpadeo_1_process, &parpadeo_2_process, &timer_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(parpadeo_1_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Wait to receive an event from timer_process in order to continue */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);

  /* Setup a periodic timer that expires after 2 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 2);

  while (1) {
    leds_single_toggle(LEDS_LED1);
    etimer_reset(&timer);

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  }

  PROCESS_END();
}

PROCESS_THREAD(parpadeo_2_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Wait to receive an event from timer_process in order to continue */
  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);

  /* Setup a periodic timer that expires after 3 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 3);

  while (1) {
    leds_single_toggle(LEDS_LED2);

    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }

  PROCESS_END();
}

PROCESS_THREAD(timer_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a timer that expires after 5 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 5);

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  process_poll(&parpadeo_1_process);
  process_poll(&parpadeo_2_process);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
