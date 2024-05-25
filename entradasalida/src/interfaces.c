#include "interfaces.h"

genericInterface *exampleInterface;

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
    newInterface->config = malloc(sizeof(ioConfigGeneric));

    newInterface->name = string_duplicate(_name);
    getIODataForGenericInterface(newInterface);

    return newInterface;
}

void *interfaceCreate(char *name){
    if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "GENERICA"))
        return genericInterfaceCreate(name);
    // Por el momento solo retornan un puntero no NULL las interfaces genéricas.
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDIN"))
        return NULL;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDOUT"))
        return NULL;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "DIALFS"))
        return NULL;
}

void genericInterfaceDestroy(genericInterface *interface){
    free(getIOConfigGeneric(interface));
    free(interface);
}

void serverIOForKernel(int *socketClient){
    bool exitLoop = false;
    while (!exitLoop)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);

        switch (opCode)
        {
        case REQUEST_OPERATION_TO_INTERFACE:
            //A implementar correctamente:
            ioGenSleep(2000);
            break;

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    /*sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);*/
}

void ioGenSleep(uint32_t workUnits){
    sleep(workUnits * exampleInterface->config->TIEMPO_UNIDAD_TRABAJO);
}