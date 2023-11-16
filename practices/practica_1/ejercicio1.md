# Ejercicio 1: Búsqueda de ficheros de la plataforma nRF52840

Las siguientes respuestas se han basado en la lectura de la documentación disponible por parte de Contiki-NG (https://docs.contiki-ng.org/en/master/_api/group__arch.html).

+ Microcontrolador:

    Como se indica en el readme.md de arch/cpu/nrf52840/lib/nrf52-sdk/modules/nrfx/README.md, los drivers del SoC han sido extraídos del SDK nRF5 desarrollado por Nordic.
    ![Drivers doc](md_images/ejercicio1/image.png)
    
    + arch/cpu/nrf52840/lib/nrf52-sdk/modules/nrfx/drivers/include/*
    + arch/cpu/nrf52840/lib/nrf52-sdk/modules/nrfx/drivers/src/*


+ Transceiver radio:
    



+ Almacenamiento externo:
    Driver genérico para memorias flash SPI externas
    + arch/dev/storage/ext-flash/ext-flash.c
    + arch/dev/storage/ext-flash/ext-flash.h

+ Almacenamiento interno:

