/**
 * \file
 *         Ejercicio 5: Medida de la temperatura interna
 * \author
 *         Sergio Leon <schoolion01@gmail.com>
 */

#include "contiki.h"
#include "lib/sensors.h"
#include "temperature-sensor.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(sensor_reading, "Sensor reading process");
PROCESS(timer_process, "Timer process");
AUTOSTART_PROCESSES(&sensor_reading, &timer_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_reading, ev, data) {
  static int16_t temperature_register_value;
  struct temperature_t {
    uint16_t temperatureDec;
    uint16_t temperatureInt;
  };
  static struct temperature_t temperature_C;
  static struct temperature_t temperature_F;

  PROCESS_BEGIN();

  while (1) {
    /* Read temperature value from sensor*/
    SENSORS_ACTIVATE(temperature_sensor);
    temperature_register_value = temperature_sensor.value(0);
    SENSORS_DEACTIVATE(temperature_sensor);

    /* Print read value */
    temperature_C.temperatureInt = (temperature_register_value & 0x3) * 25U;
    temperature_C.temperatureDec = (temperature_register_value >> 2);
    temperature_F.temperatureInt = (2 * temperature_C.temperatureInt + 32);
    temperature_F.temperatureDec = temperature_C.temperatureDec;
    printf("%.2d.%.2d\n", temperature_F.temperatureInt,
           temperature_F.temperatureDec);

    /* Wait to receive an event in order to read again */
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}

PROCESS_THREAD(timer_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 2 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 2);

  while (1) {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    process_post_synch(&sensor_reading, PROCESS_EVENT_CONTINUE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
