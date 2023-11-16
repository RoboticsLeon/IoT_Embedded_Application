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
PROCESS_THREAD(sensor_reading, ev, data)
{
  static float temperature_value;
  static int ipart_temperature_value;
  static int fpart_temperature_value;

  PROCESS_BEGIN();

  while(1) {
    /* Read temperature value from sensor*/
    SENSORS_ACTIVATE(temperature_sensor);
    temperature_value = (((float)temperature_sensor.value(0)*25.0F)/100.0F);
    SENSORS_DEACTIVATE(temperature_sensor);

    /* Print read value */
    ipart_temperature_value = (int)temperature_value;
    fpart_temperature_value = ((temperature_value*100) - (ipart_temperature_value*100));
    printf("Temperature: %d.%d ºC \n", ipart_temperature_value, fpart_temperature_value);

    /* Wait to receive an event in order to read again */
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}

PROCESS_THREAD(timer_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 3 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 3);

  while(1) {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    process_post_synch(&sensor_reading, PROCESS_EVENT_CONTINUE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
