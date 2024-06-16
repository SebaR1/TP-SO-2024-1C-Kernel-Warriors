#include "interfaces.h"

t_interfaceData interfaceData;
t_resultsForStdin resultsForStdin;
t_resultsForStdout resultsForStdout;

sem_t semaphoreForStdin;
sem_t semaphoreForStdout;
sem_t semaphoreForModule;

int socketKernel;
int socketMemory;

void createInterface(char *name)
{   
    interfaceData.name = malloc(string_length(name) + 1);
    strcpy(interfaceData.name, name);

    if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "GENERICA"))
        interfaceData.type = Generic;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDIN"))
        interfaceData.type = STDIN;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDOUT"))
        interfaceData.type = STDOUT;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "DIALFS"))
        interfaceData.type = DialFS;

    interfaceData.workUnits = getIOConfig()->TIEMPO_UNIDAD_TRABAJO;

    interfaceData.currentOperation.pid = -1;
    interfaceData.currentOperation.operation = IO_NULL;
    switch (interfaceData.type)
    {
        case Generic:
        interfaceData.currentOperation.params = malloc(sizeof(t_paramsForGenericInterface));
        log_info(getLogger(), "Creada interfaz de tipo Generica, con nombre \"%s\"", interfaceData.name);
        break;

        case STDIN:
        interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdinInterface));
        resultsForStdin.resultsFromRead = NULL;
        resultsForStdin.resultsForMemory = NULL;
        log_info(getLogger(), "Creada interfaz de tipo STDIN, con nombre \"%s\"", interfaceData.name);
        break;

        case STDOUT:
        interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdoutInterface));
        resultsForStdout.resultsForWrite = NULL;
        log_info(getLogger(), "Creada interfaz de tipo STDOUT, con nombre \"%s\"", interfaceData.name);
        break;

        case DialFS:
        log_info(getLogger(), "Creada interfaz de tipo DialFS, con nombre \"%s\"", interfaceData.name);
        break;

        default:
        break;
    }
}

void destroyInterface()
{
    free(interfaceData.name);
    free(interfaceData.currentOperation.params);
}