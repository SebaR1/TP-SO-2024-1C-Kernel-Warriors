#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include "commons/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



cpuConfig* _cpuConfig;
t_config* _configFile;


cpuConfig* getCPUConfig()
{
    return _cpuConfig;
}

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getCPUData()
{
    getCPUConfig()->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    getCPUConfig()->PUERTO_MEMORIA = config_get_string_value(_configFile, "PUERTO_MEMORIA");

    getCPUConfig()->PUERTO_ESCUCHA_DISPATCH = config_get_string_value(_configFile, "PUERTO_ESCUCHA_DISPATCH");

    getCPUConfig()->PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(_configFile, "PUERTO_ESCUCHA_INTERRUPT");

    getCPUConfig()->CANTIDAD_ENTRADAS_TLB = config_get_int_value(_configFile, "CANTIDAD_ENTRADAS_TLB");

    getCPUConfig()->ALGORITMO_TLB = getTLBAlgorithm(config_get_string_value(_configFile, "ALGORITMO_TLB"));
}

ALGORITMO_TLB_TYPE getTLBAlgorithm(char* ALGORITMO_TLB)
{
    ALGORITMO_TLB_TYPE type;

    if(strcmp(ALGORITMO_TLB, "FIFO")) type = ALGORITMO_FIFO;
    else if(strcmp(ALGORITMO_TLB, "LRU")) type = ALGORITMO_LRU;

    return type;
}

void initCPUConfig(char* path)
{
    log_info(getLogger(), "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _cpuConfig = (cpuConfig*) malloc(sizeof(cpuConfig));

    _getCPUData();

    log_info(getLogger(), "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeCPUConfig()
{
    log_info(getLogger(), "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(getCPUConfig());

    log_info(getLogger(), "Memoria liberada con exito");
}


