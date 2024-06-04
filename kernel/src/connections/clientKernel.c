#include "clientKernel.h"


void sendContextToCPU(pcb_t *process)
{
    t_package* package = createPackage(KERNEL_SEND_CONTEXT);

    contextProcess contextProcess;
    contextProcess.pc = process->pc;
    contextProcess.registersCpu = *(process->registersCpu);

    addToPackage(package, &(contextProcess.pc), sizeof(uint32_t)); // Agrego el Program Counter del registros al paquete para enviar
    addToPackage(package, &(contextProcess.registersCpu.AX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.BX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.CX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.DX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EAX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EBX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.ECX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EDX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.DI), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.SI), sizeof(uint32_t)); // Agrego los registros del contexto

    addToPackage(package, &(process->pid), sizeof(uint32_t));

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

void sendInterruptForQuantumEnd()
{

    t_package* package = createPackage(KERNEL_SEND_INTERRUPT_QUANTUM_END);

    sendPackage(package, socketClientCPUInterrupt); //Envio el paquete a memoria
    
    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

}

void sendInterruptForConsoleEndProcess()
{
    
    t_package* package = createPackage(KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS);

    sendPackage(package, socketClientCPUInterrupt); //Envio el paquete a memoria
    
    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

}

void sendIOGenSleepOperation(char* interfaceName, int timeOfOperation)
{
    t_package* package = createPackage(KERNEL_SEND_OPERATION_TO_GENERIC_INTERFACE);
}