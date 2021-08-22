/** @file main.c Archivo principal donde se instancia la aplicación e inicia el lazo infinito. */

#include "mbed.h"

#include "config.h"
#include "application.h"

int main()
{
    application app;

    /* Configuración */
    app.setup();
}