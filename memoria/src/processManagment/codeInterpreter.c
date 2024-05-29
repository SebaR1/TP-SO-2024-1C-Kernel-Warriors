#include "codeInterpreter.h"
#include <stdio.h>
#include <stdlib.h>






char* getInstruction(int PID, int PC)
{
    processInfo* currentPseudocode = getLastProcess();

    if (currentPseudocode == NULL || currentPseudocode->PID != PID)
    {
        sem_wait(semAuxPID);
        auxPID = PID;
        sem_post(semAuxPID);
        currentPseudocode = list_find_mutex(processesList, closurePIDsAreEqual);
        if (currentPseudocode == NULL)
        {
            exit(EXIT_FAILURE);
        }
        setLastProcess(currentPseudocode);
    }

    return currentPseudocode->pseudocodeInstructions[PC];
}


