#include "interfaces.h"

t_interfaceData interfaceData;
t_resultsForStdin resultsForStdin;
t_resultsForStdout resultsForStdout;
t_resultsForIOFSWrite resultsForIOFSWrite;
t_resultsForIOFSRead resultsForIOFSRead;

sem_t semaphoreForStdin;
sem_t semaphoreForStdout;
sem_t semaphoreForIOFSWrite;
sem_t semaphoreForIOFSRead;
sem_t semaphoreForModule;

FILE* blocks = NULL;
FILE* bitmap = NULL;

t_fileData* blocksData = NULL;
t_fileData* bitmapData = NULL;

t_bitarray* mappedBitmap = NULL;

t_list* listFileNames = NULL;

int socketKernel;
int socketMemory;

void createInterface(char *name)
{   
    interfaceData.name = malloc(string_length(name) + 1);
    strcpy(interfaceData.name, name);

    if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "GENERICA"))
        interfaceData.type = Generic;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDIN"))
        interfaceData.type = STDIN;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "STDOUT"))
        interfaceData.type = STDOUT;
    else if (string_equals_ignore_case(getIOConfig()->TIPO_INTERFAZ, "DIALFS"))
        interfaceData.type = DialFS;

    interfaceData.workUnits = getIOConfig()->TIEMPO_UNIDAD_TRABAJO;

    interfaceData.currentOperation.pid = -1;
    interfaceData.currentOperation.operation = IO_NULL;
    switch (interfaceData.type)
    {
        case Generic:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForGenericInterface));
            log_info(getLogger(), "Creada interfaz de tipo Generica, con nombre \"%s\"", interfaceData.name);
            break;

        case STDIN:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdinInterface));
            resultsForStdin.resultsFromRead = NULL;
            resultsForStdin.resultsForMemory = NULL;
            log_info(getLogger(), "Creada interfaz de tipo STDIN, con nombre \"%s\"", interfaceData.name);
            break;

        case STDOUT:
            interfaceData.currentOperation.params = malloc(sizeof(t_paramsForStdoutInterface));
            resultsForStdout.resultsForWrite = NULL;
            log_info(getLogger(), "Creada interfaz de tipo STDOUT, con nombre \"%s\"", interfaceData.name);
            break;

        case DialFS:
            const char *dir = getIOConfig()->PATH_BASE_DIALFS;
            mkdir(dir, 0777);
            blocksData = openCreateMapFile(blocks, "bloques.dat", getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE);
            bitmapData = openCreateMapFile(bitmap, "bitmap.dat", getIOConfig()->BLOCK_COUNT / 8);
            mappedBitmap = bitarray_create_with_mode(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, LSB_FIRST);
            log_info(getLogger(), "Creada interfaz de tipo DialFS, con nombre \"%s\"", interfaceData.name);
            createListFileNames();

            break;

        default:
        break;
    }
}

void destroyInterface()
{
    free(interfaceData.name);
    free(interfaceData.currentOperation.params);
}

t_fileData* openCreateMapFile(FILE* file, char* fileName, int fileSize)
{   
    char *fullName = string_new();
    string_append(&fullName, getIOConfig()->PATH_BASE_DIALFS);
    string_append(&fullName, "/");
    string_append(&fullName, fileName);
    file = fopen(fullName, "a+");

    int fd = fileno(file);

    ftruncate(fd, fileSize);

    char *mappedFile;
    mappedFile = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    msync(mappedFile, fileSize, MS_SYNC);

    t_fileData* fileData = malloc(sizeof(t_fileData));
    fileData->fd = fd;
    fileData->mappedFile = mappedFile;

    free(fullName);
    return fileData;
}

void createListFileNames()
{
        listFileNames = list_create();
        DIR* dir;
        struct dirent* entry;

        dir = opendir(getIOConfig()->PATH_BASE_DIALFS);

        while ((entry = readdir(dir)) != NULL)
        {   
            char* name = string_new();
            string_append(&name, entry->d_name);
            if (entry->d_type == DT_REG && string_equals_ignore_case(name + string_length(name) - 7, ".config"))
                list_add(listFileNames, name);
        }

        closedir(dir);
}

void closeBlocksFile()
{   
    if (blocks != NULL)
    {
        msync(blocksData->mappedFile, getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE, MS_SYNC);
        munmap(blocksData->mappedFile, getIOConfig()->BLOCK_COUNT * getIOConfig()->BLOCK_SIZE);
        fclose(blocks);
    }
}

void closeBitmapFile()
{   
    if (bitmap != NULL)
    {   
        msync(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8, MS_SYNC);
        munmap(bitmapData->mappedFile, getIOConfig()->BLOCK_COUNT / 8);
        fclose(bitmap);
        bitarray_destroy(mappedBitmap);
    }
}