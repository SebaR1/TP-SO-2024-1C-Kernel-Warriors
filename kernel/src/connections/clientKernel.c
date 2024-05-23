#include "clientKernel.h"


void sendContextToCPU(pcb_t *process)
{
    t_package* package = createPackage(KERNEL_SEND_CONTEXT);

    contextProcess contextProcess;
    contextProcess.pc = process->pc;
    contextProcess.registersCpu = process->registersCpu;

    addToPackage(package, &(contextProcess.pc), sizeof(contextProcess.pc)); // Agrego el Program Counter del registros al paquete para enviar
    addToPackage(package, &(contextProcess.registersCpu), sizeof(contextProcess.registersCpu)); // Agrego los registros del contexto

    sendPackage(package, socketClientCPUDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void sendProcessPathToMemory(pcb_t *process, char* path)
{
    t_package* package = createPackage(KERNEL_SEND_PROCESS_PATH);

    kernelPathProcess kernelPathProcess;
    kernelPathProcess.pid = process->pid;
    kernelPathProcess.path = path;

    addToPackage(package, &(kernelPathProcess.pid), sizeof(uint32_t));
    addToPackage(package, &(kernelPathProcess.path), string_length(kernelPathProcess.path)+1); // +1 por el nulo 

    sendPackage(package, socketClientMemory); //Envio el paquete a memoria

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}