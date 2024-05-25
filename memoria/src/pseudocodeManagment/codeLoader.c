#include "codeLoader.h"
#include "utils/utilsGeneral.h"



void initPseudocodeList()
{
    pseudocodeOfProcesses = initListMutex();
}


void loadProcessByPath(int PID, char* pseudocodePath)
{
    FILE* pseudocodeFile = fopen(pseudocodePath, "r");

    if (pseudocodeFile)
    {
        exit(EXIT_FAILURE);
    }


    loadProcessByFile(PID, pseudocodeFile);


    fclose(pseudocodeFile);
}



void loadProcessByFile(int PID, FILE* pseudocodeFile)
{
    pseudocodeInfo* newElement = malloc(sizeof(pseudocodeInfo)); // No olvidarse de liberar la memoria antes de eliminar el nodo de la lista.
    newElement->PID = PID;

    // Obtener las instrucciones del archivo y ponerlas en el newElement->pseudocodeInstructions.

    // Una opcion es crear una lista temporal donde ir agregando todas las lineas que lea en el archivo,
    // y despues pasar el contenido de esa lista al newElement->pseudocodeInstructions

    // Otra opcion es poner en un buffer temporal todo el contenido del archivo y deespues ir agregando todo lo de ese buffer en el newElement->pseudocodeInstructions
    



    newElement->pseudocodeInstructions;
    list_add(pseudocodeOfProcesses, newElement); 
}


void loadProcessByPathWithParams(void* params)
{
    kernelPathProcess* processPath = (kernelPathProcess*)params;

    loadProcessByPath(processPath->pid, processPath->path);

    free(processPath->path);
    free(processPath);
}