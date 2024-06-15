#include "MMU.h"

#include "TLB.h"
#include "utils/mathMemory.h"
#include <math.h>

int TAM_PAGINA;


void setTamPagina(int value)
{
    TAM_PAGINA = value;
}

int getTamPagina()
{
    return TAM_PAGINA;
}



physicalAddressInfo createPhysicalAddressInfo(int physicalAddress, int size)
{
    physicalAddressInfo info;
    info.physicalAddress = physicalAddress;
    info.size = size;
    return info;
}


void createPhysicalAddressInfoParam(int physicalAddress, int size, physicalAddressInfo* outPhysicalAddressInfo)
{
    outPhysicalAddressInfo->physicalAddress = physicalAddress;
    outPhysicalAddressInfo->size = size;
}


void getPageAndOffset(int logicalAddress, pageAndOffset* outPageAndOffset)
{
    outPageAndOffset->page = floor(logicalAddress / getTamPagina());
    outPageAndOffset->offset = logicalAddress - outPageAndOffset->page * getTamPagina();
}


int getAllPhysicalAddresses(int pid, int logicalAddress, int size, physicalAddressInfo* outPhysicalAddressesInfo)
{
    ////////// CASOS DE CORTE //////////
    // Si no hay que escribir o leer ninguna cantidad de bytes en esa direccion, no hago nada y retorno 0.
    if (size <= 0)
    {   
        return 0;
    }



    ////////// CASOS COMUNES, AL MENOS UNA DIRECCION FISICA ES NECESARIA //////////

    int amountOfPhysAddr;
    int amountOfPhysAddrWithoutFirstAndLast;
    int sizeAfterFirstPhysAddr;
    int sizeFirstPhysAddr;
    int sizeLastPhysAddr;


    // Obtengo la pagina y el offset dentro de esa pagina
    pageAndOffset logicalAddressSplitted;
    getPageAndOffset(logicalAddress, &logicalAddressSplitted);


    // Hay que tener en cuenta que en la primera y ultima pagina se podria necesitar un tamaño para lectura o escritura "especiales"
    // (es decir, hay que calcular cuanto es ese tamaño). Para resolver eso, la primera y ultima pagina las pongo manualmente, mientras que las paginas del medio
    // siempre van a tener un tamaño igual al tamaño de pagina.

    // Hago todos los calculos necesarios para determinar informacion del tamaño de la primera y ultima pagina, y me fijo cuantas paginas hay "en el medio"

    sizeFirstPhysAddr = getTamPagina() - logicalAddressSplitted.offset;

    sizeAfterFirstPhysAddr = size - sizeFirstPhysAddr;

    sizeLastPhysAddr = getInternalFragmentation(getTamPagina(), sizeAfterFirstPhysAddr);

    amountOfPhysAddrWithoutFirstAndLast = getAmountOfPagesAllocated(getTamPagina(), sizeAfterFirstPhysAddr) - 1; // -1 porque no tengo que contar la ultima pagina

    amountOfPhysAddr = amountOfPhysAddrWithoutFirstAndLast + 2; // + 2 porque tengo que contar la primer y ultima pagina.


    // Reservo memoria para todas las direcciones fisicas que necesito.
    outPhysicalAddressesInfo = malloc(amountOfPhysAddr * sizeof(physicalAddressInfo));

    int i = 0; // Contador para ir asignando en el array

    // Pongo la primera physicalAddressInfo
    createPhysicalAddressInfoParam(getFrame(pid, logicalAddressSplitted.page + i), sizeFirstPhysAddr, outPhysicalAddressesInfo + i);
    i++;


    // Pongo las physicalAddressInfo "del medio"
    while (i < amountOfPhysAddrWithoutFirstAndLast + 1) // + 1 para tener en cuenta que la primera pagina ya fue puesta
    {
        createPhysicalAddressInfoParam(getFrame(pid, logicalAddressSplitted.page + i), sizeFirstPhysAddr, outPhysicalAddressesInfo + i);
        i++;
    }

    
    // Pongo la ultima physicalAddressInfo si todavia sobran bytes para leer o escribir en memoria
    if (sizeLastPhysAddr > 0)
    {
        createPhysicalAddressInfoParam(getFrame(pid, logicalAddressSplitted.page + i), sizeLastPhysAddr, outPhysicalAddressesInfo + i);
    }


    return amountOfPhysAddr;
}
