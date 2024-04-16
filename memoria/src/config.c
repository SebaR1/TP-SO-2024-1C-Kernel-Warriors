#include "config.h"
#include <commons/config.h>
#include "logger.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>



memoryConfig* _memoryConfig;
t_config* _configFile;


memoryConfig* getMemoryConfig()
{
    return _memoryConfig;
}

// FUncion auxiliar para settear todos los datos desde el config hasta la variable global correspondiente
void _getMemoryData()
{
    _memoryConfig->PUERTO_ESCUCHA = config_get_int_value(_configFile, "PUERTO_ESCUCHA");

    _memoryConfig->TAM_MEMORIA = config_get_int_value(_configFile, "TAM_MEMORIA");

    _memoryConfig->TAM_PAGINA = config_get_int_value(_configFile, "TAM_PAGINA");

    _memoryConfig->PATH_INSTRUCCIONES = config_get_string_value(_configFile, "PATH_INSTRUCCIONES");

    _memoryConfig->RETARDO_RESPUESTA = config_get_int_value(_configFile, "RETARDO_RESPUESTA");
}

void initMemoryConfig(char* path)
{
    log_info(memoryLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _memoryConfig = (memoryConfig*) malloc(sizeof(memoryConfig));

    _getMemoryData();

    log_info(memoryLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeMemoryConfig()
{
    log_info(memoryLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(_memoryConfig);

    log_info(memoryLogger, "Memoria liberada con exito");
}


