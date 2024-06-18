#include "console.h"

//Habilita una consola y valida las instrucciones
void readKernelConsole(){
    log_info(getLogger(), "Consola iniciada\n");
    char* read;
    read = readline("");

    while(!string_is_empty(read)){
        if(!_isAnInstruction(read)){
            log_error(getLogger(), "Comando de consola no recognized");
            free(read);
            read = readline("");
            continue;
        }

        attendInstruction(read);
        read = readline("");
    } //Para terminar la consola se pone algo vacio
    free(read);
}

//Valida si hay alguna instruccion con lo ingresado
bool _isAnInstruction(char* instruction){
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "EJECUTAR_SCRIPT")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
            return true;
    }
    
    string_array_destroy(consoleCommand);
    return false;
}

bool flagAuxStopPlanning = false;

//Atiende la instruccion
void attendInstruction(char* instruction)
{
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        addPcbToNew(consoleCommand[1]);
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        killProcess(atoi(consoleCommand[1]));
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        sem_wait(&semPausePlanning);
        flagAuxStopPlanning = true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        if(flagAuxStopPlanning) sem_post(&semPausePlanning);
        flagAuxStopPlanning = false;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "EJECUTAR_SCRIPT")){
        executeScript(consoleCommand[1]);
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        //Implementacion para MULTIPROGRAMACION
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
        showProcessByState();
    }

     string_array_destroy(consoleCommand);
}

void executeScript(char* path)
{
    FILE *fileScript = fopen(path, "r");

    if (fileScript == NULL) {

        exit(EXIT_FAILURE);

    }
}

void showProcessByState()
{
    char* listProcessNew =  _listProcess(pcbNewList->list);
    char* listProcessReady =  _listProcess(pcbReadyList->list);
    char* listProcessExec =  _listProcess(pcbExecList->list);
    char* listProcessBlock =  _listProcess(pcbBlockList->list);
    char* listProcessExit = _listProcess(pcbExitList->list);

    log_info(getLogger(), listProcessNew, "PCB_NEW");
    log_info(getLogger(), listProcessReady, "PCB_READY");
    log_info(getLogger(), listProcessExec, "PCB_EXEC");
    log_info(getLogger(), listProcessBlock, "PCB_BLOCK");
    log_info(getLogger(), listProcessExit, "PCB_EXIT");

    free(listProcessNew);
    free(listProcessReady);
    free(listProcessExec);
    free(listProcessBlock);
    free(listProcessExit);
}

char* _listProcess(t_list *list)
{
    char* pids = string_new();
    string_append(&pids, "Estado %s");// Ready %s: [");
    string_append(&pids, " : [");

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