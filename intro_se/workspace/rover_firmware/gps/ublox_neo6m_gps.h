#ifndef UBLOX_NEO6M_GPS_H
#define UBLOX_NEO6M_GPS_H

#include <mbed.h>
#include <stdint.h>

#define DEFAULT_GPS_PIN_TX PD_5
#define DEFAULT_GPS_PIN_RX PD_6
#define GPS_MAXIMUM_BUFFER_SIZE 128

/** Interface con GPS UBLOX NEO6M
*/
class ublox_neo6m_gps {
public:
    /** Constructor por defecto.
     * @param tx Pin UART Tx.
     * @param rx Pin UART Rx.
     */
    ublox_neo6m_gps(PinName tx, PinName rx);

    /** Destructor por defecto.
    */
    ~ublox_neo6m_gps() = default;

    /** Procesar lectura del GPS. Esta función debe ser llamada periódicamente
     * por la aplicación */
    void process();

    /** Configuración. */
    void setup();

    /** Obtener última latitud leída 
     * @return última latitud leída.
     */
    inline float get_latitude() const { return this->latitude; }

    /** Obtener última longitud leída 
     * @return última longitud leída.
     */
    inline float get_longitude() const { return this->longitude; }

    /** Indicador de si la última lectura es válida.
     */
    inline bool is_valid() const { return this->valid; }

private:    
    BufferedSerial serial;
    bool valid;
    float latitude;
    float longitude;
};
    
#endif // UBLOX_NEO6M_GPS_H