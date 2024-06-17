#include "logger.h"

t_log* _logger;



///////////////////// FUNCIONES AUXILIARES DE LOGS /////////////////////


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


void logCreateProcess(int pid, int amountOfPages)
{
    log_info(getLogger(), "Creación de Tabla de Páginas - PID: %d - Tamaño: %d", pid, amountOfPages);
}

void logCreateProcessError(char* pseudocodePath)
{
    log_info(getLogger(), "No se pudo abrir el archivo \"%s\".", pseudocodePath);
}

void logDestroyProcess(int pid, int amountOfPages)
{
    log_info(getLogger(), "Destrucción de Tabla de Páginas - PID: %d - Tamaño: %d", pid, amountOfPages);
}

void logPageTableAccess(int pid, int page, int frame)
{
    log_info("Acceso a Tabla de Páginas - PID: %d - Pagina: %d - Marco: %d", pid, page, frame);
}

void logProcessSizeExpansion(int pid, int currentSize, int sizeToExpand)
{
    log_info(getLogger(), "Ampliación de proceso - PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", pid, currentSize, sizeToExpand);
}

void logProcessSizeReduction(int pid, int currentSize, int sizeToReduct)
{
    log_info(getLogger(), "Reducción de proceso - PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", pid, currentSize, sizeToReduct);
}

void logProcessSizeNotChange(int pid, int currentSize)
{
    log_info(getLogger(), "No cambia el tamaño del proceso - PID: %d - Tamaño Actual: %d - Tamaño a Ampliar/Reducir: %d", pid, currentSize, 0);
}

void logReadBytes(int pid, int physicalAddress, int size)
{
    log_info(getLogger(), "Acceso a espacio de usuario - PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño: %d", pid, physicalAddress, size);
}

void logWriteBytes(int pid, int physicalAddress, int size)
{
    log_info(getLogger(), "Acceso a espacio de usuario - PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño: %d", pid, physicalAddress, size);
}