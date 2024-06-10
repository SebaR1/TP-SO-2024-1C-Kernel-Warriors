#ifndef CPU_EXECUTE_H_
#define CPU_EXECUTE_H_

#include <stdio.h>
#include <stdint.h>
#include "essentials.h"



// Enum que determina si el registro a usar es de 1 o 4 bytes
typedef enum
{
    REGISTER_1_BYTE,
    REGISTER_4_BYTES,
} registerTypeByBytes;




void executeInstruction(instructionType type, void* paramsGeneric);



////////////////// FUNCIONES QUE REPRESENTAN LAS INSTRUCCIONES //////////////////

////////////////// WARNING! TODOS LOS STRINGS (char*) PASADOS POR PARAMETRO DEBEN LIBERAR LA MEMORIA USANDO LA FUNCION FREE CUANDO LO TERMINEN DE USAR //////////////////


// (Registro, Valor): Asigna al registro el valor pasado como parámetro.
void SET(registerType reg, uint32_t value);


// (Registro Datos, Registro Dirección): Lee el valor de memoria correspondiente a la Dirección Lógica
// que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
void MOV_IN(registerType data, registerType direction);


// (Registro Dirección, Registro Datos): Lee el valor del Registro Datos y lo escribe en la dirección física de memoria
// obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
void MOV_OUT(registerType direction, registerType data);


// (Registro Destino, Registro Origen): Suma al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
void SUM(registerType destination, registerType origin);


// (Registro Destino, Registro Origen): Resta al Registro Destino el Registro Origen y deja el resultado en el Registro Destino.
void SUB(registerType destination, registerType origin);


// (Registro, Instrucción): Si el valor del registro es distinto de cero, actualiza el program counter al número de instrucción pasada por parámetro.
void JNZ(registerType reg, uint32_t instruction);


// (Tamaño): Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro.
// En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
void RESIZE(uint32_t size);


// (Tamaño): Toma del string apuntado por el registro SI y copia la cantidad de bytes indicadas
// en el parámetro tamaño a la posición de memoria apuntada por el registro DI. 
void COPY_STRING(uint32_t size);


// (Recurso): Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro.
void WAIT(char* resource);


// (Recurso): Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro.
void SIGNAL(char* resource);


//  (Interfaz, Unidades de trabajo): Esta instrucción solicita al Kernel que se envíe a una interfaz de I/O
// a que realice un sleep por una cantidad de unidades de trabajo.
void IO_GEN_SLEEP(char* interface, uint32_t workUnits);


// (Interfaz, Registro Dirección, Registro Tamaño): Esta instrucción solicita al Kernel que mediante la interfaz ingresada
// se lea desde el STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del Registro Tamaño y el mismo se guarde
// a partir de la Dirección Lógica almacenada en el Registro Dirección.
void IO_STDIN_READ(char* resource, registerType direction, registerType size);


// (Interfaz, Registro Dirección, Registro Tamaño): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, se
// lea desde la posición de memoria indicada por la Dirección Lógica almacenada en el Registro Dirección, un tamaño indicado por el
// Registro Tamaño y se imprima por pantalla.
void IO_STDOUT_WRITE(char* resource, registerType direction, registerType size);


// (Interfaz, Nombre Archivo): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
// se cree un archivo en el FS montado en dicha interfaz.
void IO_FS_CREATE(char* resource, char* fileName);


// (Interfaz, Nombre Archivo): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
// se elimine un archivo en el FS montado en dicha interfaz.
void IO_FS_DELETE(char* resource, char* fileName);


// (Interfaz, Nombre Archivo, Registro Tamaño): Esta instrucción solicita al Kernel que mediante la interfaz seleccionada,
// se modifique el tamaño del archivo en el FS montado en dicha interfaz,
// actualizando al valor que se encuentra en el registro indicado por Registro Tamaño.
void IO_FS_TRUNCATE(char* resource, char* fileName, registerType size);


// (Interfaz, Nombre Archivo, Registro Dirección, Registro Tamaño, Registro Puntero Archivo): Esta instrucción solicita al Kernel
// que mediante la interfaz seleccionada, se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la
// dirección lógica que se encuentra en el Registro Dirección y se escriban en el archivo a partir del valor del Registro Puntero Archivo.
void IO_FS_WRITE(char* resource, char* fileName, registerType direction, registerType size, registerType pointer);


// (Interfaz, Nombre Archivo, Registro Dirección, Registro Tamaño, Registro Puntero Archivo): Esta instrucción solicita al Kernel
// que mediante la interfaz seleccionada, se lea desde el archivo a partir del valor del Registro Puntero Archivo la
// cantidad de bytes indicada por Registro Tamaño y se escriban en la Memoria a partir de la dirección lógica indicada en el Registro Dirección.
void IO_FS_READ(char* resource, char* fileName, registerType direction, registerType size, registerType pointer);


// Esta instrucción representa la syscall de finalización del proceso. Se deberá devolver el Contexto de Ejecución actualizado al Kernel para su finalización.
void EXIT();







////////////////// FUNCIONES AUXILIARES PARTICULARES DE CADA INSTRUCCION //////////////////


void _SET1(uint8_t* reg, uint32_t value);
void _SET4(uint32_t* reg, uint32_t value);


void _SUM11(uint8_t* destination, uint8_t* origin);
void _SUM14(uint8_t* destination, uint32_t* origin);
void _SUM41(uint32_t* destination, uint8_t* origin);
void _SUM44(uint32_t* destination, uint32_t* origin);


void _SUB11(uint8_t* destination, uint8_t* origin);
void _SUB14(uint8_t* destination, uint32_t* origin);
void _SUB41(uint32_t* destination, uint8_t* origin);
void _SUB44(uint32_t* destination, uint32_t* origin);


void _JNZ1(uint8_t* reg, uint32_t instruction);
void _JNZ4(uint32_t* reg, uint32_t instruction);








////////////////// FUNCIONES AUXILIARES GENERALES //////////////////



// Convierte el tipo de registro de type en una referencia al registro real.
// Si el registro de type es de 1 byte (como AX por ejemplo), almacena en outRegister1byte la direccion de memoria del registro pedido en type.
// Si el registro de type es de 8 bytes (Como EAX por ejemplo), almacena en outREgister4bytes la direccion de memoria del registro pedido en type.
// Retorna REGISTER_1_BYTE si el registro es de 1 byte, o REGISTER_4_BYTES si el registro es de 4 bytes
registerTypeByBytes _typeToRegister(registerType type, uint8_t* outRegister1byte, uint32_t* outRegister4bytes);




#endif