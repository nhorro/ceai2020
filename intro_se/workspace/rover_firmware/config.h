/**
 * @file config.h Configuración de la aplicación.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Puerto serie */
#define APP_SERIAL_IF_BAUDRATE	9600 //115200
#define APP_SERIAL_IF_BUFSIZE	32

/* Tamaño de mensajes y timeout del protocolo serie */
#define PACKET_TIMEOUT_IN_MS 	1000
#define HEARTBEAT_TIMEOUT_IN_MS 1000

/* Frecuencia de generación de reportes generales (Hz) */
#define GENERAL_TMY_REPORT_FREQ 	 3.0f

/* Frecuencia de generación de reportes de IMU (Hz) */
#define IMU_UPDATE_FREQ 			 2.0f
#define IMU_REPORT_FREQ 		 	 4.0f

/* Frecuencia del ciclo de control (Hz) */
#define CONTROL_CYCLE_FREQ          100.0f


#define WHEEL_ENCODER_N_TICKS 32

#endif