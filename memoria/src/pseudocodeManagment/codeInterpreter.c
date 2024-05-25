#include "codeInterpreter.h"
#include <stdio.h>
#include <stdlib.h>


int auxPID;

char* getInstruction(int PID, int PC)
{
    pseudocodeInfo* currentPseudocode = getCurrentPseudocode();

    if (currentPseudocode->PID != PID)
    {
        auxPID = PID; // No es necesario usar un mutex porque nunca van a haber 2 o mas hilos ejecutando esta instruccion o cambiando el auxPID, ya que solo hay 1 CPU que puede pedir las instrucciones, y solo se manda 1 instruccion a la vez.
        currentPseudocode = list_find_mutex(pseudocodeOfProcesses, closurePIDsAreEqual);
        if (currentPseudocode == NULL)
        {
            exit(EXIT_FAILURE);
        }
        setCurrentPseudocode(currentPseudocode);
    }

    return currentPseudocode->pseudocodeInstructions[PC];
}


bool closurePIDsAreEqual(void* element)
{
    return ((pseudocodeInfo*)element)->PID == auxPID;
}