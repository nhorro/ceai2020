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

void lm393_tachometer::setup()
{
    /* Registrar interrupción de incremento en flanco descendente. */
    this->irq.fall(callback(this, &lm393_tachometer::tick));
    //this->irq.rise(callback(this, &lm393_tachometer::tick));    
    this->irq.enable_irq();    
    this->timer.start();
}

void lm393_tachometer::tick() 
{
    this->dt = timer.elapsed_time().count();
    this->timer.reset(); 
    /* Evitar valores fuera de rango */    
    this->rpm = (this->dt <= lm393_tachometer::max_interval) ? 
            ( this->encoder_k  / (this->dt/float(1000000.0f))) *60.0f : 0.0f;  
    this->counter++;
    this->debug_led.write(!this->debug_led.read());
}

float lm393_tachometer::get_rpm() 
{
    /* Evitar valores fuera de rango */    
    if(this->timer.elapsed_time().count()>= lm393_tachometer::max_interval)
    {   
        this->rpm = 0.0f;
    }    

    /* A frecuencias muy bajas se producen errores ocasionales. */   
    if( this->rpm > 1000 )
    {        
        this->rpm = 0.0f;
    }    

    return this->rpm;
}

