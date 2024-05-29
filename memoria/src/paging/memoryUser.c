#include "memoryUser.h"


#include <stdio.h>
#include <stdlib.h>
#include "utilsMemory/config.h"
#include "math.h"



// El espacio contiguo de memoria que representa la memoria del usuario
void* memoryUser;


// Array de bools que dicen si un frame está libre (false) u ocupado (true)
bool memoryUserFrames[];


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

    memoryUserFrames = calloc(maxFrames); // calloc para que todas las paginas se inicialicen como libres (en 0, false)

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


    // Si se intenta reservar 0 bytes, no hace nada y retorna que se reservaron 0 paginas.
    if (bytes <= 0)
    {
        return 0;
    }



    // Compruebo si los bytes que se quieren reservar entran en la fragmentacion interna que sobró de la ultima pagina actual.
    // Si es asi, uso esa memoria de fragmentacion interna como memoria de usuario valida.
    if (bytes <= *internalFragmentation)
    {
        *internalFragmentation -= bytes;
        return 0;
    }


    // Lleno la ultima pagina actual que tenia fragmentacion interna, para que todas las paginas actuales queden ocupadas completamente.
    bytes -= *internalFragmentation;
    

    // Obtengo la cantidad de paginas que deberian reservarse.
    int amountOfNewPages = getAmountOfPages(bytes);


    // Si no hay suficientes paginas para reservar la cantidad que se pide, retorna Out Of Memory (-1)
    if (amountOfNewPages > amountOfFramesFree)
    {
        return -1;
    }



    ///////////// ALOJO SEGURO A PARTIR DE ACA, AL MENOS SE RESERVA UNA NUEVA PAGINA /////////////



    // Obtengo el indice desde el que hay que empezar a buscarle frames libres a las paginas.
    int newPagesIndex = *amountOfPages;

    // Obtengo la cantidad total de paginas que ocupará todo el proceso.
    *amountOfPages +=  amountOfNewPages;


    // Expando el array de paginas
    pages = resize(amountOfPages * sizeof(int));

    // Le asigno un frame libre a cada nueva pagina.
    for ( ; newPagesIndex < amountOfPages; newPagesIndex++)
    {
        pages[newPagesIndex] = allocNextFrameFree();
    }


    // Obtengo la nueva fragmentacion interna de la ultima pagina
    *internalFragmentation = getInternalFragmentation(bytes);


    return amountOfNewPages
}



int getInternalFragmentation(int bytesAllocated)
{
    int sizePag = getMemoryConfig()->TAM_PAGINA;
    return (  sizePag - (  bytesAllocated % sizePag  )  ) % sizePag;
}



int getAmountOfBytesAllocated(int amountOfPages, int internalFragmentation)
{
    return amountOfPages * getMemoryConfig()->TAM_PAGINA + internalFragmentation;
}

int getAmountOfPages(int bytesAllocated)
{
    return ceil(bytesAllocated / getMemoryConfig()->TAM_PAGINA);
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








