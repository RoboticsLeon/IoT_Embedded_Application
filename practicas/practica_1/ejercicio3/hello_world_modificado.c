/**
 * \file
 *         Ejercicio 3: Hello World modificado
 * \author
 *         Sergio Leon <schoolion01@gmail.com>
 */

#include "contiki.h"

#include <stdint.h>
#include <stdio.h>

#define PROCESS_EVENT_AWAKE 0 // Custom user defined event identifier
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(periodic_process, "Periodic process");
AUTOSTART_PROCESSES(&hello_world_process, &periodic_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data) {
  static uint8_t event_counter = 0;

  PROCESS_BEGIN();

  while (1) {
    /* Wait to receive an event from another process */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_AWAKE);

    printf("Hello World! (number %d)\n", event_counter);

    if (event_counter < 20) {
      event_counter++;
    } else {
      event_counter = 0;
    }
  }

  PROCESS_END();
}

PROCESS_THREAD(periodic_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 5 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 5);

  while (1) {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
    process_post_synch(&hello_world_process, PROCESS_EVENT_AWAKE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
