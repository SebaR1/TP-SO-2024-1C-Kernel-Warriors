#include "interfaces.h"

ioConfigGeneric* getIOConfigGeneric(genericInterface *interface){
    return interface->config;
}

void getIODataForGenericInterface(genericInterface *interface){
    getIOConfigGeneric(interface)->TIEMPO_UNIDAD_TRABAJO = getIOConfig()->TIEMPO_UNIDAD_TRABAJO;

    getIOConfigGeneric(interface)->IP_KERNEL = getIOConfig()->IP_KERNEL;

    getIOConfigGeneric(interface)->PUERTO_KERNEL = getIOConfig()->PUERTO_KERNEL;
}

genericInterface *genericInterfaceCreate(char *_name){
    genericInterface* newInterface = malloc(sizeof(genericInterface));

    newInterface->name = string_duplicate(_name);
    getIODataForGenericInterface(newInterface);

    return newInterface;
}

void genericInterfaceDestroy(genericInterface *interface){
    
}