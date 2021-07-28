#include "mbed.h"

#include "config.h"
#include "application.h"

int main()
{
    application app;

    /* Configuración */
    app.setup();

    /* Bucle principal */
    for(;;)
    {
        app.loop();
    }
}