#ifndef ROVER_APPLICATION_H
#define ROVER_APPLICATION_H

#include <stdint.h>
#include "protocol/protocol.h"
#include "tctm/cmd_def.h" 		// Definición de telecomandos
#include "tctm/tmy_def.h"		// Definición de variables de telemetría
#include "tctm/report_def.h"	// Definición de reportes

// FIXME
//#include "l298n_motor_control.h"
//#include "mpu9250_ahrs.h"

#include "config.h"

// Cantidad de tareas periódicas
#define N_PERIODIC_TASKS 3

/** Clase aplicación. 
 * Notar que se heredan el codificador y decodificador de paquetes, es decir, 
 * la aplicación tiene la capacidad de generar paquetes y de decodificarlos.
*/
class application: 
	public protocol::packet_decoder,
	public protocol::packet_encoder	
{
public:    
    /** Constructor por defecto. */
	application();

    /** Destructor por defecto. */
    ~application() = default; 

    /** Inicialización. Incluye configuración de periféricos, tests iniciales, etc. */
	void setup();

    /** Lazo de control. 
    *   Eliminar si se puede. 
    *   Se prefiere tareas períodicas + handlers para eventos asincrónicos. */
	void loop();
private:
    /* Tabla de telemetrías */
	uint8_t tmy[TMY_PARAM_LAST] __attribute__((aligned (4)));	
	using opcode_callback = application::error_code(application::*)(const uint8_t* payload, uint8_t n);

    /** Telecomandos */

    /** Flags de un telecomando 
     *  Por defecto los telecomandos sólo se reciben y se procesan.
     *  Los flags permiten incrementar el contador de recibidos y/o generar un reporte conteniendo
     *  el estado de ejecución del comando.
     */
	enum opcode_flags {
		default_flags 				   	= 0x00, 
		update_execution_counters		= 0x01, /**< Actualizar contador de ejecución. */
		enable_execution_status_report 	= 0x02  /**< Generar un reporte de status. */
	};

    /** Descriptor de un OPCODE. */
	struct opcode_descr {
		opcode_callback fn;
		uint8_t flags;    
	};
        
    /* Tabla de opcodes */
	opcode_descr opcodes[OPCODE_LAST];

    /* Protocolo de comunicación serie */

	/* requeridos por packet_decoder */

    /** Dispatcher de telecomandos recibidos. 
      * @param payload bytes del paylaod del telecomando.
      * @param n cantidad de bytes.
      * @warning Tener mucho cuidado de desde donde se llama a esta función.
      *          Si se está ejecutando la FSM en el contexto de interrupciones,
      *          Despachar al contexto de aplicación.
      */    
	void handle_packet(const uint8_t* payload, uint8_t n) override;

    /** Establecer código de error de último telecomando procesado.
      * @param error_code código de error (0=éxito)
      */    
	void set_error(error_code ec) override;

    /** Implementación de envío de bytes. En este caso se utiliza puerto serie.
     * @param buf buffer con datos a transmitir (inmutable).
     * @param n cantidad de bytes.
     */
	void send_impl(const uint8_t* buf, uint8_t n) override;

    /** Manejar errores de conexión (cuando no se recibe HEARTBEAT).
     */
	void handle_connection_lost() override;

    /** Clase para manejo de puerto serie (el buffering lo gestiona la librería de mbed-os) */
    BufferedSerial serial_port;

    /** Callback cuando hay datos nuevos en la UART. */
    void on_uart_rx();

    /** Leer telecomandos entrantes (hardcoded a UART). */
    void read_commands();

    /* Tareas períodicas */

    /** El ticker es el scheduler del sistema encargado de invocar a todas las funciones períodicas
     *  según la frecuencia que se haya configurado. Se usa 'task' para refereirse a estas tareas, si bien
     *  son funciones que se ejecutan un único hilo.
     */
    Ticker periodic_task_ticker;

    /** Cola utilizada para encolar funciones llamadas en una interrupción que requieren ser ejecutadas
     *  en contexto de la aplicación. 
     */
    EventQueue *task_ticker_queue;

    /** Definición de una tarea */
	struct periodic_task_entry {
		bool enabled; /**< Habilitación (se pueden deshabilitar para ensayos o para modo degradado) */
        
        /** Contexto de ejecución. Si puede ejecutarse en ISR (directo cuando se llama al ticker) o 
         * debe encolarse para que se ejecute en el contexto de la aplicación. */
        enum class execution_context{
            isr, 
            application
        } exec_ctx;

        /** Contador interno para scheduling. */
        uint32_t counter;	

        /** Período de la tarea (fijo, se calcula en la inicialización). */
		uint32_t period;

        /** Firma del entrypoint. */
		using periodic_task_entrypoint = void(application::*)();	
		periodic_task_entrypoint entrypoint;
	};

    /** Tabla con tareas de la aplicación. */
	periodic_task_entry periodic_tasks[N_PERIODIC_TASKS];    

    /** Contador global del ticker. */
    uint32_t periodic_task_counter;

    /** Valor máximo del contador global del ticker. */
    uint32_t periodic_task_counter_max;

    /** Función llamada por el ticker que ejecuta la tarea correspondiente, según el momento
     *  en el ciclo de control. */
    void execute_periodic_tasks();

    /** Función llamada al inicio de un ciclo de control. */
    void start_control_cycle();
 
    /* Definición de comandos */

	/* Comandos :: Comandos de sistema */

    /** Pedido de telemetría */
	application::error_code request_tmy(const uint8_t* payload, uint8_t n);

    /** Encendido de led. */
	application::error_code led_on(const uint8_t* payload, uint8_t n);

    /** Apagado de led. */
	application::error_code led_off(const uint8_t* payload, uint8_t n);

	/* Comandos :: Específicos de la aplicación */

	// BEGIN comandos específicos de la aplicación

    /** Encendido de motores */
	application::error_code update_motor_speeds(const uint8_t* payload, uint8_t n);

	// END comandos específicos de la aplicación

	// BEGIN Datos y métodos específicos de la aplicación

    /** Actualizar lecturas de IMU. */
    void update_imu();

    /** Enviar reporte de IMU. */
	void send_imu_report();

    /** Enviar reporte de telemetría general. */
	void send_general_tmy_report();

    /* Led de prueba */
    DigitalOut led;

    /* Motor con L298 */
	//l298_motor_control motor_ctl;
	int16_t speeds[2] = { 0, 0 };

    /* IMU MPU9250 */
    float imu_state[10];
	//mpu9250 imu;
	
	// END Datos específicos de la aplicación    
};

#endif // ROVER_APPLICATION_H