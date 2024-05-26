#ifndef MEMORY_PSEUDOCODE_LOADER_H_
#define MEMORY_PSEUDOCODE_LOADER_H_


#include <stdio.h>
#include <stdlib.h>
#include "essentials.h"


/// @brief Inicializa la lista de los pseudocodigos de los procesos.
void initPseudocodeList();


/// @brief Carga las instrucciones de pseudocodigo del proceso en la lista de pseudocodigos de procesos.
/// @param PID Process ID, necesario para identificar al proceso.
/// @param pseudocodePath El path donde se encuentra el archivo de pseudocodigo cargado en disco.
void loadCodeByPath(int PID, char* pseudocodePath);



/// @brief Cuando se hace signal del semaforo de los nuevos codigos, carga las instrucciones de pseudocodigo
/// del proceso en la lista de pseudocodigos. Los codigos que carga son los que se encuentran en la lista de nuevos codigos,
/// la cual debe haberse agregado un elemento antes de hacer el signal.
void loadCodeByPathThread(void* ignore);



/// @brief Carga las instrucciones de pseudocodigo del proceso en la lista de pseudocodigos de procesos. No cierra el archivo pasado como parametro.
/// @param PID Process ID, necesario para identificar al proceso.
/// @param pseudocodePath El archivo con el pseudocodigo.
void loadCodeByFile(int PID, FILE* pseudocodeFile);


/// @brief Llama a la funcion que carga un proceso usando su path. La unica diferencia es que recibe los parametros usando un void* en lugar de varios parametros.
/// @param params Los parametros void*
void loadCodeByPathWithParams(void* params);


/// @brief Elimina la informacion del pseudocodigo con el PID especificado (lo elimina de la lista y libera la memoria)
/// @param PID El Process ID
void destroyCode(int PID);


/// @brief Llama a la funcion que elimina la informacion del pseudocodigo con el PID especificado pasado mediante params
/// @param params El PID en forma de void*
void destroyCodeByParams(void* params);






#endif