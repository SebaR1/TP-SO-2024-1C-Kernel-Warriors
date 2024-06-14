#include "interfaces.h"

t_interfaceData interfaceData;
t_resultsForStdin resultsForStdin;
t_resultsForStdout resultsForStdout;

sem_t semaphoreForStdin;
sem_t semaphoreForStdout;
sem_t semaphoreForModule;

int socketKernel;
int socketMemory;

void createInterface()
{   
    interfaceData.name = malloc(sizeof(char) * 8);
    printf("Name: ");
    scanf(" %s", interfaceData.name);

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
        break;

        case STDIN:
        interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdinInterface));
        resultsForStdin.resultsFromRead = NULL;
        break;

        case STDOUT:
        interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdoutInterface));
        resultsForStdout.resultsForWrite = NULL;
        break;

        case DialFS:
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