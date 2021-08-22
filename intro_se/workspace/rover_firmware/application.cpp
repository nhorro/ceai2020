#include "application.h"
#include "l298n_motor_control.h"

application::application() :
	opcodes
		{ 
			// Básicos
			{ &application::request_tmy, opcode_flags::default_flags },
			{ &application::led_on, opcode_flags::enable_execution_status_report },
			{ &application::led_off, opcode_flags::enable_execution_status_report },

			// BEGIN Comandos específicos de la aplicación

			// Control del rover
			{ &application::set_motor_throttle, opcode_flags::default_flags },
            { &application::set_motor_speed_setpoints, opcode_flags::default_flags }

			// END Comandos específicos de la aplicación
		}
	, periodic_tasks 
        /* IMPORTANTE: el orden de las tareas debe ser: 
                1. Leer entradas y sensores.
                    - read_telecommands
                    - read_imu
                    - read_tachometers
                    - read_gps
                2. Aplicar filtros / estimar.
                    - N/A
                3. Actualizar valores de actuadores.
                    - write_motors
                4. Publicar telemetrías.
                    - send_general_tmy_report
                    - send_motion_control_report
                    - send_imu_report
                    - send_gps_report

            Nota: no todas las tareas requieren la misma frecuencia.
        */
		{
            // Lectura de telecomandos de UART.
			{ 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_TELECOMMANDS_FREQ), 
                &application::read_telecommands
            }, 

            // Lectura de IMU
			{ 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_IMU_FREQ), 
                &application::read_imu 
            },
            
            // Lectura de tacómetros
			{ 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_TACHOMETERS_FREQ), 
                &application::read_tachometers
            },  

            // Lectura de GPS
			{ 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_GPS_FREQ), 
                &application::read_gps
            },  

            // Escritura de motores
            { 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/WRITE_MOTORS_FREQ), 
                &application::write_motors
            },  

            // Envío de reporte de TMY general
			{ 
                true, 
                periodic_task_entry::execution_context::application, 
                0, 
                uint32_t(CONTROL_CYCLE_FREQ/GENERAL_TMY_REPORT_FREQ), 
                &application::send_general_tmy_report 
            },

            // Envío de reporte de estado de motores
			{ 
                true, 
                periodic_task_entry::execution_context::application, 
                0, 
                uint32_t(CONTROL_CYCLE_FREQ/MOTION_REPORT_FREQ), 
                &application::send_motion_control_report
            },

            // Envío de reporte de IMU
			{ 
                true, 
                periodic_task_entry::execution_context::application, 
                0, uint32_t(CONTROL_CYCLE_FREQ/IMU_REPORT_FREQ),
                &application::send_imu_report 
            },

            // Envío de reporte de GPS
			{ 
                true, 
                periodic_task_entry::execution_context::application, 
                0, uint32_t(CONTROL_CYCLE_FREQ/GPS_REPORT_FREQ),
                &application::send_gps_report
            }       
		}
    , periodic_task_counter(0)
    , task_ticker_queue(mbed_event_queue())
    , serial_port(USBTX, USBRX)
    , led(LED1)
    /*
    , tacho { 
        { D4, WHEEL_ENCODER_N_TICKS },
        { D5, WHEEL_ENCODER_N_TICKS },
        { D6, WHEEL_ENCODER_N_TICKS },
        { D7, WHEEL_ENCODER_N_TICKS }
    }
    */
{
    // Registro de tareas periódicas
    this->periodic_task_counter_max = int(CONTROL_CYCLE_FREQ);
    this->periodic_task_ticker.attach(
        callback(this, &application::execute_periodic_tasks), 
        1.0f / CONTROL_CYCLE_FREQ
    );
    this->task_ticker_queue->dispatch_forever();
}

void application::read_telecommands()
{       
    if ( this->serial_port.readable() )
    {
        // Debugging
        static char buf[32] = {0};
        ssize_t n = this->serial_port.read(buf, sizeof(buf));
        //printf("Read commands %d\n\r",n);
        if (EAGAIN != n)
        {
            char* pbuf = buf;
            do {
                //printf("Read: %c\n\r",*pbuf++);
                this->feed(*pbuf++);
            } while(--n);
        }
    }    
}

void application::setup()
{
    // 1. Configurar conexión serie de aplicación + control en modo asincrónico.
    this->serial_port.set_baud(APP_SERIAL_IF_BAUDRATE);
    this->serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );

    // 2. Configurar L298N
    //this->motor_ctl.setup();

    // 3. FIXME configurar Tacómetros
    //this->tacho[0].setup();
    //this->tacho[1].setup();
    //this->tacho[2].setup();
    //this->tacho[3].setup();

    // 4. FIXME configurar I2C + IMU

    

    // 5. FIXME configurar GPS
}


void application::handle_packet(const uint8_t* payload, uint8_t n)
{
    // El byte 0 es el código de opcode, el resto el payload. */
	uint8_t opcode = payload[0];
	if (OPCODE_REQUEST_TMY == opcode)
	{
        /* Un pedido de general de telemetría es un caso especial.
           No queremos que los contadores y status reaccionen a este opcode. */
		(this->*(opcodes[opcode].fn))(payload + 1, n - 1);
	}
	else
	{
        // Incrementar contador de paquetes aceptados.
		this->tmy[TMY_PARAM_ACCEPTED_PACKETS]++;

        // Ejecutar comando y actualizar status y código de último comando
		this->tmy[TMY_PARAM_LAST_ERROR] =
				(opcode < OPCODE_LAST) ?
						(this->*(opcodes[opcode].fn))(payload + 1,
								n - 1) :
						error_code::unknown_opcode;
		this->tmy[TMY_PARAM_LAST_OPCODE] = static_cast<uint8_t>(opcode);

		// Si está el flag habilitado, generar reporte de ejecución
		if ( this->opcodes->flags & opcode_flags::enable_execution_status_report )
		{
			this->get_payload_buffer()[0] = REPORT_COMMAND_EXECUTION_STATUS;
			this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_LAST_OPCODE];
			this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_LAST_ERROR];
			this->send(3);			
		}
	}
}

void application::set_error(error_code ec)
{
	if (packet_decoder::error_code::timeout != ec)
	{
		this->tmy[TMY_PARAM_REJECTED_PACKETS]++;
		this->tmy[TMY_PARAM_LAST_ERROR] = static_cast<uint8_t>(ec);
	}
}

void application::send_impl(const uint8_t* buf, uint8_t n)
{
    this->serial_port.write(buf,n);
}

void application::handle_connection_lost()
{
    // Si se perdió la conexión apagar el vehículo inmediatamente.

	//REQUEST_EXTERNAL_RESET

	//apagar motores
	this->throttles[0] = 0;
	this->throttles[1] = 0;
    /*
	this->motor_ctl.set_motor_speeds(this->speeds, 
    		l298_motor_control::motor_control_flags::motor_a|
	   	    l298_motor_control::motor_control_flags::motor_b );
    */
}

void application::start_control_cycle()
{
    // Marcar el inicio de un ciclo de control. Por ahora sólo debugging.
    // static uint32_t iteration_counter = 0;
    //printf("Iteration %d.\n\r",iteration_counter++);
    //this->read_commands();
}

void application::execute_periodic_tasks()
{
    // 1. Inicio de ciclo de control
    if (0 == this->periodic_task_counter)
    {
        // Parpadear led hasta que se recibe el primer comando.
        // Esto se usa como indicador de que el SW está en ejecución.
        if(this->tmy[TMY_PARAM_ACCEPTED_PACKETS]==0)
        {
            this->led.write(!this->led.read());
        }	
        this->task_ticker_queue->call(callback(this,&application::start_control_cycle));
    }
 
    // 2. Tareas períodicas
	for(int i=0;i<N_PERIODIC_TASKS;i++)
	{
		if( this->periodic_tasks[i].enabled)
		{
            if (0 == this->periodic_tasks[i].counter )
            {
                // Si la tarea puede ejecutarse en ISR, ejecutar inmediatamente.
                // De lo contrario, encolar para ejecutar en espacio de usuario.
                if(application::periodic_task_entry::execution_context::application == 
                   this->periodic_tasks[i].exec_ctx)
                {
                    this->task_ticker_queue->call(callback(this,this->periodic_tasks[i].entrypoint));
                }
                else 
                {
                    (this->*(this->periodic_tasks[i].entrypoint))();
                }
            }

            if(++this->periodic_tasks[i].counter==this->periodic_tasks[i].period)
            {
                this->periodic_tasks[i].counter = 0;
            }
		}
	}

    // 3. Incrementar contador
    if ( ++this->periodic_task_counter >= this->periodic_task_counter_max)
    {
        this->periodic_task_counter = 0;
    }
}

/* Opcodes */

application::error_code application::request_tmy(const uint8_t* payload, uint8_t n)
{    
	this->send_general_tmy_report();
	return error_code::success;
}

application::error_code application::led_on(const uint8_t* payload, uint8_t n)
{
    this->led.write(true);
	return error_code::success;
}

application::error_code application::led_off(const uint8_t* payload, uint8_t n)
{
    this->led.write(false);
	return error_code::success;
}

// BEGIN Comandos específicos de la aplicación 

application::error_code application::set_motor_throttle(const uint8_t* payload, uint8_t n)
{	
	uint8_t flags = payload[4];

    if ( flags & l298_motor_control::motor_control_flags::motor_a )
    {
        std::memcpy(&this->throttles[0],&payload[0],2);
    }

    if ( flags & l298_motor_control::motor_control_flags::motor_b )
    {
        std::memcpy(&this->throttles[1],&payload[2],2);
    }

    // Actualizar TMY
    std::memcpy(&this->tmy[TMY_PARAM_MOTOR_A_THROTTLE_B0],&this->throttles,4);

	//this->motor_ctl.set_motor_speeds(this->throttles, flags);
	return error_code::success;
}

application::error_code application::set_motor_speed_setpoints(const uint8_t* payload, uint8_t n)
{
	uint8_t flags = payload[8];

    if ( flags & l298_motor_control::motor_control_flags::motor_a )
    {
        std::memcpy(&this->speed_setpoints[0],&payload[0],4);
    }

    if ( flags & l298_motor_control::motor_control_flags::motor_b )
    {
        std::memcpy(&this->speed_setpoints[1],&payload[4],4);
    }

    // Actualizar TMY
    std::memcpy(&this->tmy[TMY_PARAM_MOTOR_A_SETPOINT_SPEED_B0],&this->speed_setpoints,8);

	//this->motor_ctl.set_motor_speeds(this->throttles, flags);
	return error_code::success;
}

/* Tareas periódicas */

void application::read_imu()
{	
    this->tmy[TMY_PARAM_ACCEPTED_PACKETS];


    //printf("%03d update_imu\n\r",this->periodic_task_counter);
    /*
	if( !this->tmy[TMY_PARAM_STATUS] & STATUS_AHRS_FAIL )
	{
        this->imu.read();
        this->imu.transform_units();
        this->imu.calc_euler_angles_from_accmag();  
        this->imu.integrate_gyro_angles(millis());
	}
    */
}

void application::read_tachometers()
{	
    // Velocidad medida en tacómetro 1 (RPM)
    // FIXME Actualizar 
    // this->tacho_readings[0]

    this->tacho_readings[0]+=0.1f;
    this->tacho_readings[1]+=0.1f;
    this->tacho_readings[2]+=0.1f;
    this->tacho_readings[3]+=0.1f;

    std::memcpy(&this->tmy[TMY_PARAM_TACHO1_SPEED_B0],this->tacho_readings,4*4);
}

void application::read_gps()
{	
    this->gps_lon = 1245.0f;
    this->gps_lat = 2241.0f;

    std::memcpy(&this->tmy[TMY_PARAM_GPS_LONG_B0],&this->gps_lon,4);
    std::memcpy(&this->tmy[TMY_PARAM_GPS_LAT_B0],&this->gps_lat,4);
}

void application::write_motors()
{
    // FIXME
}

/* Reportes */

void application::send_general_tmy_report()
{
    //printf("%03d send_general_tmy_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_GENERAL_TELEMETRY;
	this->get_payload_buffer()[1] = this->tmy[TMY_PARAM_ACCEPTED_PACKETS];
	this->get_payload_buffer()[2] = this->tmy[TMY_PARAM_REJECTED_PACKETS];
	this->get_payload_buffer()[3] = this->tmy[TMY_PARAM_LAST_OPCODE];
	this->get_payload_buffer()[4] = this->tmy[TMY_PARAM_LAST_ERROR];
	this->get_payload_buffer()[5] = this->tmy[TMY_PARAM_STATUS];
	this->send(1+5);
}

void application::send_imu_report()
{
    //printf("%03d send_imu_report\n\r",this->periodic_task_counter);

	this->get_payload_buffer()[0] = REPORT_IMU_AHRS_STATE;
	this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    // Cuidado con el endianness acá!
	//memcpy(  &this->get_payload_buffer()[4], 
    // 		 reinterpret_cast<const uint8_t*>(this->imu.get_processed_values()), 40);	
	//memcpy(  &this->get_payload_buffer()[4+40], 
	//		 reinterpret_cast<const uint8_t*>(this->imu.get_magacc_euler_angles()), 12);	
	//memcpy(  &this->get_payload_buffer()[4+40+12], 
    //		 reinterpret_cast<const uint8_t*>(this->imu.get_gyro_euler_angles()), 12);	

	this->send(4 + (10*4) + (3*4) + (3*4) );
}


void application::send_motion_control_report()
{
    //printf("%03d send_motion_control_report\n\r",this->periodic_task_counter);

	this->get_payload_buffer()[0] = REPORT_MOTION_CONTROL_STATE;
	this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 
    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_TACHO1_SPEED_B0], 7*4);
	this->send(4 + (7*4) );
}

void application::send_gps_report()
{
    //printf("%03d send_gps_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_GPS_STATE;
    this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_GPS_LONG_B0], 4);
    std::memcpy(&this->get_payload_buffer()[8], &this->tmy[TMY_PARAM_GPS_LAT_B0], 4);

	this->send(4 + (4*2)  );
}

