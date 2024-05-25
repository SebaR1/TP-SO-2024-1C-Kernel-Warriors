#ifndef IO_INTERFACES_H
#define IO_INTERFACES_H

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "utilsIO/config.h"

// Configuración del archivo de configuración para las interfaces genéricas
typedef struct
{
    int TIEMPO_UNIDAD_TRABAJO;
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
} ioConfigGeneric;

typedef struct{
    char *name;
    ioConfigGeneric *config;
} genericInterface;

// Retorna un puntero a la struct propia de la configuracion, la cual deberia contener la informacion de la configuracion de la interfaz genérica.
ioConfigGeneric* getIOConfigGeneric(genericInterface *interface);

// Funcion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void getIODataForGenericInterface(genericInterface *interface);

genericInterface *genericInterfaceCreate(char *_name);

void genericInterfaceDestroy(genericInterface *interface);

#endif