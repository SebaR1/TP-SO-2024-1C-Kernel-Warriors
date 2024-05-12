#include "kernel.h"


int main()
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("kernel.log", "kernel", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general del kernel.
    initKernelConfig("kernel.config");

    //Inicializo todo
    pid = 0;

    sem_init(&semNew, 0, 0);
    sem_init(&semReady, 0, 0);
    sem_init(&semExec, 0, 0);
    sem_init(&semBlock, 0, 0);
    sem_init(&semExit, 0, 0);
    sem_init(&semAddPid, 0, 1);
    sem_init(&semMulti, 0 , getKernelConfig()->GRADO_MULTIPROGRAMACION);
    sem_init(&semaphoreForIO, 0, 1); // Reservo memoria para mi sepaforo y lo inicializo

    pcbNewList = initListMutex();
    pcbReadyList = initListMutex();
    pcbExecList = initListMutex();
    pcbBLockList = initListMutex();
    pcbExitList = initListMutex();

    initLongTermPlanning();
    initShortTermPlanning();

    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams params;
    params.logger = getLogger();
    params.portToListen = getKernelConfig()->PUERTO_ESCUCHA;
    params.eachIterationFunc = receiveClientIteration;
    params.finishLoopSignal = &_finishAllServersSignal;
    pthread_t waitClientsLoopThread;
    pthread_create(&waitClientsLoopThread, NULL, (void*)waitClientsLoop, &params);
    //pthread_detach(waitClientsLoopThread);

    pthread_t kernelConsoleThread;
    pthread_create(&kernelConsoleThread, NULL, (void*)readKernelConsole, NULL);
    pthread_join(kernelConsoleThread, NULL);



/*

    t_package* initialPackageToMemory = createPackage(KERNEL_MODULE);
    t_package* initialPackageToCPUDispatch = createPackage(KERNEL_MODULE_TO_CPU_DISPATCH);
    t_package* initialPackageToCPUInterrupt = createPackage(KERNEL_MODULE_TO_CPU_INTERRUPT);

    t_package* testPackageToMemory = createPackage(PACKAGE_FROM_KERNEL);
    char* msg1 = "Holaaaa, soy un mensaje de prueba desde el kernel.";
    addToPackage(testPackageToMemory, msg1, string_length(msg1) + 1); // (+1) para tener en cuenta el caracter nulo
    
    t_package* testPackageToCPUDispatch = createPackage(PACKAGE_FROM_KERNEL);
    char* msg2 = "Holaaaa, soy un mensaje de prueba desde el Kernel hacia Dispatch";
    addToPackage(testPackageToCPUDispatch, msg2, string_length(msg2) + 1); // (+1) para tener en cuenta el caracter nulo

    t_package* testPackageToCPUInterrupt = createPackage(PACKAGE_FROM_KERNEL);
    char* msg3 = "Holaaaa, soy un mensaje de prueba desde el Kernel hacia Interrupt";
    addToPackage(testPackageToCPUInterrupt, msg3, string_length(msg3) + 1); // (+1) para tener en cuenta el caracter nulo

    
    log_info(getLogger(), "Creando conexion con la CPU Dispatch. Se enviara un mensaje a la CPU Dispatch");
    int socketClientCPUDispatch = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_DISPATCH);
    sendPackage(initialPackageToCPUDispatch, socketClientCPUDispatch);
    sendPackage(testPackageToCPUDispatch, socketClientCPUDispatch);
    releaseConnection(socketClientCPUDispatch);
    log_info(getLogger(), "Paquete enviado con exito.");
    

    log_info(getLogger(), "Creando conexion con la CPU Interrupt. Se enviara un mensaje a la CPU Interrupt");
    int socketClientCPUInterrupt = createConection(getLogger(), getKernelConfig()->IP_CPU, getKernelConfig()->PUERTO_CPU_INTERRUPT);
    sendPackage(initialPackageToCPUInterrupt, socketClientCPUInterrupt);
    sendPackage(testPackageToCPUInterrupt, socketClientCPUInterrupt);
    releaseConnection(socketClientCPUInterrupt);
    log_info(getLogger(), "Paquete enviado con exito.");


    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getKernelConfig()->IP_MEMORIA, getKernelConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackageToMemory, socketClientMemory);
    sendPackage(testPackageToMemory, socketClientMemory);
    releaseConnection(socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");

    destroyPackage(initialPackageToCPUDispatch);
    destroyPackage(initialPackageToCPUInterrupt);
    destroyPackage(initialPackageToMemory);
    destroyPackage(testPackageToMemory);
    destroyPackage(testPackageToCPUDispatch);
    destroyPackage(testPackageToCPUInterrupt);


    // Espero para ver si me llegan mensajes.
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    //sleep(60);

*/

    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion
    finishAllServersSignal();

    // Liberando todos los recursos
    freeKernelConfig();
    destroyLogger();

    sem_destroy(&semaphoreForIO);
    sem_destroy(&semNew);
    sem_destroy(&semReady);
    sem_destroy(&semExec);
    sem_destroy(&semBlock);
    sem_destroy(&semExit);
    sem_destroy(&semMulti);
    sem_destroy(&semAddPid);

    destroyListMutex(pcbNewList);
    destroyListMutex(pcbReadyList);
    destroyListMutex(pcbExecList);
    destroyListMutex(pcbBLockList);
    destroyListMutex(pcbExitList);

    return 0;
}
