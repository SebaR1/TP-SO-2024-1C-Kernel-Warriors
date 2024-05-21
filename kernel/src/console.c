#include "console.h"

//Habilita una consola y valida las instrucciones
void readKernelConsole(){
    log_info(getLogger(), "Consola iniciada\n");
    char* read;
    read = readline("");

    while(!string_is_empty(read)){
        if(!_isAnInstruction(read)){
            log_error(getLogger(), "Comando de consola no recognized");
            free(read);
            read = readline("");
            continue;
        }

        attendInstruction(read);
        read = readline("");
    } //Para terminar la consola se pone algo vacio
    free(read);
}

//Valida si hay alguna instruccion con lo ingresado
bool _isAnInstruction(char* instruction){
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        return true;
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        if(string_array_size(consoleCommand) == 2) {
            string_array_destroy(consoleCommand);
            return true;
            } //Se fija si tiene la cantidad de parametros que pide la instruccion
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
            return true;
    }
    
    string_array_destroy(consoleCommand);
    return false;
}


//Atiende la instruccion
void attendInstruction(char* instruction)
{
    char** consoleCommand = string_split(instruction, " ");

    if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PROCESO")){
        addPcbToNew(); //Falta agregar un parametro que pide a memoria el proceso especifico a iniciar
    }
    else if(string_equals_ignore_case(consoleCommand[0], "FINALIZAR_PROCESO")){
        //Implementacion para FINALIZAR_PROCESO
    }
    else if(string_equals_ignore_case(consoleCommand[0], "DETENER_PLANIFICACION")){
        //Implementacion para DETENER_PLANIFICACION
    }
    else if(string_equals_ignore_case(consoleCommand[0], "INICIAR_PLANIFICACION")){
        //Implementacion para INICIAR_PLANIFICACION
    }
    else if(string_equals_ignore_case(consoleCommand[0], "MULTIPROGRAMACION")){
        //Implementacion para MULTIPROGRAMACION
    }
    else if(string_equals_ignore_case(consoleCommand[0], "PROCESO_ESTADO")){
        //Implementacion para PROCESO_ESTADO
    }

     string_array_destroy(consoleCommand);
}
