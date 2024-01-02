CONEXION NRF - LAUNCHPAD - WATER SENSOR

Junto a la memoria de proyecto, se hace entrega del código fuente desarrollado.
Dentro de la carpeta launchpad-nrf-water adjuntada, se pueden ver los archivos y directorios:

+ launchpad/ : Contiene el código fuente udp-client-launchpad.c y project-conf.h, los cuales son compilados para generar la imagen binaria que será cargada en el +launchpad cc2650 para funcionar como lector del sensor de agua y cliente udp que envía la información recogida en mL, tras ser procesada.
+ mosquitto/ : Original facilitado en las prácticas
+ mqtt_exporter/ : Dentro del cuál el archivo main.py ha sido modificado para adaptarlo a nuestra aplicación.
+ nrf/ : Contiene el código fuente udp-server-nrf.c y project-conf.h, los cuales son compilados para generar la imagen que será cargada en el dongle +nrf52840 para funcionar como servidor UDP que recibe mensajes del cliente, y publica por MQTT esta información hacia Prometheus.
+ prometheus/ : Original facilitado en las prácticas
+ docker-compose.yml: Fichero para la configuración de los servicios de nuestra aplicación

A su vez, también se proporciona un vídeo demostrativo de la funcionalidad del proyecto, con nombre demo_pluviometro.mp4.
