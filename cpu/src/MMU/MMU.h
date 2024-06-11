#ifndef CPU_MMU_H_
#define CPU_MMU_H_


// El tamaño de las paginas en bytes de la memoria
extern int TAM_PAGINA;


// Struct que contiene la info de la pagina y el offset correspondiente a una direccion logica
typedef struct
{
    int page;
    int offset;
} pageAndOffset;


// Settea el valor value a la variable que contiene el tamaño de pagina en bytes de la memoria
void setTamPagina(int value);

// Obtiene el tamaño de pagina  en bytes de la memoria
int getTamPagina();



/// @brief Obtiene el numero de pagina y el desplazamiento correspondiente a la direccion logica pasada por parametro
/// @param logicalAddress La direccion logica
/// @param outPageAndOffset Retorna un struct que contiene el numero de pagina y el desplazamiento. No debe ser NULL.
void getPageAndOffset(int logicalAddress, pageAndOffset* outPageAndOffset);








#endif