/** @file mpu9250.cpp
 *  @brief Código original https://github.com/kriswiner/MPU9250 adaptado para MBedOS 6.
 */

#include "mpu9250.h"

mpu9250::mpu9250(uint8_t mpu_addr,uint8_t mag_addr, PinName sda, PinName scl):
    // MbedOS usa addresses de 8bit (ver https://os.mbed.com/docs/mbed-os/v6.5/apis/i2c.html)
    mpu_addr8(mpu_addr<<1),     /**< Dirección i2c MPU9250 */
    mag_addr8(mag_addr<<1),     /**< Dirección i2c AK8963 */
    i2c(sda, scl),          
    mpu_valid(false),           /**< ¿Hay conectividad con MPU9250? */
    mag_valid(false),           /**< ¿Hay conectividad con AK8963? */
    total_readings(0),          /**< Intentos de lectura (total) */
    mpu9250_readings_ok(0),     /**< Lecturas exitosas de MPU9250 */
    mpu9250_readings_failed(0), /**< Lecturas fallidas de MPU9250 */
    ak8963_readings_ok(0),      /**< Lecturas exitosas de AK8963 */
    ak8963_readings_failed(0)   /**< Lecturas fallidas de AK8963 */
{
    
}


void mpu9250::setup()
{
    /* Proceso de inicialización */

    // Configurar I2C en modo rápido.
    this->i2c.frequency(400000);  // 400 kHz I2C

    this->reset_device();
    this->calibrate_mpu9250(false);
    this->configure_mpu9250();
    this->configure_ak8963();    
}


void mpu9250::configure_mpu9250()
{   
    // Initialize MPU9250 device
    // wake up device
    // Clear sleep mode bit (6), enable all sensors 

    // Wake up con todos los sensores habilitados. 
    // Sleep mode (bit 6) en 0.
    this->write_byte(this->mpu_addr8, PWR_MGMT_1, 0x00); 

    // Esperar el PLL. 
    thread_sleep_for(100);
     
    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    this->write_byte(this->mpu_addr8, PWR_MGMT_1, 0x01); 
 
    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
    this->write_byte(this->mpu_addr8, CONFIG, 0x03);  
 
    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
    // Use a 200 Hz rate; the same rate set in CONFIG above
    this->write_byte(this->mpu_addr8, SMPLRT_DIV, 0x04);  

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    uint8_t cfg =  this->read_byte(this->mpu_addr8, GYRO_CONFIG);

    // Clear self-test bits [7:5] 
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, cfg & ~0xE0); 

    // Clear AFS bits [4:3]
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, cfg & ~0x18); 

    // Set full scale range for the gyro
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, cfg | this->gyroscope_scale << 3); 

    // Set accelerometer configuration
    cfg =  this->read_byte(this->mpu_addr8, ACCEL_CONFIG);

    // Clear self-test bits [7:5] 
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, cfg & ~0xE0); 

    // Clear AFS bits [4:3]
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, cfg & ~0x18); 

    // Set full scale range for the accelerometer 
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, cfg | this->accelerometer_scale << 3); 
 
    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
    cfg = this->read_byte(this->mpu_addr8, ACCEL_CONFIG2);

    // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG2, cfg & ~0x0F);

    // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz 
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG2, cfg | 0x03); 
 
    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
    // but all these rates are further reduced by a factor of 5 to 200 Hz because 
    // of the SMPLRT_DIV setting
 
    // Configure Interrupts and Bypass Enable
    // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, 
    // enable I2C_BYPASS_EN so additional chips 
    // can join the I2C bus and all can be controlled by the Arduino as master

    // INT is 50 microsecond pulse and any read to clear  
    this->write_byte(this->mpu_addr8, INT_PIN_CFG, 0x12); 

    // Enable data ready (bit 0) interrupt 
    this->write_byte(this->mpu_addr8, INT_ENABLE, 0x01);  
}

void  mpu9250::configure_ak8963()
{
    // First extract the factory calibration for each magnetometer axis
    uint8_t data[3] = {0, 0, 0};  

    // Power down magnetometer  
    this->write_byte(this->mag_addr8, AK8963_CNTL, 0x00); 
    
    // Enter Fuse ROM access mode
    thread_sleep_for(100);
    this->write_byte(this->mag_addr8,AK8963_CNTL, 0x0F); 
    thread_sleep_for(100);

    // readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  
    // Read the x-, y-, and z-axis calibration values
    this->read_bytes(this->mag_addr8, AK8963_ASAX, 3, (char*)data);
    
    // Return x-axis sensitivity adjustment values, etc.
    this->mag_calibration[0] =  (data[0] - 128.0f)/256.0f + 1.0f; 
    this->mag_calibration[1] =  (data[1] - 128.0f)/256.0f + 1.0f;  
    this->mag_calibration[2] =  (data[2] - 128.0f)/256.0f + 1.0f; 
    this->write_byte(this->mag_addr8, AK8963_CNTL, 0x00); // Power down magnetometer  
    thread_sleep_for(100);

    // Configure the magnetometer for continuous read and highest resolution
    // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
    // and enable continuous mode data acquisition Mmode (bits [3:0]),
    // 0010 for 8 Hz and 0110 for 100 Hz sample rates
    // Mode: Either 8 Hz 0x02) or 100 Hz (0x06) magnetometer data ODR  
    // Set magnetometer data resolution and sample ODR
    this->write_byte(this->mag_addr8,AK8963_CNTL, 
        this->magnetometer_scale << 4 | this->magnetometer_mode); 
    thread_sleep_for(100);
}

void mpu9250::reset_device()
{
    // Write a one to bit 7 reset bit; toggle reset device
    this->write_byte(this->mpu_addr8,PWR_MGMT_1, 0x80); 
    thread_sleep_for(100);
}



void mpu9250::calibrate_mpu9250(bool use_hw_bias)
{
    // Function which accumulates gyro and accelerometer data after 
    // device initialization. It calculates the average of the at-rest 
    // readings and then loads the resulting offsets into accelerometer 
    // and gyro bias registers.

    uint8_t data[12]; 
    int32_t gyro_bias[3] = {0, 0, 0};
    int32_t accel_bias[3] = {0, 0, 0};
    
    this->reset_device();
   
    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    this->write_byte(this->mpu_addr8, PWR_MGMT_1, 0x01);  
    this->write_byte(this->mpu_addr8, PWR_MGMT_2, 0x00); 
    thread_sleep_for(200);
  
    // Configure device for bias calculation

    // Disable all interrupts
    this->write_byte(this->mpu_addr8, INT_ENABLE, 0x00);   

    // Disable FIFO
    this->write_byte(this->mpu_addr8, FIFO_EN, 0x00);      

    // Turn on internal clock source
    this->write_byte(this->mpu_addr8, PWR_MGMT_1, 0x00);   

    // Disable I2C master
    this->write_byte(this->mpu_addr8, I2C_MST_CTRL, 0x00); 

    // Disable FIFO and I2C master modes
    this->write_byte(this->mpu_addr8, USER_CTRL, 0x00);    

    // Reset FIFO and DMP
    this->write_byte(this->mpu_addr8, USER_CTRL, 0x0C);    
    thread_sleep_for(15);
  
    // Configure MPU9250 gyro and accelerometer for bias calculation

    // Set low-pass filter to 188 Hz
    this->write_byte(this->mpu_addr8, CONFIG, 0x01);      

    // Set sample rate to 1 kHz
    this->write_byte(this->mpu_addr8, SMPLRT_DIV, 0x00);  

    // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, 0x00);  

    // Set accelerometer full-scale to 2 g, maximum sensitivity
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, 0x00); 
 
    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
 
    // Configure FIFO to capture accelerometer and gyro data for bias calculation

    // Enable FIFO  
    this->write_byte(this->mpu_addr8, USER_CTRL, 0x40);   

    // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    this->write_byte(this->mpu_addr8, FIFO_EN, 0x78);     

    // accumulate 40 samples in 80 milliseconds = 480 bytes
    thread_sleep_for(40); 
 
    // At end of sample accumulation, turn off FIFO sensor read

    // Disable gyro and accelerometer sensors for FIFO
    this->write_byte(this->mpu_addr8, FIFO_EN, 0x00);        

    // read FIFO sample count
    this->read_bytes(this->mpu_addr8, FIFO_COUNTH, 2, (char*)data); 
    uint16_t fifo_count = ((uint16_t)data[0] << 8) | data[1];
    
    // How many sets of full gyro and accelerometer data for averaging
    uint16_t packet_count = fifo_count/12;
 
    for (int i = 0; i < packet_count; i++) 
    {
        int16_t accel_temp[3] = {0, 0, 0};
        int16_t  gyro_temp[3] = {0, 0, 0};
        
        // read data for averaging
        this->read_bytes(this->mpu_addr8,  FIFO_R_W, 12, (char*)data); 

        // Form signed 16-bit integer for each sample in FIFO
        accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  
        accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
        accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
        gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
        gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
        gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;
    
        // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[0] += (int32_t) accel_temp[0]; 
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];
        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];
    }
    
    // Normalize sums to get average count biases
    accel_bias[0] /= (int32_t) packet_count; 
    accel_bias[1] /= (int32_t) packet_count;
    accel_bias[2] /= (int32_t) packet_count;
    gyro_bias[0]  /= (int32_t) packet_count;
    gyro_bias[1]  /= (int32_t) packet_count;
    gyro_bias[2]  /= (int32_t) packet_count;
    
    if(accel_bias[2] > 0L) 
    {
        // Remove gravity from the z-axis accelerometer bias calculation
        accel_bias[2] -= (int32_t) accelsensitivity;
    }  
    else 
    {
        accel_bias[2] += (int32_t) accelsensitivity;
    }
 
    // Construct the gyro biases for push to the hardware gyro bias registers, 
    // which are reset to zero upon device startup
    // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    // Biases are additive, so change sign on calculated average gyro biases
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; 
    data[1] = (-gyro_bias[0]/4)       & 0xFF; 
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    if(use_hw_bias)
    {
        /// Push gyro biases to hardware registers
        this->write_byte(this->mpu_addr8, XG_OFFSET_H, data[0]);
        this->write_byte(this->mpu_addr8, XG_OFFSET_L, data[1]);
        this->write_byte(this->mpu_addr8, YG_OFFSET_H, data[2]);
        this->write_byte(this->mpu_addr8, YG_OFFSET_L, data[3]);
        this->write_byte(this->mpu_addr8, ZG_OFFSET_H, data[4]);
        this->write_byte(this->mpu_addr8, ZG_OFFSET_L, data[5]);

        this->sw_gyro_biases[0] = 0.0f; 
        this->sw_gyro_biases[1] = 0.0f; 
        this->sw_gyro_biases[2] = 0.0f; 
    }
    else 
    {
        // construct gyro bias in deg/s for later manual subtraction
        this->sw_gyro_biases[0] = (float) gyro_bias[0]/(float) gyrosensitivity; 
        this->sw_gyro_biases[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
        this->sw_gyro_biases[2] = (float) gyro_bias[2]/(float) gyrosensitivity;
    }
 
    /* Construct the accelerometer biases for push to the hardware accelerometer 
       bias registers. These registers contain factory trim values which must be 
       added to the calculated accelerometer biases; on boot up these registers 
       will hold non-zero values. In addition, bit 0 of the lower byte must be 
       preserved since it is used for temperature compensation calculations. 
       Accelerometer bias registers expect bias input as 2048 LSB per g, so that
       the accelerometer biases calculated above must be divided by 8. */
       
    // A place to hold the factory accelerometer trim biases
    int32_t accel_bias_reg[3] = {0, 0, 0}; 

    // Read factory accelerometer trim values
    this->read_bytes(this->mpu_addr8, XA_OFFSET_H, 2, (char*)data); 
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    this->read_bytes(this->mpu_addr8, YA_OFFSET_H, 2, (char*)data);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    this->read_bytes(this->mpu_addr8, ZA_OFFSET_H, 2, (char*)data);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  
    // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint32_t mask = 1uL; 

    // Define array to hold mask bit for each accelerometer bias axis
    uint8_t mask_bit[3] = {0, 0, 0}; 
  
    for(int i = 0; i < 3; i++) 
    {
        // If temperature compensation bit is set, record that fact in mask_bit
        if(accel_bias_reg[i] & mask) mask_bit[i] = 0x01; 
    }
 
    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[0] -= (accel_bias[0]/8); 
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);
 
    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;

    // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[1] = data[1] | mask_bit[0]; 
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;

    // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[3] = data[3] | mask_bit[1]; 
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;

    // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[5] = data[5] | mask_bit[2]; 
 
    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
    if(use_hw_bias)
    {
        this->write_byte(this->mpu_addr8, XA_OFFSET_H, data[0]);
        this->write_byte(this->mpu_addr8, XA_OFFSET_L, data[1]);
        this->write_byte(this->mpu_addr8, YA_OFFSET_H, data[2]);
        this->write_byte(this->mpu_addr8, YA_OFFSET_L, data[3]);
        this->write_byte(this->mpu_addr8, ZA_OFFSET_H, data[4]);
        this->write_byte(this->mpu_addr8, ZA_OFFSET_L, data[5]);
        this->sw_acc_biases[0] = 0.0f;
        this->sw_acc_biases[1] = 0.0f;
        this->sw_acc_biases[2] = 0.0f;
    }
    else 
    {
        // Output scaled accelerometer biases for manual subtraction in the main program    
        this->sw_acc_biases[0] = (float)accel_bias[0]/(float)accelsensitivity; 
        this->sw_acc_biases[1] = (float)accel_bias[1]/(float)accelsensitivity;
        this->sw_acc_biases[2] = (float)accel_bias[2]/(float)accelsensitivity;
    }
}


void mpu9250::calibrate_ak8963()
{    
    int32_t mag_bias[3] = {0, 0, 0};
    int32_t mag_scale[3] = {0, 0, 0};
    int16_t mag_max[3] = {-32767, -32767, -32767};
    int16_t mag_min[3] = {32767, 32767, 32767};
    int16_t mag_temp[3] = {0, 0, 0};
  
    // shoot for ~fifteen seconds of mag data
    uint16_t sample_count;
    if(0x02 == this->magnetometer_mode)
    {
        sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms
    }
    else 
    {
        sample_count = 1500; // at 100 Hz ODR, new mag data is available every 10 ms
    }
    
    for(int i = 0; i < sample_count; i++) 
    {
        this->read_magnetometer();
        for (int j = 0; j < 3; j++) 
        {
            if(this->raw_readings.mag[j] > mag_max[j]) 
                mag_max[j] = mag_temp[j];
            if(this->raw_readings.mag[j] < mag_min[j]) 
                mag_min[j] = mag_temp[j];
        }

        switch(this->magnetometer_mode)
        {
            case 0x06: {
                // at 100 Hz ODR, new mag data is available every 10 ms
                thread_sleep_for(12);
            } break;

            case 0x02: {
                // at 8 Hz ODR, new mag data is available every 125 ms
                thread_sleep_for(135);
            } break;
        }
    }
 
    // Get hard iron correction
    mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
    mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
    mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts
    
    float mres = this->get_magnetometer_scale_resolution();
    this->sw_mag_biases[0] = (float) mag_bias[0]*mres*this->mag_calibration[0];  
    this->sw_mag_biases[1] = (float) mag_bias[1]*mres*this->mag_calibration[1];   
    this->sw_mag_biases[2] = (float) mag_bias[2]*mres*this->mag_calibration[2];  
       
    // Get soft iron correction estimate
    mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
    mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
    mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts
 
    float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
    avg_rad /= 3.0;
 
    this->sw_mag_scales[0] = avg_rad/((float)mag_scale[0]);
    this->sw_mag_scales[1] = avg_rad/((float)mag_scale[1]);
    this->sw_mag_scales[2] = avg_rad/((float)mag_scale[2]);
}

void mpu9250::process() 
{
    this->total_readings++;
    
    if((this->mpu_valid = this->mpu_connection_test()))
    {
        this->mpu9250_readings_ok++;
        
        /*
        printf("Conexión con MPU9250 OK (%d/%d) \n\r", 
            this->mpu9250_readings_ok, this->total_readings);
        */
    }
    else 
    {
        this->mpu9250_readings_failed++;

        /*
        printf("Se perdió conexión con MPU9250 (%d/%d) \n\r", 
            this->mpu9250_readings_failed, this->total_readings);
        */            
    }

    if((this->mag_valid = this->mag_connection_test()))
    {
        this->ak8963_readings_ok++; 
        
        /*
        printf("Conexión con AK8963 OK (%d/%d) \n\r", 
            this->ak8963_readings_ok, this->total_readings);        
        */
    }
    else 
    {
        this->ak8963_readings_failed++;

        /*
        printf("Se perdió conexión con AK8963 (%d/%d)\n\r", 
            this->ak8963_readings_failed, this->total_readings);
        */
    }

    if ( this->mpu_valid & this->mag_valid )
    {
        // If intPin goes high, all data registers have new data
        if(this->read_byte(this->mpu_addr8, INT_STATUS) & 0x01) 
        { 
            // Leer acelerómetro            
            this->read_accelerometer();

            float a_res = this->get_accelerometer_scale_resolution();
            this->eng_readings.acc[0] = (float)this->raw_readings.acc[0]*a_res-this->sw_acc_biases[0];
            this->eng_readings.acc[1] = (float)this->raw_readings.acc[1]*a_res-this->sw_acc_biases[1];
            this->eng_readings.acc[2] = (float)this->raw_readings.acc[2]*a_res-this->sw_acc_biases[2];

            // Leer giróscopo
            this->read_gyroscope();

            float gyro_res = this->get_gyroscope_scale_resolution();

            this->eng_readings.gyro[0] = (float)this->raw_readings.gyro[0]*gyro_res-this->sw_gyro_biases[0];
            this->eng_readings.gyro[1] = (float)this->raw_readings.gyro[1]*gyro_res-this->sw_gyro_biases[1];
            this->eng_readings.gyro[2] = (float)this->raw_readings.gyro[2]*gyro_res-this->sw_gyro_biases[2];

            // Leer magnetómetro
            this->read_magnetometer();

            float mag_res = this->get_magnetometer_scale_resolution();

            this->eng_readings.mag[0] = (float)this->raw_readings.mag[0]*mag_res-this->sw_mag_biases[0];
            this->eng_readings.mag[1] = (float)this->raw_readings.mag[1]*mag_res-this->sw_mag_biases[1];
            this->eng_readings.mag[2] = (float)this->raw_readings.mag[2]*mag_res-this->sw_mag_biases[2];

            // Leer temperatura
            this->read_temperature();

            // Convertir a grados Celsius.
            this->eng_readings.temp = ((float) this->raw_readings.temp) / 333.87f + 21.0f; 
        }
    }
}
   
void mpu9250::read_accelerometer()
{    
    uint8_t data[6];  
    this->read_bytes(this->mpu_addr8, ACCEL_XOUT_H, 6, (char*)data);  
    this->raw_readings.acc[0] = (((int16_t)data[0] << 8) | data[1]);
    this->raw_readings.acc[1] = (((int16_t)data[2] << 8) | data[3]);  
    this->raw_readings.acc[2] = (((int16_t)data[4] << 8) | data[5]); 
}

void mpu9250::read_gyroscope()
{    
    uint8_t data[6];  
    this->read_bytes(this->mpu_addr8, GYRO_XOUT_H, 6, (char*)data);  
    this->raw_readings.gyro[0] = (((int16_t)data[0] << 8) | data[1]);
    this->raw_readings.gyro[1] = (((int16_t)data[2] << 8) | data[3]);  
    this->raw_readings.gyro[2] = (((int16_t)data[4] << 8) | data[5]); 
}

void mpu9250::read_magnetometer()
{    
    // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
    uint8_t data[7];  
    // Read the six raw data and ST2 registers sequentially into data array
    this->read_bytes(this->mag_addr8, AK8963_XOUT_L, 7, (char*)data);      

    // Check if magnetic sensor overflow set, if not then report data
    if(!(data[6] & 0x08)) 
    { 
        this->raw_readings.mag_overflow = false;
        this->raw_readings.mag[0] = ((int16_t)data[1] << 8) | data[0] ; 
        this->raw_readings.mag[1] = ((int16_t)data[3] << 8) | data[2] ; 
        this->raw_readings.mag[2] = ((int16_t)data[5] << 8) | data[4] ; 
   }
   else {
        this->raw_readings.mag_overflow = true;
   }
}

float mpu9250::get_magnetometer_scale_resolution() 
{
    float mag_res;
    switch (this->magnetometer_scale)
    {
        // Possible magnetometer scales (and their register bit settings) are:
        // 14 bit resolution (0) and 16 bit resolution (1)
        case mpu9250::magnetometer_scale_e::MFS_14BITS: {
            mag_res = 10.0*4912.0/8190.0; // Proper scale to return milliGauss
        } break;
          
          break;
        
        case mpu9250::magnetometer_scale_e::MFS_16BITS: {
            mag_res = 10.0*4912.0/32760.0; // Proper scale to return milliGauss
        } break;
  }
  return mag_res;
}

float mpu9250::get_gyroscope_scale_resolution() 
{
    float gyro_res;
    switch (this->gyroscope_scale)
    {
        // Possible gyro scales (and their register bit settings) are:
        // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
        case gyroscope_scale_e::GFS_250DPS: {
          gyro_res = 250.0f/32768.0f;
        } break;
        case gyroscope_scale_e::GFS_500DPS: {
          gyro_res = 500.0f/32768.0f;
        }  break;
        case gyroscope_scale_e::GFS_1000DPS: {
          gyro_res = 1000.0f/32768.0f;
        } break;
        case gyroscope_scale_e::GFS_2000DPS: {
          gyro_res = 2000.0f/32768.0f;
        } break;
    }
    return gyro_res;
}

float mpu9250::get_accelerometer_scale_resolution() 
{
    float acc_res;
    switch (this->accelerometer_scale)
    {
        // Possible accelerometer scales (and their register bit settings) are:
        // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
        case accelerometer_scale_e::AFS_2G: {
            acc_res = 2.0f/32768.0f;
        } break;
        
        case accelerometer_scale_e::AFS_4G: {
            acc_res = 4.0f/32768.0f;
        } break;
        
        case accelerometer_scale_e::AFS_8G: {
            acc_res = 8.0f/32768.0f;
        } break;
    
        case accelerometer_scale_e::AFS_16G: {
            acc_res = 16.0f/32768.0f;
        } break;        
    }
    return acc_res;
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void mpu9250::self_test()
{    
    uint8_t data[6] = {0, 0, 0, 0, 0, 0};
    uint8_t selftest_factory[6];

    int32_t gyro_avg[3] = {0, 0, 0};
    int32_t acc_avg[3] = {0, 0, 0};
    int32_t a_selftest_avg[3] = {0, 0, 0};
    int32_t g_selftest_avg[3] = {0, 0, 0};
  
    float factory_trim[6];
    uint8_t fs_sel = 0;
   
    // Set gyro sample rate to 1 kHz
    this->write_byte(this->mpu_addr8, SMPLRT_DIV, 0x00); 
    
    // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
    this->write_byte(this->mpu_addr8, CONFIG, 0x02); 

    // Set full scale range for the gyro to 250 dps
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, 1<<fs_sel); 

    // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG2, 0x02); 

    // Set full scale range for the accelerometer to 2 g
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, 1<<fs_sel); 
 
    // get average current values of gyro and acclerometer
    for( int i = 0; i < 200; i++) 
    { 
        this->read_bytes(this->mpu_addr8, ACCEL_XOUT_H, 6, (char*)data); 
        acc_avg[0] += (int16_t)(((int16_t)data[0] << 8) | data[1]);
        acc_avg[1] += (int16_t)(((int16_t)data[2] << 8) | data[3]);
        acc_avg[2] += (int16_t)(((int16_t)data[4] << 8) | data[5]);
  
        this->read_bytes(this->mpu_addr8,  GYRO_XOUT_H, 6, (char*)data); 
        gyro_avg[0] += (int16_t)(((int16_t)data[0] << 8) | data[1]); 
        gyro_avg[1] += (int16_t)(((int16_t)data[2] << 8) | data[3]);
        gyro_avg[2] += (int16_t)(((int16_t)data[4] << 8) | data[5]);
    }
  
    for (int i =0; i < 3; i++) 
    {
        // Get average of 200 values and store as average current readings
        acc_avg[i] /= 200;
        gyro_avg[i] /= 200;
    }
  
    // Configure the accelerometer for self-test

    // Enable self test on all three axes and set accelerometer range to +/- 2 g
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, 0xE0);

    // Enable self test on all three axes and set gyro range to +/- 250 degrees/s 
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, 0xE0); 
   
    thread_sleep_for(25); // Delay a while to let the device stabilize
   
    for( int i = 0; i < 200; i++) 
    { 
        // get average self-test values of gyro and acclerometer

        // Read the six raw data registers into data array
        this->read_bytes(this->mpu_addr8, ACCEL_XOUT_H, 6, (char*)data); 

        // Turn the MSB and LSB into a signed 16-bit value
        a_selftest_avg[0] += (int16_t)(((int16_t)data[0] << 8) | data[1]); 
        a_selftest_avg[1] += (int16_t)(((int16_t)data[2] << 8) | data[3]);
        a_selftest_avg[2] += (int16_t)(((int16_t)data[4] << 8) | data[5]);
       
        // Read the six raw data registers sequentially into data array
        this->read_bytes(this->mpu_addr8, GYRO_XOUT_H, 6, (char*)data); 

        // Turn the MSB and LSB into a signed 16-bit value
        g_selftest_avg[0] += (int16_t)(((int16_t)data[0] << 8) | data[1]); 
        g_selftest_avg[1] += (int16_t)(((int16_t)data[2] << 8) | data[3]);
        g_selftest_avg[2] += (int16_t)(((int16_t)data[4] << 8) | data[5]);
    }
  
    for (int i =0; i < 3; i++) 
    { 
        // Get average of 200 values and store as average self-test readings
        a_selftest_avg[i] /= 200;
        g_selftest_avg[i] /= 200;
    }
  
    // Configure the gyro and accelerometer for normal operation
    this->write_byte(this->mpu_addr8, ACCEL_CONFIG, 0x00);
    this->write_byte(this->mpu_addr8, GYRO_CONFIG, 0x00);
    thread_sleep_for(25); // Delay a while to let the device stabilize
  
    // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
    selftest_factory[0] = this->read_byte(this->mpu_addr8,  SELF_TEST_X_ACCEL); 
    selftest_factory[1] = this->read_byte(this->mpu_addr8,  SELF_TEST_Y_ACCEL); 
    selftest_factory[2] = this->read_byte(this->mpu_addr8,  SELF_TEST_Z_ACCEL); 
    selftest_factory[3] = this->read_byte(this->mpu_addr8,  SELF_TEST_X_GYRO); 
    selftest_factory[4] = this->read_byte(this->mpu_addr8,  SELF_TEST_Y_GYRO); 
    selftest_factory[5] = this->read_byte(this->mpu_addr8,  SELF_TEST_Z_GYRO); 
 
    // Retrieve factory self-test value from self-test code reads
    // FT[Xa] factory trim calculation
    factory_trim[0] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[0] - 1.0) )); 
    factory_trim[1] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[1] - 1.0) )); 
    factory_trim[2] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[2] - 1.0) )); 
    factory_trim[3] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[3] - 1.0) )); 
    factory_trim[4] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[4] - 1.0) )); 
    factory_trim[5] = (float)(2620/1<<fs_sel)*(
        std::pow( 1.01 , ((float)selftest_factory[5] - 1.0) )); 
 
    // Report results as a ratio of (STR - FT)/FT; the change from 
    // Factory Trim of the Self-Test Response
    // To get percent, must multiply by 100
    for (int i = 0; i < 3; i++) 
    {
        // Report percent differences
        if (factory_trim[i] != 0.0f )
        {
            this->self_test_results[i] = 100.0f*((float)(a_selftest_avg[i] - acc_avg[i]))/factory_trim[i] - 100.0f; 
        }
        else 
        {
            this->self_test_results[i] = 0.0f;
        }

        if (factory_trim[i+3] != 0.0f )
        {
            this->self_test_results[i+3] = 100.0f*((float)(g_selftest_avg[i] - gyro_avg[i]))/factory_trim[i+3] - 100.0f; 
        }
        else 
        {
            this->self_test_results[i+3] = 0.0f;
        }        
   }
}

void mpu9250::read_temperature()
{    
    uint8_t data[2];
    this->read_bytes(this->mpu_addr8, TEMP_OUT_H, 2, (char*)data);  
    this->raw_readings.temp = (((int16_t)data[0] << 8) | data[1]);
}
 

void mpu9250::write_byte(uint8_t address, char subaddress, uint8_t value)
{
    char data[] = { subaddress, value };
    this->i2c.write(address, data, 2, 0);
}

char mpu9250::read_byte(uint8_t address, char subaddress)
{
    char value; 
    this->i2c.write(address, &subaddress, 1, 1); // no stop
    this->i2c.read(address, &value, 1, 0); 
    return value; 
}

void mpu9250::read_bytes(uint8_t address, char subaddress, uint8_t n, char * data)
{     
    this->i2c.write(address, &subaddress, 1, 1); // no stop
    this->i2c.read(address, data, n, 0); 
}   