#include "datatypes.h"
#include "StringUtils.h"

// GLOBAL CONSTANTS

// function prototypes
OpCodeType *clearMetaDataList(OpCodeType *localPtr);
void displayMetaData(OpCodeType *localPtr);
Boolean getMetaData(char *fileName, OpCodeType *opCodeDataHead, char *endStateMsg);
int getOpCommand(FILE *filePtr, OpCodeType *inData);
int updateStartCount(int count, char *opString);
int updateEndCount(int count, char *opString);
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode);
int getCommand(char *cmd, char *inputStr, int index);
Boolean verifyValidCommand(char *testCmd);
int getStringArg(char *strArg, char *inputStr, int index);
Boolean verifyFirstStringArg(char *strArg);
int getNumberArg(int *number, char *inputStr, int index);
Boolean isDigit(char testChar);
