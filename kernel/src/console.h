#ifndef CONSOLE_H
#define CONSOLE_H

#include <readline/readline.h>
#include <commons/string.h>
#include "utilsKernel/logger.h"
#include <stdlib.h>
#include "longTermPlanning.h"


//Inicia la consola interactiva de kernel
void readKernelConsole(void);

//Verifica si lo que ingresa es una instruccion posible
bool _isAnInstruction(char* instruction);

//Atiende la instruccion validada
void attendInstruction(char* instruction);

#endif