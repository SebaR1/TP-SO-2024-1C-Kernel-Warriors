#include "longTermPlanning.h"

//Hilo aparte que esta a la espera de que llegue la creacion de
//un proceso y lo mande a la lista de NEW
void newState()
{
    while(1)
    {
        sem_wait(&semMultiProgramming);
        sem_wait(&semNew);
        if(!list_is_empty(pcbNewList->list)){
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
        //Pido a memoria que libere todo lo asociado al proceso
        log_info(getLogger(), "Se borro el proceso con PID: %d", process->pid);
        destroyProcess(process);
        if(prevState != PCB_NEW){ // Este if es porque se fija si estaba en NEW, porque en ese lugar no afectaba al grado de multiprogramacion
        sem_post(&semMultiProgramming); //Manda un aviso que libera una parte del grado de multiprogramacion
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
    process->registersCpu->PC = 0;
    return process;
};

//Agrega el proceso a la lista de NEW
void addPcbToNew(char* path)
{
    pcb_t *process = createProcess();
    sendProcessPathToMemory(process, path);

    list_push(pcbNewList, process); 
    //Log obligatorio
    log_info(getLogger(), "Se crea el proceso %d en NEW", process->pid);
    sem_post(&semNew);
}

void destroyProcess(pcb_t *process)
{
    for(int i = 0; i < list_size(process->resources->list); i++) // Pasa por todos los recursos asignados que tiene para basicamente hacerles un signal.
    {
        resource_t* resourceToFree = list_pop(process->resources); // Popea el recurso asignado
        resourceToFree->instances++;

        if(list_mutex_size(resourceToFree->blockList) > 0){ // Se fija si el tiene procesos bloqueados que esperen este recurso.
        pcb_t* processBlockToReady = list_pop(resourceToFree->blockList);
        processBlockToReady->state = PCB_READY;
        list_push(pcbReadyList, processBlockToReady);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_BLOCK - Estado Actual: PCB_READY", processBlockToReady->pid);
        }

    }   

    destroyListMutex(process->resources); 
    free(process->registersCpu);
    free(process);
}

int pidToFind;

bool compare_pid(void *data) 
{
    pcb_t *pcb = (pcb_t *)data;
    return pcb->pid == pidToFind;
}

void killProcess(uint32_t pid)
{
    
    pcb_t* processFound = foundStatePcb(pid);

    if(processFound == NULL){
        log_info(getLogger(), "PID: %d - No encontrado", pid);
        return;
    }

    switch (processFound->state)
    {
    case PCB_NEW:
        list_remove_element(pcbNewList->list, processFound);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_EXIT", processFound->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
        list_push(pcbExitList, processFound);
        sem_post(&semExit);
        break;
    
    case PCB_READY:
        list_remove_element(pcbReadyList->list, processFound);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXIT", processFound->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
        list_push(pcbExitList, processFound);
        sem_post(&semExit);
        break;

    case PCB_EXEC:
        list_remove_element(pcbExecList->list, processFound);
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_EXEC - Estado Actual: PCB_EXIT", processFound->pid);
        log_info(getLogger(), "Finaliza el proceso %d - Motivo: INTERRUPTED_BY_USER", processFound->pid);
        list_push(pcbExitList, processFound);
        sendInterruptForConsoleEndProcess();
        sem_post(&semExit);
        sem_post(&semMultiProcessing);        
        break;

    default:
        //Nada
        break;
    }

}

pcb_t* foundStatePcb(uint32_t pid)
{
    pidToFind = pid;

    pcb_t* processFound = (pcb_t *)list_find(pcbNewList->list, compare_pid);
    if(processFound != NULL) return processFound;
    
    processFound = (pcb_t *)list_find(pcbReadyList->list, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find(pcbBlockList->list, compare_pid);
    if(processFound != NULL) return processFound;

    processFound = (pcb_t *)list_find(pcbExecList->list, compare_pid);
    if(processFound != NULL) return processFound;

    return NULL;
}
