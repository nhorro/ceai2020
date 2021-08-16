/** @file main.c Archivo principal donde se instancia la aplicación e inicia el lazo infinito. */

#include "mbed.h"

#include "config.h"
#include "application.h"

int main()
{
    application app;

    /* Configuración */
    app.setup();

    /* Bucle principal (eliminar en lo posible) */
    /* Nota: la idea es hacer todo con funciones períódicas manejadas por el ticker excepto el manejo
             de telecomandos, que debe ser asincrónico.
             Si no funcionan las interrupciones, reactivar este loop para escuchar comandos.
    */
    //for(;;)
    //{
    //        app.loop();
    //}
}