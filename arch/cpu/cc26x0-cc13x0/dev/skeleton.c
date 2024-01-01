#include "adc-sensor.h"

config(ADC_SENSOR_SET_CHANNEL, ADC_COMPB_IN_AUXIO0);
config(SENSORS_ACTIVE, 1);

int channel_state = status(SENSORS_ACTIVE);
printf("channel state is: %d\n", channel_state);
int adj_mv = value(ADC_SENSOR_VALUE);
printf("channel readed value in mv: %d", adj_mv);