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
	KERNEL_MODULE,
	KERNEL_MODULE_TO_CPU_DISPATCH,
	KERNEL_MODULE_TO_CPU_INTERRUPT,
	CPU_MODULE,
	MEMORY_MODULE,
	IO_MODULE,
	CPU_GIVE_ME_NEXT_INSTRUCTION,
	MEMORY_NEXT_INSTRUCTION,

	// KERNEL
	KERNEL_SEND_CONTEXT,
	KERNEL_SEND_PROCESS_PATH,
	KERNEL_SEND_INTERRUPT_QUANTUM_END,
	KERNEL_SEND_INTERRUPT_CONSOLE_END_PROCESS,

	//IO
	REQUEST_OPERATION_TO_INTERFACE,

	// TODOS LOS SIGUIENTES VALORES SON UNICAMENTE PARA TESTEOS
	PACKAGE_FROM_KERNEL,
	PACKAGE_FROM_CPU,
	PACKAGE_FROM_MEMORY,
	PACKAGE_FROM_IO,
} operationCode;

// typedef struct 
// {
// 	t_list *list;
// 	pthread_mutex_t mutex;
// } listMutex_t;

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
} registers_t;

typedef enum{
	FIFO,
	RR,
	VRR,
} t_algorithm;
typedef enum
{
	PCB_NEW,
	PCB_READY,
	PCB_EXEC,
	PCB_BLOCK,
	PCB_EXIT
} pcbState_t;

//Estructra del pcb del proceso
typedef struct 
{
	uint32_t pid;
	uint32_t pc;
	uint32_t timer;
	registers_t *registersCpu;
	pcbState_t state;
} pcb_t;

//Estructura del contexto de ejecucion de los procesos
typedef struct
{
	registers_t registersCpu;
	//instrucciones
	uint32_t pc;

} contextProcess;

//Estructura que se le manda a la memoria del path y el pid del proceso a ejecutar.
typedef struct
{
	uint32_t pid;
	char* path;
} kernelPathProcess;

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

#endif