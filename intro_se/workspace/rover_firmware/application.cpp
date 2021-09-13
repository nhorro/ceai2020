#include "application.h"
#include "l298n_motor_control.h"
#include "mbed_thread.h"

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
            { &application::set_motor_speed_setpoints, opcode_flags::default_flags },
            { &application::config_pid_controller, opcode_flags::default_flags }


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
    , periodic_task_counter{0}
    , task_ticker_queue{ mbed_event_queue() }
    , serial_port{USBTX, USBRX}
    , leds{LED1,LED2,LED3}
    /* Control de motores */
    , motor_ctl{ DEFAULT_L298_PIN_ENA, DEFAULT_L298_PIN_IN1, DEFAULT_L298_PIN_IN2, 
                 DEFAULT_L298_PIN_ENB, DEFAULT_L298_PIN_IN3, DEFAULT_L298_PIN_IN4,
                 DEFAULT_PID_MAX, DEFAULT_PID_MIN, 
                 DEFAULT_PID_KP, DEFAULT_PID_KD, DEFAULT_PID_KI }
    , speed_setpoints{ 0.0f, 0.0f }       
    /* Tacómetros */ 
    , tacho{ 
        { DEFAULT_TACHO1_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO2_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO3_PIN, WHEEL_ENCODER_N_TICKS },
        { DEFAULT_TACHO4_PIN, WHEEL_ENCODER_N_TICKS } 
    }
    , tacho_filters { 3,3,3,3 }
    , tacho_readings { 0.0f, 0.0f, 0.0f, 0.0f }
    , tacho_counters { 0, 0, 0, 0 }
    /* IMU */
    , imu{MPU9250_ADDRESS, AK8963_ADDRESS, I2C_SDA, I2C_SCL} 
    /* GPS */
    //, gps{DEFAULT_GPS_PIN_TX, DEFAULT_GPS_PIN_RX}
{      
}



void application::play_led_sequence(const led_sequence_entry_t seq[])
{
    for(int i=0;seq[i].action != led_sequence_entry_t::event_type_e::end; i++)
    {
        switch(seq[i].action)
        {
            case led_sequence_entry_t::event_type_e::leds_on: 
            {
                if(seq[i].param & led_id_e::led1) this->leds[0].write(true);
                if(seq[i].param & led_id_e::led2) this->leds[1].write(true);
                if(seq[i].param & led_id_e::led3) this->leds[2].write(true);
            } break;

            case led_sequence_entry_t::event_type_e::leds_off: 
            {
                if(seq[i].param & led_id_e::led1) this->leds[0].write(false);
                if(seq[i].param & led_id_e::led2) this->leds[1].write(false);
                if(seq[i].param & led_id_e::led3) this->leds[2].write(false);
            } break;

            case led_sequence_entry_t::event_type_e::wait: 
            {
                thread_sleep_for(seq[i].param);
            } break;
        }
    }    
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
    //    En MbedOS al parecer esto es ignorado, y vale el baudrate que se indica
    //    En el JSON de configuración.
    //this->serial_port.set_baud(APP_SERIAL_IF_BAUDRATE);
    //this->serial_port.set_format(
    //        /* bits */ 8,
    //  /* parity */ BufferedSerial::None,
    //        /* stop bit */ 1
    //);

    this->tacho[0].setup();    
    this->tacho[1].setup();
    this->tacho[2].setup();
    this->tacho[3].setup();

    this->imu.setup();
    //this->gps.setup();

    // Registro de tareas periódicas
    this->periodic_task_counter_max = int(CONTROL_CYCLE_FREQ);
    this->periodic_task_ticker.attach(
        callback(this, &application::execute_periodic_tasks), 
        1.0f / CONTROL_CYCLE_FREQ
    );  

    this->task_ticker_queue->dispatch_forever();

    // Secuencia de encendido
    /*
    using et = led_sequence_entry_t::event_type_e;
    using status_leds = led_id_e;

    static led_sequence_entry_t power_on_led_sequence[] = 
    {
        {  et::leds_off, 0x7 }, { et::wait, 500 },
        {  et::leds_on,  0x7 }, { et::wait, 100 },
        {  et::leds_off, 0x7 }, { et::wait, 100 }
    };
    this->play_led_sequence(power_on_led_sequence);     

    // 2. Configurar L298N 
    static led_sequence_entry_t motors_on_led_sequence[] = 
    {
        {  et::leds_off, 0x7 }, { et::wait, 500 },
        {  et::leds_on,  0x6 }, { et::wait, 100 },
        {  et::leds_off, 0x7 }, { et::wait, 100 }
    };
    this->play_led_sequence(motors_on_led_sequence);
    

    // 3. Configurar Tacómetros
    static led_sequence_entry_t tachos_on_led_sequence[] = 
     {
        {  et::leds_off, 0x7 }, { et::wait, 500 },
        {  et::leds_on,  0x6 }, { et::wait, 100 },
        {  et::leds_off, 0x7 }, { et::wait, 100 }
    };
    this->play_led_sequence(tachos_on_led_sequence);

    this->tacho[0].setup(false);    
    this->tacho[1].setup(false);
    this->tacho[2].setup(false);
    this->tacho[3].setup(false);
    
    // 4. Configurat IMU
    static led_sequence_entry_t imu_on_led_sequence[] = 
     {
        {  et::leds_off, 0x7 }, { et::wait, 500 },
        {  et::leds_on,  0x6 }, { et::wait, 100 },
        {  et::leds_off, 0x7 }, { et::wait, 100 }
    };
    this->play_led_sequence(imu_on_led_sequence);

    
    // 5. Configurar GPS
    static led_sequence_entry_t gps_on_led_sequence[] = 
    {
        {  et::leds_off, 0x7 }, { et::wait, 500 },
        {  et::leds_on,  0x6 }, { et::wait, 100 },
        {  et::leds_off, 0x7 }, { et::wait, 100 }
    };
    this->play_led_sequence(gps_on_led_sequence);
    

    static led_sequence_entry_t successful_boot_seq[] = 
    {
        {  et::leds_off, 0x7 }, { et::wait, 100 },
        {  et::leds_on,  0x7 }, { et::wait, 200 },
        {  et::leds_off, 0x7 }, { et::wait, 200 },
        {  et::leds_on,  0x7 }, { et::wait, 200 },
        {  et::leds_off, 0x7 }, { et::wait, 200 },
        {  et::leds_on,  0x7 }, { et::wait, 200 },
        {  et::leds_off, 0x7 }, { et::wait, 200 }
    };
    this->play_led_sequence(successful_boot_seq);
    */
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
        // Incrementar contador de paquetes aceptados y cambiar estado de led.
		this->tmy[TMY_PARAM_ACCEPTED_PACKETS]++;        
        this->leds[0].write(!this->leds[0].read());
        
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
    /*
	this->motor_ctl.set_motor_throttles(this->throttles, 
    		l298_motor_control::motor_control_flags::motor_a|
	   	    l298_motor_control::motor_control_flags::motor_b );
    */               
    
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
            this->leds[0].write(!this->leds[0].read());
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
    this->leds[0].write(true);
	return error_code::success;
}

application::error_code application::led_off(const uint8_t* payload, uint8_t n)
{
    this->leds[0].write(false);
	return error_code::success;
}

// BEGIN Comandos específicos de la aplicación 

application::error_code application::set_motor_throttles(const uint8_t* payload, uint8_t n)
{	    
	uint8_t flags = payload[8];

    float new_throttles[2];


    if ( flags & l298_motor_control::motor_control_flags::motor_a )
    {
        std::memcpy(&new_throttles[0],&payload[0],4);
    }

    if ( flags & l298_motor_control::motor_control_flags::motor_b )
    {
        std::memcpy(&new_throttles[1],&payload[4],4);
    }

	this->motor_ctl.set_motor_throttles(new_throttles, flags);    
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
    std::memcpy(&this->tmy[TMY_PARAM_MOTOR_A_SETPOINT_SPEED_B0],this->speed_setpoints,8);

    this->motor_ctl.set_motor_target_speeds(this->speed_setpoints, flags);
    this->motor_ctl.set_mode(l298_motor_control::motor_control_mode::pid);
	return error_code::success;
}


application::error_code application::config_pid_controller(const uint8_t* payload, uint8_t n)
{
    float max = 1.0f;
    float min = -1.0f;
    float kp, kd, ki;    
    std::memcpy(&kp,&payload[4],4);
    std::memcpy(&kd,&payload[8],4);
    std::memcpy(&ki,&payload[12],4);
    this->motor_ctl.get_pid_controller(0).reset(max,min,kp,kd,ki);
    this->motor_ctl.get_pid_controller(1).reset(max,min,kp,kd,ki);
	return error_code::success;
}



/* Tareas periódicas */

void application::read_imu()
{	
    //printf("%03d update_imu\n\r",this->periodic_task_counter);
    
    
    //COMENTADO HASTA RESOLVER BUG

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
    // ax,ay,az,gx,gy,gz,mx,my,mz,temp
    std::memcpy(&this->tmy[TMY_PARAM_IMU_ENG_ACCEL_X_B0],
                &this->imu.get_eng_values().acc[0],  sizeof(float)*3);

    std::memcpy(&this->tmy[TMY_PARAM_IMU_ENG_GYRO_X_B0],
                &this->imu.get_eng_values().gyro[0],  sizeof(float)*3);

    std::memcpy(&this->tmy[TMY_PARAM_IMU_ENG_MAG_X_B0],
                &this->imu.get_eng_values().mag[0],  sizeof(float)*3);

    std::memcpy(&this->tmy[TMY_PARAM_IMU_TEMP_B0],
                &this->imu.get_eng_values().temp,  sizeof(float));                
    
    // x,y,z, w
    std::memcpy(&this->tmy[TMY_PARAM_IMU_QUAT_X_B0],
                this->sensor_fusion.get_quaternion(),  sizeof(float)*4);
}

void application::read_tachometers()
{	
    for(int i=0;i<4;i++)
    {
        // Leer valor filtrado con mediana
        this->tacho_readings[i]= this->tacho_filters[i].process(this->tacho[i].get_rpm());

        // Limitar a valores razonables.
        if(this->tacho_readings[i]<0){
            this->tacho_readings[i] = 0.0f;
        }
        if(this->tacho_readings[i]> MAX_TACHO_VALID_RPM_READING ){
            this->tacho_readings[i] = 0.0f;
        }
        this->tacho_counters[i]=this->tacho[i].get_total_tick_count();        
    }    

    // Establecer las velocidades leídas. 
    // Notar que no tienen signo, el motor les pondrá el signo.
    float measured_speeds[2];

    // Modo promediado
    //measured_speeds[0] = (this->tacho_readings[0] + this->tacho_readings[2])/2.0f;
    //measured_speeds[1] = (this->tacho_readings[1] + this->tacho_readings[3])/2.0f;

    // Modo mínimo
    measured_speeds[0] = std::min(this->tacho_readings[0],this->tacho_readings[2]);
    measured_speeds[1] = std::min(this->tacho_readings[1],this->tacho_readings[3]);                
    this->motor_ctl.set_motor_measured_speeds(measured_speeds);


    std::memcpy(&this->tmy[TMY_PARAM_TACHO1_SPEED_B0],this->tacho_readings,4*4);
    std::memcpy(&this->tmy[TMY_PARAM_TACHO1_COUNT_B0],this->tacho_counters,4*4);
}

void application::read_gps()
{	    
    //this->gps.process();

    
    /*
    if(this->gps.is_valid())
    {
        this->gps_lon = this->gps.get_longitude();
        this->gps_lat = this->gps.get_latitude();

        std::memcpy(&this->tmy[TMY_PARAM_GPS_LONG_B0],&this->gps_lon,4);
        std::memcpy(&this->tmy[TMY_PARAM_GPS_LAT_B0],&this->gps_lat,4);
    } 
    */
    
}

void application::write_motors()
{
    this->motor_ctl.update(1.0f/WRITE_MOTORS_FREQ);
    
    float tmp = this->motor_ctl.get_motor_throttle(0);    
    std::memcpy(&this->tmy[TMY_PARAM_MOTOR_A_THROTTLE_B0],&tmp,4);
    tmp = this->motor_ctl.get_motor_throttle(1);
    std::memcpy(&this->tmy[TMY_PARAM_MOTOR_B_THROTTLE_B0],&tmp,4);
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
	this->get_payload_buffer()[1] = (this->imu.is_mag_valid() << 1) |
                                    (this->imu.is_mpu_valid() << 0); 
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    // ax,ay,az,gx,gy,gz,mx,my,mz,temp,qx,qy,qz,qw
    std::memcpy(&this->get_payload_buffer()[4], 
                &this->tmy[TMY_PARAM_IMU_ENG_ACCEL_X_B0], 14*4);

	this->send(4 + (14*4) );
}


void application::send_motion_control_report()
{
    //printf("%03d send_motion_control_report\n\r",this->periodic_task_counter);

	this->get_payload_buffer()[0] = REPORT_MOTION_CONTROL_STATE;
	this->get_payload_buffer()[1] = 0; // estado
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 
    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_TACHO1_SPEED_B0], 12*4);
	this->send(4 + (12*4) );
}

void application::send_gps_report()
{
    //printf("%03d send_gps_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_GPS_STATE;
    //this->get_payload_buffer()[1] = this->gps.is_valid();
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    std::memcpy(&this->get_payload_buffer()[4], &this->tmy[TMY_PARAM_GPS_LONG_B0], 4);
    std::memcpy(&this->get_payload_buffer()[8], &this->tmy[TMY_PARAM_GPS_LAT_B0], 4);

	this->send(4 + (4*2)  );
}


void application::send_pid_report()
{
    //printf("%03d send_gps_report\n\r",this->periodic_task_counter);
	this->get_payload_buffer()[0] = REPORT_PID_STATE;
    //this->get_payload_buffer()[1] = this->gps.is_valid();
	this->get_payload_buffer()[2] = 0; // spare
	this->get_payload_buffer()[3] = 0; // spare 

    float tmp;    
    tmp = this->motor_ctl.get_pid_controller(0).get_max();
    std::memcpy(&this->get_payload_buffer()[4], &tmp, 4);
    tmp = this->motor_ctl.get_pid_controller(0).get_min();
    std::memcpy(&this->get_payload_buffer()[8], &tmp, 4);
    tmp = this->motor_ctl.get_pid_controller(0).get_kp();
    std::memcpy(&this->get_payload_buffer()[12], &tmp, 4);
    tmp = this->motor_ctl.get_pid_controller(0).get_kd();
    std::memcpy(&this->get_payload_buffer()[16], &tmp, 4);
    tmp = this->motor_ctl.get_pid_controller(0).get_ki();
    std::memcpy(&this->get_payload_buffer()[20], &tmp, 4);
    
	this->send(4 + (5*4)  );
}