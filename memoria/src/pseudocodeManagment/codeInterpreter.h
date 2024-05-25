#ifndef MEMORY_PSEUDOCODE_INTERPRETER_H_
#define MEMORY_PSEUDOCODE_INTERPRETER_H_


#include <stdio.h>
#include "essentials.h"
#include "utils/listMutex.h"



char* getInstruction(int PID, int PC);


bool closurePIDsAreEqual(void* element);













#endif