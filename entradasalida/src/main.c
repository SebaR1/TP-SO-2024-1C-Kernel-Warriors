#include "interfaces.h"

extern t_interfaceData interfaceData;
extern t_currentOperation currentOperation;
extern sem_t semaphoreForStdin;
extern sem_t semaphoreForStdout;

int main(int argc, char* argv[])
{   
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("IO.log", "entrada salida", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initIOConfig("IO.config");

    // Inicializacion de semaforos y reserva de memoria
    sem_init(&semaphoreForStdin, 0, 1);
    sem_init(&semaphoreForStdout, 0, 1);

    // Se llena la lista global de operaciones posibles a realizar en base a la informacion del config
    createInterface(&interfaceData);

    t_package* initialPackageForKernel = createPackage(IO_MODULE);
    addToPackage(initialPackageForKernel, interfaceData.name, sizeof(char) * 8);
    addToPackage(initialPackageForKernel, &interfaceData.type, sizeof(interfaceType));
    log_info(getLogger(), "Creando conexion con el Kernel. Se enviara un mensaje al Kernel con el nombre y tipo de la interfaz.");
    int socketClientKernel = createConection(getLogger(), getIOConfig()->IP_KERNEL, getIOConfig()->PUERTO_KERNEL);
    sendPackage(initialPackageForKernel, socketClientKernel);
    log_info(getLogger(), "Paquete enviado con exito al Kernel.");

    t_package* initialPackageForMemory = createPackage(IO_MODULE);
    log_info(getLogger(), "Creando conexion con la memoria. Se enviara un mensaje a la Memoria.");
    int socketClientMemory = createConection(getLogger(), getIOConfig()->IP_MEMORIA, getIOConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageForMemory, socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito a la memoria.");

    initServerForASocket(socketClientKernel, serverIOForKernel);
    initServerForASocket(socketClientMemory, serverIOForMemory);

    /*

    t_package* initialPackage = createPackage(IO_MODULE);

    t_package* testPackageToKernel = createPackage(PACKAGE_FROM_IO);
    char* msg1 = "Holaaaa, soy un mensaje de prueba desde IO.";
    addToPackage(testPackageToKernel, msg1, string_length(msg1) + 1); // (+1) para tener en cuenta el caracter nulo

    t_package* testPackageToMemory = createPackage(PACKAGE_FROM_IO);
    char* msg2 = "Holaaaa, soy un mensaje de prueba desde IO.";
    addToPackage(testPackageToMemory, msg2, string_length(msg2) + 1); // (+1) para tener en cuenta el caracter nulo

    log_info(getLogger(), "Creando conexion con el Kernel. Se enviara un mensaje al Kernel");
    int socketClientKernel = createConection(getLogger(), getIOConfig()->IP_KERNEL, getIOConfig()->PUERTO_KERNEL);
    sendPackage(initialPackage, socketClientKernel);
    sendPackage(testPackageToKernel, socketClientKernel);
    // Espero para desconectarme del cliente
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    sleep(15);
    releaseConnection(socketClientKernel);
    log_info(getLogger(), "Paquete enviado con exito.");


    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getIOConfig()->IP_MEMORIA, getIOConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackage, socketClientMemory);
    sendPackage(testPackageToMemory, socketClientMemory);
    // Espero para desconectarme del cliente
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    sleep(15);
    releaseConnection(socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");


    destroyPackage(initialPackage);
    destroyPackage(testPackageToKernel);
    destroyPackage(testPackageToMemory);

    */

    destroyPackage(initialPackageForKernel);
    destroyPackage(initialPackageForMemory);

    // Liberando todos los recursos
    freeIOConfig();
    destroyLogger();

    sem_destroy(&semaphoreForStdin);
    sem_destroy(&semaphoreForStdout);

    return 0;
}