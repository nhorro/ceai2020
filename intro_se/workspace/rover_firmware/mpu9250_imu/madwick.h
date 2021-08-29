/** @file madwick.h Fusión de sensores por algoritmo de Madwick.
 *  @brief Código original: https://github.com/kriswiner/MPU9250/blob/master/STM32F401/MPU9250.h
 */

#ifndef MADWICK_H
#define MADWICK_H

#include <mbed.h>
#include <cmath>

/** Fusión de sensores para AHRS con algoritmo de Madwick.
 */
class madwick_sensor_fusion{
public:
    enum euler_angle_e {
        yaw = 0,
        pitch,
        roll
    };

    // Consantes varias
    static constexpr float PI = 3.14159265358979323846f;

    /** Error del giróscopo en radianes por segundogyroscope measurement error in radianes/seg.
     * @note Comienza a 60 grados/seg, then reduce after ~10 s to 3
    */
    static constexpr float gyroscope_measurement_error = PI * (60.0f / 180.0f);  

    /** Drift del giróscopo en rad/s/s
     * @note Comienza en 0 grados/s/s
    */
    static constexpr float gyroscope_measurement_drift = PI * (1.0f / 180.0f);   


    /** Constructor por defecto
     */
    madwick_sensor_fusion();

    /** Destructor por defecto */
    ~madwick_sensor_fusion()=default;

    void setup();

    /** Actualizar la estimación del cuaternión.
     * @param ax Componente x de vector aceleración (en m/s^2)
     * @param ay Componente y de vector aceleración (en m/s^2)
     * @param az Componente z de vector aceleración (en m/s^2)
     * @param gx Componente x de vector velocidad de giro (en radiantes/seg)
     * @param gy Componente y de vector velocidad de giro (en radiantes/seg)
     * @param gz Componente z de vector velocidad de giro (en radiantes/seg)
     * @param mx Componente x de vector de campo magnético (en milligauss)
     * @param my Componente y de vector de campo magnético (en milligauss)
     * @param mz Componente z de vector de campo magnético (en milligauss)
     * @param dt Delta de tiempo respecto de la última actualización (en segundos).
    */
    void update( float ax, float ay, float az, 
                 float gx, float gy, float gz, 
                 float mx, float my, float mz, float dt );

    /** Obtener el cuaternión de orientación. 
     * @see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
     */
    inline const float* get_quaternion() const { return this->q; }

    /** Obtener orientación en ángulos yaw,pitch,roll (convención Tait-Bryan) 
     * @see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
     */
    inline const float* get_euler_angles() const { return this->euler_angles; }


    /** Convertir cuaternión a ángulos yaw,pitch,roll (convención Tait-Bryan) */
    void convert_to_euler_angles()
    {  
        this->euler_angles[euler_angle_e::yaw] = std::atan2(2.0f * (
            this->q[1] * this->q[2] + this->q[0] * this->q[3]), 
            this->q[0] * this->q[0] + this->q[1] * this->q[1] - 
            this->q[2] * this->q[2] - this->q[3] * this->q[3] );   
        this->euler_angles[euler_angle_e::pitch] = -std::asin(2.0f * 
            (this->q[1] * this->q[3] - this->q[0] * this->q[2]));
        this->euler_angles[euler_angle_e::roll] = std::atan2(2.0f * (
            this->q[0] * this->q[1] + this->q[2] * this->q[3]), this->q[0] * this->q[0] - 
            this->q[1] * this->q[1] - this->q[2] * this->q[2] + this->q[3] * this->q[3] );
        this->euler_angles[euler_angle_e::pitch] *= 180.0f / PI;
        this->euler_angles[euler_angle_e::yaw]    *= 180.0f / PI; 
        //this->euler_angles[euler_angle_e::yaw]    -= 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
        this->euler_angles[euler_angle_e::roll]  *= 180.0f / PI;
    }

private:
    float q[4];
    float euler_angles[3]; // yaw, pitch, roll

    float beta;
    float zeta;    
};

#endif // MADWICK_H