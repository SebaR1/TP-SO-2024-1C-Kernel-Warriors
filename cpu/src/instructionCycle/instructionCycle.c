#include "instructionCycle.h"


void runInstructionCycle()
{
    memoryInstructionString* instructionString = getNextInstruction(getPC(), socketMemory);

    void* instructionParams;
    instructionType type = decodeInstruction(instructionString->instructionString, instructionParams);

    executeInstruction(type, instructionParams);



    free(instructionParams);
    free(instructionString->instructionString);
    free(instructionString);
}