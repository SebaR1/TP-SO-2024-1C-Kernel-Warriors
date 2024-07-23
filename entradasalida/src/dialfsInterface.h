#ifndef DIALFS_INTERFACES_H
#define DIALFS_INTERFACES_H

#include "interfaces.h"

void executeIOFSCreateAndSendResults();

int executeIOFSCreate();

void executeIOFSDeleteAndSendResults();

void executeIOFSDelete();

void executeIOFSTruncateAndSendResults();

void executeIOFSTruncate();

void executeIOFSWriteAndSendResults();

void executeIOFSWrite();

void executeIOFSReadAndSendResults();

void executeIOFSRead();

int lookForFirstFreeBlock();

void compactFSAndSendFileToLastPosition(char* name, int* initialBlock, int byteSize, int blockAmount);

#endif