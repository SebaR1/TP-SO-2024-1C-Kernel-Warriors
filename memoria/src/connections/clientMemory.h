#ifndef MEMORY_CLIENT_H_
#define MEMORY_CLIENT_H_


#include "utils/utilsGeneral.h"
#include "utils/client/utils.h"


void sendTamPagina(int* socketClient, int tamPagina);

void sendInstructionToCpu(int* socketClient, cpuGiveMeNextInstruction* params);


void sendData(int* socketClient, void* data, int size);


/// @brief Envia un codigo de operacion de confirmacion al socket esepcificado
/// @param socketClient El socket a quien mandarle la confirmacion
void sendConfirmation(int* socketClient);


/// @brief Le envia un frame a quien lo pida (tipicamente la CPU)
/// @param socketClient El socket quien pidió el frame.
/// @param frame El frame a enviar.
void sendFrame(int* socketClient, int frame);

#endif