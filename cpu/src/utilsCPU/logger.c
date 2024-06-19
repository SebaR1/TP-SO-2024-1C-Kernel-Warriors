#include "logger.h"
#include <stdint.h>

t_log* _logger;



///////////////////// FUNCIONES PARA MANEJO DE LOGS /////////////////////


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _logger = log_create(file, processName, isActiveConsole, level);
}

t_log* getLogger()
{
    return _logger;
}

void destroyLogger()
{
    log_destroy(_logger);
}





///////////////////// LOGS ESPECIFICOS /////////////////////


void logFetchInstruction(int pid, uint32_t pc)
{
    log_info(getLogger(), "Fetch Instrucción: PID: %d - FETCH - Program Counter: %d", pid, pc);
}

void logExecutingInstruction(int pid, char* instruction)
{
    log_info(getLogger(), "Ejecutando Instrucción: PID: %d - Ejecutando: %s", pid, instruction);
}

void logTLBHit(int pid, int page)
{
    log_info(getLogger(), "TLB Hit: PID: %d - TLB HIT - Pagina: %d", pid, page);
}

void logTLBMiss(int pid, int page)
{
    log_info(getLogger(), "TLB Miss: PID: %d - TLB MISS - Pagina: %d", pid, page);
}

void logGetFrame(int pid, int page, int frame)
{
    log_info(getLogger(), "Obtener Marco: PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid, page, frame);
}

void logReadMemoryUint(int pid, int physicalAddress, uint32_t value)
{
    log_info(getLogger(), "Lectura Memoria: PID: %d - Acción: LEER - Dirección Física: %d - Valor: %u", pid, physicalAddress, value);
}

void logReadMemoryString(int pid, int physicalAddress, char* value)
{
    log_info(getLogger(), "Lectura Memoria: PID: %d - Acción: LEER - Dirección Física: %d - Valor: %s", pid, physicalAddress, value);
}

void logWriteMemoryUint(int pid, int physicalAddress, uint32_t value)
{
    log_info(getLogger(), "Escritura Memoria: PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %u", pid, physicalAddress, value);
}

void logWriteMemoryString(int pid, int physicalAddress, char* value)
{
    log_info(getLogger(), "Escritura Memoria: PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %s", pid, physicalAddress, value);
}





////////////////////// LOGS AUXILIARES GENERALES //////////////////////



void logLineSeparator()
{
    log_info(getLogger(), LINE_SEPARATOR);
}



////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_CPU


void logPreInitialMessageDebug()
{
    log_info(getLogger(), "INICIANDO EL MODULO CPU EN MODO DEBUG");
}

void logInitialMessageDebug()
{
    log_info(getLogger(), "INICIADO EL MODULO CPU EN MODO DEBUG");
}


#else


void logInitialMessageRealese()
{
    logLineSeparator();
    log_info(getLogger(), "INICIADO EL MODULO CPU");
    logLineSeparator();
}


#endif