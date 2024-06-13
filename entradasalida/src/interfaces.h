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
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

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

typedef enum
{
    GENERIC_TYPE,
    STDIN_TYPE,
    STDOUT_TYPE,
    DIALFS_TYPE
} interface_type;

typedef struct
{   
    uint32_t pid;
    supported_operations operation;
    void* params;
} t_currentOperation;

typedef struct
{
    char *name;
    interface_type type;
    t_currentOperation currentOperation;
    uint32_t workUnits;
} t_interfaceData;

typedef struct
{
    uint32_t workUnits;
} t_paramsForGenericInterface;

typedef struct
{
    uint32_t registerDirection;
    uint32_t registerSize;
} t_paramsForStdinInterface;

typedef struct
{
    uint32_t registerDirection;
    uint32_t registerSize;
} t_paramsForStdoutInterface;

typedef struct
{
    char *resultsFromRead;
} t_resultsForStdin;

typedef struct
{
    char *resultsFromRead;
} t_resultsForStdout;


void createInterface(t_interfaceData *interfaceData);

#endif