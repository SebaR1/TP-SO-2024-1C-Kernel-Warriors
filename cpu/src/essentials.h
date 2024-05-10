#ifndef CPU_ESSENTIALS_H_
#define CPU_ESSENTIALS_H_

#include <stdint.h>



#define INSTRUCTION_SEPARATOR " "
#define SET_STRING "SET"
#define MOV_IN_STRING "MOV_IN"
#define MOV_OUT_STRING "MOV_OUT"
#define SUM_STRING "SUM"
#define SUB_STRING "SUB"
#define JNZ_STRING "JNZ"
#define RESIZE_STRING "RESIZE"
#define COPY_STRING_STRING "COPY_STRING"
#define WAIT_STRING "WAIT"
#define SIGNAL_STRING "SIGNAL"
#define IO_GEN_SLEEP_STRING "IO_GEN_SLEEP"
#define IO_STDIN_READ_STRING "IO_STDIN_READ"
#define IO_STDOUT_WRITE_STRING "IO_STDOUT_WRITE"
#define IO_FS_CREATE_STRING "IO_FS_CREATE"
#define IO_FS_DELETE_STRING "IO_FS_DELETE"
#define IO_FS_TRUNCATE_STRING "IO_FS_TRUNCATE"
#define IO_FS_WRITE_STRING "IO_FS_WRITE"
#define IO_FS_READ_STRING "IO_FS_READ"
#define EXIT_STRING "EXIT"


#define PC_STRING "PC"
#define AX_STRING "AX"
#define BX_STRING "BX"
#define CX_STRING "CX"
#define DX_STRING "DX"
#define EAX_STRING "EAX"
#define EBX_STRING "EBX"
#define ECX_STRING "ECX"
#define EDX_STRING "EDX"
#define SI_STRING "SI"
#define DI_STRING "DI"




// Enum que determina el tipo de registro
typedef enum
{
    PC_TYPE,
    AX_TYPE,
    BX_TYPE,
    CX_TYPE,
    DX_TYPE,
    EAX_TYPE,
    EBX_TYPE,
    ECX_TYPE,
    EDX_TYPE,
    SI_TYPE,
    DI_TYPE,
} registerType;


// Enum que dice de qué tipo es una instrucción
typedef enum
{
    INVALID_TYPE = -1,
    SET_TYPE,
    MOV_IN_TYPE,
    MOV_OUT_TYPE,
    SUM_TYPE,
    SUB_TYPE,
    JNZ_TYPE,
    RESIZE_TYPE,
    COPY_STRING_TYPE,
    WAIT_TYPE,
    SIGNAL_TYPE,
    IO_GEN_SLEEP_TYPE,
    IO_STDIN_READ_TYPE,
    IO_STDOUT_WRITE_TYPE,
    IO_FS_CREATE_TYPE,
    IO_FS_DELETE_TYPE,
    IO_FS_TRUNCATE_TYPE,
    IO_FS_WRITE_TYPE,
    IO_FS_READ_TYPE,
    EXIT_TYPE,
} instructionType;





////////////////////// ESTRUCTURAS QUE CONTIENEN LA INFORMACION QUE SE LES PASARA POR PARAMETRO A LAS FUNCIONES QUE REPRESENTAN LAS INSTRUCCIONES //////////////////////


typedef struct
{
    registerType reg;
    uint32_t value;
} SET_STRUCT;

typedef struct
{
    registerType data;
    registerType direction;
} MOV_IN_STRUCT;

typedef struct
{
    registerType direction;
    registerType data;
} MOV_OUT_STRUCT;

typedef struct
{
    registerType destination;
    registerType origin;
} SUM_STRUCT;

typedef struct
{
    registerType destination;
    registerType origin;
} SUB_STRUCT;

typedef struct
{
    registerType reg;
    uint32_t instruction;
} JNZ_STRUCT;

typedef struct
{
    uint32_t size;
} RESIZE_STRUCT;

typedef struct
{
    uint32_t size;
} COPY_STRING_STRUCT;

typedef struct
{
    char* resource;
} WAIT_STRUCT;

typedef struct
{
    char* resource;
} SIGNAL_STRUCT;

typedef struct
{
    char* interface;
    uint32_t workUnits;
} IO_GEN_SLEEP_STRUCT;

typedef struct
{
    char* resource;
    registerType direction;
    registerType size;
} IO_STDIN_READ_STRUCT;

typedef struct
{
    char* resource;
    registerType direction;
    registerType size;
} IO_STDOUT_WRITE_STRUCT;

typedef struct
{
    char* resource;
    char* fileName;
} IO_FS_CREATE_STRUCT;

typedef struct
{
    char* resource;
    char* fileName;
} IO_FS_DELETE_STRUCT;

typedef struct
{
    char* resource;
    char* fileName;
    registerType size;
} IO_FS_TRUNCATE_STRUCT;

typedef struct
{
    char* resource;
    char* fileName;
    registerType direction;
    registerType size;
    registerType pointer;
} IO_FS_WRITE_STRUCT;

typedef struct
{
    char* resource;
    char* fileName;
    registerType direction;
    registerType size;
    registerType pointer;
} IO_FS_READ_STRUCT;




#endif