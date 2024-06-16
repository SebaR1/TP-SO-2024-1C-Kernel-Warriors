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



void sendInterruptForQuantumEnd(pcb_t* process)
{

    t_package* package = createPackage(KERNEL_SEND_INTERRUPT_QUANTUM_END);

    addToPackage(package, &(process->pid), sizeof(uint32_t));

    sendPackage(package, socketClientCPUInterrupt); //Envio el paquete a memoria
    
    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

}

void sendInterruptForConsoleEndProcess(pcb_t* process)
{
    t_package* package = createPackage(KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS);

    addToPackage(package, &(process->pid), sizeof(uint32_t));

    sendPackage(package, socketClientCPUInterrupt); //Envio el paquete a memoria
    
    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

}

void sendIOGenSleepOperationToIO(interface_t* interface, uint32_t timeOfOperation)
{
    t_package* package = createPackage(KERNEL_SEND_OPERATION_TO_GENERIC_INTERFACE);

    addToPackage(package, &(interface->processAssign->pid), sizeof(uint32_t));
    addToPackage(package, &(timeOfOperation), sizeof(uint32_t));

    // Uso el socket de la interfaz.
    sendPackage(package, *interface->socket);

    destroyPackage(package);
}

void sendIOStdinReadOperationToIO(interface_t* interface, uint32_t registerDirection, uint32_t registerSize)
{
    t_package* package = createPackage(KERNEL_SEND_OPERATION_TO_STDIN_INTERFACE);

    addToPackage(package, &(interface->processAssign->pid), sizeof(uint32_t));
    addToPackage(package, &(registerDirection), sizeof(uint32_t));
    addToPackage(package, &(registerSize), sizeof(uint32_t));

    // Uso el socket de la interfaz.
    sendPackage(package, *interface->socket);

    destroyPackage(package);
}

void sendIOStdoutWriteOperationToIO(interface_t* interface, uint32_t registerDirection, uint32_t registerSize)
{
    t_package* package = createPackage(KERNEL_SEND_OPERATION_TO_STDOUT_INTERFACE);

    addToPackage(package, &(interface->processAssign->pid), sizeof(uint32_t));
    addToPackage(package, &(registerDirection), sizeof(uint32_t));
    addToPackage(package, &(registerSize), sizeof(uint32_t));

    // Uso el socket de la interfaz.
    sendPackage(package, *interface->socket);

    destroyPackage(package);
}

void sendProcessPathToMemory(pcb_t *process, char* path)
{
    
    t_package* package = createPackage(KERNEL_SEND_PROCESS_PATH);

    kernelPathProcess kernelPathProcess;
    kernelPathProcess.pid = process->pid;
    kernelPathProcess.path = path;

    addToPackage(package, &(kernelPathProcess.pid), sizeof(uint32_t));
    addToPackage(package, (kernelPathProcess.path), string_length(kernelPathProcess.path)+1); // +1 por el nulo 

    sendPackage(package, socketClientMemory); //Envio el paquete a memoria

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void sendEndProcessToMemory(pcb_t* processToEnd)
{
    t_package* package = createPackage(KERNEL_END_PROCESS);

    addToPackage(package, &(processToEnd->pid), sizeof(uint32_t)); // Agrego el PID para mandar la senial a memoria y que termine el proceso.

    sendPackage(package, socketClientMemory);

    destroyPackage(package);
}
