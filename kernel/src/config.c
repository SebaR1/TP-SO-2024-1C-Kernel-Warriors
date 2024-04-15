#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



kernelConfig* _kernelConfig;
t_config* _configFile;


kernelConfig* getKernelConfig()
{
    return _kernelConfig;
}


void initKernelConfig(char* path)
{
    log_info(kernelLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _kernelConfig = (kernelConfig*) malloc(sizeof(kernelConfig));

    _getKernelData();

    log_info(kernelLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak en los testeos.
void freeKernelConfig()
{
    log_info(kernelLogger, "Liberando la memoria usada para el archivo de configuracion");

    list_destroy_and_destroy_elements(_kernelConfig->RECURSOS, free);
    list_destroy(_kernelConfig->INSTANCIAS_RECURSOS);
    config_destroy(_configFile);
    free(_kernelConfig);

    log_info(kernelLogger, "Memoria liberada con exito");
}


// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getKernelData()
{
    _kernelConfig->PUERTO_ESCUCHA = config_get_int_value(_configFile, "PUERTO_ESCUCHA");

    _kernelConfig->IP_MEMORIA = config_get_string_value(_configFile, "IP_MEMORIA");

    _kernelConfig->PUERTO_MEMORIA = config_get_int_value(_configFile, "PUERTO_MEMORIA");

    _kernelConfig->IP_CPU = config_get_string_value(_configFile, "IP_CPU");

    _kernelConfig->PUERTO_CPU_DISPATCH = config_get_int_value(_configFile, "PUERTO_CPU_DISPATCH");

    _kernelConfig->PUERTO_CPU_INTERRUPT = config_get_int_value(_configFile, "PUERTO_CPU_INTERRUPT");

    _kernelConfig->ALGORITMO_PLANIFICACION = config_get_string_value(_configFile, "ALGORITMO_PLANIFICACION");

    _kernelConfig->QUANTUM = config_get_int_value(_configFile, "QUANTUM");

    _kernelConfig->RECURSOS = getListOfStringsFromConfig(_configFile, "RECURSOS", kernelLogger, "No se pudieron obtener los recursos del archivo de configuracion");

    _kernelConfig->INSTANCIAS_RECURSOS = getListOfIntsFromConfig(_configFile, "INSTANCIAS_RECURSOS", kernelLogger, "No se pudo obtener la cantidad de instancias de los recursos del archivo de configuracion");

    _kernelConfig->GRADO_MULTIPROGRAMACION = config_get_int_value(_configFile, "GRADO_MULTIPROGRAMACION");
}