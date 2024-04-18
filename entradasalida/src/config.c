#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



ioConfig* _ioConfig;
t_config* _configFile;


ioConfig* getIOConfig()
{
    return _ioConfig;
}

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getIOData()
{
    getIOConfig()->TIPO_INTERFAZ = config_get_string_value(_configFile, "TIPO_INTERFAZ");

    getIOConfig()->TIEMPO_UNIDAD_TRABAJO = config_get_int_value(_configFile, "TIEMPO_UNIDAD_TRABAJO");

    getIOConfig()->IP_KERNEL = config_get_string_value(_configFile, "IP_KERNEL");

    getIOConfig()->PUERTO_KERNEL = config_get_string_value(_configFile, "PUERTO_KERNEL");

    getIOConfig()->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    getIOConfig()->PUERTO_MEMORIA = config_get_string_value(_configFile, "PUERTO_MEMORIA");

    getIOConfig()->PATH_BASE_DIALFS = config_get_string_value(_configFile, "PATH_BASE_DIALFS");

    getIOConfig()->BLOCK_SIZE = config_get_int_value(_configFile, "BLOCK_SIZE");

    getIOConfig()->BLOCK_COUNT = config_get_int_value(_configFile, "BLOCK_COUNT");
}

void initIOConfig(char* path)
{
    log_info(_ioLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    if (_configFile == NULL){
        log_info(_ioLogger, "FAllo en la creación del config");
        exit(1);
    }

    _ioConfig = (ioConfig*) malloc(sizeof(ioConfig));

    _getIOData();

    log_info(_ioLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeIOConfig()
{
    log_info(_ioLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(getIOConfig());

    log_info(_ioLogger, "Memoria liberada con exito");
}


