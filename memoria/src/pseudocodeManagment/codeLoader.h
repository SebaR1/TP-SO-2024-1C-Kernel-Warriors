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
void loadProcessByPath(int PID, char* pseudocodePath);



/// @brief Carga las instrucciones de pseudocodigo del proceso en la lista de pseudocodigos de procesos.
/// @param PID Process ID, necesario para identificar al proceso.
/// @param pseudocodePath El archivo con el pseudocodigo.
void loadProcessByFile(int PID, FILE* pseudocodeFile);


/// @brief Llama a la funcion que carga un proceso usando su path. La unica diferencia es que recibe los parametros usando un void* en lugar de varios parametros.
/// @param params Los parametros void*
void loadProcessByPathWithParams(void* params);




#endif