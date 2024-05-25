#include "essentials.h"



int PID;

sem_t semaphoreWaitInstruction;




void setCurrentPID(int value)
{
    PID = value;
}

int getCurrentPID()
{
    return PID;
}