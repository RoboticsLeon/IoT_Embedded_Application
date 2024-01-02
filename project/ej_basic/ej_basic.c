#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-hal.h"
#include "button-sensor.h"
#include "board-peripherals.h"
#include "target-conf.h"

#include <stdio.h>
#include <stdint.h>

/*---------------------------------------------------------------------------*/

PROCESS(button_demo_process, "Button Demo Process");
AUTOSTART_PROCESSES(&button_demo_process);

PROCESS_THREAD(button_demo_process, ev, data)
{
    PROCESS_BEGIN();

    printf("Button Demo\n");

    while (1)
    {
        PROCESS_WAIT_EVENT();

        if (ev == button_hal_press_event)
        {
            button_hal_button_t *btn = (button_hal_button_t *)data;

            printf("%s pressed\n", BUTTON_HAL_GET_DESCRIPTION(btn));

            if (btn->unique_id == CC26XX_DEMO_TRIGGER_1)
            {
                printf("Button 1 pressed\n");
                leds_toggle(LEDS_RED); // Enciende/Apaga el LED rojo
            }
            else if (btn->unique_id == CC26XX_DEMO_TRIGGER_2)
            {
                printf("Button 2 pressed\n");
                leds_toggle(LEDS_GREEN); // Enciende/Apaga el LED verde
            }
        }
        else if (ev == button_hal_release_event)
        {
            button_hal_button_t *btn = (button_hal_button_t *)data;

            printf("%s released\n", BUTTON_HAL_GET_DESCRIPTION(btn));

            if (btn->unique_id == CC26XX_DEMO_TRIGGER_1 || btn->unique_id == CC26XX_DEMO_TRIGGER_2)
            {
                leds_off(LEDS_RED | LEDS_GREEN); // Apaga ambos LEDs al soltar cualquier botón
            }
        }
    }

    PROCESS_END();
}
