#include "inOut.h"

interface_t *createInterface(char* nameInterface, interfaceType typeInterface)
{
    interface_t *newInterface = malloc(sizeof(interface_t));
    newInterface->name = nameInterface;
    newInterface->interfaceType = typeInterface;
    newInterface->isBusy = false;
    newInterface->blockList = initListMutex();

    return newInterface;
}

char* interfaceNameToFind;

bool compareNameInterface(void *data)
{
    interface_t *interface = (interface_t *)data;
    return string_equals_ignore_case(interface->name, interfaceNameToFind);
}

interface_t *foundInterface(char* nameRequestInterface)
{
    interfaceNameToFind = nameRequestInterface;

    interface_t *interfaceFound = (interface_t*)list_find(interfacesList->list, compareNameInterface);

    if (interfaceFound == NULL) return NULL;

    return interfaceFound;
}

