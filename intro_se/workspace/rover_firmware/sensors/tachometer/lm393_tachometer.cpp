#include "lm393_tachometer.h"

lm393_tachometer::lm393_tachometer(PinName pin,int n_ticks_per_rev):
          irq(pin) //PullUp, PullDown, PullNone, PullDefault
        , counter(0)
        , dt(0)
        , rpm(0)
        , encoder_k(1.0f/n_ticks_per_rev)
        , debug_mode(false)
        , debug_led(LED2)
{    
}

void lm393_tachometer::setup(bool debug_mode)
{
    /* Registrar interrupción de incremento en flanco descendente. */
    this->irq.fall(callback(this, &lm393_tachometer::tick));
    //this->debug_mode = debug_mode;
    this->irq.enable_irq();
    //this->irq.rise(callback(this, &lm393_tachometer::tick));
    this->timer.start();
}

void lm393_tachometer::tick() 
{
    this->dt = timer.elapsed_time().count();
    this->timer.reset(); 
    this->rpm = ( this->encoder_k  / (this->dt/1000000.0f) ) *60.0f ;  
    this->counter++;
    
    //if ( this->debug_mode )
    //{
    this->debug_led.write(!this->debug_led.read());
    //}
    
}