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

    /** Calcular salida
     * @param dt tiempo respecto de la última llamada.
     * @param setpoint objetivo.
     * @param pv valor de proceso.
     */
    float process( float dt, float setpoint, float pv );
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