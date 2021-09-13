#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

/** Controlador PID 
 */
class pid_controller {
public:
    /** Constructor por defecto.
     * @param kp Ganancia proporcional.
     * @param ki Ganancia integral.
     * @param kd Gananancia derivativa.
     * @param max Valor máximo de la variable de salida.
     * @param min Valor mínimo de la variable de salida.            
     */
    pid_controller(float max, float min, float kp, float kd, float ki );
    ~pid_controller() = default;

    /** Reiniciar el PID
     * @param kp Ganancia proporcional.
     * @param ki Ganancia integral.
     * @param kd Gananancia derivativa.
     * @param max Valor máximo de la variable de salida.
     * @param min Valor mínimo de la variable de salida.      
    */
    void reset(float max, float min, float kp, float kd, float ki );

    /** Calcular salida
     * @param dt tiempo respecto de la última llamada.
     * @param setpoint objetivo.
     * @param pv valor de proceso.
     */
    float process( float dt, float setpoint, float pv );


    float get_kp() const { return this->kp; }
    float get_kd() const { return this->kp; }
    float get_ki() const { return this->kp; }
    float get_max() const { return this->max; }
    float get_min() const { return this->min; }
    float get_prev_error() const { return this->prev_error; }
    float get_integral() const { return this->integral; }
private:
    float max;
    float min;
    float kp;
    float kd;
    float ki;
    float prev_error;
    float integral;
};

#endif // PID_CONTROLLER_H