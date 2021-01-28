#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "datatypes.h"

// GLOBAL CONSTANTS
extern const int MAX_STR_LEN;
extern const int STD_STR_LEN;
extern const char NULL_CHAR;
extern const int SUBSTRING_NOT_FOUND;
extern const char SPACE;
extern const char COMMA;
extern const char COLON;
extern const char SEMICOLON;
extern const char PERIOD;
extern const int STR_EQ;
extern const Boolean IGNORE_LEADING_WS;
extern const Boolean ACCEPT_LEADING_WS;

typedef enum {
    NO_ERR,
    INCOMPLETE_FILE_ERR,
    INPUT_BUFFER_OVERRUN_ERR
} StringManipCode;

int getStringLength(char *testStr);
void copyString(char *destination, char *source);
int compareString(char *oneStr, char *otherStr);
void getSubString(char *destStr, char *sourceStr, int startIndex, int endIndex);
int findSubString(char *testStr, char *searchSubStr);
void setStrToLowerCase(char *destStr, char *sourceStr);
char setCharToLowerCase(char testChar);
void concatenateString(char *destination, char *source);
int getLineTo(FILE *filePtr, int bufferSize, char stopChar, char *buffer, Boolean omitLeadingWhiteSpace, Boolean stopAtNonPrintable);
Boolean isEndOfFile(FILE *filePtr);

#endif
