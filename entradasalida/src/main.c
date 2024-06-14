#include "interfaces.h"

int main(int argc, char* argv[])
{   
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("IO.log", "entrada salida", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initIOConfig("IO.config");

    // Inicializacion de semaforos y reserva de memoria
    sem_init(&semaphoreForStdin, 0, 0);
    sem_init(&semaphoreForStdout, 0, 0);
    sem_init(&semaphoreForModule, 0, 0);

    // Se llena la lista global de operaciones posibles a realizar en base a la informacion del config
    createInterface(&interfaceData);

    t_package* initialPackageForKernel = createPackage(IO_MODULE);
    log_info(getLogger(), "Creando conexion con el Kernel. Se enviara un mensaje al Kernel con el nombre y tipo de la interfaz.");
    socketKernel = createConection(getLogger(), getIOConfig()->IP_KERNEL, getIOConfig()->PUERTO_KERNEL);
    sendPackage(initialPackageForKernel, socketKernel);
    log_info(getLogger(), "Paquete enviado con exito al Kernel.");

    t_package* initialPackageForMemory = createPackage(IO_MODULE);
    log_info(getLogger(), "Creando conexion con la memoria. Se enviara un mensaje a la Memoria.");
    socketMemory = createConection(getLogger(), getIOConfig()->IP_MEMORIA, getIOConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageForMemory, socketMemory);
    log_info(getLogger(), "Paquete enviado con exito a la memoria.");

    initServerForASocket(socketKernel, serverIOForKernel);
    initServerForASocket(socketMemory, serverIOForMemory);

    sendInterfaceToKernel();
    
    sem_wait(&semaphoreForModule);

    destroyPackage(initialPackageForKernel);
    destroyPackage(initialPackageForMemory);

    // Liberando todos los recursos
    freeIOConfig();
    destroyLogger();

    // Señal para todos los servidores para que dejen de escuchar clientes
    finishAllServersSignal();

    // Se cierran los sockets de conexión
    releaseConnection(socketKernel);
    releaseConnection(socketMemory);

    // Destrucción de semáforos
    sem_destroy(&semaphoreForStdin);
    sem_destroy(&semaphoreForStdout);
    sem_destroy(&semaphoreForModule);

    return 0;
}