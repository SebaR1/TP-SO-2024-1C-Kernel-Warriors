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
        
        switch (algorithm)
        {
        case FIFO:
            //No hace nada porque es el caso default
            break;
        
        case RR:
            pthread_t QuantumInterruptThread;
            pthread_create(&QuantumInterruptThread, NULL, (void*)quantumControlInterrupt, NULL);
            pthread_detach(QuantumInterruptThread);
            break;

        case VRR:
            break;

        default:
            break;
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
        sem_post(&semMultiProgramming);
        pcbToExec->state = PCB_EXEC;
        log_info(getLogger(), "PID: %d - Estado Anterior: PCB_READY - Estado Actual: PCB_EXEC", pcbToExec->pid);
        if(algorithm == RR){
            sendContextToCPU(pcbToExec);
            sem_post(&semQuantum);
            //recieveResponseOfCPU();
        } else { // Si es FIFO
        sendContextToCPU(pcbToExec);
        //recieveResponseOfCPU();
        }
    }
  
}

void initShortTermPlanning()
{
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

void quantumControlInterrupt()
{
    while(1)
    {
        sem_wait(&semQuantum);
        pcb_t* pcbExec = list_get(pcbExecList->list, 0);
        // Bloquea el hilo durante el quantum de tiempo
        sleep(getKernelConfig()->QUANTUM/1000);

        
        // Verifica si el proceso sigue ejecutándose
        if (pcbExec->state == PCB_EXEC || pcbExec == NULL) {
            // Enviar interrupción para desalojar el proceso
            sendInterruptForQuantumEnd();
        }
    }
}