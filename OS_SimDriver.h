#ifndef OS_SIMDRIVER_H
#define OS_SIMDRIVER_H

#include "datatypes.h"
#include "StringUtils.h"
#include "metadataops.h"
#include "configops.h"

void showProgramFormat();
void runSim(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr);

#endif
