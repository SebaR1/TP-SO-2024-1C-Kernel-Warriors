#ifndef IO_CLIENT_H
#define IO_CLIENT_H

#include "serverIO.h"
#include "utils/client/utils.h"

void sendInterfaceToKernel();

void sendIOGenSleepResultsToKernel();

void sendIOStdinReadResultsToKernel();

void sendResultsFromStdinToMemory();

void sendIOStdoutWriteResultsToKernel();

void sendIOReadRequestToMemory();

#endif