#include "longTermPlanning.h"

//Hilo aparte que esta a la espera de que llegue la creacion de
//un proceso y lo mande a la lista de NEW
void newState()
{
    while(1)
    {
        sem_wait(&semMultiProgramming);
        sem_wait(&semNew);
        pcb_t* pcbToReady = list_pop(pcbNewList);
        list_push(pcbReadyList, pcbToReady);
        pcbToReady->state = PCB_READY;
        //Log obligatorio
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_READY", pcbToReady->pid);
        sem_post(&semReady);
    }
}

//Espera la llegada de procesos a EXIT
void exitState()
{
    while(1){
        sem_wait(&semExit);
        pcb_t *process = list_pop(pcbExitList);
        process->state = PCB_EXIT;
        //Pido a memoria que libere todo lo asociado al proceso
        log_info(getLogger(), "Se borro el proceso con PID: %d", process->pid);
        destroyProcess(process);
        sem_post(&semMultiProgramming); //Manda un aviso que libera una parte del grado de multiprogramacion
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
    free(process->registersCpu);
    free(process);
}

int pid_to_find;

bool compare_pid(void *data) 
{
    pcb_t *pcb = (pcb_t *)data;
    return pcb->pid == pid_to_find;
}

void finalizar_proceso(uint32_t pid)
{
    pid_to_find = pid;
    
    pcb_t* processFound = (pcb_t *)list_find(pcbReadyList->list, compare_pid);
    if(processFound !=NULL){
        log_info(getLogger(), "LO ENCONTRE");
        if(list_remove_element(pcbReadyList->list, processFound)){
        log_info(getLogger(), "SE BORROOO");
        }
        processFound->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXIT", processFound->pid);
        list_push(pcbExitList, processFound);
        sem_post(&semExit);
    } 

    pcb_t* processFound2 = (pcb_t *)list_find(pcbNewList->list, compare_pid);

    if (processFound2 != NULL){
        log_info(getLogger(), "LO ENCONTRE EN NEW");
        if(list_remove_element(pcbNewList->list, processFound2)){
        log_info(getLogger(), "SE BORROOO EN NEW");
        }
        processFound2->state = PCB_EXIT;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_EXIT", processFound2->pid);
        list_push(pcbExitList, processFound2);
        log_info(getLogger(), "LLEGA A PUSHEAR");
        sem_post(&semExit);
    }
	
}







