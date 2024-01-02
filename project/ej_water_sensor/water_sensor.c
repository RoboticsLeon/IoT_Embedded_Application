#include "contiki.h"
#include "adc-sensor.h"
#include "aux_adc.h"
#include <stdio.h>

PROCESS(sensor_reading_process, "Sensor Reading Process");
AUTOSTART_PROCESSES(&sensor_reading_process);

PROCESS_THREAD(sensor_reading_process, ev, data)
{
    int adc_value, ml_value;
    float lineal_coef_1[2] = {0.0508, -104.1384};
    float lineal_coef_2[2] = {0.1512, -350.9302};
    float lineal_coef_3[2] = {0.3466, -857.4212};

    PROCESS_BEGIN();

    adc_sensor.configure(ADC_SENSOR_SET_CHANNEL, ADC_COMPB_IN_AUXIO0);
    adc_sensor.configure(SENSORS_ACTIVE, 1);
    // int channel_state = adc_sensor.status(SENSORS_ACTIVE);

    while (1)
    {
        // Lectura ADC water sensor
        adc_value = adc_sensor.value(ADC_SENSOR_VALUE);
        printf("ADC_value: %d\n", adc_value);

        // Conversion a ml (aproximacion relacion por polinomio 2 orden o lineal)
        if (adc_value > 2000)
        {
            if (adc_value < 2450)
                ml_value = (int)(lineal_coef_1[0] * adc_value + lineal_coef_1[1]);
            else if(adc_value < 2580)
                ml_value = (int)(lineal_coef_2[0] * adc_value + lineal_coef_2[1]);
            else
                ml_value = (int)(lineal_coef_3[0] * adc_value + lineal_coef_3[1]);

            if(ml_value > 90)
                ml_value = 90;
        }
        else
            ml_value = 0;

        printf("ML_value: %d\n", ml_value);

        // Esperar un tiempo antes de tomar otra lectura
        static struct etimer timer;
        etimer_set(&timer, 5 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    }

    PROCESS_END();
}
