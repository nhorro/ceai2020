#ifndef L298N_MOTOR_CONTROL_H
#define L298N_MOTOR_CONTROL_H

#include <mbed.h>

#include <stdint.h>

#include "../../controllers/pid.h"

#define DEFAULT_L298_PIN_ENA PB_11 // BLANCO
#define DEFAULT_L298_PIN_IN1 PB_10 // VERDE
#define DEFAULT_L298_PIN_IN2 PE_15 // BLANCO
#define DEFAULT_L298_PIN_IN3 PE_14 // VERDE
#define DEFAULT_L298_PIN_IN4 PE_12 // BLANCO
#define DEFAULT_L298_PIN_ENB PE_10 // VERDE

/* Calibración de velocidad */
//#define MINIMUM_THROTTLE_A		0
//#define MAXIMUM_THROTTLE_A		1024
//#define MINIMUM_THROTTLE_B		0
//#define MAXIMUM_THROTTLE_B		1024
//#define DEFAULT_SPEED 		512
//#define MAXIMUM_THROTTLE    	1024

/** Control de motores con doble puente L298N con PID.
 */
class l298_motor_control{
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
     * @param pid_max Valor máximo de la variable de salida del controlador PID. 
     * @param pid_min Valor mínimo de la variable de salida del controlador PID.  
     * @param pid_kp  Ganancia proporcional del controlador PID.
     * @param pid_kd  Gananancia derivativa del controlador PID.
     * @param pid_ki  Ganancia integral del controlador PID.
     */
    l298_motor_control( PinName ena, PinName in1, PinName in2, PinName enb, PinName in3, PinName in4,
                        float pid_max, float pid_min, float pid_kp, float pid_kd, float pid_ki );

    /** Configuración. */
    void setup();


    /** Obtener el controlador pid de un motor, para reconfigución, pruebas, autotuning. */
    pid_controller& get_pid_controller(int motor_idx) { return this->pids[motor_idx]; }

    
    
    /** Establecer potencia de motores (sólo modo manual). 
     *  En el modo de control manual, se establecerán las velocidades seleccionadas.
     * @param throttles velocidades para motor A y B respectivamente. Valores válidos: (-1.0f a 1.0f)
     * @param mask aplicar a motor A, B o ambos.
    */
    void set_motor_throttles(float throttles[2], uint8_t mask );

    /** Establecer potencia de motores (sólo modo manual). 
     *  En el modo de control manual, se establecerán las velocidades seleccionadas.
     * @param throttles velocidades para motor A y B respectivamente. Valores válidos: ()
     * @param mask aplicar a motor A, B o ambos.
    */
    inline float get_motor_throttle(int motor_idx) const { return this->motors[motor_idx].throttle; }

    /*
     *  En el modo PID, usar se establecerá el setpoint, y se modificará la velocidad 
     *  hasta alcanzar el valor del setpoint.    
     */
    
    /** En el modo PID, se debe actualizar periódicamente la velocidad medida.
     * @param speeds velocidades para el motor A y B respectivamente (se puede 
     *                tomar la de una sola rueda o promediar la delantera y trasera).              
     */
    void set_motor_measured_speeds(float speeds[2]);    
    
    /**  En el modo PID se establecerá el setpoint en RPM, y se modificará la velocidad 
     *  hasta alcanzar el valor del setpoint.    
     * @param speeds velocidades para el motor A y B respectivamente (se puede 
     *                tomar la de una sola rueda o promediar la delantera y trasera).              
     * @param mask aplicar a motor A, B o ambos.
     */
    void set_motor_target_speeds(float speeds[2], uint8_t mask );

    /** Modo de control del motor. Automático (PID) o manual. */
    enum class motor_control_mode {
        manual,
        pid
    };

    /** Establecer el modo de control. */
    inline void set_mode(motor_control_mode mode) { this->control_mode = mode; }

    /** Obtener el modo de control. */
    inline const motor_control_mode get_mode() const { return this->control_mode; }

    /** Actualiza estado de los motores en modo PID. En modo manual no tiene efecto. 
     * @param dt Tiempo en segundos desde la última actualización.
     */
    void update(float dt);
private:
    
    /** Descriptor del estado de cada motor */
    struct motor_descriptor{        
        PwmOut ena_pin;             /**< Pin de control de velocidad */
        DigitalOut in1_pin;         /**< Pin de control de sentido 1 */
        DigitalOut in2_pin;         /**< Pin de control de sentido 2 */

        float throttle;                     /**< Potencia de motor (-1 a 1) */;
        volatile bool is_moving_forward;    /**< Sentido (1=adelate,0=atrás). */        
        volatile float measured_speed;      /**< Velocidad medida (se provee externamente) */        
        float setpoint_speed;               /**< Velocidad objetivo */      

    };
    motor_descriptor motors[2];
    pid_controller   pids[2];
        
    volatile motor_control_mode control_mode;

    /** Escribir la potencia actual del motor. 
     * @param motor_idx Índice del motor (0-1)
     * @param throttle Potencia (-1.0f a 1.0f)
     */
    void write_throttle(int motor_idx,float throttle);
};

#endif // L298N_MOTOR_CONTROL_H
