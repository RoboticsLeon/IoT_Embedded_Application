#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/log.h"

#include "adc-sensor.h"

#define LOG_MODULE "Launchpad-client"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define ADC_LOWER_LIMIT 2100
#define ADC_L1_LIMIT 2450
#define ADC_L2_LIMIT 2580
#define ML_SATURATION 90
#define SENSOR_ERROR -1

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen)
{
  LOG_INFO("Server response status code: '%.*s' received from ", datalen, (char *)data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
}

int water_measure(void)
{
  int adc_value, ml_value;
  // Experimental Relation
  float lineal_coef_1[2] = {0.0508, -104.1384};
  float lineal_coef_2[2] = {0.1512, -350.9302};
  float lineal_coef_3[2] = {0.3466, -857.4212};
  // Water sensor configuration (ADC)
  adc_sensor.configure(ADC_SENSOR_SET_CHANNEL, ADC_COMPB_IN_AUXIO0);
  adc_sensor.configure(SENSORS_ACTIVE, 1);

  if (adc_sensor.status(SENSORS_ACTIVE) == 0)
  {
    return SENSOR_ERROR;
  }

  else
  {
    // Read ADC water sensor
    adc_value = adc_sensor.value(ADC_SENSOR_VALUE);
    LOG_INFO("ADC value: %d\n", adc_value);
    // Convertion to ml (Using experimental relation)
    if (adc_value > ADC_LOWER_LIMIT)
    {
      if (adc_value < ADC_L1_LIMIT)
        ml_value = (int)(lineal_coef_1[0] * adc_value + lineal_coef_1[1]);
      else if (adc_value < ADC_L2_LIMIT)
        ml_value = (int)(lineal_coef_2[0] * adc_value + lineal_coef_2[1]);
      else
        ml_value = (int)(lineal_coef_3[0] * adc_value + lineal_coef_3[1]);

      if (ml_value > ML_SATURATION)
        ml_value = ML_SATURATION;
    }
    else
    {
      ml_value = 0;
    }

    return ml_value;
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  uip_ipaddr_t dest_ipaddr;
  int ml_value;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, 1 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if (NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr))
    {
      ml_value = water_measure();
      if (ml_value == SENSOR_ERROR)
      {
        LOG_INFO("Error: Water sensor not found\n");
        break;
      }
      LOG_INFO("Water measure (ml): %d\n", ml_value);
      simple_udp_sendto(&udp_conn, &ml_value, sizeof(ml_value), &dest_ipaddr);
    }
    else
    {
      LOG_INFO("Not reachable yet\n");
    }

    etimer_set(&periodic_timer, 10 * CLOCK_SECOND);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
