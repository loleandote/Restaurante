// Semáforos
#define MUTEXPEDIDO "mutexped"
#define MUTEXCOMIDA "mutexcom"
#define MUTEXPQUETE "mutexpac"
#define RECEPTORES "contadorrec"
#define COCINEROS  "contadorco"
#define EMPAQUETADORES "contadoremp"
#define CAJEROS "contadorcaj"

//Memoria Compartida
#define PEDIDOS "pedidos"
#define COMIDAS "comidas"
#define PAQUETES "paquetes"

// CLASES y PATHS
#define CLASE_RECEPTOR "RECEPTOR"
#define RUTA_RECEPTOR "./exec/receptor"
#define CLASE_COCINERO "COCINERO"
#define RUTA_COCINERO "./exec/cocinero"
#define CLASE_EMPAQUETADOR "EMPAQUETADOR"
#define RUTA_EMPAQUETADOR "./exec/empaquetador"
#define CLASE_CAJERO "CAJERO"
#define RUTA_CAJERO "./exec/cajero"

struct TProcess_t
{
  pid_t pid;
  char *clase;
};
