#ifndef MEMORY_CODE_ESSENTIALS_H_
#define MEMORY_CODE_ESSENTIALS_H_


#include <stdint.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "utils/listMutex.h"


// Lista que contiene todas las instrucciones de pseudocodigo de todos
// los procesos que están cargados (junto con su pid para identificarlos)
extern listMutex_t* pseudocodeOfProcesses;



// Estructura que contiene informacion del pseudocodigo perteneciente a un proceso (identificado con su PID)
typedef struct
{
    int PID;
    char** pseudocodeInstructions;
} pseudocodeInfo;


/// @brief Obtiene la informacion del pseudocodigo que estaba corriendo en la instruccion anterior
/// @return La informacion del pseudocodigo
/// @note Es como si fuese una cache. Lo mas probable es que la siguiente instruccion que se pida sea del mismo archivo de pseudocodigo.
/// Si no lo guardo, tendria que buscar el archivo con el PID entre la lista de todos los procesos, entonces lo guardo para que sea más rápido.
/// En caso de que el PID no sea el mismo que el de la instruccion anterior, sí necesitaria buscar el proceso dentro de la lista de pseudocodigos de los procesos.
pseudocodeInfo* getCurrentPseudocode();


/// @brief Settea el valor a la variable que contiene informacion del pseudocodigo del proceso actual.
/// @param value El nuevo valor.
void setCurrentPseudocode(pseudocodeInfo* value);




#endif