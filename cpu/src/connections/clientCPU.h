#ifndef CPU_CLIENT_H_
#define CPU_CLIENT_H_


#include <stdint.h>
#include "utils/utilsGeneral.h"
#include "utils/client/utils.h"
#include "serverCPU.h"



/// @brief Le envia mediante a la memoria el Program Counter pasado por parametro mediante sockets.
/// @param PID El Process ID
/// @param PC El Program Counter
void sendPCToMemory(int PID, uint32_t PC);
























#endif