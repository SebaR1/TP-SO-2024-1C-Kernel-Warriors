#ifndef UTILS_CONFIG_H_
#define UTILS_CONFIG_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>


t_list* getListOfStringsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage);
t_list* _fromConfigToListOfStrings(char** listChars);

t_list* getListOfIntsFromConfig(t_config* config, char* key, t_log* logger, const char* errorMessage);
t_list* _fromConfigToListOfInts(char** listChars);





#endif
