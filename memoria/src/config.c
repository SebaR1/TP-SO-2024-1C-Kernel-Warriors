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
    getMemoryConfig()->PUERTO_ESCUCHA = config_get_string_value(_configFile, "PUERTO_ESCUCHA");

    getMemoryConfig()->TAM_MEMORIA = config_get_int_value(_configFile, "TAM_MEMORIA");

    getMemoryConfig()->TAM_PAGINA = config_get_int_value(_configFile, "TAM_PAGINA");

    getMemoryConfig()->PATH_INSTRUCCIONES = config_get_string_value(_configFile, "PATH_INSTRUCCIONES");

    getMemoryConfig()->RETARDO_RESPUESTA = config_get_int_value(_configFile, "RETARDO_RESPUESTA");
}

void initMemoryConfig(char* path)
{
    log_info(_memoryLogger, "Obteniendo los datos del archivo de configuracion");

    _configFile = config_create(path);

    _memoryConfig = (memoryConfig*) malloc(sizeof(memoryConfig));

    _getMemoryData();

    log_info(_memoryLogger, "Datos obtenidos con exito");
}

// Posible riesgo de memory leak o de Segmentation Fault en esta funcion. No deberia haber ninguno, pero revisar esta funcion en caso de que se detecte un memory leak o un Segmentation Fault en los testeos
void freeMemoryConfig()
{
    log_info(_memoryLogger, "Liberando la memoria usada para el archivo de configuracion");

    config_destroy(_configFile);
    free(getMemoryConfig());

    log_info(_memoryLogger, "Memoria liberada con exito");
}


