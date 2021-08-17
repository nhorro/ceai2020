#include "l298n_motor_control.h"

#define DEFAULT_MOTOR_ACC 1.0 

l298_motor_control::l298_motor_control() 
: 
  motors {
	    { L298_PIN_ENA, L298_PIN_IN1, L298_PIN_IN2, 0.0, 0.0 },
	    { L298_PIN_ENB, L298_PIN_IN3, L298_PIN_IN4, 0.0, 0.0 }
	}
    , control_mode(motor_control_mode::manual)
{

}

void l298_motor_control::setup()
{    

}

void l298_motor_control::set_motor_speeds(int16_t speeds[2], uint8_t mask )
{
 	for(int i=0;i<2;i++)
 	{
	    if ( mask & (motor_control_flags::motor_a<<i) ) 
	    {	    	
			this->motors[i].setpoint_speed = float(speeds[i])/MAXIMUM_SPEED;
			this->write_speed(i,this->motors[i].setpoint_speed);
	    }        
	}
}

void l298_motor_control::update()
{
    if (this->control_mode == motor_control_mode::pid)
    {
        // FIXME PID
    }
}

void l298_motor_control::write_speed(int motor_idx,float speed)
{
    /* El sentido se controla con los pines 1 y 2. */
    if ( this->motors[motor_idx].setpoint_speed >= 0 ) 
    {
        this->motors[motor_idx].in1_pin = 1;
        this->motors[motor_idx].in2_pin = 0;
    }
    else
    {
        this->motors[motor_idx].in1_pin = 0;
        this->motors[motor_idx].in2_pin = 1;        
    } 

    /* La intensidad con el ancho de pulso del pin ENA. */
    this->motors[motor_idx].ena_pin.write( abs(speed) );  
}