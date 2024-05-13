#include "instructionCycle.h"


void runInstructionCycle()
{
    // Fase fetch, busco la siguiente instruccion
    memoryInstructionString* instructionString = getNextInstruction(getPC());

    // Fase decode, descifro qué me pide la instrucción (la cual llegó en forma de string)
    void* instructionParams;
    instructionType type = decodeInstruction(instructionString->instructionString, instructionParams);

    // Ejecuto la instruccion
    executeInstruction(type, instructionParams);


    // Libero la memoria que usé
    free(instructionParams);
    free(instructionString->instructionString);
    free(instructionString);
}