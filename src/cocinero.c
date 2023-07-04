#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
void cocinero();
void cocinar(sem_t *cocinero);
// Modulo principal
int main(int argc, char *argv[])
{
    cocinero();
    return EXIT_SUCCESS;
}
/**
 * Esta función simula una conversación telefónica obteniendo semáforos y memoria compartida, poniendo
 * el teléfono en un estado libre y luego esperando las llamadas entrantes mientras se reduce el número
 * de llamadas en la cola de espera.
 */
void cocinero()
{
    pid_t pid = getpid();
 sem_t *cocinero = get_sem(COCINEROS);
        sem_t *mutexPedido = get_sem(MUTEXPEDIDO);
        sem_t *empaquetador = get_sem(EMPAQUETADORES);
        sem_t *mutexComida = get_sem(MUTEXCOMIDA);
    // Se pone en estado de libre incrementando el número de teléfonos libres
    while (1)
    {
        int pedidosEspera = obtener_var(PEDIDOS);
        int comidasEspera = obtener_var(COMIDAS);
        int i = 0;
        int j = 0;
        // Coge semáforos y memoria compartida
       
        // Mensaje de Espera
        printf("Cocinero [%d] en espera...\n", pid);
        // printf("estoy atrapado");
        wait_sem(cocinero);
        // Obtenemos el número de llamadas en espera
        // decrementamos el numero de llamadas en espera
        wait_sem(mutexPedido);
        consultar_var(pedidosEspera, &i);
        modificar_var(pedidosEspera, --i);
        signal_sem(mutexPedido);

        sleep(rand() % 30 + 1);
        // Aumenta las llamadas en espera
        wait_sem(mutexComida);
        consultar_var(comidasEspera, &j);
        modificar_var(comidasEspera, ++j);
        printf("Cocinero [%d] cocinando... Nº pedidos en espera: %d Nº platos en espera %d\n", pid, i, j);
        signal_sem(mutexComida);
        // Espera telefono libre
        signal_sem(empaquetador);
    }
}