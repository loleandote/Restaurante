#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

void procesar_argumentos(int argc, char *argv[]);
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void iniciar_tabla_procesos();
void crear_procesos();
void lanzar_proceso_receptor(const int indice_tabla);
void lanzar_proceso_cocinero(const int indice_tabla);
void lanzar_proceso_empaquetador(const int indice_tabla);
void lanzar_proceso_cajero(const int indice_tabla);
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);
void liberar_recursos();

// variables globales que usaremos en toda la clase
int g_receptoresProcesses = 0;
int g_cocinerosProcesses = 0;
int g_empaquetadoresProcesses = 0;
int g_cajerosProcesses = 0;
// estas variables globales son punteros con estructura TProcess_t ya que las utilizaremos para acceder a tablas de procesos
/*La estructura "TProcess_t" puede contener información relevante sobre los procesos en ejecución, como el identificador de proceso (PID),
el estado del proceso, el tiempo de ejecución, etc.*/
struct TProcess_t *g_process_receptores_table;
struct TProcess_t *g_process_cocineros_table;
struct TProcess_t *g_process_empaquetadores_table;
struct TProcess_t *g_process_cajeros_table;

int main(int argc, char *argv[])
{
    // Define variables locales
    if (argc != 5)
    {
        printf("Error: Los argumentos pasados por el terminal son incorrectos %d\n", argc);
        printf("Ejemplo de ejecución: \n ./exec/manager <nReceptores> <nCocineros> <nEmpaquetadores> <nCajeros>\n");
        exit(EXIT_FAILURE);
    }

    // Procesa los argumentos y los guarda en las dos variables
    procesar_argumentos(argc, argv);

    // Creamos semáforos y memoria compartida
    crear_sem(MUTEXCOMIDA, 1);
    crear_sem(MUTEXPEDIDO, 1);
    crear_sem(MUTEXPQUETE, 1);
    crear_sem(RECEPTORES, 0);
    crear_sem(COCINEROS, 0);
    crear_sem(EMPAQUETADORES, 0);
    crear_sem(CAJEROS, 0);
    crear_var(PEDIDOS, 0);
    crear_var(COMIDAS, 0);
    crear_var(PAQUETES, 0);

    // Manejador de Ctrl-C
    instalar_manejador_senhal();

    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos();

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos();
    return EXIT_SUCCESS;
}

void procesar_argumentos(int argc, char *argv[])
{
    g_receptoresProcesses = atoi(argv[1]);
    g_cocinerosProcesses = atoi(argv[2]);
    g_empaquetadoresProcesses = atoi(argv[3]);
    g_cajerosProcesses = atoi(argv[4]);
}

// Instala un manejador de señales en el programa y muestra un mensaje de error si la operación falla.
void instalar_manejador_senhal()
{
    // utilizaos la llamada al sistema signal para asociar el metodo manejador_senhal a la señal SIGNIT
    // De esta manera podamos posteriormente realizar un apagado de emergencia con el comando Ctrl-C
    if (signal(SIGINT, manejador_senhal) == SIG_ERR)
    {
        fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

// Apagado de Emergencia (Ctrl + C)
void manejador_senhal(int sign)
{
    printf("\n[MANAGER] Apagado de emergencia (Ctrl + C).\n");
    // al apagar todo tenemos que forzar la finalizacion de todos los procesos y luego liberar los recursos por ello llamamos a esos metodos
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_SUCCESS);
}

// inicializamos dos tablas de procesos una para las llamadas y otra para las lineas
void iniciar_tabla_procesos()
{
    // Lo primero que hacemos es asignar memoria dinamica a esta tabla por medio de malloc
    // lo que le asignamos a cada tabla la calculamos por el Nº de procesos de lineas que tengamos por el tamaño a estructura "TProcess_t"
    g_process_receptores_table = malloc(g_receptoresProcesses * sizeof(struct TProcess_t));
    g_process_cocineros_table = malloc(g_cocinerosProcesses * sizeof(struct TProcess_t));
    g_process_empaquetadores_table = malloc(g_empaquetadoresProcesses * sizeof(struct TProcess_t));
    g_process_cajeros_table = malloc(g_cajerosProcesses * sizeof(struct TProcess_t));

    // Ahora que ya hemos asigando la memoria a las tablas que de procesos que hemos creado, recorremos dichas tablas por un for
    for (int i = 0; i < g_receptoresProcesses; i++)
        g_process_cocineros_table[i].pid = 0;

    for (int i = 0; i < g_cocinerosProcesses; i++)
        g_process_receptores_table[i].pid = 0;

    for (int i = 0; i < g_empaquetadoresProcesses; i++)
        g_process_empaquetadores_table[i].pid = 0;

    for (int i = 0; i < g_cajerosProcesses; i++)
        g_process_cajeros_table[i].pid = 0;
}

void crear_procesos()
{
    printf("[MANAGER] %d receptores creados.\n", g_receptoresProcesses);
    for (int i = 0; i < g_receptoresProcesses; i++)
    {
        lanzar_proceso_receptor(i);
    }
    printf("[MANAGER] %d cocineros creados.\n", g_cocinerosProcesses);
    for (int i = 0; i < g_cocinerosProcesses; i++)
    {
        lanzar_proceso_cocinero(i);
    }
    printf("[MANAGER] %d empaquetadores creados.\n", g_empaquetadoresProcesses);
    for (int i = 0; i < g_empaquetadoresProcesses; i++)
    {
        lanzar_proceso_empaquetador(i);
    }
    printf("[MANAGER] %d cajeros creados.\n", g_cajerosProcesses);
    for (int i = 0; i < g_cajerosProcesses; i++)
    {
        lanzar_proceso_cajero(i);
    }
}

void lanzar_proceso_receptor(const int indice_tabla)
{
    // creamos la varaible pid de tipo pid_t para poder alamcenar el PID correspondiente a cada proceso hijo
    pid_t pid;
    // Creamos un switch para controlar los difrenets casos cuando el fork nos de difernetes valores
    // declaramos que el pid sera igual al valor devuelto de la primitiva fork(), la cual es la que usamos para crear procesos hijos
    switch (pid = fork())
    {
    // en caso que sea -1 es que ha sucedido un error
    case -1:
        fprintf(stderr, "[MANAGER] Hemos encontrado un error al lanzar el proceso receptor: %s.\n", strerror(errno));
        // ya que hemos encontrado un error necesitamso termianr los procesos y liberar los recursos
        terminar_procesos();
        liberar_recursos();
        break;
    // Si devuelve 0, significa que se está ejecutando en el proceso hijo.
    case 0:
        // realizamos la primitiva execl para que el hijo ejecute otro proceso dirferente al del padre, en el caso de que de -1 se marca un eror
        // y nos imprime lo que ha sucedido.
        if (execl(RUTA_RECEPTOR, CLASE_RECEPTOR, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Hemos encontrado un error usando execl () en el proceso %s: %s. \n", CLASE_RECEPTOR, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    g_process_receptores_table[indice_tabla].pid = pid;
    g_process_receptores_table[indice_tabla].clase = CLASE_RECEPTOR;
}

void lanzar_proceso_cocinero(const int indice_tabla)
{
    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Hemos encontrado un error al lanzar proceso cocinero: %s.\n", strerror(errno));
        terminar_procesos();
        liberar_recursos();
        exit(EXIT_FAILURE);
    case 0:
        if (execl(RUTA_COCINERO, CLASE_COCINERO, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Hemos encontrado un error usando execl() en el poceso %s: %s.\n", CLASE_COCINERO, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    g_process_cocineros_table[indice_tabla].pid = pid;
    g_process_cocineros_table[indice_tabla].clase = CLASE_COCINERO;
}

void lanzar_proceso_empaquetador(const int indice_tabla)
{
    // creamos la varaible pid de tipo pid_t para poder alamcenar el PID correspondiente a cada proceso hijo
    pid_t pid;
    // Creamos un switch para controlar los difrenets casos cuando el fork nos de difernetes valores
    // declaramos que el pid sera igual al valor devuelto de la primitiva fork(), la cual es la que usamos para crear procesos hijos
    switch (pid = fork())
    {
    // en caso que sea -1 es que ha sucedido un error
    case -1:
        fprintf(stderr, "[MANAGER] Hemos encontrado un error al lanzar el proceso empaquetador: %s.\n", strerror(errno));
        // ya que hemos encontrado un error necesitamso termianr los procesos y liberar los recursos
        terminar_procesos();
        liberar_recursos();
        break;
    // Si devuelve 0, significa que se está ejecutando en el proceso hijo.
    case 0:
        // realizamos la primitiva execl para que el hijo ejecute otro proceso dirferente al del padre, en el caso de que de -1 se marca un eror
        // y nos imprime lo que ha sucedido.
        if (execl(RUTA_EMPAQUETADOR, CLASE_EMPAQUETADOR, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Hemos encontrado un error usando execl () en el proceso %s: %s. \n", CLASE_EMPAQUETADOR, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    g_process_empaquetadores_table[indice_tabla].pid = pid;
    g_process_empaquetadores_table[indice_tabla].clase = CLASE_EMPAQUETADOR;
}

void lanzar_proceso_cajero(const int indice_tabla)
{
    pid_t pid;

    switch (pid = fork())
    {
    case -1:
        fprintf(stderr, "[MANAGER] Hemos encontrado un error al lanzar proceso cajero: %s.\n", strerror(errno));
        terminar_procesos();
        liberar_recursos();
        exit(EXIT_FAILURE);
    case 0:
        if (execl(RUTA_CAJERO, CLASE_CAJERO, NULL) == -1)
        {
            fprintf(stderr, "[MANAGER] Hemos encontrado un error usando execl() en el poceso %s: %s.\n", CLASE_CAJERO, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    g_process_cajeros_table[indice_tabla].pid = pid;
    g_process_cajeros_table[indice_tabla].clase = CLASE_CAJERO;
}


// Este metodo se encarga de terminar los procesos enviadno una señal de SIGINT y asi terminarlo de manera ordenada
void terminar_procesos(void)
{
    terminar_procesos_especificos(g_process_receptores_table, g_receptoresProcesses);
    terminar_procesos_especificos(g_process_cocineros_table, g_cocinerosProcesses);
    terminar_procesos_especificos(g_process_empaquetadores_table, g_empaquetadoresProcesses);
    terminar_procesos_especificos(g_process_cajeros_table, g_cajerosProcesses);
}

// al igual lo que temrianr procesos este envia una señal de SIGNIT para finalizar un proceso solo que este caso sera un proceso en particular.
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num)
{
    for (int i = 0; i < process_num; i++)
    {
        if (g_process_receptores_table[i].pid != 0)
        {
            printf("[MANAGER] Terminando proceso %s [%d]...\n", process_table[i].clase, process_table[i].pid);
            // Utilizamos la primitiva kill para matar al proceso especifico, ya que especificamos elindice de la tabla y el numero del proceso.
            // si en el caso de que kill sea -1, se amrca como un error y se notifica
            if (kill(process_table[i].pid, SIGINT) == -1)
            {
                fprintf(stderr, "[MANAGER] Hemos encontrado un error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno));
            }
            else
            {
                process_table[i].pid = 0;
            }
        }
    }
}

// es imporatnte siempre liberar los recursos entonces este metodo se encarga de ello
void liberar_recursos()
{
    // utilizmaos la funcion free para liberar la memoria que antes hemos asignado con malloc
    free(g_process_receptores_table);
    free(g_process_cocineros_table);
    free(g_process_empaquetadores_table);
    free(g_process_cajeros_table);
    destruir_sem(RECEPTORES);
    destruir_sem(COCINEROS);
    destruir_sem(EMPAQUETADORES);
    destruir_sem(CAJEROS);
    destruir_sem(MUTEXPEDIDO);
    destruir_sem(MUTEXCOMIDA);
    destruir_sem(MUTEXPQUETE);
    destruir_var(PEDIDOS);
    destruir_var(COMIDAS);
    destruir_var(PAQUETES);
}