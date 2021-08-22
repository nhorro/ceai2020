#ifndef L298N_MOTOR_CONTROL_H
#define L298N_MOTOR_CONTROL_H

#include <mbed.h>

#include <stdint.h>

#define DEFAULT_L298_PIN_ENA D0 // FIXME
#define DEFAULT_L298_PIN_IN1 D1 // FIXME
#define DEFAULT_L298_PIN_IN2 D2 // FIXME
#define DEFAULT_L298_PIN_IN3 D3 // FIXME
#define DEFAULT_L298_PIN_IN4 D4 // FIXME
#define DEFAULT_L298_PIN_ENB D5 // FIXME

/* Calibración de velocidad */
#define MINIMUM_SPEED_A		0
#define MAXIMUM_SPEED_A		1024
#define MINIMUM_SPEED_B		0
#define MAXIMUM_SPEED_B		1024
#define DEFAULT_SPEED 		512
#define MAXIMUM_SPEED 		1024



/** Control de motores con doble puente L298N-
 */
class l298_motor_control {
public:
    /** Flags para seleccionar motores en funciones de control. */
    enum motor_control_flags {
        motor_a = 0x1,
        motor_b = 0x2
    };

    /** Constructor por defecto.
     * @param ena Identificador de pin para control de velocidad de motor A (PWM).
     * @param in1 Control de sentido de motor A, pin 1 (pin de salida digital).
     * @param in2 Control de sentido de motor A, pin 2 (pin de salida digital).
     * @param enb Identificador de pin para control de velocidad de motor B.
     * @param in3 Control de sentido de motor B, pin 3 (pin de salida digital).
     * @param in4 Control de sentido de motor B, pin 4 (pin de salida digital).
     */
    l298_motor_control( PinName ena, PinName in1, PinName in2, PinName enb, PinName in3, PinName in4);

    /** Configuración. */
    void setup();
    
    /** Establecer velocidades. 
     *  En el modo de control manual, se establecerán las velocidades seleccionadas.
     *  En el modo PID, se establecerá el setpoint, y se modificará la velocidad 
     *  hasta alcanzar el valor del setpoint.    
     * @param speeds velocidades para motor A y B respectivamente. Valores válidos 
     * @param mask aplicar a motor A, B o ambos.
    */
    void set_motor_speeds(int16_t speeds[2], uint8_t mask );

    /** Modo de control del motor. Automático (PID) o manual. */
    enum class motor_control_mode {
        manual,
        pid
    };

    /** Establecer el modo de control. */
    inline void set_mode(motor_control_mode mode) { this->control_mode = mode; }

    /** Actualiza estado de los motores en modo PID. En modo manual no tiene efecto. */
    void update();
private:
    
    /** Descriptor del estado de cada motor */
    struct motor_descriptor{        
        PwmOut ena_pin;         /**< Pin de control de velocidad */
        DigitalOut in1_pin;     /**< Pin de control de sentido 1 */
        DigitalOut in2_pin;     /**< Pin de control de sentido 2 */

        float measured_speed;   /**< Velocidad medida (se provee externamente) */        
        float setpoint_speed;   /**< Velocidad objetivo */        
    };
    motor_descriptor motors[2];

    motor_control_mode control_mode;

    /** Escribir la velocidad actual del motor. 
     * @param motor_idx Índice del motor (0-1)
     * @param speed Velocidad (-1.0f a 1.0f)
     */
    void write_speed(int motor_idx,float speed);
};

#endif // L298N_MOTOR_CONTROL_H
