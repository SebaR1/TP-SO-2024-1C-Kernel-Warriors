#include "longTermPlanning.h"

//Hilo aparte que esta a la espera de que llegue la creacion de
//un proceso y lo mande a la lista de NEW
void newState()
{
    while(1)
    {
        sem_wait(&semNew);
        pcb_t* pcbToReady = list_pop(pcbNewList);
        list_push(pcbReadyList, pcbToReady);
        pcbToReady->state = PCB_READY;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_NEW - Estado Actual: PCB_READY", pcbToReady->pid);
        sem_post(&semReady);
        
    }
}

//Espera la llegada de procesos a EXIT
void exitState()
{
    while(1){
        sem_wait(&semExit);
        //Pido a memoria que libere todo lo asociado al proceso
        sem_post(&semMulti); //Manda un aviso que libera una parte del grado de multiprogramacion
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
void addPcbToNew()
{
    pcb_t *process = createProcess();
    list_push(pcbNewList, process); 
    log_info(getLogger(), "Se crea el proceso %d en NEW", process->pid);
    sem_post(&semNew);
}