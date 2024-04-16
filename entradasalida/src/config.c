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
    _ioConfig->TIPO_INTERFAZ = config_get_string_value(_configFile, "TIPO_INTERFAZ");

    _ioConfig->TIEMPO_UNIDAD_TRABAJO = config_get_int_value(_configFile, "TIEMPO_UNIDAD_TRABAJO");

    _ioConfig->IP_KERNEL = config_get_string_value(_configFile, "IP_KERNEL");

    _ioConfig->PUERTO_KERNEL = config_get_int_value(_configFile, "PUERTO_KERNEL");

    _ioConfig->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    _ioConfig->PUERTO_MEMORIA = config_get_int_value(_configFile, "PUERTO_MEMORIA");

    _ioConfig->PATH_BASE_DIALFS = config_get_string_value(_configFile, "PATH_BASE_DIALFS");

    _ioConfig->BLOCK_SIZE = config_get_int_value(_configFile, "BLOCK_SIZE");

    _ioConfig->BLOCK_COUNT = config_get_int_value(_configFile, "BLOCK_COUNT");
}

void initIOConfig(char* path)
{
    log_info(ioLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _ioConfig = (ioConfig*) malloc(sizeof(ioConfig));

    _getIOData();

    log_info(ioLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeIOConfig()
{
    log_info(ioLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(_ioConfig);

    log_info(ioLogger, "Memoria liberada con exito");
}


