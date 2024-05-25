#include "clientMemory.h"
#include "pseudocodeManagment/codeInterpreter.h"
#include "serverMemory.h"



void sendInstructionToCpu(int* socketClient, cpuGiveMeNextInstruction* params)
{
    // Envio el string con la instruccion.
    t_package* package = createPackage(MEMORY_NEXT_INSTRUCTION);

    instructionString newInstruction;
    newInstruction.string = getInstruction(params->PID, params->PC);

    addToPackage(package, &(newInstruction.string), string_length(newInstruction.string) + 1); // Agrego el string de la instruccion (+ 1 en el length por el caracter nulo \0)

    sendPackage(package, socketClient); // Envio el paquete

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}