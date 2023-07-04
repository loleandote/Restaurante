#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>
void receptor();
int main(int argc, char *argv[])
{
    receptor();
    return EXIT_SUCCESS;
}

/**
 * La función "receptor" simula una línea telefónica esperando una llamada para luego redirigirla a un
 * teléfono libre.
 */
void receptor()
{
    pid_t pid = getpid();

    // Realiza una espera entre 1..60 segundos
    while (1)
    {
        int valorPedidos = obtener_var(PEDIDOS);
        int i = 0;
        // Coge semáforos y memoria compartida
        sem_t *cocinero = get_sem(COCINEROS);
        sem_t *mutex = get_sem(MUTEXPEDIDO);
        printf("Receptor [%d] esperando pedido...\n", pid);
        sleep(rand() % 30 + 1);
        // Aumenta los pedidos en espera
        wait_sem(mutex);
        consultar_var(valorPedidos, &i);
        modificar_var(valorPedidos, ++i);
        signal_sem(mutex);
        printf("Receptor [%d] esperando cocinero libre...Nº pedidos en espera: %d\n", pid, i);
        // Espera cocinero libre
        signal_sem(cocinero);
    }
}