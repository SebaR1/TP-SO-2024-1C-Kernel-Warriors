#include "TLB.h"
#include "utilsCPU/config.h"
#include "connections/clientCPU.h"
#include "utilsCPU/logger.h"
#include <commons/collections/node.h>


TLBElement* TLBFIFO;


int fifoCounter;


t_list* TLBLRU;

int currentFrame;

sem_t semTLBMiss;


void setCurrentFrame(int value)
{
    currentFrame = value;
}

int getCurrentFrame()
{
    return currentFrame;
}


void initTLB()
{
    sem_init(&semTLBMiss, 0, 0);

    switch (getCPUConfig()->ALGORITMO_TLB)
    {
    case ALGORITMO_FIFO:
        initFIFOAlgorithm();
        break;
    
    case ALGORITMO_LRU:
        initLRUAlgorithm();
        break;
    }
}

void initFIFOAlgorithm()
{
    TLBFIFO = malloc(getCPUConfig()->CANTIDAD_ENTRADAS_TLB * sizeof(TLBElement));
    fifoCounter = 0;
}


void initLRUAlgorithm()
{
    TLBLRU = list_create();

    t_list_iterator* victimsIterator = list_iterator_create(TLBLRU);

    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB; i++)
    {
        TLBElement* newElement = malloc(sizeof(TLBElement));
        newElement->pid = -1; // Le asigno -1 para que no ocurra un TLB_HIT sin querer al principio

        list_iterator_add(victimsIterator, newElement);
    }

    list_iterator_destroy(victimsIterator);    
}


int getFrame(int pid, int page)
{
    int frame;


    /////////////////// CASO EN EL QUE LA TLB TIENE 0 ENTRADAS ///////////////////

    if (getCPUConfig()->CANTIDAD_ENTRADAS_TLB <= 0)
    {
        // Pido el frame a Memoria
        frame = waitFrameFromMemory(pid, page);

        // HACER EL LOG DEL TLB_MISS ACAAAAAA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        return frame;
    }



    /////////////////// CASO EN EL QUE LA TLB TIENE 1 O MÁS ENTRADAS ///////////////////


    TLBResult result;
    

    //// Obtengo el frame de la TLB, o lo pido a Memoria y reemplazo una de las entradas segun el algoritmo
    switch (getCPUConfig()->ALGORITMO_TLB)
    {
    case ALGORITMO_FIFO:
        result = getFrameFIFO(pid, page, &frame);
        break;
    
    case ALGORITMO_LRU:
        result = getFrameLRU(pid, page, &frame);
        break;
    }
    

    //// Hago los logs necesarios si ocurrió TLB_HIT o TLB_MISS
    switch (result)
    {
    case TLB_HIT:
        logTLBHit(pid, page);
        break;
    
    case TLB_MISS:
        logTLBMiss(pid, page);
        break;
    }


    return frame;
}

TLBResult getFrameFIFO(int pid, int page, int* frame)
{
    ////// La TLB tiene al menos 1 entrada

    TLBResult result = TLB_MISS;

    //// Recorro la TLB hasta encontrar el pid y pagina o hasta que llegue al final de la TLB

    for (int i = 0; i < getCPUConfig()->CANTIDAD_ENTRADAS_TLB && result == TLB_MISS; i++)
    {
        if (TLBFIFO[i].pid == pid && TLBFIFO[i].page == page)
        {
            result = TLB_HIT;
            *frame = TLBFIFO[i].frame;
        }
    }

    //// Si la pagina no se encuentra en la TLB, se la pide a la memoria, espera a que llegue el frame y reemplaza segun el algoritmo.

    if (result == TLB_MISS)
    {
        *frame = waitFrameFromMemory(pid, page);

        TLBFIFO[fifoCounter].pid = pid;
        TLBFIFO[fifoCounter].page = page;
        TLBFIFO[fifoCounter].frame = *frame;

        fifoCounter = (fifoCounter + 1) % getCPUConfig()->CANTIDAD_ENTRADAS_TLB;

        logGetFrame(pid, page, *frame);
    }

    return result;
}

TLBResult getFrameLRU(int pid, int page, int* frame)
{
        ///// La TLB tiene al menos 1 entrada

    TLBResult result = TLB_MISS;


    t_list_iterator* iterator = list_iterator_create(TLBLRU);

    //// Recorro la TLB hasta encontrar el pid y pagina o hasta que llegue al final de la TLB

    while (list_iterator_has_next(iterator) && result == TLB_MISS)
    {
        TLBElement* element = (TLBElement*)list_iterator_next(iterator);
        if (element->pid == pid && element->page == page)
        {
            result = TLB_HIT;
            *frame = element->frame;
            moveElementOfListToLast(iterator, element);
        }
    }

    //// Si la pagina no se encuentra en la TLB, se la pide a la memoria, espera a que llegue el frame y reemplaza segun el algoritmo.

    if (result == TLB_MISS)
    {
        *frame = waitFrameFromMemory(pid, page);

        TLBElement* element = (TLBElement*)list_remove(TLBLRU, 0);

        element->pid = pid;
        element->page = page;
        element->frame = *frame;

        list_add(TLBLRU, element);
    }
    
    list_iterator_destroy(iterator);

    return result;
}


void moveElementOfListToLast(t_list_iterator* iterator, void* actualElement)
{
    list_iterator_remove(iterator);

    
    while (list_iterator_has_next(iterator))
    {
        list_iterator_next(iterator);
    }
    
    list_iterator_add(iterator, actualElement);
}


int waitFrameFromMemory(int pid, int page)
{
    requestFrame(pid, page);

    sem_wait(&semTLBMiss);

    return getCurrentFrame();
}