#include "processLoader.h"
#include "utils/utilsGeneral.h"
#include "paging/memoryUser.h"
#include "utilsMemory/delay.h"
#include "utils/mathMemory.h"
#include "utilsMemory/logger.h"
#include "connections/clientMemory.h"
#include "connections/serverMemory.h"



void initProcessesList()
{
    processesList = initListMutex();
}


bool loadProcessByPath(int PID, char* pseudocodePath)
{
    FILE* pseudocodeFile = fopen(pseudocodePath, "r");

    if (pseudocodeFile == NULL)
    {
        return false;
    }


    loadProcessByFile(PID, pseudocodeFile);

    fclose(pseudocodeFile);

    return true;
}



void loadProcessByFile(int PID, FILE* pseudocodeFile)
{
    processInfo* newElement = malloc(sizeof(processInfo)); // No olvidarse de liberar la memoria antes de eliminar el nodo de la lista.
    newElement->PID = PID;
    newElement->pageTable = NULL;

    // Obtener las instrucciones del archivo y ponerlas en el newElement->pseudocodeInstructions.

    char* buffer = 0;
    long length;

    // Obtengo todo el contenido del archivo de pseudocodigo y lo pongo en un buffer.
    fseek(pseudocodeFile, 0, SEEK_END);
    length = ftell(pseudocodeFile);
    fseek(pseudocodeFile, 0, SEEK_SET);

    buffer = malloc(length + 1);
    
    fread(buffer, 1, length, pseudocodeFile);
    buffer[length] = '\0';


    // Separo las instrucciones del buffer y lo asigno a la informacion de la estructura del pseudocodigo.
    newElement->pseudocodeInstructions = string_split(buffer, "\n");

    // Libero la memoria del buffer.
    free(buffer);

    // Agrego la informacion del pseudocodigo del nuevo proceso en la lista.
    list_push(processesList, newElement); 
}


void loadProcessByPathWithParams(void* params)
{
    // Retardo de la operacion
    memoryDelay();

    // La operacion

    kernelPathProcess* processPath = (kernelPathProcess*)params;

    bool result = loadProcessByPath(processPath->pid, processPath->path);

    if (result)
    {
        logCreateProcess(processPath->pid, 0);
    }
    else
    {
        logCreateProcessError(processPath->path);
    }

    sendProcessCreatedResult(socketKernel, result);

    free(processPath->path);
    free(processPath);
}


void destroyProcess(int PID)
{
    sem_wait(&semAuxPID);
    auxPID = PID;
    processInfo* info = list_remove_by_condition_mutex(processesList, closurePIDsAreEqual);
    sem_post(&semAuxPID);

    // LIbero las instrucciones guardadas
    string_array_destroy(info->pseudocodeInstructions);


    // Libero la memoria de usuario
    int bytesToFree = getAmountOfBytesAllocated(getMemoryConfig()->TAM_PAGINA, info->amountOfPages, info->internalFragmentation);
    int amountOfPagesFree = freeMemory(bytesToFree, info->pageTable, info->amountOfPages, info->internalFragmentation);

    free(info);

    logDestroyProcess(PID, amountOfPagesFree);
}


void destroyProcessWithParams(void* params)
{
    // Retardo de la operacion
    memoryDelay();

    // La operacion
    
    kernelEndProcess* processEnd = params;

    destroyProcess(processEnd->pid);

    free(processEnd);
}