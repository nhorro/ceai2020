#include "pid.h"

pid_controller::pid_controller(float max, float min, float kp, float kd, float ki )
    :
      max(max)
    , min(min)
    , kp(kp)
    , kd(kd)
    , ki(ki)
    , prev_error(0)
    , integral(0)
{

}

void pid_controller::reset(float max, float min, float kp, float kd, float ki )
{
    this->max = max;
    this->min = min;
    this->kp = kp;
    this->kd = kd;
    this->ki = ki;
    this->prev_error =0;
    this->integral = 0;
}

float pid_controller::process( float dt, float setpoint, float pv )
{
    // 1. Calcular error
    float error = setpoint - pv;

    // 2. Componente proporcional
    float pout = this->kp * error;

    // 3. Componente integral
    this->integral += error * dt;
    float iout = this->ki * this->integral;

    // 3. Componente derivativa
    float derivative = (error - this->prev_error) / dt;
    float dout = this->kd * derivative;

    // Salida
    float output = pout + iout + dout;
    if( output > this->max )
        output = this->max;
    else if( output < this->min )
        output = this->min;

    // Error para próxima iteración
    this->prev_error = error;

    return output;
}