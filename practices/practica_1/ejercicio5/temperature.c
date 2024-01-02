/**
 * \file
 *         Ejercicio 5: Medida de la temperatura interna
 * \author
 *         Sergio Leon <schoolion01@gmail.com>
 */

#include "contiki.h"
#include "lib/sensors.h"
#include "temperature-sensor.h"

#include <stdint.h>
#include <stdio.h>

#define PROCESS_EVENT_AWAKE 0 // Custom user defined event identifier
/*---------------------------------------------------------------------------*/
PROCESS(sensor_reading, "Sensor reading process");
PROCESS(timer_process, "Timer process");
AUTOSTART_PROCESSES(&sensor_reading, &timer_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_reading, ev, data) {
  uint16_t sensor_register_value;
  struct temperature_t {
    uint8_t temperatureInt;
    uint8_t temperatureDec;
  };
  struct temperature_t temperature;

  PROCESS_BEGIN();

  while (1) {
    /* Wait to receive an event in order to read again */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_AWAKE);

    /* Read temperature value from sensor*/
    SENSORS_ACTIVATE(temperature_sensor);
    sensor_register_value = temperature_sensor.value(0);
    SENSORS_DEACTIVATE(temperature_sensor);

    /* Print read value */
    temperature.temperatureInt = (sensor_register_value >> 2);
    temperature.temperatureDec = ((sensor_register_value & 0x3) * 25U);
    printf("Temperature: %.2d.%.2d ºC \n", temperature.temperatureInt,
           temperature.temperatureDec);
  }

  PROCESS_END();
}

PROCESS_THREAD(timer_process, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 3 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 3);

  while (1) {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    process_post_synch(&sensor_reading, PROCESS_EVENT_AWAKE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
