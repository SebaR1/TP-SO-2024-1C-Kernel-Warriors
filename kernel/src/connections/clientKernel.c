#include "clientKernel.h"


void sendContextToCPU(pcb_t *process)
{
    t_package* package = createPackage(KERNEL_SEND_CONTEXT);

    contextProcess contextProcess;
    contextProcess.pc = process->pc;
    contextProcess.registersCpu = process->registersCpu;

    addToPackage(package, &(contextProcess.pc), sizeof(contextProcess.pc)); // Agrego el Program Counter del registros al paquete para enviar
    addToPackage(package, &(contextProcess.registersCpu), sizeof(contextProcess.registersCpu)); // Agrego los registros del contexto

    //sendPackage(package, socketDispatchKernel); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}
