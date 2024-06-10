#include "memoryUser.h"


#include <stdio.h>
#include <stdlib.h>
#include "utilsMemory/config.h"
#include "math.h"
#include <string.h>



// El espacio contiguo de memoria que representa la memoria del usuario
void* memoryUser;


// Array de bools que dicen si un frame está libre (false) u ocupado (true)
bool* memoryUserFrames;


sem_t semMemoryUserFrames;


// Cantidad maxima de frames que tiene la memoria de usuario
int maxFrames;


// Cantidad de frames libres que tiene la memoria de usuario actualmente.
int amountOfFramesFree;


// Ultimo framq que se comprobó si está libre u ocupado.
int lastIndexChecked;




void initMemoryUser()
{
    memoryUser = malloc(getMemoryConfig()->TAM_MEMORIA);

    maxFrames = getMemoryConfig()->TAM_MEMORIA / getMemoryConfig()->TAM_PAGINA;

    memoryUserFrames = calloc(maxFrames, sizeof(bool)); // calloc para que todas las paginas se inicialicen como libres (en 0, false)

    sem_init(&semMemoryUserFrames, 0, 1);

    amountOfFramesFree = maxFrames;

    lastIndexChecked = 0;
}


bool isFrameBusy(int frame)
{
    return memoryUserFrames[frame];
}


int allocMemory(int bytes, int* pages, int* const amountOfPages, int* const internalFragmentation)
{
    ///////////// CASOS DE CORTE /////////////


    // Compruebo si los bytes que se quieren reservar entran en la fragmentacion interna que sobró de la ultima pagina actual.
    // Si es asi, uso esa memoria de fragmentacion interna como memoria de usuario valida.
    // Tambien entra en este if si se intentan reservar 0 bytes, caso en el que no cambiaria nada y solamente retornaria 0
    if (bytes <= *internalFragmentation)
    {
        *internalFragmentation -= bytes;
        return 0;
    }


    // Lleno la ultima pagina actual que tenia fragmentacion interna, para que todas las paginas actuales queden ocupadas completamente.
    bytes -= *internalFragmentation;
    

    // Obtengo la cantidad de paginas que deberian reservarse.
    int amountOfNewPages = getAmountOfPagesAllocated(bytes);


    /////// A partir de aca solo un hilo puede reservar o liberar la memoria, para que no haya inconsistencias poniendo como libres u ocupados los frames ///////
    sem_wait(&semMemoryUserFrames);



    // Si no hay suficientes paginas para reservar la cantidad que se pide, retorna Out Of Memory (-1)
    if (amountOfNewPages > amountOfFramesFree)
    {
        sem_post(&semMemoryUserFrames);
        return OUT_OF_MEMORY;
    }



    ///////////// ALOJO SEGURO A PARTIR DE ACA, AL MENOS SE RESERVA UNA NUEVA PAGINA /////////////



    // Obtengo el indice desde el que hay que empezar a buscarle frames libres a las paginas.
    int newPagesIndex = *amountOfPages;

    // Obtengo la cantidad total de paginas que ocupará todo el proceso.
    *amountOfPages +=  amountOfNewPages;


    // Expando el array de paginas
    pages = realloc(pages, *amountOfPages * sizeof(int));

    // Le asigno un frame libre a cada nueva pagina.
    for ( ; newPagesIndex < amountOfPages; newPagesIndex++)
    {
        pages[newPagesIndex] = allocNextFrameFree();
    }


    // A partir de aca, ya se terminó de settear los valores correctos a los frames de la memoria.
    sem_post(&semMemoryUserFrames);


    // Obtengo la nueva fragmentacion interna de la ultima pagina
    *internalFragmentation = getInternalFragmentation(bytes);


    return amountOfNewPages;
}



int freeMemory(int bytes, int* pages, int* const amountOfPages, int* const internalFragmentation)
{
    ///////////// CASOS DE CORTE /////////////


    // Si se intenta liberar 0 bytes, no hace nada y retorna que se liberaron 0 paginas.
    // Teoricamente, este caso solo es necesario para contemplar la posibilidad de que la memoria del proceso sea 0 y se intentasen liberar 0 bytes
    if (bytes <= 0)
    {
        return 0;
    }

    // Obtengo la cantidad de bytes que hay reservados en la ultima pagina.
    int amountOfBytesInTheLastPage = getAmountOfBytesInTheLastPage(*internalFragmentation);

    // Compruebo si la cantidad de bytes que quiero liberar es posible liberarlos solamente de la ultima pagina.
    // Si es asi, los libero de la ultima pagina solamente.
    // Tambien entra en este if si se intentan liberar 0 bytes, caso en el que no cambiaria nada y solamente retornaria 0.
    if (bytes < amountOfBytesInTheLastPage)
    {
        *internalFragmentation += bytes;
        return 0;
    }



    ///////////// LIBERACION SEGURA A PARTIR DE ACA, AL MENOS SE LIBERA UNA PAGINA /////////////


    // Libero completa la ultima liberado todos sus bytes reservados.
    bytes -= amountOfBytesInTheLastPage;


    // Obtengo la cantidad de paginas que debo liberar.
    int amountOfPagesToFree = floor(bytes / getMemoryConfig()->TAM_PAGINA) + 1;


    // Indice con el que recorrer el final del array de paginas.
    int i = *amountOfPages - 1;

    // Actualizo la cantidad de paginas.
    *amountOfPages -= amountOfPagesToFree;



    /////// A partir de aca solo un hilo puede reservar o liberar la memoria, para que no haya inconsistencias poniendo como libres u ocupados los frames ///////
    sem_wait(&semMemoryUserFrames);


    // Libero los frames que ya no estan en uso.
    for ( ; i >= *amountOfPages; i--)
    {
        freeFrame(pages[i]);
    }


    // A partir de aca, ya se terminó de settear los valores correctos a los frames de la memoria.
    sem_post(&semMemoryUserFrames);

    
    // Reduzco el array de paginas
    pages = realloc(pages, *amountOfPages * sizeof(int));


    // Obtengo la nueva fragmentacion interna de la ultima pagina
    *internalFragmentation = bytes % getMemoryConfig()->TAM_PAGINA;


    return amountOfPagesToFree;
}



int getInternalFragmentation(int bytesAllocated)
{
    int sizePag = getMemoryConfig()->TAM_PAGINA;
    return (  sizePag - (  bytesAllocated % sizePag  )  ) % sizePag;
}

int getAmountOfBytesInTheLastPage(int internalFragmentation)
{
    return getMemoryConfig()->TAM_PAGINA - internalFragmentation;
}

int getAmountOfBytesAllocated(int amountOfPages, int internalFragmentation)
{
    return amountOfPages * getMemoryConfig()->TAM_PAGINA - internalFragmentation;
}

int getAmountOfPagesAllocated(int bytesAllocated)
{
    return ceil(bytesAllocated / getMemoryConfig()->TAM_PAGINA);
}

int getAmountOfPagesToFree(int bytesToFree)
{
    return floor(bytesToFree / getMemoryConfig()->TAM_PAGINA) + 1;
}


int allocNextFrameFree()
{
    do
    {
        lastIndexChecked = (lastIndexChecked + 1) % maxFrames;
    } while (isFrameBusy(lastIndexChecked));
    
    memoryUserFrames[lastIndexChecked] = true;

    amountOfFramesFree--;

    return lastIndexChecked;
}



void freeFrame(int frame)
{
    memoryUserFrames[frame] = false;

    amountOfFramesFree++;
}





void* readBytes(int physicalAddress, int size)
{
    void* data = malloc(size);
    
    memcpy(data, memoryUser + physicalAddress, size);

    return data;
}


void writeBytes(void* data, int physicalAddress, int size)
{
    memcpy(memoryUser + physicalAddress, data, size);
}


int getFrame(int PID, int page)
{
    processInfo* info;
    sem_wait(&semAuxPID);
    auxPID = PID;
    info = list_find_mutex(processesList, closurePIDsAreEqual);
    sem_post(&semAuxPID);

    return info->pageTable[page];
}