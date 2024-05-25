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