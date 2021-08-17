#ifndef LM393_H
#define LM393_H

#include <mbed.h>

class lm393_tachometer {
public:
    /** Constructor por defecto.
     * @param pin pin de entrada asignado.
     * @param n_ticks_per_rev cantidad de rayos o marcas del encoder por vuelta completa.
     */
    lm393_tachometer(PinName pin,int n_ticks_per_rev);
    
    /** Destructor por defecto.
    */
    ~lm393_tachometer() = default;

    /** Configuración. */
    void setup();

    /** Obtener RPM */
    inline float get_rpm() const { return this->rpm; }

    /** Obtener total de ticks */
    inline float get_total_tick_count() const { return this->counter; }

private:        
    Timer timer;
    InterruptIn irq;
    int counter;
    float dt;
    float rpm;
    float encoder_k;

    void tick();
};

#endif