#include "MMU.h"
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



void getPageAndOffset(int logicalAddress, pageAndOffset* outPageAndOffset)
{
    outPageAndOffset->page = floor(logicalAddress / getTamPagina());
    outPageAndOffset->offset = logicalAddress - outPageAndOffset->page * getTamPagina();
}