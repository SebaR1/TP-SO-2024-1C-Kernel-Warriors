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






///////////////////////// INSTRUCCION SET /////////////////////////

void SET(registerType registerType, uint32_t value)
{
    uint8_t* reg1bytes;
    uint32_t* reg4bytes;


    switch (_TypeToRegister(registerType, reg1bytes, reg4bytes))
    {
    case REGISTER_1_BYTE:
        _SET1(reg1bytes, value);
        break;

    case REGISTER_4_BYTES:
        _SET4(reg4bytes, value);
        break;
    }

    PC++;
}

void _SET1(uint8_t* reg, uint32_t value)
{
    *reg = value;
}

void _SET4(uint32_t* reg, uint32_t value)
{
    *reg = value;
}




///////////////////////// INSTRUCCION SUM /////////////////////////


void SUM(registerType destination, registerType origin)
{
    uint8_t* destination1bytes;
    uint8_t* origin1bytes;
    uint32_t* destination4bytes;
    uint32_t* origin4bytes;


    switch (_TypeToRegister(destination, destination1bytes, destination4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // Destination 1 bytes
        switch (_TypeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 1 bytes y origin 1 bytes
            _SUM11(destination1bytes, origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 1 bytes y origin 4 bytes
            _SUM14(destination1bytes, origin4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // Destination 4 bytes
        switch (_TypeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 4 bytes y origin 1 bytes
            _SUM41(destination4bytes, origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 4 bytes y origin 4 bytes
            _SUM44(destination4bytes, origin4bytes);
            break;
        }

        break;
    }

    PC++;
}

void _SUM11(uint8_t* destination, uint8_t* origin)
{
    *destination = *destination + *origin;
}

void _SUM14(uint8_t* destination, uint32_t* origin)
{
    *destination = *destination + *origin;
}

void _SUM41(uint32_t* destination, uint8_t* origin)
{
    *destination = *destination + *origin;
}

void _SUM44(uint32_t* destination, uint32_t* origin)
{
    *destination = *destination + *origin;
}




///////////////////////// INSTRUCCION SUB /////////////////////////

void SUB(registerType destination, registerType origin)
{
    uint8_t* destination1bytes;
    uint8_t* origin1bytes;
    uint32_t* destination4bytes;
    uint32_t* origin4bytes;


    switch (_TypeToRegister(destination, destination1bytes, destination4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // Destination 1 bytes
        switch (_TypeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 1 bytes y origin 1 bytes
            _SUB11(destination1bytes, origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 1 bytes y origin 4 bytes
            _SUB14(destination1bytes, origin4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // Destination 4 bytes
        switch (_TypeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 4 bytes y origin 1 bytes
            _SUB41(destination4bytes, origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 4 bytes y origin 4 bytes
            _SUB44(destination4bytes, origin4bytes);
            break;
        }

        break;
    }

    PC++;
}

void _SUB11(uint8_t* destination, uint8_t* origin)
{
    *destination = *destination - *origin;
}

void _SUB14(uint8_t* destination, uint32_t* origin)
{
    *destination = *destination - *origin;
}

void _SUB41(uint32_t* destination, uint8_t* origin)
{
    *destination = *destination - *origin;
}

void _SUB44(uint32_t* destination, uint32_t* origin)
{
    *destination = *destination - *origin;
}





///////////////////////// INSTRUCCION JNZ /////////////////////////

void JNZ(registerType reg, uint32_t instruction)
{
    uint8_t* reg1bytes;
    uint32_t* reg4bytes;


    switch (_TypeToRegister(reg, reg1bytes, reg4bytes))
    {
    case REGISTER_1_BYTE:
        _JNZ1(reg1bytes, instruction);
        break;

    case REGISTER_4_BYTES:
        _JNZ4(reg4bytes, instruction);
        break;
    }
}

void _JNZ1(uint8_t* reg, uint32_t instruction)
{
    if (*reg != 0)
    {
        PC = instruction;
        return;
    }

    PC++;
}

void _JNZ4(uint32_t* reg, uint32_t instruction)
{
    if (*reg != 0)
    {
        PC = instruction;
        return;
    }

    PC++;
}





///////////////////////// FUNCIONES AUXILIARES GENERALES /////////////////////////

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





