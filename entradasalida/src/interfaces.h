#ifndef IO_INTERFACES_H
#define IO_INTERFACES_H

#include "utilsIO/config.h"
#include "utilsIO/logger.h"
#include "utils/client/utils.h"
#include "utils/server/utils.h"
#include "connections/serverIO.h"
#include "connections/clientIO.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
typedef enum
{   
    IO_NULL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} supported_operations;

/*typedef enum
{
    GENERIC_TYPE,
    STDIN_TYPE,
    STDOUT_TYPE,
    DIALFS_TYPE
} interface_type;*/

typedef struct
{   
    uint32_t pid;
    supported_operations operation;
    void* params;
} t_currentOperation;

typedef struct
{
    char *name;
    interfaceType type;
    t_currentOperation currentOperation;
    uint32_t workUnits;
} t_interfaceData;

typedef struct
{
    uint32_t workUnits;
} t_paramsForGenericInterface;

typedef struct
{
    uint32_t amountOfPhysicalAddresses;
    physicalAddressInfo* addressesInfo;
    uint32_t totalSize;
} t_paramsForStdinInterface;

typedef struct
{
    uint32_t amountOfPhysicalAddresses;
    physicalAddressInfo* addressesInfo;
    uint32_t totalSize;
} t_paramsForStdoutInterface;

typedef struct
{
    char *fileName;
} t_paramsForIOFSCreateOrDelete;

typedef struct
{
    char *fileName;
    uint32_t registerSize;
} t_paramsForIOFSTruncate;

typedef struct
{
    char *fileName;
    uint32_t registerDirection;
    uint32_t registerSize;
    uint32_t registerFilePointer;
} t_paramsForIOFSWriteOrRead;

typedef struct
{
    char *resultsFromRead;
    char *resultsForMemory;
} t_resultsForStdin;

typedef struct
{
    char *resultsForWrite;
} t_resultsForStdout;

typedef struct
{
    char *resultsForWrite;
} t_resultsForIOFSWrite;

typedef struct
{
    char *resultsFromRead;
} t_resultsForIOFSRead;

typedef struct
{   
    char* mappedFile;
    int fd;
} t_fileData;


extern t_interfaceData interfaceData;
extern t_resultsForStdin resultsForStdin;
extern t_resultsForStdout resultsForStdout;
extern t_resultsForIOFSWrite resultsForIOFSWrite;
extern t_resultsForIOFSRead resultsForIOFSRead;

extern void* dataReceivedFromMemory;

extern sem_t semaphoreForStdin;
extern sem_t semaphoreForStdout;
extern sem_t semaphoreForIOFSWrite;
extern sem_t semaphoreForIOFSRead;
extern sem_t semaphoreForModule;

extern int socketKernel;
extern int socketMemory;

extern FILE* blocks;
extern FILE* bitmap;

extern t_fileData* blocksData;
extern t_fileData* bitmapData;

extern t_bitarray* mappedBitmap;

extern t_list* listFileNames;

void createInterface(char* name);

void destroyInterface();

/// @brief Le manda la data especificada a la Memoria, junto con las direcciones fisicas.
/// @param data La data a mandar.
/// @param addressesInfo Array de physicalAddressInfo, donde cada indice contiene la direccion fisica y el tamaño a escribir en la misma.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas (el tamaño del array).
void writeToMemory(void* data, physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses);

/// @brief Le pide a Memoria la data que está en la direccion fisica dada, con su tamaño.
/// @param addressesInfo Array de physicalAddressInfo, donde cada indice contiene la direccion fisica y el tamaño a escribir en la misma.
/// @param amountOfPhysicalAddresses La cantidad de direcciones fisicas (el tamaño del array).
void readFromMemory(physicalAddressInfo* addressesInfo, int amountOfPhysicalAddresses);

t_fileData* openCreateMapFile(FILE* file, char* fileName, int fileSize);

void createListFileNames();

void closeBlocksFile();

void closeBitmapFile();

#endif