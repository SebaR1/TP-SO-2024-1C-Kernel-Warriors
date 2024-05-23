#include "shortTermPlanning.h"

void readyState()
{
    
    while (1)
    {
        sem_wait(&semReady);
        char* listPids = _listPids(pcbReadyList->list);
        //Log Obligatorio
        log_info(getLogger(), listPids, getKernelConfig()->ALGORITMO_PLANIFICACION);
        free(listPids);

        //Si el algoritmo es FIFO no tiene que hacer nada
        //Ya que es el algoritmo por defecto
        if(string_equals_ignore_case("RR", getKernelConfig()->ALGORITMO_PLANIFICACION)){
            //Ordenar pcbReadyList segun RR
        }   else if(string_equals_ignore_case("VRR", getKernelConfig()->ALGORITMO_PLANIFICACION)){
            //Ordenar pcbReadyList segun VRR 
        }
        

        
        sem_post(&semExec);
    }
}

void execState()
{
    while(1)
    {
        sem_wait(&semMultiProcessing);
        sem_wait(&semExec);
        // Espero que se desocupe la CPU
        pcb_t *pcbToExec = list_pop(pcbReadyList);
        list_push(pcbExecList, pcbToExec);
        pcbToExec->state = PCB_EXEC;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXEC", pcbToExec->pid);
        pcbToExec = list_pop(pcbExecList);
        
        sendContextToCPU(pcbToExec);
    }
  
}

void initShortTermPlanning(){
    pthread_t readyStateThread;
    pthread_t execStateThread;
    //pthread_t blockStateThread;
    pthread_create(&readyStateThread, NULL, (void*)readyState, NULL);
    pthread_create(&execStateThread, NULL, (void*)execState, NULL);
    //pthread_create(&readyStateThread, NULL, (void*), NULL);
    pthread_detach(readyStateThread);
    pthread_detach(execStateThread);
}

char* _listPids(t_list *list)
{
    char* pids = string_new();
    string_append(&pids, "Cola Ready %s: [");

    for (int i = 0; i < list_size(list); i++)
    {
        pcb_t *process = list_get(list, i);
        char *pid = string_from_format("%d", process->pid);
        string_append(&pids, " ");
        string_append(&pids, pid);

        free(pid);
    }
    string_append(&pids, " ]");

    return pids;
}