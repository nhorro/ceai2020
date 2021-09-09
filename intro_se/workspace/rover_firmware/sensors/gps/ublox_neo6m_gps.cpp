#include "ublox_neo6m_gps.h"

ublox_neo6m_gps::ublox_neo6m_gps(PinName tx, PinName rx)
    :
        serial{tx,rx},
        latitude(0.0f),
        longitude(0.0f),
        valid(false)
{
    
}

void ublox_neo6m_gps::setup()
{

}

void ublox_neo6m_gps::process()
{
    static char buf[GPS_MAXIMUM_BUFFER_SIZE+2] = {0};

    if(this->serial.readable())
    {
        if (uint32_t n = this->serial.read(buf, GPS_MAXIMUM_BUFFER_SIZE)) 
        {
            //buf[n] = '\0';                
            //char*  p_buf = buf;
            //while(*p_buf++)
            //{                    
                // FIXME: Parsear
                //printf("%c", *p_buf);
            //}
        }
    }
}