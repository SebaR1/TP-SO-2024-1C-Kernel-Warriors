#include "dialfsInterface.h"

void executeIOFSCreateAndSendResults()
{
    int success = executeIOFSCreate();

    sendIOFSCreateResultsToKernel(success);

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

int executeIOFSCreate()
{
    int firstFreeBlock = lookForFirstFreeBlock();

    if (firstFreeBlock < getIOConfig()->BLOCK_COUNT)
    {
        bitarray_set_bit(mappedBitmap, firstFreeBlock);

        msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);

        t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
        
        char *nameForMetaDataFile = string_new();
        string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
        string_append(&nameForMetaDataFile, "/");
        string_append(&nameForMetaDataFile, params->fileName);
        string_append(&nameForMetaDataFile, ".config");
        FILE *metaData = fopen(nameForMetaDataFile, "a+");
        t_config *metaDataFile = config_create(nameForMetaDataFile);

        config_set_value(metaDataFile, "BLOQUE_INICIAL", string_itoa(firstFreeBlock));
        config_set_value(metaDataFile, "TAMANIO_ARCHIVO", string_itoa(0));

        config_save(metaDataFile);
        fclose(metaData);
        free(nameForMetaDataFile);

        return 1;
    }

    else return 0;
}

void executeIOFSDeleteAndSendResults()
{
    executeIOFSDelete();

    sendIOFSDeleteResultsToKernel();

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSDelete()
{   
    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
        
    char *nameForMetaDataFile = string_new();
    string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&nameForMetaDataFile, "/");
    string_append(&nameForMetaDataFile, params->fileName);
    string_append(&nameForMetaDataFile, ".config");
    t_config *metaDataFile = config_create(nameForMetaDataFile);

    int initialBlock = config_get_int_value(metaDataFile, "BLOQUE_INICIAL");
    int byteSize = config_get_int_value(metaDataFile, "TAMANIO_ARCHIVO");

    int blockAmount = ceil(byteSize / getIOConfig()->BLOCK_SIZE);

    for (int i = initialBlock; i < initialBlock + blockAmount; i++)
        bitarray_clean_bit(mappedBitmap, i);

    msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);

    remove(nameForMetaDataFile);

    free(nameForMetaDataFile);
}

void executeIOFSTruncateAndSendResults()
{
    executeIOFSTruncate();

    sendIOFSTruncateResultsToKernel();

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSTruncate()
{
    t_paramsForIOFSTruncate *params = (t_paramsForIOFSTruncate*)interfaceData.currentOperation.params;
        
    char *nameForMetaDataFile = string_new();
    string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&nameForMetaDataFile, "/");
    string_append(&nameForMetaDataFile, params->fileName);
    string_append(&nameForMetaDataFile, ".config");
    t_config *metaDataFile = config_create(nameForMetaDataFile);

    int initialBlock = config_get_int_value(metaDataFile, "BLOQUE_INICIAL");
    int byteSize = config_get_int_value(metaDataFile, "TAMANIO_ARCHIVO");

    int blockAmount = ceil(byteSize / getIOConfig()->BLOCK_SIZE);
    int newBlockAmount = ceil(params->registerSize / getIOConfig()->BLOCK_SIZE);

    if (params->registerSize < byteSize)
    {
        for (int i = initialBlock + newBlockAmount; i < initialBlock + blockAmount; i++)
            bitarray_clean_bit(mappedBitmap, i);
        
        msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);

        config_set_value(metaDataFile, "TAMANIO_ARCHIVO", string_itoa(params->registerSize));
    }
    else if (params->registerSize > byteSize)
    {
        int availableSpace = 1;

        for (int i = initialBlock + blockAmount; i < initialBlock + newBlockAmount; i++)
        {
            if (bitarray_test_bit(mappedBitmap, i))
            {
                availableSpace = 0;
                break;
            }
        }

        if (availableSpace)
        {
            for (int i = initialBlock + blockAmount; i < initialBlock + newBlockAmount; i++)
                bitarray_set_bit(mappedBitmap, i);
            
            config_set_value(metaDataFile, "TAMANIO_ARCHIVO", string_itoa(params->registerSize));
        }
        else
        {
            compactFSAndSendFileToLastPosition(params->fileName, &initialBlock, byteSize, blockAmount);

            for (int i = initialBlock + blockAmount; i < initialBlock + newBlockAmount; i++)
                bitarray_set_bit(mappedBitmap, i);
            
            config_set_value(metaDataFile, "TAMANIO_ARCHIVO", string_itoa(params->registerSize));
        }
    }

    usleep(1000 * getIOConfig()->RETRASO_COMPACTACION);
}

void executeIOFSWriteAndSendResults()
{
    executeIOFSWrite();

    sendIOFSWriteResultsToKernel();

    free(resultsForIOFSWrite.resultsForWrite);
    resultsForIOFSWrite.resultsForWrite = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSWrite()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_WRITE", (int)interfaceData.currentOperation.pid);
    sendIOReadRequestToMemory();
    
    //Se espera a recibir el contenido de la memoria
    sem_wait(&semaphoreForIOFSWrite);

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;
    char *nameForMetaDataFile = string_new();
    string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&nameForMetaDataFile, "/");
    string_append(&nameForMetaDataFile, params->fileName);
    string_append(&nameForMetaDataFile, ".config");
    t_config *metaDataFile = config_create(nameForMetaDataFile);
    int fileBeginning = config_get_int_value(metaDataFile, "BLOQUE_INICIAL") * getIOConfig()->BLOCK_SIZE;
    memcpy(blocksData->mappedFile + fileBeginning + params->registerFilePointer, resultsForIOFSWrite.resultsForWrite, params->registerSize);
    msync(blocksData->mappedFile, getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE, MS_SYNC);
}

void executeIOFSReadAndSendResults()
{
    executeIOFSRead();

    sendResultsFromIOFSReadToMemory();

    // Se espera a recibir confirmación de la memoria de que salió todo bien
    sem_wait(&semaphoreForIOFSRead);

    sendIOFSReadResultsToKernel();

    free(resultsForIOFSRead.resultsFromRead);
    resultsForIOFSRead.resultsFromRead = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOFSRead()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_FS_READ", (int)interfaceData.currentOperation.pid);

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;
    char *nameForMetaDataFile = string_new();
    string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&nameForMetaDataFile, "/");
    string_append(&nameForMetaDataFile, params->fileName);
    string_append(&nameForMetaDataFile, ".config");
    t_config *metaDataFile = config_create(nameForMetaDataFile);
    int fileBeginning = config_get_int_value(metaDataFile, "BLOQUE_INICIAL") * getIOConfig()->BLOCK_SIZE;
    memcpy(resultsForIOFSRead.resultsFromRead, blocksData->mappedFile + fileBeginning + params->registerFilePointer, params->registerSize);
}

int lookForFirstFreeBlock()
{
    int block = 0;

    while(block < getIOConfig()->BLOCK_COUNT && bitarray_test_bit(mappedBitmap, block) != 0)
        block++;

    return block;
}

void compactFSAndSendFileToLastPosition(char* name, int* initialBlock, int byteSize, int blockAmount)
{
    for (int i = 0; i < getIOConfig()->BLOCK_COUNT; i++)
        bitarray_clean_bit(mappedBitmap, i);
    msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);
    
    char* bufferBlocks = malloc(sizeof(char) * getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE);
    int bufferPointer = 0;
    int newInitialBlock = 0;
    int totalSize = 0;

    for (int i = 0; i < list_size(listFileNames); i++)
    {   
        char* currentName = list_get(listFileNames, i);
        if (!string_equals_ignore_case(currentName, name))
        {
            char *nameForMetaDataFile = string_new();
            string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
            string_append(&nameForMetaDataFile, "/");
            string_append(&nameForMetaDataFile, currentName);
            string_append(&nameForMetaDataFile, ".config");
            t_config *metaDataFile = config_create(nameForMetaDataFile);

            int initialBlockCurrent = config_get_int_value(metaDataFile, "BLOQUE_INICIAL");
            int byteSizeCurrent = config_get_int_value(metaDataFile, "TAMANIO_ARCHIVO");
            totalSize += byteSizeCurrent;
            int blockAmountCurrent = ceil(byteSizeCurrent / getIOConfig()->BLOCK_SIZE);


            memcpy(bufferBlocks + bufferPointer, blocksData->mappedFile + (initialBlockCurrent * getIOConfig()->BLOCK_SIZE), byteSizeCurrent);
            bufferPointer += byteSizeCurrent;

            for (int j = newInitialBlock; j < newInitialBlock + blockAmountCurrent; j++)
                bitarray_set_bit(mappedBitmap, j);
            newInitialBlock += blockAmountCurrent;
            msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);

            config_set_value(metaDataFile, "BLOQUE_INICIAL", string_itoa(newInitialBlock));

            config_destroy(metaDataFile);
        }
    }

    char *nameForMetaDataFile = string_new();
    string_append(&nameForMetaDataFile, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&nameForMetaDataFile, "/");
    string_append(&nameForMetaDataFile, name);
    string_append(&nameForMetaDataFile, ".config");
    t_config *metaDataFile = config_create(nameForMetaDataFile);

    totalSize += config_get_int_value(metaDataFile, "TAMANIO_ARCHIVO");

    memcpy(bufferBlocks + bufferPointer, blocksData->mappedFile + ((*initialBlock) * getIOConfig()->BLOCK_SIZE), byteSize);

    for (int j = newInitialBlock; j < newInitialBlock + blockAmount; j++)
        bitarray_set_bit(mappedBitmap, j);

    config_set_value(metaDataFile, "BLOQUE_INICIAL", string_itoa(newInitialBlock));

    (*initialBlock) = newInitialBlock;

    memcpy(blocksData->mappedFile, bufferBlocks, totalSize);
    msync(blocksData->mappedFile, getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE, MS_SYNC);
}