#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
void empaquetador();
int main(int argc, char *argv[])
{
    empaquetador();
    return EXIT_SUCCESS;
}

/**
 * La función "empaquetador" simula una línea telefónica esperando una llamada para luego redirigirla a un
 * teléfono libre.
 */
void empaquetador()
{
    pid_t pid = getpid();

    while (1)
    {
        int valorPaquetes = obtener_var(PAQUETES);
        int valorComidas = obtener_var(COMIDAS);
        int i = 0;
        int j = 0;
        // Coge semáforos y memoria compartida
        sem_t *empaquetador = get_sem(EMPAQUETADORES);
        sem_t *mutexPaquete = get_sem(MUTEXPQUETE);
        sem_t *mutexComida = get_sem(MUTEXCOMIDA);
        sem_t *cajero = get_sem(CAJEROS);
        printf("Empaquetador [%d] esperando comida...\n", pid);
        wait_sem(empaquetador);
        // Realiza una espera entre 1..60 segundos
        sleep(rand() % 30 + 1);
        // Aumenta las llamadas en espera
        wait_sem(mutexComida);
        consultar_var(valorComidas, &i);
        modificar_var(valorComidas, --i);
        signal_sem(mutexComida);
        wait_sem(mutexPaquete);
        consultar_var(valorPaquetes, &j);
        modificar_var(valorPaquetes, ++j);
        signal_sem(mutexPaquete);
        signal_sem(cajero);
        printf("Empaquetador [%d] empaquetando comida... Nº comidas restantes: %d   Nº paquetes restantes: %d\n", pid, i, j);
    }
}