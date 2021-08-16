/**
 * @file report_def.h Definición de códigos de reportes generados por la aplicación.
 */

#ifndef REPORT_DEF_H
#define REPORT_DEF_H

/* Códigos de reportes.
*/
enum report_index_e {
	// Reportes generales

    /** Reporte general de telemetría. */
	REPORT_GENERAL_TELEMETRY       					=	0x80,

    /** Reporte de resultado de ejecución de un comando. */
	REPORT_COMMAND_EXECUTION_STATUS					=	0x81,

	// BEGIN Reportes específicos de la aplicación

    /** Reporte con estado de IMU. */
	REPORT_IMU_AHRS_STATE							=	0x82,

	// END Reportes específicos de la aplicación
    
	REPORT__LAST
};

#endif // REPORT_DEF_H