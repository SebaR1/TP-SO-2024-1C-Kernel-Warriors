#ifndef UTILS_GENERAL_H_
#define UTILS_GENERAL_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <netdb.h>
#include "listMutex.h"


#define DEFAULT_CASE_MESSAGE "Un cliente me envio una operacion no valida o desconocida."
#define ERROR_CASE_MESSAGE "Un cliente se desconecto."

// Estructura usada para saber de que tipo es el paquete que envia un cliente.
typedef enum
{
    ERROR = -1,
	DO_NOTHING = 0, // Segun testeos, cuando un cliente se conecta, en su primer o segundo mensaje, envia un mensaje vacio (es decir, con codigo operacion 0). No es un error, simplemente es algo a tener en cuenta y no hacer nada al respecto.
	EXIT_CPU,
	EXIT_MEMORY,

	// HANDSHAKES
	KERNEL_MODULE,
	KERNEL_MODULE_TO_CPU_DISPATCH,
	KERNEL_MODULE_TO_CPU_INTERRUPT,
	CPU_MODULE,
	MEMORY_MODULE,
	IO_MODULE,

	// MENSAJES GENERICOS (PUEDE ENVIARLO MÁS DE UN MODULO)
	READ_MEMORY,
	WRITE_MEMORY,

	// ENVIADOS DESDE CPU
	CPU_GIVE_ME_NEXT_INSTRUCTION,
	CPU_GET_FRAME,

	// ENVIADOS DESDE MEMORIA
	MEMORY_NEXT_INSTRUCTION,
	MEMORY_SEND_DATA,
	MEMORY_OK,
	MEMORY_SEND_FRAME,

	// ENVIADOS DESDE KERNEL
	KERNEL_SEND_PROCESS_PATH,
	KERNEL_END_PROCESS,

	KERNEL_SEND_CONTEXT,
	KERNEL_SEND_INTERRUPT_QUANTUM_END,
	KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS,
	
	KERNEL_SEND_OPERATION_TO_GENERIC_INTERFACE,

	// ENVIADOS DESDE IO
	REQUEST_OPERATION_TO_INTERFACE,

	// TODOS LOS SIGUIENTES VALORES SON UNICAMENTE PARA TESTEOS
	PACKAGE_FROM_KERNEL,
	PACKAGE_FROM_CPU,
	PACKAGE_FROM_MEMORY,
	PACKAGE_FROM_IO,
} operationCode;

typedef struct
{
	uint32_t PC; // Program Counter, indica la próxima instrucción a ejecutar una vez completado un ciclo de ejecucion
	uint8_t AX; // Registro Numérico de propósito general
	uint8_t BX; // Registro Numérico de propósito general
	uint8_t CX; // Registro Numérico de propósito general
	uint8_t DX; // Registro Numérico de propósito general
	uint32_t EAX; // Registro Numérico de propósito general
	uint32_t EBX; // Registro Numérico de propósito general
	uint32_t ECX; // Registro Numérico de propósito general
	uint32_t EDX; // Registro Numérico de propósito general
	uint32_t SI; // Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string.
	uint32_t DI; // Contiene la dirección lógica de memoria de destino a donde se va a copiar un string.
} t_registers;

//Estructura de los estados del PCB. 
typedef enum
{
	PCB_NEW,
	PCB_READY,
	PCB_EXEC,
	PCB_BLOCK,
	PCB_EXIT
} pcbState_t;

typedef struct
{
	char* param1;
	char* param2;
	char* param3;
	char* param4;
} paramsKernelForIO;

//Estructra del pcb del proceso
typedef struct 
{
	uint32_t pid;
	uint32_t pc;
	uint32_t timer;
	paramsKernelForIO *params;
	t_registers *registersCpu;
	listMutex_t *resources;
	pcbState_t state;
} pcb_t;

// Estructura de un recurso en Kernel. 
typedef struct 
{
	char* name;
	int instances;
	pthread_mutex_t mutexForInstances;
	listMutex_t *blockList;
} resource_t;

typedef enum
{
	Generic, 
	STDIN, 
	STDOUT, 
	DialFS
} interfaceType;

// Estructura de una interfaz en Kernel.
typedef struct 
{
	char* name;
	interfaceType interfaceType;
	bool isBusy;
	pcb_t *processAssign;
	listMutex_t *blockList;
} interface_t;

// Tipo de algoritmo seleccionado. 
typedef enum
{
	FIFO,
	RR,
	VRR,
} t_algorithm;


//Estructura del contexto de ejecucion de los procesos
typedef struct
{
	uint32_t pc;
	t_registers registersCpu;
} contextProcess;

// Estructura que se le manda a la memoria del path y el pid del proceso a ejecutar.
typedef struct
{
	uint32_t pid;
	char* path;
} kernelPathProcess;

// Estructura que se le manda a la memoria para que elimine un proceso.
typedef struct
{
	uint32_t pid;
} kernelEndProcess;

// Estructura que se le manda a la memoria para decirle que debe enviarle a la CPU la proxima instruccion.
typedef struct
{
	int PID;
	uint32_t PC; // Program Counter
} cpuGiveMeNextInstruction;


// Estructura que se le manda a la CPU con el string de la instruccion que debe ejecutar
typedef struct
{
	char* string;
} instructionString;


typedef struct
{
	int physicalAddress;
	int size;
} requestReadMemoryInfo;


typedef struct
{
	void* data;
	int physicalAddress;
	int size;
} requestWriteMemoryInfo;


typedef struct
{
	void* data;
	int size;
} sendDataInfo;


typedef struct
{
	int PID;
	int page;
} getFrameInfo;



typedef struct
{
	int frame;
} sendFrameInfo;




#endif