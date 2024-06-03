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

        switch (algorithm)
        {
        case FIFO:
            //No hace nada porque es el caso default
            sendContextToCPU(pcbToExec);
            break;
        
        case RR:
            sendContextToCPU(pcbToExec);
            pthread_t QuantumInterruptThread;
            pthread_create(&QuantumInterruptThread, NULL, (void*)quantumControlInterrupt, pcbToExec);
            pthread_detach(QuantumInterruptThread);
            break;

        case VRR:
            break;

        default:
            break;
        }
    }
  
}

void blockState()
{
    while (1)
    {
        sem_wait(&semBlock);
    }
    
}

void initShortTermPlanning()
{
    pthread_t readyStateThread;
    pthread_t execStateThread;
    pthread_t blockStateThread;
    pthread_create(&readyStateThread, NULL, (void*)readyState, NULL);
    pthread_create(&execStateThread, NULL, (void*)execState, NULL);
    pthread_create(&blockStateThread, NULL, (void*)blockState, NULL);
    pthread_detach(readyStateThread);
    pthread_detach(execStateThread);
    pthread_detach(blockStateThread);
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

void quantumControlInterrupt(pcb_t* pcbToExec)
{
        // Bloquea el hilo durante el quantum de tiempo
        usleep(getKernelConfig()->QUANTUM*1000);

        // Verifica si el proceso sigue ejecutándose o si termino el proceso y se elimino
        if (pcbToExec->state == PCB_EXEC || pcbToExec == NULL) {
            // Enviar interrupción para desalojar el proceso
            sendInterruptForQuantumEnd();
        }
}


void defineAlgorithm()
{
    if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "FIFO")) algorithm = FIFO;
    else if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "RR")) algorithm = RR;
    else if(string_equals_ignore_case(getKernelConfig()->ALGORITMO_PLANIFICACION, "VRR")) algorithm = VRR;
}