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
 * La función "empaquetador" es una función que representa un proceso empaquetador, el cual espera
 * comida, disminuye la cantidad de comida disponible, aumenta la cantidad de paquetes disponibles y
 * llama al proceso cajero.
 */
void empaquetador()
{
    pid_t pid = getpid();
    // Coge semáforos y memoria compartida
    sem_t *empaquetador = get_sem(EMPAQUETADORES);
    sem_t *mutexPaquete = get_sem(MUTEXPQUETE);
    sem_t *mutexComida = get_sem(MUTEXCOMIDA);
    sem_t *cajero = get_sem(CAJEROS);
    int valorPaquetes = obtener_var(PAQUETES);
    int valorComidas = obtener_var(COMIDAS);
    while (1)
    {
        int i = 0;
        int j = 0;
        printf("Empaquetador [%d] esperando comida...\n", pid);
        wait_sem(empaquetador);
        sleep(5);
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