#include "serverKernel.h"

int numberOfIOClients = 0;

sem_t semaphoreForIO;

bool _finishAllServersSignal = false;

// Funcion auxiliar que se ejecuta en cada iteracion de una lista para loggear su valor. Usada para el primer checkpoint.
void listIterator(char *element)
{
    log_info(getLogger(), "%s", element);
}

void receiveClientIteration(int socketServer)
{
    if (_finishAllServersSignal)
    {
        return;
    }

    // Esperar la conexión de un cliente
    int socketClient = waitClient(getLogger(), socketServer);
    if (socketClient == -1)
    {
        log_error(getLogger(), "Error al esperar cliente.\n");
        exit(EXIT_FAILURE);
    }

    // Recibir el codigo de operacion para saber de que tipo es el cliente que se quiere conectar
    operationCode opCode = getOperation(socketClient);
    if (_finishAllServersSignal)
    {
        return;
    }

    switch (opCode)
    {
    case IO_MODULE:

        if (numberOfIOClients >= MAX_IO_CLIENTS)
        {
            log_info(getLogger(), "Un cliente IO se intento conectar. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo IO");

        initServerForASocket(socketClient, serverKernelForIO);

        sem_wait(&semaphoreForIO);
        numberOfIOClients++;
        sem_post(&semaphoreForIO);

        break;

    case DO_NOTHING:
        break;

    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        break;
    }
}

void serverKernelForIO(int *socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_IO:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo IO");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo IO");
            operationPackageFromIO(listPackage);
            break;

        case DO_NOTHING:
            break;


        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}

void serverKernelForCPU(int *socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {

        case KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS: // Mock si llegara una señal de CPU si el proceso termino y pasa a exit
            cpuSendEndProcess();
            break;

        case KERNEL_SEND_INTERRUPT_QUANTUM_END: // Mock si llegara una señal de CPU por una interrupcion de kernel
            cpuSendInterruptQ(socketClient);
            break;
    
        case KERNEL_SEND_PROCESS_PATH: // Mock si llegara un WAIT de algun recurso. 
            cpuSendWaitOfProcess(socketClient);

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    //free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}

void operationPackageFromIO(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}

void cpuSendEndProcess()
{
    pcb_t* processExecToExit = list_pop(pcbExecList);
    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExecToExit->pid);
    list_push(pcbExitList, processExecToExit);
    sem_post(&semExit);
    sem_post(&semMultiProcessing);
}

void cpuSendInterruptQ(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExecToReady = assignContextToPcb(contextProcess);

    list_push(pcbReadyList, processExecToReady);
    processExecToReady->state = PCB_READY;
    log_info(getLogger(), "PID: %d - Desalojado por fin de Quantum", processExecToReady->pid);
    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_READY", processExecToReady->pid);

    list_destroy(listPackage);

    sem_post(&semMultiProcessing);
    sem_post(&semReady);
}

void cpuSendWaitOfProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    char* resourceName = (char*)list_remove(listPackage, 0);

    resource_t* resourceFound = foundResource(resourceName);

    if(resourceFound == NULL){ //Si el recurso pedido no existe, el proceso pasa a exit y se destruye.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {

        resourceFound->instances--; //Resta una instancia del recurso

        if(resourceFound->instances >= 0){ //Si el recurso queda >= 0 se le asigna ese recurso al proceso
            list_push(processExec->resources, resourceFound); // Asigna el recurso al proceso
            sendContextToCPU(processExec); // Manda el contexto devuelta a la Cpu para que siga ejecutando.

        } else { 
            list_push(resourceFound->blockList, processExec); // Pasa a la lista de bloqueados del recurso, esperando que se libere.
            log_info(getLogger(), "PID: %d - Bloqueado por: %s", processExec->pid, resourceFound->name);
            processExec->state = PCB_BLOCK;
            list_push(pcbBlockList, processExec);
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);

            sem_post(&semMultiProcessing);
            
        }
    }

    list_destroy(listPackage);
}

void cpuSendSignalofProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    char* resourceName = (char*)list_remove(listPackage, 0);

    resource_t* resourceFound = foundResource(resourceName);

    if(resourceFound == NULL){ //Si el recurso pedido no existe, el proceso pasa a exit y se destruye.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        resourceFound->instances++;

        // Se fija si el proceso tenia el recurso asignado, si lo tiene lo libera.
        if(list_remove_element(processExec->resources->list, resourceFound)){ // Se deberia hacer un mutex de esto... NOTA PARA SEBA DEL FUTURO
            log_info(getLogger(), "Se libero el recurso que tenia en WAIT");
        }

        // Se fija si hay algun proceso que este bloqueado para liberarlo por el signal.
        if(list_mutex_size(resourceFound->blockList) > 0){
            pcb_t* processBlockToReady = list_pop(resourceFound->blockList);
            processBlockToReady->state = PCB_READY;
            list_push(pcbReadyList, processBlockToReady);
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

        }

        sendContextToCPU(processExec);

        sem_post(&semBlock);
    }
}


contextProcess recieveContextFromPackage(t_list* package)
{
    contextProcess contextProcess;
    
    // Recibe todo el contexto del proceso ejecutando de CPU
    contextProcess.pc = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.AX = *(uint8_t *)list_remove(package, 0);
    contextProcess.registersCpu.BX = *(uint8_t *)list_remove(package, 0);
    contextProcess.registersCpu.CX = *(uint8_t *)list_remove(package, 0);
    contextProcess.registersCpu.DX = *(uint8_t *)list_remove(package, 0);
    contextProcess.registersCpu.EAX = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.EBX = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.ECX = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.EDX = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.DI = *(uint32_t *)list_remove(package, 0);
    contextProcess.registersCpu.SI = *(uint32_t *)list_remove(package, 0);

    return contextProcess;
}

pcb_t* assignContextToPcb(contextProcess contextProcess)
{
    pcb_t* process = list_pop(pcbExecList);

    // Asigna todo el contexto al proceso ejecutando
    process->pc = contextProcess.pc;
    process->registersCpu->AX = contextProcess.registersCpu.AX;
    process->registersCpu->BX = contextProcess.registersCpu.BX;
    process->registersCpu->CX = contextProcess.registersCpu.CX;
    process->registersCpu->DX = contextProcess.registersCpu.DX;
    process->registersCpu->EAX = contextProcess.registersCpu.EAX;
    process->registersCpu->EBX = contextProcess.registersCpu.EBX;
    process->registersCpu->ECX = contextProcess.registersCpu.ECX;
    process->registersCpu->EDX = contextProcess.registersCpu.EDX;
    process->registersCpu->DI = contextProcess.registersCpu.DI;
    process->registersCpu->SI = contextProcess.registersCpu.SI;

    return process;
}
