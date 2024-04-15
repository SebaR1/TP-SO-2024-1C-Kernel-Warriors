#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



cpuConfig* _cpuConfig;
t_config* _configFile;


cpuConfig* getCPUConfig()
{
    return _cpuConfig;
}


void initCPUConfig(char* path)
{
    log_info(cpuLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _cpuConfig = (cpuConfig*) malloc(sizeof(cpuConfig));

    _getCPUData();

    log_info(cpuLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeCPUConfig()
{
    log_info(cpuLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(_cpuConfig);

    log_info(cpuLogger, "Memoria liberada con exito");
}


// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getCPUData()
{
    _cpuConfig->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    _cpuConfig->PUERTO_MEMORIA = config_get_int_value(_configFile, "PUERTO_MEMORIA");

    _cpuConfig->PUERTO_ESCUCHA_DISPATCH = config_get_int_value(_configFile, "PUERTO_ESCUCHA_DISPATCH");

    _cpuConfig->PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(_configFile, "PUERTO_ESCUCHA_INTERRUPT");

    _cpuConfig->CANTIDAD_ENTRADAS_TLB = config_get_int_value(_configFile, "CANTIDAD_ENTRADAS_TLB");

    _cpuConfig->ALGORITMO_TLB = config_get_string_value(_configFile, "ALGORITMO_TLB");
}