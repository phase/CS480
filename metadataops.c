#include "metadataops.h"

const int BAD_ARG_VAL = -1;

/*
 * Function Name: clearMetaDataList
 * Algorithm: recursively iterates through op code linked list, returns memory to OS from the bottom of the list upward
 * Precondition: linked list, with or without data
 * Postcondition: all node memory, if any, is returned to OS, return pointer (head) is set to null
 * Exceptions: none
 * Notes: none
 */
OpCodeType *clearMetaDataList(OpCodeType *localPtr) {
    // check for local pointer not set to null (list not empty)
    if (localPtr != NULL) {
        // call recursive function with next pointer
        clearMetaDataList(localPtr->nextNode);
        // after recursive call, release memory to OS
        free(localPtr);
        // set local pointer to null
        localPtr = NULL;
    }
    // return null to calling function
    return NULL;
}

/*
 * Function Name: displayMetaData
 * Algorithm: iterates through op code linked list, displays op code data individually
 * Precondition: linked list, with or without data (should not be called if no data)
 * Postcondition: displays all op codes in list
 * Exceptions: none
 * Notes: none
 */
void displayMetaData(OpCodeType *localPtr) {
    // display title, with underline
    printf("Meta-Data File Display\n");
    printf("----------------------\n");
    // loop to end of linked list
    while (localPtr != NULL) {
        // print leader
        printf("Op Code: ");
        // print op code pid
        printf("/pid: %d", localPtr->pid);
        // print op code command
        printf("/cmd: %s", localPtr->command);
        // check for dev op command
        if (compareString(localPtr->command, "dev") == STR_EQ) {
            // print i/o parameter
            printf("/io: %s", localPtr->inOutArg);
        }
        // otherwise assume other than dev
        else {
            // print NA
            printf("/io: NA");
        }
        // print first string argument
        printf("\n\t /arg 1: %s", localPtr->strArg1);
        // print first int argument
        printf("/arg 2: %d", localPtr->intArg2);
        // print second int argument
        printf("/arg 3: %d", localPtr->intArg3);
        // print op end time
        printf("/op end time: %8.6f", localPtr->opEndTime);
        // end line
        printf("\n\n");
        // assign local pointer to next node
        localPtr = localPtr->nextNode;
    // end loop across nodes
    }
}

/*
 * Function Name: getMetaData
 * Algorithm: acquires the metadata items from file, stores them in a linked list
 * Precondition: provided file name
 * Postocndition: returns success (Boolean), returns linked list head via parameter, returns
 *      endstate/error message via parameter
 * Exceptions: function halted and error message returned if bad input data
 * Notes: none
 */
Boolean getMetaData(char *fileName, OpCodeType **opCodeDataHead, char *endStateMsg) {
    // initialize variables
    // init read only constant
    const char READ_ONLY_FLAG[] = "r";
    int accessResult, startCount = 0, endCount = 0;
    char dataBuffer[MAX_STR_LEN];
    Boolean ignoreLeadingWhiteSpace = True;
    Boolean stopAtNonPrintable = True;
    Boolean returnState = True;
    OpCodeType *newNodePtr;
    OpCodeType *localHeadPtr = NULL;
    FILE *fileAccessPtr;
    // init op code data pointer in case of return error
    *opCodeDataHead = NULL;
    // init end state message
    copyString(endStateMsg, "Metadata file upload successful");
    // open file for reading
    fileAccessPtr = fopen(fileName, READ_ONLY_FLAG);
    //check for file open failure
    if (fileAccessPtr == NULL) {
        // set end state message
        copyString(endStateMsg, "Metadata file access error");
        // return file access error
        return False;
    }
    // check first line for correct leader
    if (getLineTo(fileAccessPtr, MAX_STR_LEN, COLON, dataBuffer, ignoreLeadingWhiteSpace, stopAtNonPrintable) != NO_ERR
        || compareString(dataBuffer, "Start Program Meta-Data Code") != STR_EQ) {
        // close file
        fclose(fileAccessPtr);
        // set end state message
        copyString(endStateMsg, "Corrupt metadata leader line error");
        // return corrupt descriptor error
        return False;
    }
    // allocate memory for the temp data structure
    newNodePtr = (OpCodeType *) malloc(sizeof(OpCodeType));
    // get first op command
    accessResult = getOpCommand(fileAccessPtr, newNodePtr);
    // check for failure of first complete op command
    if (accessResult != COMPLETE_OPCMD_FOUND_MSG) {
        // close file
        fclose(fileAccessPtr);
        // clear data from the structure list
        *opCodeDataHead = clearMetaDataList(localHeadPtr);
        // free temp structure memory
        free(newNodePtr);
        // set end state message
        copyString(endStateMsg, "Metadata incomplete first op command found");
        // return result of operation
        return False;
    }
    // get start and end counts for later comparison
    startCount = updateStartCount(startCount, newNodePtr->strArg1);
    endCount = updateEndCount(endCount, newNodePtr->strArg1);
    // loop across all remaining op commands (while complete op commands are found)
    while (accessResult == COMPLETE_OPCMD_FOUND_MSG) {
        // add the new op command to the linked list
        localHeadPtr = addNode(localHeadPtr, newNodePtr);
        // get a new op command
        accessResult = getOpCommand(fileAccessPtr, newNodePtr);
        // update start and end counts for later comparison
        startCount = updateStartCount(startCount, newNodePtr->strArg1);
        endCount = updateEndCount(endCount, newNodePtr->strArg1);
    // end loop across remaining op commands
    }
    // after loop completion, check for last op command found
    if (accessResult == LAST_OPCMD_FOUND_MSG) {
        // check for start and end op code counts equal
        if (startCount == endCount) {
            // add the last node to the linked list
            localHeadPtr = addNode(localHeadPtr, newNodePtr);
            // set access result to no error for later operation
            accessResult = NO_ERR;
            // check last line for incorrect end descriptor
            if (getLineTo(fileAccessPtr, MAX_STR_LEN, PERIOD, dataBuffer, ignoreLeadingWhiteSpace, stopAtNonPrintable) != NO_ERR
                || compareString(dataBuffer, "End Program Meta-Data Code") != STR_EQ) {
                // set access result to corrupted descriptor error
                accessResult = MD_CORRUPT_DESCRIPTOR_ERR;
                // set end state message
                copyString(endStateMsg, "Metadata corrupted descriptor error");
            }
        }
    }
    // otherwise, assume didn't find end
    else {
        // set end state message
        copyString(endStateMsg, "Corrupted metadata op code");
        // unset return state
        returnState = False;
    }
    // check for any errors found (not no error)
    if (accessResult != NO_ERR) {
        // clear the op command list
        localHeadPtr = clearMetaDataList(localHeadPtr);
    }
    // close access file
    fclose(fileAccessPtr);
    // release temp struct memory
    free(newNodePtr);
    // assign temp local head pointer to parameter return return pointer
    *opCodeDataHead = localHeadPtr;
    // return access result
    return returnState;
}

/*
 * Function Name: getOpCommand
 * Algorithm: acquires one op command, verifies all parts of it, returns as a parameter
 * Precondition: file is open and file cursor is at beginning of an op code
 * Postcondition: in correct operations, finds, tests and returns op command as parameter, and returns status as integer
 *      - either complete op command found, or last op command found
 * Exceptions: correctly and appropriately (without program failure) responds to and reports file access failure,
 *      incorrectly formatted op command letter, incorrectly formatted op command name, incorrect or out of range
 *      op command value
 * Notes: none
 */
int getOpCommand(FILE *filePtr, OpCodeType *inData) {
    // init variables
        // init local constants
        const int MAX_CMD_LENGTH = 5;
        const int MAX_ARG_STR_LENGTH = 15;
        // init other variables
        int accessResult, numBuffer = 0;
        char strBuffer[STD_STR_LEN];
        char cmdBuffer[MAX_CMD_LENGTH];
        char argStrBuffer[MAX_ARG_STR_LENGTH];
        int runningStringIndex = 0;
        Boolean stopAtNonPrintable = True;
        Boolean arg2FailureFlag = False;
        Boolean arg3FailureFlag = False;
    // get whole op command as a string
    accessResult = getLineTo(filePtr, STD_STR_LEN, SEMICOLON, strBuffer, IGNORE_LEADING_WS, stopAtNonPrintable);
    // check for successful access
    if (accessResult == NO_ERR) {
        // get three-letter command
        runningStringIndex = getCommand(cmdBuffer, strBuffer, runningStringIndex);
        // assign op command to node
        copyString(inData->command, cmdBuffer);
    }
    // otherwise, assume unsuccessful access
    else {
        // set pointer to data structure to null
        inData = NULL;
        // return op command access failure
        return OPCMD_ACCESS_ERR;
    }
    // verify op command
    if (verifyValidCommand(cmdBuffer) == False) {
        // return op command error
        return CORRUPT_OPCMD_ERR;
    }
    // set all struct values that may not be initialized to defaults
    inData->pid = 0;
    inData->inOutArg[0] = NULL_CHAR;
    inData->intArg2 = 0;
    inData->intArg3 = 0;
    inData->opEndTime = 0.0;
    inData->nextNode = NULL;
    // check for device command
    if (compareString(cmdBuffer, "dev") == STR_EQ) {
        // get in/out argument
        runningStringIndex = getStringArg(argStrBuffer, strBuffer, runningStringIndex);
        // set device in/out argument
        copyString(inData->inOutArg, argStrBuffer);
        // check correct argument
        if (compareString(argStrBuffer, "in") != STR_EQ && compareString(argStrBuffer, "out") != STR_EQ) {
            // return argument error
            return CORRUPT_OPCMD_ARG_ERR;
        }
    }
    // get first string arg
    runningStringIndex = getStringArg(argStrBuffer, strBuffer, runningStringIndex);
    // set device in/out argument
    copyString(inData->strArg1, argStrBuffer);
    // check for legitimate first string arg
    if (verifyFirstStringArg(argStrBuffer) == False) {
        // return argument error
        return CORRUPT_OPCMD_ARG_ERR;
    }
    // check for last op command found
    if (compareString(inData->command, "sys") == STR_EQ && compareString(inData->strArg1, "end") == STR_EQ) {
        // return last op command found
        return LAST_OPCMD_FOUND_MSG;
    }
    // check for app start seconds argument
    if (compareString(inData->command, "sys") == STR_EQ && compareString(inData->strArg1, "end") == STR_EQ) {
        // get number arg
        runningStringIndex = getNumberArg(&numBuffer, strBuffer, runningStringIndex);
        // check for failed number access
        if (numBuffer <= BAD_ARG_VAL) {
            // set failure flag
            arg2FailureFlag = True;
        }
        // set first in arg to number
        inData->intArg2 = numBuffer;
    }
    // check for cpu cycle time
    if (compareString(inData->command, "cpu") == STR_EQ) {
        // get number argument
        runningStringIndex = getNumberArg(&numBuffer, strBuffer, runningStringIndex);
        // check for failed number access
        if (numBuffer <= BAD_ARG_VAL) {
            // set failure flag
            arg2FailureFlag = True;
        }
        // set first int argument to number
        inData->intArg2 = numBuffer;
    }
    // check for device cycle time
    if (compareString(inData->command, "dev") == STR_EQ) {
        // get number argument
        runningStringIndex = getNumberArg(&numBuffer, strBuffer, runningStringIndex);
        // check for failed number access
        if (numBuffer <= BAD_ARG_VAL) {
            // set failure flag
            arg2FailureFlag = True;
        }
        // set first int argument to number
        inData->intArg2 = numBuffer;
    }
    // check for memory base and offset
    if (compareString(inData->command, "mem") == STR_EQ) {
        // get number argument for base
        runningStringIndex = getNumberArg(&numBuffer, strBuffer, runningStringIndex);
        // check for failed number access
        if (numBuffer <= BAD_ARG_VAL) {
            // set failure flag
            arg2FailureFlag = True;
        }
        // set first int argument to number
        inData->intArg2 = numBuffer;
        // get number argument for offset
        runningStringIndex = getNumberArg(&numBuffer, strBuffer, runningStringIndex);
        // check for failed number access
        if (numBuffer <= BAD_ARG_VAL) {
            // set failure flag
            arg3FailureFlag = True;
        }
        // set second int argument to number
        inData->intArg3 = numBuffer;
    }
    // check int args for upload failure
    if (arg2FailureFlag == True || arg3FailureFlag == True) {
        // return argument error
        return CORRUPT_OPCMD_ARG_ERR;
    }
    // return complete op command found message
    return COMPLETE_OPCMD_FOUND_MSG;
}

/*
 * Function Name: updateStartCount
 * Algorithm: updates number of "start" op commands found in file
 * Precondition: count >= 0, op string has "start" or other op name
 * Postcondition: if op string has "start", input count + 1 is returned; otherwise, input count is returned unchanged
 * Exceptions: none
 * Notes: none
 */
int updateStartCount(int count, char *opString) {
    // check for "start" in op string
    if (compareString(opString, "start") == STR_EQ) {
        // return incremented start count
        return count + 1;
    }
    // return unchanged start count
    return count;
}

/*
 * Function Name: updateEndCount
 * Algorithm: updates nunber of "end" op commands found in file
 * Precondition: count >= 0, op string has "end" or other op name
 * Postcondition: if op string has "end", input count + 1 is returned; otherwise, input count is returned unchanged
 * Exceptions: none
 * Notes: none
 */
int updateEndCount(int count, char *opString) {
    // check for "end" in op string
    if (compareString(opString, "end") == STR_EQ) {
        // return incremented end count
        return count + 1;
    }
    // return unchanged end count
    return count;
}

/*
 * Function Name: addNode
 * Algorithm: adds op command structure with data to a linked list
 * Precondition: linked list pointer assigned to null or to one op command link, struct pointer assigned to op command
 *      struct data
 * Postcondition: assigns new structure node to beginning of linked list or and of established linked list
 * Exceptions: none
 * Notes: assumes memory access/availability
 */
OpCodeType *addNode(OpCodeType *localPtr, OpCodeType *newNode) {
    // check for local pointer to null
    if (localPtr == NULL) {
        // access memory for new link/node
        localPtr = (OpCodeType *) malloc(sizeof(OpCodeType));
        // assign all three values to newly created node
        // assign next pointer to null
        localPtr->pid = newNode->pid;
        copyString(localPtr->command, newNode->command);
        copyString(localPtr->inOutArg, newNode->inOutArg);
        copyString(localPtr->strArg1, newNode->strArg1);
        localPtr->intArg2 = newNode->intArg2;
        localPtr->intArg3 = newNode->intArg3;
        localPtr->opEndTime = newNode->opEndTime;

        localPtr->nextNode = NULL;
        // return current local pointer
        return localPtr;
    }
    // assume end of list not found yet
    localPtr->nextNode = addNode(localPtr->nextNode, newNode);
    // assign recursive function to current's next link
    return localPtr;
}

/*
 * Function Name: getCommand
 * Algorithm: gets first three letters of input string to get command
 * Precondition: provided starting index
 * Postcondition: returns command (ref param) and updated index (return)
 * Exceptions: none
 * Notes: none
 */
int getCommand(char *cmd, char *inputStr, int index) {
    // init variables
    int lengthOfCommand = 3;
    // loop across command length
    while(index < lengthOfCommand) {
        // assign character from input string to buffer string
        cmd[index] = inputStr[index];
        // increment index
        index++;
        // set next character to null character
        cmd[index] = NULL_CHAR;
    }
    // return current index
    return index;
}

/*
 * Function Name: verifyValidCommand
 * Algorithm: check string argument for one of the allowed commands
 * Precondition: input string is provided
 * Postcondition: in correct operation, reports if given string is a command, False if not
 * Exceptions: none
 * Notes: none
 */
Boolean verifyValidCommand(char *testCmd) {
    // check for five string command arguments
    if (compareString(testCmd, "sys") == STR_EQ
        || compareString(testCmd, "app") == STR_EQ
        || compareString(testCmd, "cpu") == STR_EQ
        || compareString(testCmd, "mem") == STR_EQ
        || compareString(testCmd, "dev") == STR_EQ) {
        // return True
        return True;
    }
    // return False
    return False;
}

/*
 * Function Name: getStringArg
 * Algorithm: skips leading white space, acquires sub string from op command input string ending at command or end of string
 * Precondition: input string has some remaining string argument
 * Postcondition: in correct operationg, captures next string argument
 * Exceptions: none
 * Notes nonel
 */
int getStringArg(char *strArg, char *inputStr, int index) {
    // init variables
    int localIndex = 0;
    // loop to skip white space and comma
    while (inputStr[index] <= SPACE || inputStr[index] == COMMA) {
        // increment index
        index++;
    }
    // loop across string length
    while(inputStr[index] != COMMA && inputStr[index] != NULL_CHAR) {
        // assign character from input string to buffer string
        strArg[localIndex] = inputStr[index];
        // increment index
        index++;
        localIndex++;
        // set next character to null character
        strArg[localIndex] = NULL_CHAR;
    }
    // end loop across string length
    // return current index
    return index;
}

/*
 * Function Name: verifyFirstStringArg
 * Algorithm: check string argument for one of the allowed string arguments
 * Precondition: input string is provided
 * Postcondition: in correct operation, reports True if given string is in argument list, False if not
 * Exceptions: none
 * Notes: none
 */
Boolean verifyFirstStringArg(char *strArg) {
    // check for all possible string arg 1 possibilities
    if (compareString(strArg, "access") == STR_EQ
        || compareString(strArg, "allocate") == STR_EQ
        || compareString(strArg, "end") == STR_EQ
        || compareString(strArg, "ethernet") == STR_EQ
        || compareString(strArg, "hard drive") == STR_EQ
        || compareString(strArg, "keyboard") == STR_EQ
        || compareString(strArg, "monitor") == STR_EQ
        || compareString(strArg, "printer") == STR_EQ
        || compareString(strArg, "process") == STR_EQ
        || compareString(strArg, "serial") == STR_EQ
        || compareString(strArg, "sound signal") == STR_EQ
        || compareString(strArg, "start") == STR_EQ
        || compareString(strArg, "usb") == STR_EQ
        || compareString(strArg, "video signal") == STR_EQ) {
        // return True
        return True;
    }
    // return False
    return False;
}

/*
 * Function Name: getNumberArg
 * Algorithm: skips leading white space, acquires next integer from op command input string ending at command or end
 *      of string
 * Precondition: input string has some remaining string argument
 * Postcondition: in correct operation, captures next integer argument, returns index location after process finished
 * Exceptions: none
 * Notes: none
 */
int getNumberArg(int *number, char *inputStr, int index) {
    // init variables
    Boolean foundDigit = False;
    *number = 0;
    int multiplier = 10;
    // loop to skip white space
    while (inputStr[index] <= SPACE || inputStr[index] == COMMA) {
        // increment index
        index++;
    }
    // loop across string length
    while(isDigit(inputStr[index]) == True && inputStr[index] != NULL_CHAR) {
        // set digit found flag
        foundDigit = True;
        // assign digit to output
        (*number) = ((*number) * multiplier) + (inputStr[index] - '0');
        // increment index
        index++;
    }
    // end loop across string length

    // check for digit not found
    if (foundDigit == False) {
        // set number to BAD_ARG_VAL constant
        *number = BAD_ARG_VAL;
    }
    // return current index
    return index;
}

/*
 * Function Name: isDigit
 * Algorithm: checks for character digit, returns result
 * Precondition: test value is character
 * Postcondition: if test value is a value '0' < value '9', returns true, otherwise returns false
 * Exceptions: none
 * Notes: none
 */
Boolean isDigit(char testChar) {
    // check for test character between characters '0' - '9'
    if (testChar >= '0' && testChar <= '9') {
        // return true
        return True;
    }
    // otherwise, assume character is not digit, return false
    return False;
}
