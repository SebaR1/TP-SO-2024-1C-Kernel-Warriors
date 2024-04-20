#ifndef UTILS_GENERAL_H_
#define UTILS_GENERAL_H_


#include <commons/log.h>
#include <commons/collections/list.h>


#define DEFAULT_CASE_MESSAGE "Un cliente me envio una operacion no valida o desconocida."
#define ERROR_CASE_MESSAGE "Un cliente se desconecto."

// Estructura usada para saber de que tipo es el paquete que envia un cliente.
typedef enum
{
    ERROR = -1,
	DO_NOTHING, // Segun testeos, cuando un cliente se conecta, en su primer o segundo mensaje, envia un mensaje vacio (es decir, con codigo operacion 0). No es un error, simplemente es algo a tener en cuenta y no hacer nada al respecto.
	EXIT_CPU,
	EXIT_MEMORY,
	KERNEL_MODULE,
	KERNEL_MODULE_TO_CPU_DISPATCH,
	KERNEL_MODULE_TO_CPU_INTERRUPT,
	CPU_MODULE,
	MEMORY_MODULE,
	IO_MODULE,
	PACKAGE_FROM_KERNEL,
	PACKAGE_FROM_CPU,
	PACKAGE_FROM_MEMORY,
	PACKAGE_FROM_IO,
} operationCode;







#endif