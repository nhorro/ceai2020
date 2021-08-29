#include "l298n_motor_control.h"

#define DEFAULT_MOTOR_ACC 1.0 

l298_motor_control::l298_motor_control(PinName ena, PinName in1, PinName in2, PinName enb, PinName in3, PinName in4) 
: 
  motors {
	    { ena, in1, in2, 0.0, 0.0 },
	    { enb, in3, in4, 0.0, 0.0 }
	}
    , control_mode(motor_control_mode::manual)
{

}

void l298_motor_control::setup()
{    
    // Para configuraciones más específicas en el futuro.
}

void l298_motor_control::set_motor_throttles(int16_t throtttles[2], uint8_t mask )
{
 	for(int i=0;i<2;i++)
 	{
	    if ( mask & (motor_control_flags::motor_a<<i) ) 
	    {	    	
			this->write_throttle(i,float(throtttles[i])/MAXIMUM_THROTTLE);
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

void l298_motor_control::write_throttle(int motor_idx,float throttle)
{
    /* El sentido se controla con los pines 1 y 2. */
    if ( throttle >= 0 ) 
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
    this->motors[motor_idx].ena_pin.write( abs(throttle) );  
}