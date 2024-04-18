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

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getCPUData()
{
    getCPUConfig()->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    getCPUConfig()->PUERTO_MEMORIA = config_get_string_value(_configFile, "PUERTO_MEMORIA");

    getCPUConfig()->PUERTO_ESCUCHA_DISPATCH = config_get_string_value(_configFile, "PUERTO_ESCUCHA_DISPATCH");

    getCPUConfig()->PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(_configFile, "PUERTO_ESCUCHA_INTERRUPT");

    getCPUConfig()->CANTIDAD_ENTRADAS_TLB = config_get_int_value(_configFile, "CANTIDAD_ENTRADAS_TLB");

    getCPUConfig()->ALGORITMO_TLB = config_get_string_value(_configFile, "ALGORITMO_TLB");
}

void initCPUConfig(char* path)
{
    log_info(_cpuLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _cpuConfig = (cpuConfig*) malloc(sizeof(cpuConfig));

    _getCPUData();

    log_info(_cpuLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeCPUConfig()
{
    log_info(_cpuLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(getCPUConfig());

    log_info(_cpuLogger, "Memoria liberada con exito");
}


