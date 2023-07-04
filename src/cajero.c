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
 * La función "cajero" simula una línea telefónica esperando una llamada para luego redirigirla a un
 * teléfono libre.
 */
void cajero()
{
    pid_t pid = getpid();

    int i = 0;
    // Coge semáforos y memoria compartida

    while (1)
    {
        sem_t *mutex = get_sem(MUTEXPQUETE);
        sem_t *cajero = get_sem(CAJEROS);
        int valorPaquetes = obtener_var(PAQUETES);
        // Realiza una espera entre 1..60 segundos
        printf("Cajero [%d] esperando paquete...\n", pid);
        wait_sem(cajero);
        sleep(rand() % 30 + 1);
        wait_sem(mutex);
        consultar_var(valorPaquetes, &i);
        modificar_var(valorPaquetes, --i);
        signal_sem(mutex);
        printf("Cajero [%d] vendiendo paquete... Nº paquetes restantes: %d\n", pid, i);
    }
}