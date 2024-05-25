#include "instructionCycle.h"


void runInstructionCycle()
{
    // Fase fetch, busco la siguiente instruccion
    instructionString* newInstructionString = getNextInstruction(getCurrentPID(), getPC());

    // Fase decode, descifro qué me pide la instrucción (la cual llegó en forma de string)
    void* instructionParams;
    instructionType type = decodeInstruction(newInstructionString->string, instructionParams);

    // Ejecuto la instruccion
    executeInstruction(type, instructionParams);


    // Libero la memoria que usé
    free(instructionParams);
    free(newInstructionString->string);
    free(newInstructionString);
}