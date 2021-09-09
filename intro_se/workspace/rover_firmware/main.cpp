/** @file main.c Archivo principal donde se instancia la aplicación e inicia el lazo infinito. */

#include "mbed.h"

#include "config.h"
#include "application.h"
#include "mbed_thread.h"

int main()
{
    application app;

    /* Configuración */
    app.setup();
    
    /* No debería ser necesario */
    for(;;)
    {
        thread_sleep_for(100);
    }
}