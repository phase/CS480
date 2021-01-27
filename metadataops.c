#include "metadataops.h"

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
    while(localPtr != NULL) {
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
Boolean getMetaData(char *fileName, OpCodeType *opCodeDataHead, char *endStateMsg) {
    // initialize variables
        // init read only constant
    // init op code data pointer in case of return error
    // init end state message
    // open file for reading
    //check for file open failure
        // set end state message
        // return file access error
    // check first line for correct leader
        // close file
        // set end state message
        // return corrupt descriptor error
    // allocate memory for the temp data structure
    // get first op command
    // get start and end counts for later comparison
    // check for failure of first complete op command
        // close file
        // clear data from the structure list
        // free temp structure memory
        // set end state message
        // return result of operation
    // loop across all remaining op commands (while complete op commands are found)
        // add the new op command to the linked list
        // update start and end counts for later comparison
    // end loop across remaining op commands
    // after loop completion, check for last op command found
        // check for start and end op code counts equal
            // add the last node to the linked list
            // set access result to no error for later opation
            // check last line for incorrect end descriptor
                // set access result to corrupted descriptor error
                // set end state message
    // otherwise, assume didn't find end
        // set end state message
        // unset return state
    // check for any errors found (not no error)
        //clear the op command list
    // close access file
    // release temp struct memory
    // assign temp local head pointer to parameter return return pointer
    // return access result
    return False;
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
        // init other variables
    // get whole op command as a string
    // check for successful access
        // get three-letter command
        // assign op command to node
    // otherwise, assume unsuccessful access
        // set pointer to data structure to null
        // return op command access failure
    // verify op command
        // return op command error
    // set all struct values that may not be initialized to defaults
    // check for device command
        // get in/out argument
        // set device in/out argument
        // check correct argument
            // return argument error
    // get first string arg
    // set device in/out argument
    // check for legitimate first string arg
        // return argument error
    // check for last op command found
        // return last op command found
    // check for app start seconds argument
        // get number arg
        // check for failed number access
            // set failure flag
        // set first in arg to number
    // check for cpu cycle time
        // get number argument
        // check for failed number access
            // set failure flag
        // set first int argument to number
    // check for device cycle time
        // get number argument
        // check for failed number access
            // set failure flag
        // set first int argument to number
    // check for memory base and offset
        // get number argument for base
        // check for failed number access
            // set failure flag
        // set first int argument to number
        // get number argument for offset
        // check for failed number access
            // set failure flag
        // set second int argument to number
    // check int args for upload failure
        // return argument error
    // return complete op command found message
    return 0;
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
        // return incremented start count
    // return unchanged start count
    return 0;
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
        // return incremented end count
    // return unchanged end count
    return 0;
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
        // return True
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
    // loop to skip white space and comma
        // increment index
    // loop across string length
        // assign character from input string to buffer string
        // increment index
        // set next character to null character
    // end loop across string length
    // return current index
    return 0;
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
        // return True
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
    // loop to skip white space
        // increment index
    // loop across string length
        // set digit found flag
        // assign digit to output
        // increment index and multiplier
    // end loop across string length
    // check for digit not found
        // set number to BAD_ARG_VAL constant
    // return current index
    return 0;
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
        // return true
    // otherwise, assume character is not digit, return false
    return False;
}
