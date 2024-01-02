#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "random.h"
#include "sys/log.h"
#include "temperature-sensor.h"

#include <stdint.h>

#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL (1 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port, const uint8_t *data,
                            uint16_t datalen) {
  typedef float FahrenheitDegrees_t;
  uint8_t temperatureInt;
  uint8_t temperatureDec;

  FahrenheitDegrees_t receivedTemperature = *((float *)data);
  temperatureInt = (uint8_t)receivedTemperature;
  uint8_t temperatureDec =
      ((receivedTemperature - (float)temperatureInt) * 100U);
  LOG_INFO("Temperatura en Fahrenheit calculada por el servidor= '%.2d.%.2d' "
           "grados.",
           temperatureInt, temperatureDec);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data) {
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;

  typedef float CelsiusDegrees_t;
  CelsiusDegrees_t temperatureToSend;
  uint16_t sensorRegisterValue;
  uint8_t temperatureInt;
  uint8_t temperatureDec;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT,
                      udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if (NETSTACK_ROUTING.node_is_reachable() &&
        NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      SENSORS_ACTIVATE(temperature_sensor);
      sensorRegisterValue = temperature_sensor.value(0);
      SENSORS_DEACTIVATE(temperature_sensor);
      temperatureInt = (sensorRegisterValue >> 2);
      temperatureDec = ((sensorRegisterValue & 0x3) * 25U);
      LOG_INFO("Enviando temperatura interna en Celsius = %.2d.%.2d\n",
               temperatureInt, temperatureDec);
      temperatureToSend = ((CelsiusDegrees_t)temperatureInt +
                           ((CelsiusDegrees_t)temperatureDec / 100.0F));
      simple_udp_sendto(&udp_conn, (void *)&temperatureToSend,
                        sizeof(temperatureToSend), &dest_ipaddr);
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND +
                                    (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
