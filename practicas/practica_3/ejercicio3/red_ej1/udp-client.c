#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "random.h"
#include "sys/log.h"

#include <stdint.h>

#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL (60 * CLOCK_SECOND)

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

  LOG_INFO("Temperatura en Fahrenheit calculada por el servidor= '%d' grados.",
           *data);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data) {
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;

  typedef uint8_t CelsiusDegrees_t;
  const CelsiusDegrees_t INTERNAL_TEMPERATURE = 38U;
  const CelsiusDegrees_t EXTERNAL_TEMPERATURE = 35U;
  enum informationType { internal_temperature, external_temperature };
  static enum informationType informationSelector = external_temperature;
  CelsiusDegrees_t temperatureToSend;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT,
                      udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if (NETSTACK_ROUTING.node_is_reachable() &&
        NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      if (informationSelector == external_temperature) {
        LOG_INFO("Enviando temperatura externa en Celsius = %d\n",
                 EXTERNAL_TEMPERATURE);
        temperatureToSend = EXTERNAL_TEMPERATURE;
        informationSelector = internal_temperature;
      } else {
        LOG_INFO("Enviando temperatura interna en Celsius = %d\n",
                 INTERNAL_TEMPERATURE);
        temperatureToSend = INTERNAL_TEMPERATURE;
        informationSelector = external_temperature;
      }
      simple_udp_sendto(&udp_conn, &temperatureToSend,
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
