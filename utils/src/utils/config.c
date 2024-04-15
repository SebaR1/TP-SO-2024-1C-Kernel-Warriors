#include "config.h"
#include <stdio.h>





// Convierte un doble puntero a char leido de un archivo de configuracion a una lista de strings. Retorna esa lista
t_list* getListOfStringsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage)
{
    char** listChars = config_get_array_value(config, key);

    if (listChars == NULL)
    {
        log_error(logger, errorMessage);
        return NULL;
    }

    return _fromConfigToListOfStrings(listChars);
}

// Funcion auxiliar para agregar todos los strings a una lista
t_list* _fromConfigToListOfStrings(char** listChars)
{
    t_list* list = list_create();

    int i = 0;
    while (listChars[i] != NULL)
    {
        list_add(list, listChars[i]);
        i++;
    }

    return list;
}




// Convierte un doble puntero a char leido de un archivo de configuracion a una lista de ints. Retorna esa lista
t_list* getListOfIntsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage)
{
    char** listChars = config_get_array_value(config, key);

    if (listChars == NULL)
    {
        log_error(logger, errorMessage);
        return NULL;
    }

    return _fromConfigToListOfInts(listChars);
}

// Funcion auxiliar para agregar todos los int a una lista
t_list* _fromConfigToListOfInts(char** listChars)
{
    t_list* list = list_create();

    int i = 0;
    while (listChars[i] != NULL)
    {
        list_add(list, atoi(listChars[i]));
        i++;
    }

    return list;
}