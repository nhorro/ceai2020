#include "application.h"

application::application() :
	opcodes
		{ 
			// Básicos
			{ &application::request_tmy, opcode_flags::default_flags },
			{ &application::led_on, opcode_flags::enable_execution_status_report },
			{ &application::led_off, opcode_flags::enable_execution_status_report },

			// BEGIN Comandos específicos de la aplicación

			// Control del rover
			{ &application::set_motor_throttles, opcode_flags::default_flags },
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
			[task_read_telecommands] = { 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_TELECOMMANDS_FREQ), 
                &application::read_telecommands
            }, 

            // Lectura de IMU
			[task_read_imu] = { 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_IMU_FREQ), 
                &application::read_imu 
            },
            
            // Lectura de tacómetros
			[task_read_tachometers]= { 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_TACHOMETERS_FREQ), 
                &application::read_tachometers
            },  

            // Lectura de GPS
			[task_read_gps]={ 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/READ_GPS_FREQ), 
                &application::read_gps
            },  

            // Escritura de motores
            [task_write_motors] = { 
                true, // enabled
                periodic_task_entry::execution_context::application, // ISR/APP
                0, // internal counter
                uint32_t(CONTROL_CYCLE_FREQ/WRITE_MOTORS_FREQ), 
                &application::write_motors
            },  

            // Envío de reporte de TMY general
			[task_send_general_report] = { 
                true, 
                periodic_task_entry::execution_context::application, 
                0, 
                uint32_t(CONTROL_CYCLE_FREQ/GENERAL_TMY_REPORT_FREQ), 
                &application::send_general_tmy_report 
            },

            // Envío de reporte de estado de motores
			[task_send_motion_report] = { 
                true, 
                periodic_task_entry::execution_context::application, 
                0, 
                uint32_t(CONTROL_CYCLE_FREQ/MOTION_REPORT_FREQ), 
                &application::send_motion_control_report
            },

            // Envío de reporte de IMU
			[task_send_imu_report] = { 
                true, 
                periodic_task_entry::execution_context::application, 
                0, uint32_t(CONTROL_CYCLE_FREQ/IMU_REPORT_FREQ),
                &application::send_imu_report 
            },

            // Envío de reporte de GPS
			[task_send_gps_report] = { 
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
    , motor_ctl( DEFAULT_L298_PIN_ENA, DEFAULT_L298_PIN_IN1, DEFAULT_L298_PIN_IN2, 
                 DEFAULT_L298_PIN_ENB, DEFAULT_L298_PIN_IN3, DEFAULT_L298_PIN_IN4)    
    , tacho{ 
        { DEFAULT_TACHO1_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO2_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO3_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO4_PIN, WHEEL_ENCODER_N_TICKS } }
    // BEGIN  BUG
    /* La siguiente línea que instancia la clase IMU es la causante del error 
       que se muestra a continuación:      
    
    ++ MbedOS Error Info ++
    Error Status: 0x80020125 Code: 293 Module: 2
    Error Message: CMSIS-RTOS error: Stack overflow
    Location: 0x800D38F
    Error Value: 0x1
    Current Thread: main Id: 0x20005D98 Entry: 0x800CEA9 StackSize: 0x1000 StackMem: 0x200045E0 SP: 0x2007FF30 
    For more info, visit: https://mbed.com/s/error?error=0x80020125&tgt=NUCLEO_F767ZI
    -- MbedOS Error Info --

    Esta línea lo único que hace es instanciar una clase mpu9250 (imu), que a su vez 
    instancia una clase I2C.
    El resto de las invocaciones a la instancia imu han sido comentadas en el código.

    ¿Qué implica instanciar imu?

    La clase imu, con excepción de i2c, sólo instancia tipos primitivos:

    ~~~
    mpu9250::mpu9250(uint8_t mpu_addr,uint8_t mag_addr, PinName sda, PinName scl):
    // MbedOS usa addresses de 8bit (ver https://os.mbed.com/docs/mbed-os/v6.5/apis/i2c.html)
    mpu_addr8(mpu_addr<<1), // uint8_t
    mag_addr8(mag_addr<<1), // uint8_t
    i2c(sda, scl), // <-- acá está el problema?
    mpu_valid(false), // bool
    mag_valid(false), // bool
    total_readings(0), // uint32_t
    mpu9250_readings_ok(0), // uint32_t
    mpu9250_readings_failed(0), // uint32_t
    ak8963_readings_ok(0),  // uint32_t
    ak8963_readings_failed(0)   // uint32_t
    {
        // (no hay código)
    }
    ~~~

    Entonces, el problema parece ser la instanciación de i2c(sda, scl).
    Pero ejecutándolo en una aplicación limpia funciona. 
    Cómo se pued debuggear esto?
    Posibles causas:
        - ¿Conflicto de pines? ¿Pero porqué segfault?
        - ¿Memoria? Pero no hay alocación dinámica, debería saltar en tiempo de ejecución.
        - ¿Superación de stack por variables locales? Se inicializan miembros de la clase, por lo 
          tanto no hay uso de stack.

    Nota: el error se puede repetir sin conectar ningún periférico, únicamente con USB. Por
    más que el I2C esté desconectado, no debería fallar.
    */
    , imu(MPU9250_ADDRESS, AK8963_ADDRESS, I2C_SDA, I2C_SCL) // Comentar para evitar bug
    // END BUG
    , gps(DEFAULT_GPS_PIN_TX, DEFAULT_GPS_PIN_RX)
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
        static char buf[32] = {0};
        ssize_t n = this->serial_port.read(buf, sizeof(buf));
        if (EAGAIN != n)
        {
            char* pbuf = buf;
            do {
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
    this->motor_ctl.setup();

    // 3. Configurar Tacómetros
    this->tacho[0].setup(false);
    this->tacho[1].setup(false);
    this->tacho[2].setup(false);
    this->tacho[3].setup(false);

    // 4. Configurat IMU
    //this->imu.setup();
    
    // 5. Configurar GPS
    this->gps.setup();
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
    
	this->motor_ctl.set_motor_throttles(this->throttles, 
    		l298_motor_control::motor_control_flags::motor_a|
	   	    l298_motor_control::motor_control_flags::motor_b );
    
}

void application::start_control_cycle()
{
    // Marcar el inicio de un ciclo de control. Por ahora sólo debugging.
    // static uint32_t iteration_counter = 0;
    //printf("Iteration %d.\n\r",iteration_counter++);
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
	for(int i=0;i<periodic_task_e::last_task;i++)
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

application::error_code application::set_motor_throttles(const uint8_t* payload, uint8_t n)
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

	this->motor_ctl.set_motor_throttles(this->throttles, flags);
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
    //printf("%03d update_imu\n\r",this->periodic_task_counter);
    /*
    
    COMENTADO HASTA RESOLVER BUG

    this->imu.process();    
    this->sensor_fusion.update( this->imu.get_eng_values().acc[0],
                                this->imu.get_eng_values().acc[1],
                                this->imu.get_eng_values().acc[2],
                                this->imu.get_eng_values().gyro[0]*madwick_sensor_fusion::PI/180.0f, 
                                this->imu.get_eng_values().gyro[1]*madwick_sensor_fusion::PI/180.0f, 
                                this->imu.get_eng_values().gyro[2]*madwick_sensor_fusion::PI/180.0f,  
                                this->imu.get_eng_values().mag[0],  
                                this->imu.get_eng_values().mag[1],  
                                this->imu.get_eng_values().mag[2],
                                1.0f/READ_IMU_FREQ );
    */
    // ax,ay,az,gx,gy,gz,mx,my,mz,temp
    /*
    std::memcpy(&this->tmy[TMY_PARAM_IMU_RAW_ACCEL_X_B0],
                &this->imu.get_raw_values(),  sizeof(float)*10);
    
    // x,y,z, w
    std::memcpy(&this->tmy[TMY_PARAM_IMU_QUAT_X_B0],
                this->sensor_fusion.get_quaternion(),  sizeof(float)*4);
    */
}

void application::read_tachometers()
{	
    //this->tacho[0].debug();

    // Velocidad medida en tacómetro 1 (RPM)
    this->tacho_readings[0]+=0.1f;
    this->tacho_readings[1]+=0.2f;
    this->tacho_readings[2]+=0.3f;
    this->tacho_readings[3]+=0.4f;

    //this->tacho_readings[0]=this->tacho[0].get_rpm();
    //this->tacho_readings[1]=this->tacho[1].get_rpm();
    //this->tacho_readings[2]=this->tacho[2].get_rpm();
    //this->tacho_readings[3]=this->tacho[3].get_rpm();

    this->tacho_counters[0]=this->tacho[0].get_total_tick_count();
    this->tacho_counters[1]=this->tacho[1].get_total_tick_count();
    this->tacho_counters[2]=this->tacho[2].get_total_tick_count();
    this->tacho_counters[3]=this->tacho[3].get_total_tick_count();

    std::memcpy(&this->tmy[TMY_PARAM_TACHO1_SPEED_B0],this->tacho_readings,8*4);
}

void application::read_gps()
{	    
    this->gps.process();

    if(this->gps.is_valid())
    {
        this->gps_lon = this->gps.get_longitude();
        this->gps_lat = this->gps.get_latitude();

        std::memcpy(&this->tmy[TMY_PARAM_GPS_LONG_B0],&this->gps_lon,4);
        std::memcpy(&this->tmy[TMY_PARAM_GPS_LAT_B0],&this->gps_lat,4);
    }     
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
    // COMENTADO HASTA RESOLVER BUG DE IMU
	//this->get_payload_buffer()[1] = (this->imu.is_mag_valid() << 1) |
    //                                (this->imu.is_mpu_valid() << 0); 
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    // ax,ay,az,gx,gy,gz,mx,my,mz,temp,qx,qy,qz,qw
    std::memcpy(&this->get_payload_buffer()[4], 
                &this->tmy[TMY_PARAM_IMU_RAW_ACCEL_X_B0], 14*4);

	this->send(4 + (14*4) );
}


void application::send_motion_control_report()
{
    //printf("%03d send_motion_control_report\n\r",this->periodic_task_counter);

	this->get_payload_buffer()[0] = REPORT_MOTION_CONTROL_STATE;
	this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 
    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_TACHO1_SPEED_B0], 11*4);
	this->send(4 + (11*4) );
}

void application::send_gps_report()
{
    //printf("%03d send_gps_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_GPS_STATE;
    this->get_payload_buffer()[1] = this->gps.is_valid();
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_GPS_LONG_B0], 4);
    std::memcpy(&this->get_payload_buffer()[8], &this->tmy[TMY_PARAM_GPS_LAT_B0], 4);

	this->send(4 + (4*2)  );
}

