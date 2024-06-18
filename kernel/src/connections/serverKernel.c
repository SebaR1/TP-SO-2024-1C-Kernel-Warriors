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


void serverKernelForMemory(int *socketClient)
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

        case MEMORY_SEND_RESPONSE_FOR_NEW_PROCESS:
            memorySendResponseForNewProcess(socketClient);
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

        case IO_SEND_INTERFACE: 
            ioSendInterface(socketClient);
            break;

        case IO_OK: 
            ioSendEndOperation(socketClient);
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

        case CPU_SEND_CONTEXT_FOR_EXIT: 
            cpuSendExitProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_END_PROCESS:
            cpuSendInterruptKillProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_END_QUANTUM: 
            cpuSendInterruptQ(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_WAIT: 
            cpuSendWaitOfProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_SIGNAL: 
            cpuSendSignalofProcess(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_GENERIC: 
            cpuSendRequestForIOGenSleep(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_STDIN:
            cpuSendRequestForIOStdinRead(socketClient);
            break;

        case CPU_SEND_CONTEXT_FOR_IO_STDOUT:
            cpuSendRequestForIOStdoutWrite(socketClient);
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

    //free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}

void memorySendResponseForNewProcess(int *socketClientMemory)
{
    t_list *listPackage = getPackage(*socketClientMemory);

    flagMemoryResponse = *(bool*)list_remove(listPackage, 0);

    sem_post(&semMemoryOk);
}

void operationPackageFromIO(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}

void ioSendInterface(int *socketClientIO)
{
    t_list *listPackage = getPackage(*socketClientIO);

    // Recibo el nombre y tipo de la interfaz.
    char* nameInterface = (char*)list_remove(listPackage, 0);
    interfaceType typeInterface = *(interfaceType*)list_remove(listPackage, 0);
    
    // Creo la interfaz.
    interface_t *newInterface = createInterface(nameInterface, typeInterface);

    // Guardo el socket, para tener la conexion.
    newInterface->socket = socketClientIO;

    list_push(interfacesList, newInterface);

    list_destroy(listPackage);

}

void ioSendEndOperation(int *socketClientIO)
{
    t_list *listPackage = getPackage(*socketClientIO);

    // Recibo el nombre y tipo de la interfaz.
    char* nameInterface = (char*)list_remove(listPackage, 0);

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameInterface);

    pcb_t *processBlockToReady = interfaceFound->processAssign;

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    if(!interfaceFound->flagKillProcess){ // Este flag es por si se tiro un finalizar_proceso mientras el proceso estaba en una interfaz realizando una operacion. Caso MUY particular.

        interfaceFound->isBusy = false;
        interfaceFound->processAssign = NULL;

        processBlockToReady->isInInterface = false;

        list_remove_element_mutex(pcbBlockList, processBlockToReady); // Remuevo el proceso de la pcbBlockList.

        if(algorithm != VRR){ //ESTO SI EL ALGORITMO DE PLANIFICACION ES RR O FIFO.

            list_push(pcbReadyList, processBlockToReady); // Lo paso a la lista de ready porque ya termino su operacion.

            processBlockToReady->state = PCB_READY;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

            sem_post(&semReady);

        } else {

            list_push(pcbReadyPriorityList, processBlockToReady); // Pasa a pcbReadyPriorityList porque es casi seguro que no se va a dar la casualidad de que haya entrado a block justo en el tiempo que se termino el Quantum.

            processBlockToReady->state = PCB_READY_PLUS;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY_PLUS", processBlockToReady->pid);

            sem_post(&semReady);

        }

    } else {

        sem_post(&semKillProcessInInterface);

    }

    


    if(!list_mutex_is_empty(interfaceFound->blockList)){ // Se fija si la interfaz tiene algun otro proceso en espera. 
        pcb_t* processBlocked = list_pop(interfaceFound->blockList);

        interfaceFound->processAssign = processBlocked;
        interfaceFound->isBusy = true;
        
        processBlocked->isInInterface = true;

        switch (interfaceFound->interfaceType)
        {
        case Generic:
            int timeOfOperation = atoi(processBlocked->params->param1); // Agarro el parametro que tenia y lo convierto a int.
            uint32_t timeOfOperationCast = (uint32_t)timeOfOperation; // Lo casteo para que se mande de esa forma por problemas de arquitectura entre computadoras. 
            sendIOGenSleepOperationToIO(interfaceFound, timeOfOperationCast); 
            break;

        case STDIN:
            int registerDirectionRead = atoi(processBlocked->params->param1);
            int registerSizeRead = atoi(processBlocked->params->param2);
            sendIOStdinReadOperationToIO(interfaceFound, registerDirectionRead, registerSizeRead);
            break;
        
        case STDOUT:
            int registerDirectionWrite = atoi(processBlocked->params->param1);
            int registerSizeWrite = atoi(processBlocked->params->param2);
            sendIOStdoutWriteOperationToIO(interfaceFound, registerDirectionWrite, registerSizeWrite);
            break;

        default:
            break;
        }
    }

    list_destroy(listPackage);

}

void cpuSendRequestForIOGenSleep(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    // Recibo la cantidad de tiempo a utilizar.
    uint32_t timeOfOperation = *(uint32_t*)list_remove(listPackage, 0);

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != Generic){ // El tipo de interfaz Generic es el unico que puede realizar la operacion IO_GEN_SLEEP. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por INTERFAZ : %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing); // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.


            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOGenSleepOperationToIO(interfaceFound, timeOfOperation);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = string_itoa(timeOfOperation); // Se guarda el tiempo de operacion para usarse despues que la interfaz este liberada. 
            }
        }
    }

    list_destroy(listPackage);

}

void cpuSendRequestForIOStdinRead(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    uint32_t registerDirection = *(uint32_t*)list_remove(listPackage, 0);

    uint32_t registerSize = *(uint32_t*)list_remove(listPackage, 0);

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != STDIN){ // El tipo de interfaz STDIN es el unico que puede realizar la operacion IO_STDIN_READ. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por INTERFAZ : %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR); 

            sem_post(&semMultiProcessing);; // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.
            
            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOStdinReadOperationToIO(interfaceFound, registerDirection, registerSize);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = string_itoa(registerDirection); // Se guarda el la direccion de registro para usarse despues que la interfaz este liberada. 
                processExec->params->param2 = string_itoa(registerSize); // Se guarda el la direccion de registro para usarse despues que la interfaz este liberada. 

            }
        }
    }

    list_destroy(listPackage);

}

void cpuSendRequestForIOStdoutWrite(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso 
    pcb_t *processExec = assignContextToPcb(contextProcess);

    // Recibo el nombre de la interfaz a usar.
    char* nameRequestInterface = list_remove(listPackage, 0);

    uint32_t registerDirection = *(uint32_t*)list_remove(listPackage, 0);

    uint32_t registerSize = *(uint32_t*)list_remove(listPackage, 0);

    // Busco la interfaz por el nombre identificador.
    interface_t *interfaceFound = foundInterface(nameRequestInterface);

    if(interfaceFound == NULL){ // Si no existe se manda el proceso a exit.
        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        if(interfaceFound->interfaceType != STDOUT){ // El tipo de interfaz STDOUT es el unico que puede realizar la operacion IO_STDOUT_WRITE. COn verificar que no sea de este tipo directamente no admite la operacion y pasa a exit.
            list_push(pcbExitList, processExec);
            processExec->state = PCB_EXIT;
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);
            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_INTERFACE", processExec->pid);

            sem_post(&semMultiProcessing);
            sem_post(&semExit);

        } else {
            list_push(pcbBlockList, processExec); // Una vez dada las validaciones el kernel envia el proceso a pcbBlockList.
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_BLOCK", processExec->pid);
            log_info(getLogger(), "PID: %d - Bloqueado por INTERFAZ : %s", processExec->pid, interfaceFound->name); // Para testeos puse el nombre de la interfaz para que sea mas claro.

            processExec->state = PCB_BLOCK;

            // Detengo el tiempo en el que estuvo en exec si el algoritmo de plani es VRR.
            if(algorithm == VRR) temporal_stop(processExec->quantumForVRR);
            
            sem_post(&semMultiProcessing);; // Una vez que pasan a pcbBlockList dejan lugar en exec a otro proceso.
            
            if(!interfaceFound->isBusy){ // Se fija si la interfaz no esta ocupada y lo asigna. 
                interfaceFound->isBusy = true;
                interfaceFound->processAssign = processExec;

                processExec->isInInterface = true;

                sendIOStdoutWriteOperationToIO(interfaceFound, registerDirection, registerSize);
                
            } else {
                list_push(interfaceFound->blockList, processExec); // Se agrega el proceso a la lista de espera de esa interfaz.

                processExec->params->param1 = string_itoa(registerDirection); // Se guarda el la direccion de registro para usarse despues que la interfaz este liberada. 
                processExec->params->param2 = string_itoa(registerSize); // Se guarda el la direccion de registro para usarse despues que la interfaz este liberada. 

            }
        }
    }

    list_destroy(listPackage);

}



void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}

void cpuSendExitProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado de pcbExecList
    pcb_t *processExecToExit = assignContextToPcb(contextProcess);

    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExecToExit->pid);

    processExecToExit->state = PCB_EXIT;

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    log_info(getLogger(), "Finaliza el proceso %d - Motivo: SUCCESS", processExecToExit->pid);

    list_push(pcbExitList, processExecToExit);

    list_destroy(listPackage);

    sem_post(&semExit);
    sem_post(&semMultiProcessing);
}

void cpuSendInterruptKillProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec.
    pcb_t *processExecToExit = assignContextToPcb(contextProcess);

    processExecToExit->state = PCB_EXEC; //Esto solo para que no me tire el warning.

    sem_post(&semKillProcessExec);

    list_destroy(listPackage);

}

void cpuSendInterruptQ(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec.
    pcb_t *processExecToReady = assignContextToPcb(contextProcess);

    list_push(pcbReadyList, processExecToReady);
    processExecToReady->state = PCB_READY;

    log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_READY", processExecToReady->pid);

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    log_info(getLogger(), "PID: %d - Desalojado por fin de Quantum", processExecToReady->pid);

    list_destroy(listPackage);

    sem_post(&semMultiProcessing);
    sem_post(&semReady);
}

void cpuSendWaitOfProcess(int *socketClientCPUDispatch)
{
    t_list *listPackage = getPackage(*socketClientCPUDispatch);

    // Recibo el contexto del paquete
    contextProcess contextProcess = recieveContextFromPackage(listPackage);

    // Asigno todo el contexto que recibi de CPU al proceso popeado en Exec
    pcb_t *processExec = assignContextToPcb(contextProcess);

    char* resourceName = (char*)list_remove(listPackage, 0);

    resource_t* resourceFound = foundResource(resourceName);


    if(resourceFound == NULL){ //Si el recurso pedido no existe, el proceso pasa a exit y se destruye.

        list_push(pcbExitList, processExec);
        processExec->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processExec->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {

        subtractInstanceResource(resourceFound); // Resta una instancia del recurso.

        if(resourceFound->instances >= 0){ //Si el recurso queda >= 0 se le asigna ese recurso al proceso

            list_push(processExec->resources, resourceFound); // Asigna el recurso al proceso

            list_push(pcbExecList, processExec); // El proceso tiene que volver devuelta a exec PORQUE PUEDE. (Podria generar segmentacion fault sino)

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

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INVALID_RESOURCE", processExec->pid);

        sem_post(&semMultiProcessing);
        sem_post(&semExit);

    } else {
        addInstanceResource(resourceFound); // Suma uno a la instancia del recurso.

        // Se fija si el proceso tenia el recurso asignado, si lo tiene lo libera.
        if(list_remove_element_mutex(processExec->resources, resourceFound)){ 
            log_info(getLogger(), "Se libero el recurso que tenia en WAIT");
        }

        // Se fija si hay algun proceso que este bloqueado para asignarlo por el signal.
        if(list_mutex_size(resourceFound->blockList) > 0){
            pcb_t* processBlockToReady = list_pop(resourceFound->blockList);
            list_push(processBlockToReady->resources, resourceFound);
            processBlockToReady->state = PCB_READY;
            list_push(pcbReadyList, processBlockToReady);
            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

        }
        list_push(pcbExecList, processExec); // El proceso tiene que volver devuelta a exec PORQUE PUEDE. (Podria generar segmentacion fault sino)

        sendContextToCPU(processExec);

        sem_post(&semBlock);  // No hace nada pero me quedaba bonito. :s
    }

    list_destroy(listPackage);
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
