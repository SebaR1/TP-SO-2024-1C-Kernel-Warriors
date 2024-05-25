#ifndef IO_INTERFACES_H
#define IO_INTERFACES_H

#include "utilsIO/config.h"
#include "utilsIO/logger.h"
#include "utils/client/utils.h"
#include "utils/server/utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/string.h>

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

/*typedef struct{
    char *name;

} interfaceParams;
*/


// Retorna un puntero a la struct propia de la configuracion, la cual deberia contener la informacion de la configuracion de la interfaz genérica.
ioConfigGeneric* getIOConfigGeneric(genericInterface *interface);

// Funcion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void getIODataForGenericInterface(genericInterface *interface);

genericInterface *genericInterfaceCreate(char *_name);

void *interfaceCreate(char *name);

void genericInterfaceDestroy(genericInterface *interface);

void serverIOForKernel(int *socketClient);

//void handleInterfaceLoop(interfaceParams params);

void ioGenSleep(uint32_t workUnits);

#endif