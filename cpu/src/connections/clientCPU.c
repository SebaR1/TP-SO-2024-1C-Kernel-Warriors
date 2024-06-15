#include "clientCPU.h"
#include "instructionCycle/essentials.h"



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

void sendContextToKernel(operationCode opCode, int pid)
{
    // Envio la operacion y el Program Counter a la memoria para avisarle que me tiene que dar la proxima instruccion
    t_package* package = createPackage(opCode);

    contextProcess contextProcess;
    contextProcess.pc = getPC();
    contextProcess.registersCpu.AX = getAX();
    contextProcess.registersCpu.BX = getBX();
    contextProcess.registersCpu.CX = getCX();
    contextProcess.registersCpu.DX = getDX();
    contextProcess.registersCpu.EAX = getEAX();
    contextProcess.registersCpu.EBX = getEBX();
    contextProcess.registersCpu.ECX = getECX();
    contextProcess.registersCpu.EDX = getEDX();
    contextProcess.registersCpu.DI = getDI();
    contextProcess.registersCpu.SI = getSI();

    addToPackage(package, &(contextProcess.pc), sizeof(uint32_t)); // Agrego el Program Counter del registros al paquete para enviar
    addToPackage(package, &(contextProcess.registersCpu.AX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.BX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.CX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.DX), sizeof(uint8_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EAX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EBX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.ECX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.EDX), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.DI), sizeof(uint32_t)); // Agrego los registros del contexto
    addToPackage(package, &(contextProcess.registersCpu.SI), sizeof(uint32_t)); // Agrego los registros del contexto

    addToPackage(package, &(pid), sizeof(uint32_t));

    sendPackage(package, socketKernelDispatch); // Envio el Contexto

    destroyPackage(package); // Destruyo el paquete (libero la memoria usada)

    sem_wait(&semContinueInstructionCycle);
}