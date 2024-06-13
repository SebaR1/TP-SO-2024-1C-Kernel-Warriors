#include "interfaces.h"

t_interfaceData interfaceData;
t_resultsForStdin resultsForStdin;
t_resultsForStdout resultsForStdout;

void createInterface(t_interfaceData *interfaceData)
{
    interfaceData->name = malloc(sizeof(char) * 8);
    printf("Name: ");
    scanf(" %s", interfaceData->name);

    if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "GENERICA"))
        interfaceData->type = GENERIC_TYPE;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDIN"))
        interfaceData->type = STDIN_TYPE;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDOUT"))
        interfaceData->type = STDOUT_TYPE;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "DIALFS"))
        interfaceData->type = DIALFS_TYPE;

    interfaceData->workUnits = getIOConfig()->TIEMPO_UNIDAD_TRABAJO;

    interfaceData->currentOperation.pid = -1;
    interfaceData->currentOperation.operation = IO_NULL;
    switch (interfaceData->type)
    {
        case GENERIC_TYPE:
        interfaceData->currentOperation.params = malloc(sizeof(t_paramsForGenericInterface));
        break;

        case STDIN_TYPE:
        interfaceData->currentOperation.params = malloc(sizeof(t_paramsForStdinInterface));
        resultsForStdin.resultsFromRead = NULL;
        break;

        case STDOUT_TYPE:
        interfaceData->currentOperation.params = malloc(sizeof(t_paramsForStdoutInterface));
        resultsForStdout.resultsFromRead = NULL;
        break;

        default:
        break;
    }
}