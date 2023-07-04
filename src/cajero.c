#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
void cajero();
int main(int argc, char *argv[])
{
    cajero();
    return EXIT_SUCCESS;
}


/**
 * La función "cajero" representa a un cajero en un sistema que maneja paquetes, donde el cajero espera
 * una llamada del empaquetador, lo vende y actualiza el número restante de paquetes.
 */
void cajero()
{
    pid_t pid = getpid();

    int i = 0;
    // Coge semáforos y memoria compartida
    sem_t *mutex = get_sem(MUTEXPQUETE);
    sem_t *cajero = get_sem(CAJEROS);
    int valorPaquetes = obtener_var(PAQUETES);
    while (1)
    {
        printf("Cajero [%d] esperando paquete...\n", pid);
        wait_sem(cajero);
        sleep(5);
        wait_sem(mutex);
        consultar_var(valorPaquetes, &i);
        modificar_var(valorPaquetes, --i);
        signal_sem(mutex);
        printf("Cajero [%d] vendiendo paquete... Nº paquetes restantes: %d\n", pid, i);
    }
}