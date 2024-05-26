#include "codeLoader.h"
#include "utils/utilsGeneral.h"



void initPseudocodeList()
{
    pseudocodeOfProcesses = initListMutex();
}


void loadCodeByPath(int PID, char* pseudocodePath)
{
    FILE* pseudocodeFile = fopen(pseudocodePath, "r");

    if (pseudocodeFile)
    {
        exit(EXIT_FAILURE);
    }


    loadCodeByFile(PID, pseudocodeFile);


    fclose(pseudocodeFile);
}



void loadCodeByFile(int PID, FILE* pseudocodeFile)
{
    pseudocodeInfo* newElement = malloc(sizeof(pseudocodeInfo)); // No olvidarse de liberar la memoria antes de eliminar el nodo de la lista.
    newElement->PID = PID;

    // Obtener las instrucciones del archivo y ponerlas en el newElement->pseudocodeInstructions.

    char* buffer = 0;
    long length;

    // Obtengo todo el contenido del archivo de pseudocodigo y lo pongo en un buffer.
    fseek(pseudocodeFile, 0, SEEK_END);
    length = ftell(pseudocodeFile);
    fseek(pseudocodeFile, 0, SEEK_SET);

    buffer = malloc(length);
    
    fread(buffer, 1, length, pseudocodeFile);


    // Separo las instrucciones del buffer y lo asigno a la informacion de la estructura del pseudocodigo.
    newElement->pseudocodeInstructions = string_split(buffer, "\n");

    // Libero la memoria del buffer.
    free(buffer);

    // Agrego la informacion del pseudocodigo del nuevo proceso en la lista.
    list_push(pseudocodeOfProcesses, newElement); 
}


void loadCodeByPathWithParams(void* params)
{
    kernelPathProcess* processPath = (kernelPathProcess*)params;

    loadCodeByPath(processPath->pid, processPath->path);

    free(processPath->path);
    free(processPath);
}


void destroyCode(int PID)
{
    auxPID = PID;
    pseudocodeInfo* info = list_remove_by_condition_mutex(pseudocodeOfProcesses, closurePIDsAreEqual);

    string_array_destroy(info->pseudocodeInstructions);
    free(info);
}


void destroyCodeByParams(void* params)
{
    kernelEndProcess* processEnd = params;

    destroyCode(processEnd->pid);

    free(processEnd);
}