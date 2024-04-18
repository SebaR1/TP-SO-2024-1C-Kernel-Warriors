#ifndef MEMORY_SERVER_H_
#define MEMORY_SERVER_H_




// Hilo que funciona como servidor para la escucha de paquetes por parte del modulo especificado en el nombre de la funcion
void serverMemoryForKernel(void* args);


// Hilo que funciona como servidor para la escucha de paquetes por parte del modulo especificado en el nombre de la funcion
void serverMemoryForCPU(void* args);

// Hilo que funciona como servidor para la escucha de paquetes por parte del modulo especificado en el nombre de la funcion
void serverMemoryForIO(void* args);











#endif