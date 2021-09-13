#ifndef LM393_H
#define LM393_H

#include "DigitalOut.h"
#include <mbed.h>

#define DEFAULT_TACHO1_PIN PE_7
#define DEFAULT_TACHO2_PIN PE_8
#define DEFAULT_TACHO3_PIN PG_9
#define DEFAULT_TACHO4_PIN PG_14

class lm393_tachometer {
public:
    static constexpr uint32_t max_interval = 1000000;
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
    float get_rpm();

    /** Obtener total de ticks */
    inline uint32_t get_total_tick_count() const { return this->counter; }

private:        
    Timer timer;
    InterruptIn irq;
    volatile uint32_t counter;
    volatile float dt;
    volatile float rpm;
    const float encoder_k;
    
    bool debug_mode;

    void tick();    

    DigitalOut debug_led;
};

#endif