/**
 * @file tmy_def.h Definición de telemetrías de la aplicación.
 */

#ifndef TMY_DEF_H
#define TMY_DEF_H

/** Tabla de telemetrías de la aplicación. 
 *  Por compatibilidad con sistemas de 8bit, cada telemetría es un valor de un byte 
 *  (los valores de 32bits se indican como B0,B1,B2,B3 donde B0 es MSW, B3 LSW.
 */
enum tmy_index_e {
	// Telemetría general

    /** Contador de paquetes aceptados. */
	TMY_PARAM_ACCEPTED_PACKETS		= 0,

    /** Contador de paquetes rechazados. */
	TMY_PARAM_REJECTED_PACKETS		= 1,

    /** Código de último comando ejecutado. */
	TMY_PARAM_LAST_OPCODE			= 2,

    /** Código de último error/status. */
	TMY_PARAM_LAST_ERROR			= 3,

	TMY_PARAM_STATUS				= 4,

	// IMU

    /** Eje X de Acelerómetro (crudo) */
	TMY_PARAM_IMU_RAW_ACCEL_X_B0, TMY_PARAM_IMU_RAW_ACCEL_X_B1, TMY_PARAM_IMU_RAW_ACCEL_X_B2, TMY_PARAM_IMU_RAW_ACCEL_X_B3,

    /** Eje Y de Acelerómetro (crudo) */
	TMY_PARAM_IMU_RAW_ACCEL_Y_B0, TMY_PARAM_IMU_RAW_ACCEL_Y_B1, TMY_PARAM_IMU_RAW_ACCEL_Y_B2, TMY_PARAM_IMU_RAW_ACCEL_Y_B3,

    /** Eje Z de Acelerómetro (crudo) */
	TMY_PARAM_IMU_RAW_ACCEL_Z_B0, TMY_PARAM_IMU_RAW_ACCEL_Z_B1, TMY_PARAM_IMU_RAW_ACCEL_Z_B2, TMY_PARAM_IMU_RAW_ACCEL_Z_B3,

    /** Eje X de Giróscopo (crudo) */
	TMY_PARAM_IMU_RAW_GYRO_X_B0, TMY_PARAM_IMU_RAW_GYRO_X_B1, TMY_PARAM_IMU_RAW_GYRO_X_B2, TMY_PARAM_IMU_RAW_GYRO_X_B3,

    /** Eje Y de Giróscopo (crudo) */
	TMY_PARAM_IMU_RAW_GYRO_Y_B0, TMY_PARAM_IMU_RAW_GYRO_Y_B1, TMY_PARAM_IMU_RAW_GYRO_Y_B2, TMY_PARAM_IMU_RAW_GYRO_Y_B3,

    /** Eje Z de Giróscopo (crudo) */
	TMY_PARAM_IMU_RAW_GYRO_Z_B0, TMY_PARAM_IMU_RAW_GYRO_Z_B1, TMY_PARAM_IMU_RAW_GYRO_Z_B2, TMY_PARAM_IMU_RAW_GYRO_Z_B3,			

    /** Eje X de Magnetómetro (crudo) */
	TMY_PARAM_IMU_RAW_MAG_X_B0, TMY_PARAM_IMU_RAW_MAG_X_B1, TMY_PARAM_IMU_RAW_MAG_X_B2, TMY_PARAM_IMU_RAW_MAG_X_B3,

    /** Eje Y de Magnetómetro (crudo) */
	TMY_PARAM_IMU_RAW_MAG_Y_B0, TMY_PARAM_IMU_RAW_MAG_Y_B1, TMY_PARAM_IMU_RAW_MAG_Y_B2, TMY_PARAM_IMU_RAW_MAG_Y_B3,
	
    /** Eje Z de Magnetómetro (crudo) */
    TMY_PARAM_IMU_RAW_MAG_Z_B0, TMY_PARAM_IMU_RAW_MAG_Z_B1, TMY_PARAM_IMU_RAW_MAG_Z_B2, TMY_PARAM_IMU_RAW_MAG_Z_B3,

    /** Temperatura */
	TMY_PARAM_IMU_TEMP_B0, TMY_PARAM_IMU_TEMP_B1, TMY_PARAM_IMU_TEMP_B2, TMY_PARAM_IMU_TEMP_B3,

	// IMU (Estimaciones)

    /** Componente X de cuaternion estimado (Kalman o Madwick) */
	TMY_PARAM_IMU_QUAT_X_B0, TMY_PARAM_IMU_QUAT_X_B1, TMY_PARAM_IMU_QUAT_X_B2, TMY_PARAM_IMU_QUAT_X_B3,

    /** Componente Y de cuaternion estimado (Kalman o Madwick) */
	TMY_PARAM_IMU_QUAT_Y_B0, TMY_PARAM_IMU_QUAT_Y_B1, TMY_PARAM_IMU_QUAT_Y_B2, TMY_PARAM_IMU_QUAT_Y_B3,

    /** Componente Z de cuaternion estimado (Kalman o Madwick) */
	TMY_PARAM_IMU_QUAT_Z_B0, TMY_PARAM_IMU_QUAT_Z_B1, TMY_PARAM_IMU_QUAT_Z_B2, TMY_PARAM_IMU_QUAT_Z_B3,

    /** Componente W de cuaternion estimado (Kalman o Madwick) */
	TMY_PARAM_IMU_QUAT_W_B0, TMY_PARAM_IMU_QUAT_W_B1, TMY_PARAM_IMU_QUAT_W_B2, TMY_PARAM_IMU_QUAT_W_B3,

	// GPS (FIXME)

    
	TMY_PARAM_LAST
};

/* Status Interpretation */

#define STATUS_AHRS_FAIL			0x01
#define STATUS_GPS_FAIL				0x02

#endif