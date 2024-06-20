#include "longTermPlanning.h"
#include "pthread.h"


//Hilo aparte que esta a la espera de que llegue la creacion de
//un proceso y lo mande a la lista de NEW
void newState()
{
    while(1)
    {
        sem_wait(&semMultiProgramming);
        sem_wait(&semNew);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        if(!list_mutex_is_empty(pcbNewList)){
        pcb_t* pcbToReady = list_pop(pcbNewList);
        list_push(pcbReadyList, pcbToReady);
        pcbToReady->state = PCB_READY;

        //Log obligatorio
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_READY", pcbToReady->pid);
        sem_post(&semReady);
        }
    }
}

//Espera la llegada de procesos a EXIT
void exitState()
{
    while(1){

        sem_wait(&semExit);
        pcb_t *process = list_pop(pcbExitList);

        pcbState_t prevState = process->state;

        process->state = PCB_EXIT;

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        // Pido a memoria que libere todo lo asociado al proceso.
        pthread_mutex_lock(&mutex2);
        sendEndProcessToMemory(process);
        pthread_mutex_unlock(&mutex2);
        log_info(getLogger(), "Se borro el proceso con PID: %d", process->pid);
        destroyProcess(process);

        if(prevState != PCB_NEW){
            if(diffBetweenNewAndPrevMultiprogramming > 0){
                diffBetweenNewAndPrevMultiprogramming --;
            } else {
                sem_post(&semMultiProgramming);
            }
        }

    }
}

//Inicia el planificador a largo plazo
void initLongTermPlanning(){
    pthread_t newStateThread;
    pthread_t exitStateThread;
    pthread_create(&newStateThread, NULL, (void*)newState, NULL);
    pthread_create(&exitStateThread, NULL, (void*)exitState, NULL);
    pthread_detach(newStateThread);
    pthread_detach(exitStateThread);
}

//Crea un proceso
pcb_t* createProcess()
{
    pcb_t *process = malloc(sizeof(pcb_t));
    sem_wait(&semAddPid);
    process->pid = ++pid;
    sem_post(&semAddPid);
    process->pc = 0;
    process->state = PCB_NEW;
    process->isInInterface = false;
    process->resources = initListMutex();
    process->registersCpu = malloc(sizeof(t_registers));
    process->registersCpu->AX = 0;
    process->registersCpu->BX = 0;
    process->registersCpu->CX = 0;
    process->registersCpu->DX = 0;
    process->registersCpu->EAX = 0;
    process->registersCpu->EBX = 0;
    process->registersCpu->ECX = 0;
    process->registersCpu->EDX = 0;
    process->registersCpu->SI = 0;
    process->registersCpu->DI = 0;
    process->params = malloc(sizeof(paramsKernelForIO));
    return process;
};



//Agrega el proceso a la lista de NEW
void addPcbToNew(char* path)
{

    pcb_t *process = createProcess();

    sem_wait(&semPausePlanning);
    sem_post(&semPausePlanning);

    pthread_mutex_lock(&mutexSendProcessToMemory);
    sendProcessPathToMemory(process, path);
    sem_wait(&semMemoryOk); // Esperan a que la memoria de el ok de que el proceso se creo correctamente
    pthread_mutex_unlock(&mutexSendProcessToMemory);

    if(flagMemoryResponse)
    {
        list_push(pcbNewList, process); 
        //Log obligatorio
        log_info(getLogger(), "Se crea el proceso %d en NEW", process->pid);
        sem_post(&semNew);
    } else {

        log_info(getLogger(), "El proceso no se pudo abrir en memoria.");
        destroyProcess(process);

    }
    
    free(path);
}

void destroyProcess(pcb_t *process)
{
    for(int i = 0; i < list_size(process->resources->list); i++) // Pasa por todos los recursos asignados que tiene para basicamente hacerles un signal.
    {
        resource_t* resourceToFree = list_pop(process->resources); // Popea el recurso asignado
        addInstanceResource(resourceToFree);
        
        if(list_mutex_size(resourceToFree->blockList) > 0){ // Se fija si tiene procesos bloqueados que esperen este recurso.
        pcb_t* processBlockToReady = list_pop(resourceToFree->blockList);
        processBlockToReady->state = PCB_READY;
        list_push(pcbReadyList, processBlockToReady);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);

        sem_post(&semReady);
        }

    }   

    destroyListMutex(process->resources); 
    free(process->params);
    free(process->registersCpu);
    free(process);
}

void killProcess(uint32_t *paramkillProcessThread)
{    
    uint32_t pid = *paramkillProcessThread;
    pcb_t* processFound = foundStatePcb(pid);

    if(processFound == NULL){
        log_info(getLogger(), "PID: %d - No encontrado", pid);
        return;
    }

    switch (processFound->state)
    {
    case PCB_NEW:
        list_remove_element_mutex(pcbNewList, processFound);

        processFound->state = PCB_EXIT;
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        processFound->state = PCB_NEW; // Este cambio devuelta es para que en exitState() pueda saber que era un proceso que no afectaba al grado de multiprogramacion.
        
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
        sem_post(&semExit);
        break;
    
    case PCB_READY:
        list_remove_element_mutex(pcbReadyList, processFound);

        char* listPids = _listPids(pcbReadyList->list);
        //Log Obligatorio
        log_info(getLogger(), listPids, getKernelConfig()->ALGORITMO_PLANIFICACION);
        free(listPids);


        processFound->state = PCB_EXIT; 
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);

        sem_post(&semExit);
        break;

    case PCB_EXEC:
        sendInterruptForConsoleEndProcess(processFound);

        sem_wait(&semKillProcessExec);

        processFound->state = PCB_EXIT;
        list_push(pcbExitList, processFound);

        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processFound->pid);

        sem_wait(&semPausePlanning);
        sem_post(&semPausePlanning);

        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);

        sem_post(&semExit);
        sem_post(&semMultiProcessing);        
        break;

    case PCB_BLOCK:

        if(processFound->isInInterface) // Esto es para el caso que se finalice un proceso y justo este operando algo en una interfaz, tiene que esperar hasta que termine para matarlo.
        { 
            interface_t* interface = foundInterfaceByProcessPidAssign(processFound->pid);

            if(interface == NULL) log_error(getLogger(), "Este error no deberia pasar nunca.");

            interface->flagKillProcess = true;

            sem_wait(&semKillProcessInInterface);

            interface->flagKillProcess = false;

            list_remove_element_mutex(pcbBlockList, processFound);

            processFound->state = PCB_EXIT;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_EXIT", processFound->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
            list_push(pcbExitList, processFound);
            sem_post(&semExit);
        } else {

            list_remove_element_mutex(pcbBlockList, processFound);

            processFound->state = PCB_EXIT;

            log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_EXIT", processFound->pid);

            sem_wait(&semPausePlanning);
            sem_post(&semPausePlanning);

            log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
            list_push(pcbExitList, processFound);
            sem_post(&semExit);

        } 
        break;

    default:
        log_error(getLogger(), "Este error no deberia pasar nunca.");
        break;
    }

    free(paramkillProcessThread);

}

int pidToFind;

bool compare_pid(void *data) 
{
    pcb_t *pcb = (pcb_t *)data;
    return pcb->pid == pidToFind;
}

pcb_t* foundStatePcb(uint32_t pid)
{
    pidToFind = pid;

    pcb_t* processFound = (pcb_t *)list_find_mutex(pcbNewList, compare_pid);
    if(processFound != NULL) return processFound;
    
    processFound = (pcb_t *)list_find_mutex(pcbReadyList, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find_mutex(pcbBlockList, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find_mutex(pcbExecList, compare_pid);
    if(processFound != NULL) return processFound;

    return NULL;
}
