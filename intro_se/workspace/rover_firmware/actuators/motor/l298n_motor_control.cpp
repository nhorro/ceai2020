#include "l298n_motor_control.h"

#define DEFAULT_MOTOR_ACC 1.0 

l298_motor_control::l298_motor_control(PinName ena, PinName in1, PinName in2, 
                                       PinName enb, PinName in3, PinName in4,
                                       float pid_max, float pid_min, 
                                       float pid_kp, float pid_kd, float pid_ki ) 
: 
    motors {
	    { ena, in1, in2, 0.0f, true, 0.0f, 0.0f },
	    { enb, in3, in4, 0.0f, true, 0.0f, 0.0f }
	}
    , pids{
        { pid_max, pid_min, pid_kp, pid_kd, pid_ki },
        { pid_max, pid_min, pid_kp, pid_kd, pid_ki }
    }
    , control_mode(motor_control_mode::manual)
{

}

void l298_motor_control::setup()
{    
    // Para configuraciones más específicas en el futuro.
}

void l298_motor_control::set_motor_throttles(float throttles[2], uint8_t mask )
{
    // Establecer modo manual. Esto es inseguro, debería estar en critical section.
    this->set_mode(l298_motor_control::motor_control_mode::manual);
    if( mask & motor_control_flags::motor_a )
    {
        this->write_throttle(0,throttles[0]);
    }
 	if( mask & motor_control_flags::motor_b )
    {
        this->write_throttle(1,throttles[1]);
    }
}

   
void l298_motor_control::set_motor_measured_speeds(float speeds[2])
{
    // Las velocidades medidas por el tacómetro, por el tipo de encoders, no tienen
    // dirección. Por lo tanto, se debe usar el último valor escrito en los motores 
    // para calcular la dirección.
    this->motors[0].measured_speed = this->motors[0].is_moving_forward ? speeds[0] : -speeds[0];
    this->motors[1].measured_speed = this->motors[1].is_moving_forward ? speeds[1] : -speeds[1];
}
    
void l298_motor_control::set_motor_target_speeds(float speeds[2], uint8_t mask )
{
    // Establecer modo PID. Esto es inseguro, debería estar en critical section.
    this->set_mode(l298_motor_control::motor_control_mode::pid);
    if ( mask & motor_control_flags::motor_a ) 
    {	    	
        this->motors[0].setpoint_speed = speeds[0];
    }  

    if ( mask & motor_control_flags::motor_b ) 
    {	    	
        this->motors[1].setpoint_speed = speeds[1];
    }  
}

void l298_motor_control::update(float dt)
{
    if (this->control_mode == motor_control_mode::pid)
    {
        for(int i=0;i<2;i++)
        {
            float new_throttle = this->pids[i].process(dt, 
                this->motors[i].setpoint_speed, 
                this->motors[i].measured_speed );
            
            /* Para evitar saltos abruptos, solo reescribir la salida si hubo un cambio 
               significativo respecto de la última actualización */
            //float diff = std::abs(new_throttle - this->motors[i].throttle);            
            //if(diff >= 0.01)
            //{
            //if (new_throttle < 0.2 )
            //{
            // new_throttle = 0;
            // }
            this->write_throttle(i, new_throttle);
            //}
        }
    }
    else {
        // En modo manual no se hace nada.
    }

    
}

void l298_motor_control::write_throttle(int motor_idx,float throttle)
{
    this->motors[motor_idx].throttle = throttle;

    /* El sentido se controla con los pines 1 y 2. */
    if ( throttle >= 0 ) 
    {
        this->motors[motor_idx].is_moving_forward = true;
        this->motors[motor_idx].in1_pin = 1;
        this->motors[motor_idx].in2_pin = 0;
    }
    else
    {
        this->motors[motor_idx].is_moving_forward = false;
        this->motors[motor_idx].in1_pin = 0;
        this->motors[motor_idx].in2_pin = 1;        
    } 

    /* La intensidad con el ancho de pulso del pin ENA/B. */
    this->motors[motor_idx].ena_pin.write( abs(this->motors[motor_idx].throttle) );  
}