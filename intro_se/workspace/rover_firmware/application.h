#ifndef ROVER_APPLICATION_H
#define ROVER_APPLICATION_H

#include <stdint.h>
#include "protocol/protocol.h"
#include "tctm/cmd_def.h" 		// Definición de telecomandos
#include "tctm/tmy_def.h"		// Definición de variables de telemetría
#include "tctm/report_def.h"	// Definición de reportes

//#include "l298n_motor_control.h"
//#include "mpu9250_ahrs.h"

#include "config.h"

// Cantidad de tareas periódicas
#define N_PERIODIC_TASKS 3

/* Clase aplicación. 
 * Notar que se heredan el codificador y decodificador de paquetes, es decir, 
 * la aplicación tiene la capacidad de generar paquetes y de decodificarlos.
*/
class application: 
	public protocol::packet_decoder,
	public protocol::packet_encoder	
{
public:    
	application();
    ~application() = default; 
	void setup();
	void loop();
private:
    /* Tabla de telemetrías */
	uint8_t tmy[TMY_PARAM_LAST] __attribute__((aligned (4)));
	float imu_state[10];
	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);

	enum opcode_flags {
		default_flags 				   	= 0x00,
		update_execution_counters		= 0x01,
		enable_execution_status_report 	= 0x02
	};

	struct opcode_descr {
		opcode_callback fn;
		uint8_t flags;
	};

	opcode_descr opcodes[OPCODE_LAST];

	/* requirods por packet_decoder */
	void handle_packet(const uint8_t* payload, uint8_t n) override;
	void set_error(error_code ec) override;
	void send_impl(const uint8_t* buf, uint8_t n) override;
	void handle_connection_lost() override;

	/* Comandos :: Comandos de sistema */
	application::error_code request_tmy(const uint8_t* payload, uint8_t n);
	application::error_code led_on(const uint8_t* payload, uint8_t n);
	application::error_code led_off(const uint8_t* payload, uint8_t n);

	/* Comandos :: Específicos de la aplicación */

	// BEGIN comandos específicos de la aplicación
	application::error_code update_motor_speeds(const uint8_t* payload, uint8_t n);

	void update_imu();
	void send_imu_report();
	void send_general_tmy_report();
	// END comandos específicos de la aplicación

	// BEGIN Datos específicos de la aplicación
	//l298_motor_control motor_ctl;
	int16_t speeds[2] = { 0, 0 };

	//mpu9250 imu;

	uint32_t control_cycle_t0;
	// END Datos específicos de la aplicación

	struct periodic_task_entry {
		bool enabled;
		uint32_t t0;
		uint32_t period;
		using periodic_task_entrypoint = void(application::*)();	
		periodic_task_entrypoint entrypoint;
	};

	periodic_task_entry periodic_tasks[N_PERIODIC_TASKS];

	void align_periodic_tasks_with_control_cycle();
	void execute_periodic_tasks(uint32_t t);

    // Timer FIXME: ver de usar timer global
    Timer timer;

    // Serial I/F
    BufferedSerial serial_port;

    DigitalOut led;
};

#endif // ROVER_APPLICATION_H