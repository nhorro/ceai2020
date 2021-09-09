#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H
 
#include <mbed.h>
#include <deque>
#include <algorithm>
 
 /** Filtro de mediana */
class median_filter {
public:
    /** Constructor por defecto.
     * @param size Tamaño de la ventana.
     */
    median_filter(int size = 3);
 
    /* Procesar un nuevo valor y devolver valor filtrado. */
    float process(float value);
 
private:
    int size;
    std::deque<float> window;
};
#endif