#include "instructions.h"

uint32_t PC = 0; // Program Counter, indica la próxima instrucción a ejecutar
uint8_t AX = 0; // Registro Numérico de propósito general
uint8_t BX = 0; // Registro Numérico de propósito general
uint8_t CX = 0; // Registro Numérico de propósito general
uint8_t DX = 0; // Registro Numérico de propósito general
uint32_t EAX = 0; // Registro Numérico de propósito general
uint32_t EBX = 0; // Registro Numérico de propósito general
uint32_t ECX = 0; // Registro Numérico de propósito general
uint32_t EDX = 0; // Registro Numérico de propósito general
uint32_t SI = 0; // Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string.
uint32_t DI = 0; // Contiene la dirección lógica de memoria de destino a donde se va a copiar un string.




void SET(registerType registerType, uint32_t value)
{
    uint8_t* register1byte;
    uint32_t* register4bytes;


    switch (_TypeToRegister(registerType, register1byte, register4bytes))
    {
    case REGISTER_1_BYTE:
        *register1byte = value;
        break;

    case REGISTER_4_BYTES:
        *register4bytes = value;
        break;
    }
}




registerTypeByBytes _TypeToRegister(registerType type, uint8_t* outRegister1byte, uint32_t* outRegister4bytes)
{
    registerTypeByBytes typeByBytes;
    outRegister1byte = NULL;
    outRegister4bytes = NULL;

    switch (type)
    {
    case PC_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &PC;

    case AX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        outRegister1byte = &AX;
        break;

    case BX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        outRegister1byte = &BX;
        break;

    case CX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        outRegister1byte = &CX;
        break;

    case DX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        outRegister1byte = &DX;
        break;

    case EAX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &EAX;
        break;

    case EBX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &EBX;
        break;

    case ECX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &ECX;
        break;

    case EDX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &EDX;
        break;

    case SI_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &SI;
        break;

    case DI_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        outRegister4bytes = &DI;
        break;
    }

    return typeByBytes;
}





