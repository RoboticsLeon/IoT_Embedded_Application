/**
 * \file
 *         Ejercicio 1: Medida de la temperatura interna
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
  uint16_t temperature_register_value;
  struct temperature_t {
    uint8_t temperatureDec;
    uint8_t temperatureInt;
  };
  struct temperature_t temperature_C;
  struct temperature_t temperature_F;

  PROCESS_BEGIN();

  while (1) {
    /* Read temperature value from sensor*/
    SENSORS_ACTIVATE(temperature_sensor);
    temperature_register_value = temperature_sensor.value(0);
    SENSORS_DEACTIVATE(temperature_sensor);

    /* Print read value */
    temperature_C.temperatureInt = (temperature_register_value >> 2);
    temperature_C.temperatureDec = (temperature_register_value & 0x3) * 25U;
    temperature_F.temperatureInt = (2U * temperature_C.temperatureInt + 32U);
    temperature_F.temperatureDec = (2U * temperature_C.temperatureDec);
    if (temperature_F.temperatureDec >= 100U) {
      temperature_F.temperatureDec -= 100U;
      temperature_F.temperatureInt++;
    }

    printf("%.2d.%.2d\n", temperature_F.temperatureInt,
           temperature_F.temperatureDec);

    /* Wait to receive an event in order to read again */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_AWAKE);
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

    process_post_synch(&sensor_reading, PROCESS_EVENT_AWAKE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
