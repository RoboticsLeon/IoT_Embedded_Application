/**
 * \file
 *         Ejercicio 2: Medida de la temperatura interna + Interruptor
 * \author
 *         Sergio Leon <schoolion01@gmail.com>
 */

#include "button-hal.h"
#include "contiki.h"
#include "lib/sensors.h"
#include "temperature-sensor.h"

#include <stdint.h>
#include <stdio.h>

#define PROCESS_EVENT_REFRESH                                                  \
  0 // Custom user event identifier to awake a process in order to update a
    // measurement

enum switch_status { OPEN = 1, CLOSE = 2 };
static enum switch_status virtual_switch_state;
struct temperature_t {
  uint8_t temperatureDec;
  uint8_t temperatureInt;
};
static struct temperature_t temperature_C;
static struct temperature_t temperature_F;
/*---------------------------------------------------------------------------*/
PROCESS(temperature_sensor_reading,
        "Sensor reading process"); // Process to update temperature sensor
                                   // measurement
PROCESS(button_status_reading,
        "Sensor reading process"); // Process to receive hardware interruptions
                                   // from physical button
PROCESS(serial_port_sender,
        "Sensor reading process"); // Process to send data to mqtt_exporter via
                                   // serial port msgs
AUTOSTART_PROCESSES(&temperature_sensor_reading, &button_status_reading,
                    &serial_port_sender);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(temperature_sensor_reading, ev, data) {
  uint16_t temperature_register_value;

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

    /* Wait to receive an event in order to read again */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_REFRESH);
  }

  PROCESS_END();
}

PROCESS_THREAD(button_status_reading, ev, data) {
  static button_hal_button_t *button_p;
  PROCESS_BEGIN();

  button_p = button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO);

  if (button_hal_get_state(button_p)) {
    virtual_switch_state = CLOSE;
  } else {
    virtual_switch_state = OPEN;
  }

  while (1) {
    /* Wait to receive a hardware interruption event caused by button  */
    PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_press_event);
    virtual_switch_state = (~virtual_switch_state & 0x3);
  }

  PROCESS_END();
}

PROCESS_THREAD(serial_port_sender, ev, data) {
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 2 seconds. */
  etimer_set(&timer, CLOCK_SECOND * 2);

  while (1) {
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    process_post_synch(&temperature_sensor_reading, PROCESS_EVENT_REFRESH,
                       NULL);

    printf("%.2d.%.2d;%.2d.%.2d;%d\n", temperature_C.temperatureInt,
           temperature_C.temperatureDec, temperature_F.temperatureInt,
           temperature_F.temperatureDec, virtual_switch_state);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
