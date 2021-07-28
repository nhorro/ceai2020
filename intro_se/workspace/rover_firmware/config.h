/* Configuración
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Configuración de puerto serie */

#define APP_SERIAL_IF_BAUDRATE	115200
#define APP_SERIAL_IF_BUFSIZE	32

#define PACKET_TIMEOUT_IN_MS 	1000
#define HEARTBEAT_TIMEOUT_IN_MS 1000


/* Frecuencia de generación de reportes (Hz) */
#define GENERAL_TMY_REPORT_FREQ 	 1

/* Frecuencia de generación de reportes de IMU (Hz) */
#define IMU_UPDATE_FREQ 			 1
#define IMU_REPORT_FREQ 		 	 1

#endif