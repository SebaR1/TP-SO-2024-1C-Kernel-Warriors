#include "execute.h"
#include "MMU/MMU.h"
#include "connections/clientCPU.h"
#include "utilsCPU/logger.h"

uint32_t PC = 0; // Program Counter, indica la próxima instrucción a ejecutar una vez completado un ciclo de ejecucion
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




void executeInstruction(instructionType type, void* paramsGeneric)
{
    switch (type)
    {
    case INVALID_TYPE:
        break;

    case SET_TYPE:
    {
        SET_STRUCT* params = paramsGeneric;
        SET(params->reg, params->value);
        break;
    }

    case MOV_IN_TYPE:
    {
        MOV_IN_STRUCT* params = paramsGeneric;
        MOV_IN(params->data, params->direction);
        break;
    }

    case MOV_OUT_TYPE:
    {
        MOV_OUT_STRUCT* params = paramsGeneric;
        MOV_OUT(params->direction, params->data);
        break;
    }

    case SUM_TYPE:
    {
        SUM_STRUCT* params = paramsGeneric;
        SUM(params->destination, params->origin);
        break;
    }

    case SUB_TYPE:
    {
        SUB_STRUCT* params = paramsGeneric;
        SUB(params->destination, params->origin);
        break;
    }

    case JNZ_TYPE:
    {
        JNZ_STRUCT* params = paramsGeneric;
        JNZ(params->reg, params->instruction);
        break;
    }

    case RESIZE_TYPE:
    {
        RESIZE_STRUCT* params = paramsGeneric;
        RESIZE(params->size);
        break;
    }

    case COPY_STRING_TYPE:
    {
        COPY_STRING_STRUCT* params = paramsGeneric;
        COPY_STRING(params->size);
        break;
    }

    case WAIT_TYPE:
    {
        WAIT_STRUCT* params = paramsGeneric;
        WAIT(params->resource);
        break;
    }

    case SIGNAL_TYPE:
    {
        SIGNAL_STRUCT* params = paramsGeneric;
        SIGNAL(params->resource);
        break;
    }

    case IO_GEN_SLEEP_TYPE:
    {
        IO_GEN_SLEEP_STRUCT* params = paramsGeneric;
        IO_GEN_SLEEP(params->interface, params->workUnits);
        break;
    }

    case IO_STDIN_READ_TYPE:
    {
        IO_STDIN_READ_STRUCT* params = paramsGeneric;
        IO_STDIN_READ(params->resource, params->direction, params->size);
        break;
    }

    case IO_STDOUT_WRITE_TYPE:
    {
        IO_STDOUT_WRITE_STRUCT* params = paramsGeneric;
        IO_STDOUT_WRITE(params->resource, params->direction, params->size);
        break;
    }

    case IO_FS_CREATE_TYPE:
    {
        IO_FS_CREATE_STRUCT* params = paramsGeneric;
        IO_FS_CREATE(params->resource, params->fileName);
        break;
    }

    case IO_FS_DELETE_TYPE:
    {
        IO_FS_DELETE_STRUCT* params = paramsGeneric;
        IO_FS_DELETE(params->resource, params->fileName);
        break;
    }

    case IO_FS_TRUNCATE_TYPE:
    {
        IO_FS_TRUNCATE_STRUCT* params = paramsGeneric;
        IO_FS_TRUNCATE(params->resource, params->fileName, params->size);
        break;
    }

    case IO_FS_WRITE_TYPE:
    {
        IO_FS_WRITE_STRUCT* params = paramsGeneric;
        IO_FS_WRITE(params->resource, params->fileName, params->direction, params->size, params->pointer);
        break;
    }

    case IO_FS_READ_TYPE:
    {
        IO_FS_READ_STRUCT* params = paramsGeneric;
        IO_FS_READ(params->resource, params->fileName, params->direction, params->size, params->pointer);
        break;
    }

    case EXIT_TYPE:
    {
        EXIT();
        break;
    }

    default:
        break;
    }

}



///////////////////////// INSTRUCCION SET /////////////////////////

void SET(registerType registerType, uint32_t value)
{
    uint8_t** reg1bytes = malloc(sizeof(uint8_t**));
    uint32_t** reg4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(registerType, reg1bytes, reg4bytes))
    {
    case REGISTER_1_BYTE:
        _SET1(*reg1bytes, value);
        break;

    case REGISTER_4_BYTES:
        _SET4(*reg4bytes, value);
        break;
    }

    free(reg1bytes);
    free(reg4bytes);
}

void _SET1(uint8_t* reg, uint32_t value)
{
    *reg = value;
}

void _SET4(uint32_t* reg, uint32_t value)
{
    *reg = value;
}



///////////////////////// INSTRUCCION MOV_IN /////////////////////////


void MOV_IN(registerType data, registerType direction)
{
    uint8_t** data1bytes = malloc(sizeof(uint8_t**));
    uint8_t** direction1bytes = malloc(sizeof(uint8_t**));
    uint32_t** data4bytes = malloc(sizeof(uint32_t**));
    uint32_t** direction4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(data, data1bytes, data4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // 1 bytes
        switch (_typeToRegister(direction, direction1bytes, direction4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // 1 bytes y 1 bytes
            _MOV_IN11(*data1bytes, *direction1bytes);
            break;
        
        case REGISTER_4_BYTES: // 1 bytes y 4 bytes
            _MOV_IN14(*data1bytes, *direction4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // 4 bytes
        switch (_typeToRegister(direction, direction1bytes, direction4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // 4 bytes y 1 bytes
            _MOV_IN41(*data4bytes, *direction1bytes);
            break;
        
        case REGISTER_4_BYTES: // 4 bytes y 4 bytes
            _MOV_IN44(*data4bytes, *direction4bytes);
            break;
        }

        break;
    }

    free(data1bytes);
    free(data4bytes);
    free(direction1bytes);
    free(direction4bytes);
}

void _MOV_IN11(uint8_t* data, uint8_t* direction)
{
    readFromMemory((void*)data, *direction, 1, MEMORY_UINT8_TYPE);
}

void _MOV_IN14(uint8_t* data, uint32_t* direction)
{
    readFromMemory((void*)data, *direction, 1, MEMORY_UINT8_TYPE);
}

void _MOV_IN41(uint32_t* data, uint8_t* direction)
{
    readFromMemory((void*)data, *direction, 4, MEMORY_UINT32_TYPE);
}

void _MOV_IN44(uint32_t* data, uint32_t* direction)
{
    readFromMemory((void*)data, *direction, 4, MEMORY_UINT32_TYPE);
}



///////////////////////// INSTRUCCION MOV_OUT /////////////////////////


void MOV_OUT(registerType direction, registerType data)
{
    uint8_t** direction1bytes = malloc(sizeof(uint8_t**));
    uint8_t** data1bytes = malloc(sizeof(uint8_t**));
    uint32_t** direction4bytes = malloc(sizeof(uint32_t**));
    uint32_t** data4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(direction, direction1bytes, direction4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // 1 bytes
        switch (_typeToRegister(data, data1bytes, data4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // 1 bytes y 1 bytes
            _MOV_OUT11(*direction1bytes, *data1bytes);
            break;
        
        case REGISTER_4_BYTES: // 1 bytes y 4 bytes
            _MOV_OUT14(*direction1bytes, *data4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // 4 bytes
        switch (_typeToRegister(data, data1bytes, data4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // 4 bytes y 1 bytes
            _MOV_OUT41(*direction4bytes, *data1bytes);
            break;
        
        case REGISTER_4_BYTES: // 4 bytes y 4 bytes
            _MOV_OUT44(*direction4bytes, *data4bytes);
            break;
        }

        break;
    }

    free(direction1bytes);
    free(direction4bytes);
    free(data1bytes);
    free(data4bytes);
}


void _MOV_OUT11(uint8_t* direction, uint8_t* data)
{
    writeToMemory((void*)data, *direction, 1, MEMORY_UINT8_TYPE);
}

void _MOV_OUT14(uint8_t* direction, uint32_t* data)
{
    writeToMemory((void*)data, *direction, 4, MEMORY_UINT32_TYPE);
}

void _MOV_OUT41(uint32_t* direction, uint8_t* data)
{
    writeToMemory((void*)data, *direction, 1, MEMORY_UINT8_TYPE);
}

void _MOV_OUT44(uint32_t* direction, uint32_t* data)
{
    writeToMemory((void*)data, *direction, 4, MEMORY_UINT32_TYPE);
}


///////////////////////// INSTRUCCION SUM /////////////////////////


void SUM(registerType destination, registerType origin)
{
    uint8_t** destination1bytes = malloc(sizeof(uint8_t**));
    uint8_t** origin1bytes = malloc(sizeof(uint8_t**));
    uint32_t** destination4bytes = malloc(sizeof(uint32_t**));
    uint32_t** origin4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(destination, destination1bytes, destination4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // Destination 1 bytes
        switch (_typeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 1 bytes y origin 1 bytes
            _SUM11(*destination1bytes, *origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 1 bytes y origin 4 bytes
            _SUM14(*destination1bytes, *origin4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // Destination 4 bytes
        switch (_typeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 4 bytes y origin 1 bytes
            _SUM41(*destination4bytes, *origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 4 bytes y origin 4 bytes
            _SUM44(*destination4bytes, *origin4bytes);
            break;
        }

        break;
    }

    free(origin1bytes);
    free(origin4bytes);
    free(destination1bytes);
    free(destination4bytes);
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
    uint8_t** destination1bytes = malloc(sizeof(uint8_t**));
    uint8_t** origin1bytes = malloc(sizeof(uint8_t**));
    uint32_t** destination4bytes = malloc(sizeof(uint32_t**));
    uint32_t** origin4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(destination, destination1bytes, destination4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
    {
    case REGISTER_1_BYTE: // Destination 1 bytes
        switch (_typeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 1 bytes y origin 1 bytes
            _SUB11(*destination1bytes, *origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 1 bytes y origin 4 bytes
            _SUB14(*destination1bytes, *origin4bytes);
            break;
        }

        break;

    case REGISTER_4_BYTES: // Destination 4 bytes
        switch (_typeToRegister(origin, origin1bytes, origin4bytes)) // Se fija si la variable pasada por parametro es de 1 o 4 bytes
        {
        case REGISTER_1_BYTE: // Destination 4 bytes y origin 1 bytes
            _SUB41(*destination4bytes, *origin1bytes);
            break;
        
        case REGISTER_4_BYTES: // Destination 4 bytes y origin 4 bytes
            _SUB44(*destination4bytes, *origin4bytes);
            break;
        }

        break;
    }

    free(destination1bytes);
    free(destination4bytes);
    free(origin1bytes);
    free(origin4bytes);
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
    uint8_t** reg1bytes = malloc(sizeof(uint8_t**));
    uint32_t** reg4bytes = malloc(sizeof(uint32_t**));


    switch (_typeToRegister(reg, reg1bytes, reg4bytes))
    {
    case REGISTER_1_BYTE:
        _JNZ1(*reg1bytes, instruction);
        break;

    case REGISTER_4_BYTES:
        _JNZ4(*reg4bytes, instruction);
        break;
    }

    free(reg1bytes);
    free(reg4bytes);
}

void _JNZ1(uint8_t* reg, uint32_t instruction)
{
    if (*reg != 0)
    {
        PC = instruction;
        return;
    }
}

void _JNZ4(uint32_t* reg, uint32_t instruction)
{
    if (*reg != 0)
    {
        PC = instruction;
        return;
    }
}



///////////////////////// INSTRUCCION RESIZE /////////////////////////


void RESIZE(uint32_t size)
{

}



///////////////////////// INSTRUCCION COPY_STRING /////////////////////////


void COPY_STRING(uint32_t size)
{

}



///////////////////////// INSTRUCCION WAIT /////////////////////////


void WAIT(char* resource)
{

}



///////////////////////// INSTRUCCION SIGNAL /////////////////////////


void SIGNAL(char* resource)
{

}



///////////////////////// INSTRUCCION IO_GEN_SLEEP /////////////////////////


void IO_GEN_SLEEP(char* interface, uint32_t workUnits)
{

}



///////////////////////// INSTRUCCION IO_STDIN_READ /////////////////////////


void IO_STDIN_READ(char* resource, registerType direction, registerType size)
{

}



///////////////////////// INSTRUCCION IO_STDOUT_WRITE /////////////////////////


void IO_STDOUT_WRITE(char* resource, registerType direction, registerType size)
{

}



///////////////////////// INSTRUCCION IO_FS_CREATE /////////////////////////


void IO_FS_CREATE(char* resource, char* fileName)
{

}



///////////////////////// INSTRUCCION IO_FS_DELETE /////////////////////////


void IO_FS_DELETE(char* resource, char* fileName)
{

}



///////////////////////// INSTRUCCION IO_FS_TRUNCATE /////////////////////////


void IO_FS_TRUNCATE(char* resource, char* fileName, registerType size)
{

}



///////////////////////// INSTRUCCION IO_FS_WRITE /////////////////////////


void IO_FS_WRITE(char* resource, char* fileName, registerType direction, registerType size, registerType pointer)
{

}



///////////////////////// INSTRUCCION IO_FS_READ /////////////////////////


void IO_FS_READ(char* resource, char* fileName, registerType direction, registerType size, registerType pointer)
{

}



///////////////////////// INSTRUCCION EXIT /////////////////////////


void EXIT()
{
    sendContextToKernel(CPU_SEND_CONTEXT_FOR_EXIT, getCurrentPID());
}









///////////////////////// FUNCIONES AUXILIARES GENERALES /////////////////////////

registerTypeByBytes _typeToRegister(registerType type, uint8_t** outRegister1byte, uint32_t** outRegister4bytes)
{
    registerTypeByBytes typeByBytes;
    *outRegister1byte = NULL;
    *outRegister4bytes = NULL;

    switch (type)
    {
    case PC_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &PC;

    case AX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        *outRegister1byte = &AX;
        break;

    case BX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        *outRegister1byte = &BX;
        break;

    case CX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        *outRegister1byte = &CX;
        break;

    case DX_TYPE:
        typeByBytes = REGISTER_1_BYTE;
        *outRegister1byte = &DX;
        break;

    case EAX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &EAX;
        break;

    case EBX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &EBX;
        break;

    case ECX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &ECX;
        break;

    case EDX_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &EDX;
        break;

    case SI_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &SI;
        break;

    case DI_TYPE:
        typeByBytes = REGISTER_4_BYTES;
        *outRegister4bytes = &DI;
        break;
    }

    return typeByBytes;
}



void incrementPC()
{
    PC++;
}

uint32_t getPC() { return PC; }

uint8_t getAX() { return AX; }

uint8_t getBX() { return BX; }

uint8_t getCX() { return CX; }

uint8_t getDX() { return DX; }

uint32_t getEAX() { return EAX; }

uint32_t getEBX() { return EBX; }

uint32_t getECX() { return ECX; }

uint32_t getEDX() { return EDX; }

uint32_t getSI() { return SI; }

uint32_t getDI() { return DI; }



void setPC(uint32_t pc) { PC = pc; }
void setAX(uint8_t ax) { AX = ax; }
void setBX(uint8_t bx) { BX = bx; }
void setCX(uint8_t cx) { CX = cx; }
void setDX(uint8_t dx) { DX = dx; }
void setEAX(uint32_t eax) { EAX = eax; }
void setEBX(uint32_t ebx) { EBX = ebx; }
void setECX(uint32_t ecx) { ECX = ecx; }
void setEDX(uint32_t edx) { EDX = edx; }
void setDI(uint32_t di) { DI = di; }
void setSI(uint32_t si) { SI = si; }




void* dataReceivedFromMemory;

void readFromMemory(void* data, uint32_t direction, int size, readWriteMemoryType type)
{
    physicalAddressInfo* info;
    int amountOfPhysicalAddresses = getAllPhysicalAddresses(getCurrentPID(), direction, size, info);

    int offset = 0;
    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        sendReadMemory(getCurrentPID(), info[i].physicalAddress, info[i].size);
        sem_wait(&semWaitDataFromMemory);
        memcpy(data + offset, dataReceivedFromMemory, info[i].size);
        offset += info[i].size;
        free(dataReceivedFromMemory);
    }


    ////// Solo para logs este switch
    switch (type)
    {
    case MEMORY_UINT8_TYPE:
        logReadMemoryUint(getCurrentPID(), info[0].physicalAddress, *((uint8_t*)data));
        break;

    case MEMORY_UINT32_TYPE:
        logReadMemoryUint(getCurrentPID(), info[0].physicalAddress, *((uint32_t*)data));
        break;

    case MEMORY_STRING_TYPE:
        logReadMemoryString(getCurrentPID(), info[0].physicalAddress, (char*)data);
        break;
    }


    free(info);
}


void writeToMemory(void* data, uint32_t direction, int size, readWriteMemoryType type)
{
    physicalAddressInfo* info;
    int amountOfPhysicalAddresses = getAllPhysicalAddresses(getCurrentPID(), direction, size, info);

    int offset = 0;
    for (int i = 0; i < amountOfPhysicalAddresses; i++)
    {
        sendWriteMemory(getCurrentPID(), data + offset, info[i].physicalAddress, info[i].size);
        offset += info[i].size;
        sem_wait(&semWaitConfirmationFromMemory);
    }



    ////// Solo para logs este switch
    switch (type)
    {
    case MEMORY_UINT8_TYPE:
        logWriteMemoryUint(getCurrentPID(), info[0].physicalAddress, *((uint8_t*)data));
        break;

    case MEMORY_UINT32_TYPE:
        logWriteMemoryUint(getCurrentPID(), info[0].physicalAddress, *((uint32_t*)data));
        break;

    case MEMORY_STRING_TYPE:
        logWriteMemoryString(getCurrentPID(), info[0].physicalAddress, (char*)data);
        break;
    }


    free(info);
}