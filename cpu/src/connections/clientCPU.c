#include "clientCPU.h"




void sendPCToMemory(int PID, uint32_t PC)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(CPU_GIVE_ME_NEXT_INSTRUCTION);

    cpuGiveMeNextInstruction giveMeNextInstruction;
    giveMeNextInstruction.PID = PID;
    giveMeNextInstruction.PC = PC;

    addToPackage(package, &(giveMeNextInstruction.PID), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(giveMeNextInstruction.PC), sizeof(uint32_t)); // Agrego el Program Counter al paquete para enviar

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}

void requestFrame(int pid, int page)
{
    // Envio la operacion y la info para pedirle el frame que necesito a la Memoria
    t_package* package = createPackage(CPU_GET_FRAME);

    getFrameInfo frameInfo;
    frameInfo.PID = pid;
    frameInfo.page = page;

    addToPackage(package, &(frameInfo.PID), sizeof(int)); // Agrego el Process ID al paquete para enviar
    addToPackage(package, &(frameInfo.page), sizeof(int)); // Agrego el Program Counter al paquete para enviar

    sendPackage(package, socketMemory); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)
}