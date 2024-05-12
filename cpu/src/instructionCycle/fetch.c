#include "fetch.h"
#include "utils/client/utils.h"


memoryInstructionString* instructionString;


memoryInstructionString* getNextInstruction(uint32_t PC, int socketClient)
{

    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(CPU_GIVE_ME_NEXT_INSTRUCTION);

    cpuGiveMeNextInstruction giveMeNextInstruction;
    giveMeNextInstruction.PC = PC;

    addToPackage(package, &(giveMeNextInstruction.PC), sizeof(uint32_t)); // Agrego el Program Counter al paquete para enviar

    sendPackage(package, socketClient); // Envio el Program Counter

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)


    // Espero a que la memoria me responda con la proxima instruccion (El servidor de la CPU para la Memoria va a hacer un signal cuando ya haya recibido la instruccion)
    sem_wait(&semaphoreWaitInstruction);

    
    return instructionString;
}
