#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <pthread.h>
#include "datatypes.h"
#include "StringUtils.h"
#include "metadataops.h"
#include "configops.h"

typedef enum {
    NEW,
    READY,
    RUNNING,
    EXIT
} ProcessState;

typedef struct ProcessControlBlock {
    int id;
    ProcessState state;
    struct ProcessControlBlock *next;
} ProcessControlBlock;

void runSim(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr);

#endif
