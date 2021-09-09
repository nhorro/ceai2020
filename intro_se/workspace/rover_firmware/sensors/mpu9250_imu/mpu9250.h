/** @file mpu9250.h
 *  @brief Código original https://github.com/kriswiner/MPU9250 adaptado para MBedOS 6.
 */

#ifndef MPU9250_AHRS_H
#define MPU9250_AHRS_H

#include <mbed.h>

#include "mpu9250_registers.h"

/** Clase para acceder a las lecturas de MPU9250.
 */
class mpu9250 {
public:    
    /** Escala del acelerómetro (ver MPU-9250 Register Map and Descriptions Revision 1.4)
     */
    enum accelerometer_scale_e {
        AFS_2G = 0,
        AFS_4G,
        AFS_8G,
        AFS_16G
    };
 
    /** Escala del giróscopo (ver MPU-9250 Register Map and Descriptions Revision 1.4)
     */
    enum gyroscope_scale_e {
        GFS_250DPS = 0,
        GFS_500DPS,
        GFS_1000DPS,
        GFS_2000DPS
    };
 
    /** Escala del magnetómetro (ver MPU-9250 Register Map and Descriptions Revision 1.4)
     */
    enum magnetometer_scale_e {
        MFS_14BITS = 0, // 0.6 mG per LSB
        MFS_16BITS      // 0.15 mG per LSB
    };

    /** Constructor por defecto 
     * @param mpu_addr Dirección del dispositivo MPU (usar dirección 7 bit original, luego se convierte a 8bit).
     * @param mag_addr Dirección del dispositivo MPU (usar dirección 7 bit original, luego se convierte a 8bit).
     * @param sda Pin i2c SDA.
     * @param scl Pin i2c SCL.
     */
    mpu9250(uint8_t mpu_addr,uint8_t mag_addr, PinName sda, PinName scl);

    ~mpu9250() = default;

    /** Configuración inicial de acelerómetro, giróscopo y magnetómetro */
    void setup();

    /** Verificar la conexión con MPU.
     *  Una forma sencilla de verificar la conexión es leyendo el registro WHO_AM_I y comprobando
     *  que el valor retornado es 0x71.
     */
    inline bool mpu_connection_test()
    {
        return (0x71 == this->read_byte( this->mpu_addr8, WHO_AM_I_MPU9250));
    }

    /** Verificar la conexión con magnetómetro.
     *  Una forma sencilla de verificar la conexión es leyendo el registro WHO_AM_I y comprobando
     *  que el valor retornado es 0x71.
     */
    inline bool mag_connection_test()
    {
        return (0x48 == this->read_byte( this->mag_addr8, WHO_AM_I_AK8963));
    }

    /** Llamar a esta función periódicamente para obtener las lecturas del sensor. */
    void process();
    
    /** Indicador de si la última lectura del MPU9250 es válida.
     */
    inline bool is_mpu_valid() const { return this->mpu_valid; }

        
    /** Indicador de si la última lectura del AK8963 es válida.
     */
    inline bool is_mag_valid() const { return this->mag_valid; }

    /** Realizar el Self Test del chip (ver MPU-9250 Register Map and Descriptions Revision 1.4)) */
    void self_test();

    /** Obtener resultados del test expresados en porcentajes del ratio respecto a los valores 
     *  del fabricante. */
    inline const float* get_self_test_results() const { return this->self_test_results; }

    /** Obtener cantidad total de lecturas (cada vez que se invoca process(). */
    inline uint32_t get_total_readings() const { return this->total_readings; }

    /** Obtener cantidad de lecturas en las que MPU9250 pasó el test de conección. */
    inline uint32_t get_mpu9250_readings_ok() const { return this->mpu9250_readings_ok; }

    /** Obtener cantidad de lecturas en las que MPU9250 falló el test de conección. */
    inline uint32_t get_mpu9250_readings_failed() const { return this->mpu9250_readings_failed; }

    /** Obtener cantidad de lecturas en las que magnetrómetro AK8963 pasó el test de conección. */
    inline uint32_t get_ak8963_readings_ok() const { return this->ak8963_readings_ok; }

    /** Obtener cantidad de lecturas en las que magnetrómetro AK8963 falló el test de conección. */
    inline uint32_t get_ak8963_readings_failed() const { return this->ak8963_readings_failed; }

    /** Lecturas de los registros (valores sin convertir) */
    struct raw_readings_t {
        int16_t acc[3];
        int16_t gyro[3];
        int16_t mag[3];        
        int16_t temp;
        bool mag_overflow;
    } __attribute__((aligned (4)));

    /** Obtener últimas lecturas de los registros. */
    const raw_readings_t& get_raw_values() const { return this->raw_readings; }    

    /** Lecturas convertidas a unidades de ingeniería. */
    struct eng_readings_t{
        float acc[3];       /**< Aceleración (metros/seg^2) */
        float gyro[3];      /**< Velocidad de giro (grados/seg) */
        float mag[3];       /**< Intensidad de campo magnético (milliGauss) */
        float temp;         /**< Temperatura en grados Celsius */
    } __attribute__((aligned (4)));


    const eng_readings_t& get_eng_values() const { return this->eng_readings; }    

private:
    // MbedOS usa addresses de 8bit (ver https://os.mbed.com/docs/mbed-os/v6.5/apis/i2c.html)
    uint8_t mpu_addr8; 
    uint8_t mag_addr8; 
    I2C i2c;

    uint32_t total_readings;
    uint32_t mpu9250_readings_ok;
    uint32_t mpu9250_readings_failed;
    uint32_t ak8963_readings_ok;
    uint32_t ak8963_readings_failed;

    bool mpu_valid;
    bool mag_valid;
    raw_readings_t raw_readings;

    eng_readings_t eng_readings;

    void reset_device();
    void configure_mpu9250();
    void configure_ak8963();

    void calibrate_mpu9250(bool use_hw_bias);
    void calibrate_ak8963();

    void read_accelerometer();
    void read_gyroscope();
    void read_magnetometer();
    void read_temperature();

    const uint8_t magnetometer_mode = 0x6;

    const gyroscope_scale_e gyroscope_scale = gyroscope_scale_e::GFS_250DPS; 
    const accelerometer_scale_e accelerometer_scale = accelerometer_scale_e::AFS_2G;
    const magnetometer_scale_e magnetometer_scale = magnetometer_scale_e::MFS_16BITS;

    // Valores de calibración de fábrica
    float mag_calibration[3] = {0, 0, 0};
    
    float sw_gyro_biases[3];
    float sw_acc_biases[3];
    float sw_mag_biases[3];
    float sw_mag_scales[3];

    float get_magnetometer_scale_resolution();
    float get_gyroscope_scale_resolution();
    float get_accelerometer_scale_resolution();

    float self_test_results[6];
    
    // Helpers i2c 
    void write_byte(uint8_t address, char subaddress, uint8_t value);
    char read_byte(uint8_t address, char subaddress);
    void read_bytes(uint8_t address, char subaddress, uint8_t n, char * data);
};

#endif // MPU9250_AHRS_H