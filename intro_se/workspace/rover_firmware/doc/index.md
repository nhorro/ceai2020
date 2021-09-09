# Firmware para prototipo de vehículo de operación remota

Versión del documento: 0.2.0

[TOC]

## Descripción de este trabajo

Se realizó como trabajo final para el curso de Introducción a Sistemas Embebidos de la carrera de Especialización en Inteligencia Artificial la implementación en la placa Nucleo F767ZI del firmware de la placa de control de un vehículo de operación remota (generalmente llamado *rover* por sus siglas en inglés: *Remotely Operated Vehicle for Emplacement & Reconnaissance*).

### Contexto

El presente trabajo es parte de un proyecto más grande en el que la placa de control del vehículo es  uno de los subsistemas a desarrollar.  El objetivo final del proyecto es contar con una plataforma para:

- Aprendizaje y prototipado de aplicaciones en el ambiente [ROS (Robot Operative System)](https://www.ros.org/) 
- Aprendizaje y prototipado de aplicaciones de visión artificial.
- Experimentación con formas de navegación autónoma usando inteligencia artificial, comenzando por la detección de objetos en una escena e incorporando gradualmente otros sensores y técnicas: SLAM, etc.
- Prototipado de aplicaciones IoT en que se colecta algún tipo de información y se registra en bases de datos.

Se espera que, con excepción del firmware, el software desarrollado sea válido tanto para un rover real  como para un modelo simulado (por ejemplo en [Gazebo](http://gazebosim.org/), [Webots](https://cyberbotics.com/) o [V-Rep](https://www.coppeliarobotics.com/)). 

En el siguiente diagrama se muestra el sistema completo, para el que se se ha realizado una implementación preliminar del firmware en una placa arduino. También se utilizó una SBC Raspberry Pi 3B+ para ensayar la conexión serie con la placa de control.



![Diagrama de componentes](/home/nhorro/Desktop/FIUBA IA 2020/Trabajo Final INVAP/gestion/assets/rover-concept.png) 

Se utiliza una arquitectura típica para este tipo de sistemas, en las que como mínimo se dispone del vehículo y un software para operarlo (ground station). 

### ROVER (versión 1.0)

Se utiliza el término **plataforma** para referirse al vehículo físico y su placa de control y **carga útil** a la computadora y otros sensores e instrumentos  adicionales que permitan al vehículo cumplir una misión (obtener mediciones, manipular un brazo u otro mecanismo, explorar de forma autónoma, transmitir video, etc). 

Esta división es habitual en este tipo de aplicaciones y tiene como objetivo separar las funciones mínimas que debe cumplir un robot teleoperado de aquellas más específicas del uso que se le quiere dar, permitiendo reutilizar el hardware y software de una misma plataforma en diferentes misiones.

A modo de ejemplo: si se trata de un drone se espera que la plataforma proporcione la capacidad de desplazar el vehículo, mantenerlo estabilizado y exponer su posición, actitud y velocidad  pero no necesariamente esquivar obstáculos o definir una trayectoria.

Del mismo modo, se espera que un vehículo terrestre pueda ser comandado para avanzar a una velocidad determinada, detenerse, o cambiar su orientación, pero la planificación de trayectorias u otras tareas más complejas es competencia de otro subsistema.

En su primera versión el vehículo se construirá con los siguientes componentes:

Plataforma:

- 1x microcontrolador (placa [NUCLEO-F767ZI](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html)) que implementa el lazo de control del vehículo.
- Sensores:
  - [4x tacómetros utilizando encoders ópticos LM393](https://candy-ho.com/producto/sensor-optico-horquilla-velocidad-tacometro-lm393-arduino/).
  - [1x MPU9250](https://articulo.mercadolibre.com.ar/MLA-618704275-mpu-9250-mpu9250-acelerometro-magnetometro-giroscopo-arduino-_JM#position=2&search_layout=grid&type=item&tracking_id=5be31c5c-a195-44a5-9254-faf2f07b738c).
  - [1x GPS Neo6M](https://articulo.mercadolibre.com.ar/MLA-684956175-neo6m-gy-gps6mv2-gps-apm25-neo-6m-modulo-antena-a0129-_JM#position=2&search_layout=grid&type=item&tracking_id=110a4426-ee0b-4a96-8a97-0db8e81b4a8f).
- Actuadores:
  - [Módulo controlador de motores L298N](https://candy-ho.com/producto/doble-puente-h-driver-l298n-motor-dc-arduino-arm-avr-l298/).

Carga útil:

- Una computadora [Raspberry Pi 3B+](https://articulo.mercadolibre.com.ar/MLA-905487539-raspberry-pi-3-b-microcentro-_JM#position=1&search_layout=grid&type=item&tracking_id=b91faaf8-6d0f-4403-9218-64f45a0c3a25) para las funciones de alto nivel, como por ejemplo el software aplicativo, la comunicación con estación de control (WIFI), transmisión de video, etc. Se utiliza en la etapa temprana de desarrollo como sustituto más económico de una computadora más apropiada para la tarea, como por ejemplo [NVIDIA Jetson](https://articulo.mercadolibre.com.ar/MLA-650445732-nvidia-jetson-tx1-development-kit-espectacular-a-pedido-_JM#position=24&search_layout=stack&type=item&tracking_id=0d5721d2-8004-4187-8632-154e602442e8).
  - Opcional: [adaptador wireless USB](https://articulo.mercadolibre.com.ar/MLA-821851223-adaptador-tp-link-wireless-archer-t2u-plus-ac600-_JM?matt_tool=28960764&matt_word=&matt_source=google&matt_campaign_id=11617319696&matt_ad_group_id=108457960970&matt_match_type=&matt_network=g&matt_device=c&matt_creative=479788905259&matt_keyword=&matt_ad_position=&matt_ad_type=pla&matt_merchant_id=114675143&matt_product_id=MLA821851223&matt_product_partition_id=300169103711&matt_target_id=aud-1250848972253:pla-300169103711&gclid=Cj0KCQjw9O6HBhCrARIsADx5qCRV5xhMd9srqBK5Z9eRFATgobgF0cxrIgKTIVSMb2nEiIrNwlCN8L4aAp5IEALw_wcB) para mejorar velocidad y rango de comunicación.
- Cámara USB: Logitech Webcam C170.

Alimentación: 

- Se utilizan baterías 18650 para el microcontrolador y motores y una batería USB para la computadora.

Nota: para el chasis se utiliza el [kit 4WD](https://candy-ho.com/producto/chasis-kit-armar-robot-4wd-auto-motor-arduino-raspberry/) que incluye 4 motores DC c/ reducción y ruedas.

### Componentes de software

El código está organizado siguiendo la filosofía de ROS de separar las aplicaciones y conectarlas en una red TCP/IP donde se publican y se suscriben a datos de interés mediante un sistema de mensajería. Esto permite construir aplicaciones complejas aprovechando la extensa cantidad de componentes disponibles para todo tipo de tareas: planificación de trayectoria, corrección de distorsión de cámara, fusión de datos, IA, etc. además de facilitar la integración prácticamente con cualquier otro SW existente.

Puede hacerse una primera división del software entre el firmware y el código de aplicación. Para el trabajo final de este curso **únicamente se desarrolló el firmware**.

- Código de Aplicación (ARM o x86): desarrollado en Python o C++ usando ROS para ejecutarse en Ubuntu en ARM o x86, dependiendo de la exigencia de la tarea. Organizado como servicios/aplicaciones separadas:
  - **Bridge con ROS (roverbridge)**: comunicación con firmware por UART. Envío de comandos y recepción de telemetría usando un protocolo sencillo similar a MAVLINK. Publicación de telemetría en los formatos de ROS (posición de cuaternión de orientación del vehículo, variables de estado, contadores, etc.).
  - **Estación de control (roverstation)**: aplicación para teleoperación del rover con dos modos: modelo uniciclo o controlando directamente los motores. 
  - **Sistema de visión (rovervision)**: cadena de procesamiento de video que funciona por bloques. Cada imagen entrante es recibida por cada componente de la cadena (se pueden configurar, habilitar y deshabilitar los componentes en tiempo de ejecución). Cada componente puede escribir información en un contexto global o publicarla o almacenarla en otro medio, de manera que se vaya enriqueciendo la información disponible de la escena, guardando alguna similaridad con el [patrón de diseño blackboard](https://en.wikipedia.org/wiki/Blackboard_(design_pattern)). De este modo, un componente puede por ejemplo extraer el fondo, otro realizar detección de objetos, otro dibujar un HUD, etc. 
- Firmware (NUCLEO F67ZI):
  - Código en C/C++ desarrollado para MbedOS siguiendo estándares de programación de software embebido.
  - Modelo de ejecución: único hilo (a confirmar).
  - Código con el lazo de sensado y control que comprende:
    - Lectura de sensores.
    - Control de actuadores.
    - Comunicación por puerto serie con computadora utilizando un protocolo propio basado en mensajes (similar a MAVLINK) que incluye:
      - Un conjunto de comandos básicos de control: establecer velocidad, detenerse, etc.
      - Publicación períodica de telemetría: estado del sistema, lecturas de sensores, etc.

## Diagrama en bloques de la placa de control

El siguiente diagrama muestra los componentes del sistema 

![Diagrama de bloques](./assets/rover-fiuba-blocks.png)

- La lectura del sensor IMU se implementará por I2C.
- La lectura del GPS se realizará por UART.
- Los tacómetros se conectarán a pines de entrada digitales. Nota: se utilizarán capacitores para reducir el ruido en la lectura (no se muestran en el diagrama)
- El control de los motores se realizará por salidas pulsadas de ancho modulado (PWM).

**Alimentación:** se utilizarán dos baterías 18650 (8.4V) para alimentar el controlador de los motores, los motores y la placa. Para los sensores se utilizará la tensión de entrada recomendada en el datasheet de cada sensor utilizando las salidas de la placa NUCLEO (3.3V o 5V).

## Conexionado de hardware

Sección en preparación para futura consulta de asignación de pines. Se completará más avanzado el proyecto. 

#### NUCLEO - L298N

| Pin L298N | Pin Nucleo | Color de cable |
| --------- | ---------- | -------------- |
| ENA       | PB_11      | Blanco         |
| IN1       | PB_10      | Verde          |
| IN2       | PE_15      | Blanco         |
| IN3       | PE_14      | Verde.         |
| IN4       | PE_12      | Blanco         |
| ENB       | PE_10      | Verde          |

#### L298N - Motores

| Pin L298N | Motor      |
| --------- | ---------- |
| OUT1      | LF, LB (-) |
| OUT2      | LF, LB (+) |
| OUT3      | RF,RB(+)   |
| OUT4      | RF,RB(-)   |

Nota: con el vehículo apuntando hacia adelante: LF=Left/Front, RB=Right/Back,etc.

#### Conexión serie con Raspberry Pi 3B+

| Pin NUCLEO | Pin Raspberry Pi 3B+ |
| ---------- | -------------------- |
| GND        | 6 (GND)              |
| RX (?)     | 8 (TXD)              |
| TX (?)     | 10 (RXD)             |

#### NUCLEO - MPU9250

| Pin NUCLEO | Pin MPU9250 |
| ---------- | ----------- |
| 3.3V       | 3.3V        |
| GND        | GND         |
| SDA (?)    | SDA         |
| SCL (?)    | SCL         |

#### NUCLEO - GPS

| Pin NUCLEO | Pin GPS | Cable (color) |
| ---------- | ------- | ------------- |
| 5V         | VCC     | Rojo          |
| GND        | GND     | Gris          |
| Rx (PD_6)  | Tx      | Amarillo      |
| Tx (PD_5)  | Rx      | Fucsia        |

#### NUCLEO - Tacómetros LM393

| Pin Nucleo | Pin LM393        |
| ---------- | ---------------- |
| GND        | GND              |
| 3.3V       | VCC              |
| PE_7       | DO (Tacómetro 1) |
| PE_8       | DO (Tacómetro 2) |
| PG_9       | DO (Tacómetro 3) |
| PG_14      | DO (Tacómetro 4) |

## Modelado de SW

### Arquitectura estática

#### Diagrama de clases

- Organización del código.

### Arquitectura dinámica

### Protocolo de comunicación 

Para comunicarse con la computadora, se utiliza un protocolo de SW basado en mensajes, donde cada paquete tiene el siguiente formato:

```
"PKT!<payload><crc16>\n"
```

siendo:

- "PKT!" la palabra de sincronismo (4bytes).
- *payload* es el la carga útil del paquete, una secuencia de un máximo de 64 bytes.
- *crc16* es el código CRC del payload.
- *\n* es el byte terminador del paquete.

Se definen dos tipos de paquetes:

- **Comandos**: la carga útil se compone de un byte indicando un código de operación (OPCODE)  seguido de datos específicos del comando.
- **Reportes**: la carga útil consiste en un byte indicando un código de reporte (REPORT_ID) seguido de datos específicos de cada reporte. Es mandatoria la implementación como mínimo de dos tipos de reporte:
  - Telemetría.
  - Resultado de ejecución de un comando.

El siguiente diagrama muestra la máquina de estados que se utiliza para reconocer el protocolo.

![Máquina de estados del protocolo de comunicación](./assets/rover-serial-protocol.png)

Secuencia nominal:

1. El estado inicial es *LIBRE* (es decir, se está a la espera de iniciar la recepción de un paquete).
2. Ante la llegada de un nuevo caracter, si el mismo coincide con el esperado para la secuencia de sincronismo se avanza al siguiente estado.
3. Una vez reconocido el último caracter de la secuencia de sincronismo se inicializa en cero  un contador de bytes leidos *n_recv* y se pasa al estado de recepción de bytes *LEER_C*.
4. En el estado *LEER_C* cada nuevo byte recibido se almacena en un buffer y se incrementa *n_recv*. El máximo es *MAX_BYTES*=64.
5. Se sale de *LEER_C* si se recibe el caracter terminador. En este caso se pasa al estado de *VALIDAR_Y_DESPACHAR*.
6. En el estado de validación se realiza el chequeo de CRC, y si es válido se despacha el paquete (es decir, se ejecuta la función asociada a ese OPCODE). De lo contrario se pasa a *ERROR* con el código *CRC_INVÁLIDO*.

Error:

1. Recibir un caracter inesperado, superar la cantidad máxima de bytes de un paquete o un error en CRC llevará al estado de error. En este estado se incrementará el contador del tipo de error y se transicionará al estado inicial.

### Lazo de control

- Esta sección se completará más avanzado el proyecto. 
- Descripción de la secuencia del lazo y parámetros.

## Repositorio de código

Firmware: https://github.com/nhorro/ceai2020/tree/master/intro_se/workspace/rover_firmware

Cliente python y cuaderno de banco de pruebas: https://github.com/nhorro/ceai2020/tree/master/intro_se/workspace/rover_groundcontrol

## Ejemplos

- Links a videos  e imagenes.
- Esta sección se completará más avanzado el proyecto. 

## Referencia del protocolo de comunicación

A modo de ejemplo, se describen en esta sección los comandos y reportes a implementar. Se actualizarán una vez más avanzada la aplicación.

### Comandos

| Opcode | Mnemónico              | Descripción                                                  | Parámetros              |
| ------ | ---------------------- | ------------------------------------------------------------ | ----------------------- |
| 0x00   | REQUEST_TMY            | Solicitar telemetría. Se generará un reporte general de telemetría. | Ninguno.                |
| 0x01   | LED_ON                 | Encender led de prueba.                                      | Ninguno.                |
| 0x02   | LED_OFF                | Apagar led de prueba.                                        | Ninguno.                |
| 0x03   | UPDATE_MOTOR_THROTTLES | Actualizar potencia de los motores (manual).                 | Ver detalles más abajo. |

#### 0x03 UPDATE_MOTOR_THROTTLES

Actualizar las velocidades de los motores. Cada velocidad se indica como un int16 entre -255 y 255.

| Byte Offset | Descripción                                            |
| ----------- | ------------------------------------------------------ |
| 0           | MSB de velocidad de motor A                            |
| 1           | LSB de velocidad de motor A                            |
| 2           | MSB de velocidad de motor B                            |
| 3           | LSB de velocidad de motor B                            |
| 4           | Flags. b01: habilitar motor A, b10: habilitar motor B. |

LSB/MSB: less/most significant bit.

### Reportes

#### 0x80 GENERAL_TELEMETRY_REPORT

Este reporte se genera en respuesta a un comando REQUEST_TMY.

| Offset | Parámetro                      | Descripción                                                  |
| ------ | ------------------------------ | ------------------------------------------------------------ |
| 0      | REPORT_TELEMETRY_REQUEST(0x80) | Identificador del reporte.                                   |
| 1      | TMY_PARAM_ACCEPTED_PACKETS     | Cantidad de paquetes aceptados (un pedido de telemetría no incrementa este contador). |
| 2      | TMY_PARAM_REJECTED_PACKETS     | Cantidad de paquetes rechzados (un pedido de telemetría no incrementa este contador). |
| 3      | TMY_PARAM_LAST_OPCODE          | Último OPCODE recibido.                                      |
| 4      | TMY_PARAM_LAST_ERROR           | Código de error del último comando ejecutado.                |

#### 0x81 REPORT_COMMAND_EXECUTION_STATUS

Este reporte se genera automáticamente como resultado de la ejecución de un comando.

| Offset | Parámetro                             | Descripción                       |
| ------ | ------------------------------------- | --------------------------------- |
| 0      | REPORT_COMMAND_EXECUTION_STATUS(0x81) | Identificador del reporte.        |
| 1      | OPCODE                                | OPCODE ejecutado.                 |
| 2      | STATUS_CODE                           | Código de status de la ejecución. |

## Referencias

- Material del curso Introducción a los sistemas embebidos.
- Arquitectura para robots de exploración
  - [Software Architecture for Planetary & Lunar Robotics](https://ti.arc.nasa.gov/m/pub-archive/1205h/1205%20(Utz).pdf)
  - Software libre para drones y otros vehículos teleoperados:
    - [Learning the Ardupilot codebase](https://ardupilot.org/dev/docs/learning-the-ardupilot-codebase.html)
    - [Liprepilot system architecture](https://librepilot.atlassian.net/wiki/spaces/LPDOC/pages/100523730/LibrePilot+System+Architecture)
- Protocolos de telecomandos y telemetría para sistemas de aviónica:
  - [ECSS-E-ST-70-41C – Telemetry and telecommand packet utilization (15 April 2016)](https://ecss.nl/standard/ecss-e-st-70-41c-space-engineering-telemetry-and-telecommand-packet-utilization-15-april-2016/)
  - [MAVLink Developer Guide](https://mavlink.io/en/)
- Documentación de MBedOS:
  - [MBed API references and tutorials - Standard Pin Names](https://os.mbed.com/docs/mbed-os/v6.12/apis/standard-pin-names.html)
  - [MBed API references and tutorials - I2C](https://os.mbed.com/docs/mbed-os/v6.12/apis/i2c.html)
  - [MBed API references and tutorials - BufferedSerial](https://os.mbed.com/docs/mbed-os/v6.12/apis/serial-uart-apis.html)
  - [MBed API references and tutorials - PwmOut](https://os.mbed.com/docs/mbed-os/v6.12/apis/pwmout.html)