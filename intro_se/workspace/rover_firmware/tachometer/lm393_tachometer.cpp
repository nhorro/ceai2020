#include "lm393_tachometer.h"

lm393_tachometer::lm393_tachometer(PinName pin,int n_ticks_per_rev):
          irq(pin) 
        , counter(0)
        , dt(0)
        , rpm(0)
        , encoder_k(1.0f/n_ticks_per_rev)
{
    /* Registrar interrupción de incremento en flaco descendente. */
    this->irq.fall(callback(this, &lm393_tachometer::tick));
}

void lm393_tachometer::setup()
{
    // Modo debugging?
}

void lm393_tachometer::tick() 
{
    this->dt = timer.elapsed_time().count();
    this->timer.reset(); 
    this->rpm = ( this->encoder_k  / (this->dt/1000000.0f) ) *60.0f ;  
    this->counter++;
}