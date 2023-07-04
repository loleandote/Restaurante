#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
void cocinero();
// Modulo principal
int main(int argc, char *argv[])
{
    cocinero();
    return EXIT_SUCCESS;
}
/**
 * La función "cocinero" representa a un cocinero que espera pedidos, disminuye el número de pedidos
 * pendientes, aumenta el número de platos pendientes y le indica al empaquetador (empacador) que
 * comience a envasar los platos cocinados.
 */
void cocinero()
{
    pid_t pid = getpid();
    // Coge semáforos y memoria compartida
    int pedidosEspera = obtener_var(PEDIDOS);
    int comidasEspera = obtener_var(COMIDAS);
    sem_t *cocinero = get_sem(COCINEROS);
    sem_t *mutexPedido = get_sem(MUTEXPEDIDO);
    sem_t *empaquetador = get_sem(EMPAQUETADORES);
    sem_t *mutexComida = get_sem(MUTEXCOMIDA);
    while (1)
    {
        int i = 0;
        int j = 0;
        // Mensaje de Espera
        printf("Cocinero [%d] en espera...\n", pid);
        wait_sem(cocinero);
        // Obtenemos el número de pedidos en espera
        // decrementamos el numero de pedidos en espera
        wait_sem(mutexPedido);
        consultar_var(pedidosEspera, &i);
        modificar_var(pedidosEspera, --i);
        signal_sem(mutexPedido);
        sleep(5);
        // Aumenta las comidas en espera
        wait_sem(mutexComida);
        consultar_var(comidasEspera, &j);
        modificar_var(comidasEspera, ++j);
        printf("Cocinero [%d] cocinando... Nº pedidos en espera: %d Nº platos en espera %d\n", pid, i, j);
        signal_sem(mutexComida);
        signal_sem(empaquetador);
    }
}