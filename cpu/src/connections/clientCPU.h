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



/// @brief Le pide a la Memoria el frame correspondiente a los parametros
/// @param pid Process id
/// @param page La pagina para pedir
void requestFrame(int pid, int page);



/// @brief Envia el contexto actualizado del proceso al Kernel
/// @param opCode El codigo de operacion por el que se va a enviar el contexto. Es el motivo de enviar el contexto.
/// @param pid Process ID
void sendContextToKernel(operationCode opCode, int pid);

















#endif