#include "StringUtils.h"

const int MAX_STR_LEN = 200;
const int STD_STR_LEN = 80;
const char NULL_CHAR = '\0';
const int SUBSTRING_NOT_FOUND = -102;
const char SPACE = ' ';
const char COMMA = ',';
const char COLON = ':';
const char SEMICOLON = ';';
const char PERIOD = '.';
const int STR_EQ = 0;
const Boolean IGNORE_LEADING_WS = True;
const Boolean ACCEPT_LEADING_WS = False;

/*
 * Function Name: getStringLength
 * Algorithm: find the length of the string, up to the null character
 * Precondition: given C-style with null character at end
 * Postcondition: return the number of characters (int) from the beginning to the null character
 * Exceptions: none
 * Note: limit test loop to maximum characters for safety
 */
int getStringLength(char *testStr) {
    // init variables
    int index = 0;

    // loop up to null character or limit
    while (index < MAX_STR_LEN && testStr[index] != NULL_CHAR) {
        // increment the counter index
        index++;
    }
    // end loop

    // return the counter index value
    return index;
}

/*
 * Function Name: copyString
 * Algorithm: copies contents of one string into another
 * Precondition: given C-style source string, having a null character at end of string;
 *      destination string is passed in as a parameter with enough memory to accept the source string
 * Poscondition: destination string contains an exact copy of the source string
 * Exceptions: none
 * Note: limit test loop to maximum characters for safety
 */
void copyString(char *destination, char *source) {
    // init variables
    int index = 0;
    // loop until null character is found in source string
    // loop limits to MAX_STR_LEN
    while (index < MAX_STR_LEN && source[index] != NULL_CHAR) {
        // assign source character to destination element
        destination[index] = source[index];
        // increment index
        index++;
        // assign null char to next dest element
        destination[index] = NULL_CHAR;
    }
    // end loop
}

/*
 * Function Name: compareString
 * Algorithm: compares two strings alphabetically such that:
 *      if oneStr < otherStr, the function returns a value < 0
 *      if oneStr > otherStr, the function returns a value > 0
 *      if oneStr == otherStr, the function returns a 0
 *      if two strings are identical up to the point that one is longer, the difference in lengths will be returned
 * Precondition: given two C-style strings, having a null character at the end of each string
 * Postcondition: integer value returned as specified
 * Exceptions: none
 * Note: limit test loop to maximum characters for safety
 */
int compareString(char *oneStr, char *otherStr) {
    // init variables
    int index = 0;
    int difference;
    // loop to end of one of the two strings
    // loop limit to MAX_STR_LEN
    while (index < MAX_STR_LEN && oneStr[index] != NULL_CHAR && otherStr[index] != NULL_CHAR) {
        // find the diff between the currently aligned characters
        difference = oneStr[index] - otherStr[index];
        // check for non-zero diff
        if (difference != 0) {
            // return non-zero diff
            return difference;
        }
        // increment index
        index++;
    }
    // end loop
    // assume strings are equal at this point, return string length diff
    return getStringLength(oneStr) - getStringLength(otherStr);
}

/*
 * Function Name: getSubString
 * Algorithm: captures sub string within larger string between two inclusive indices
 * Precondition: given two C-style strings, having a null character at the end of string,
 *      and another string parameter with enough memory to hold the resulting substring
 * Postcondition: substring is returned as a parameter
 * Exceptions: empty string returned if any of the index parameters are out of range
 * Note: copy of source string is made internally to protect from aliasing
 */
void getSubString(char *destStr, char *sourceStr, int startIndex, int endIndex) {
    // init variables
        // set length of source string
        int sourceStrLen = getStringLength(sourceStr);
        // init dest index to zero
        int destIndex = 0;
        // init source index to start index paramter
        int sourceIndex = startIndex;
        // create pointer to temp string
        char *tempSourceStr;
    // check for indices within limits
    if (startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen) {
        // create temp string, copy source string to it
        tempSourceStr = (char *) malloc(sourceStrLen + 1);
        copyString(tempSourceStr, sourceStr);
        // loop across requested substring (indices)
        while (sourceIndex <= endIndex) {
            // assign source character to destination element
            destStr[destIndex] = tempSourceStr[sourceIndex];
            // increment indices
            destIndex++;
            sourceIndex++;
            // add null character to next dest string element
            destStr[destIndex] = NULL_CHAR;
        }
        // end loop
        // release memory used for temp source string
        free(tempSourceStr);
    }
    // otherwise, assume indices not in limits
    else {
        // create empty string with null char
        destStr[0] = NULL_CHAR;
    }
}

/*
 * Function Name: findSubString
 * Algorithm: linear search for given substring within a given test string
 * Precondition: given a C-style test string, having a null character at end of string
 * Postcondition: index of substring location returned, or SUBSTRING_NOT_FOUND constant is returned
 * Exceptions: none
 * Note: none
 */
int findSubString(char *testStr, char *searchSubStr) {
    // init variables
        // init test string length
        int testStrLen = getStringLength(testStr);
        // init other vars
        int masterIndex = 0;
        int searchIndex, internalIndex;
    // loop across test string
    while (masterIndex < testStrLen) {
        // set internal loop index to current test string index
        internalIndex = masterIndex;
        // set internal search index to zero
        searchIndex = 0;
        // loop to end of test string
        // while test string/sub string chars are the same
        while (internalIndex <= testStrLen && testStr[internalIndex] == searchSubStr[searchIndex]) {
            // increment test string, substring indices
            internalIndex++;
            searchIndex++;
            // check for end of substring (search complete)
            if (searchSubStr[searchIndex] == NULL_CHAR) {
                // return current test string index
                return masterIndex;
            }
        // end loop
        }
        // increment current test string index
        masterIndex++;
    // end loop
    }
    // assume tests have failed at this point, return SUBSTRING_NOT_FOUND
    return SUBSTRING_NOT_FOUND;
}

/*
 * Function Name: setStrToLowerCase
 * Algorithm: iterates through string, set all upper case letters to lowercase without changing any other characters
 * Precondition: given a C-style source string, having a null character at end of string,
 *      and destination string parameter
 * PostCondition: all upper case letters in a given string are set to lower case, no change to any other characters
 * Exceptions: limit on string loop in case incorrect string format
 * Note: copy of source string is made internally to protect from aliasing
 */
void setStrToLowerCase(char *destStr, char *sourceStr) {
    // init variables
        // create temp source string
        int strLen = getStringLength(sourceStr);
        char *tempStr = (char *) malloc(strLen + 1);
        // init source string index to zero
        int index = 0;
    // copy source string to temp string
    copyString(tempStr, sourceStr);
    // loop to end of temp/source string
    // loop limited to MAX_STR_LEN
    while (index < MAX_STR_LEN && tempStr[index] != NULL_CHAR) {
        // change letter to lower case as needed and assign to dest string
        destStr[index] = setCharToLowerCase(tempStr[index]);
        // increment index
        index++;
        // add null char to enext dest string element
        destStr[index] = NULL_CHAR;
    }
    // end loop
    // release temp string memory
    free(tempStr);
}

/*
 * Function Name: concatenateString
 * Algorithm: concatenates or appends contents of one string onto the end of another
 * Precondition: given C-style source string, having a null character at end of string,
 *      destination string is passed in as a paramter with enough memory to accept the source string
 * Postcondition: destination string contains its original string with the source string appended or
 *      concatenated to the end of it
 * Exceptions: none
 * Note: limit test loop to maximum characters for safety
 */
void concatenateString(char *destination, char *source) {
    // init variables
        // set destination index to length of dest string
        int destIndex = getStringLength(destination);
        // set source index to 0
        int sourceIndex = 0;
    // loop until null character is found in source string
    // loop limits to MAX_STR_LEN
    while (sourceIndex < MAX_STR_LEN && source[sourceIndex] != NULL_CHAR) {
        // assign source character to destination at dest index
        destination[destIndex] = source[sourceIndex];
        // increment source and dest indices
        sourceIndex++;
        destIndex++;
        // assign null char to next dest element
        destination[destIndex] = NULL_CHAR;
    }
    // end loop
}

/*
 * Function Name: setCharToLowerCase
 * Algorithm: tests character parameter for upper case, changes it to lower case, makes no changes if not upper case
 * Precondition: given char value
 * Postcondition: upper case letter is set to lower case, otherwise char is returned unchanged
 * Exceptions: none
 * Note: none
 */
char setCharToLowerCase(char testChar) {
    // check for char between A and Z inclusive
    if (testChar >= 'A' && testChar <= 'Z') {
        // convert upper case letter to lower case
        testChar = (char) (testChar - 'A' + 'a');
    }
    // return character
    return testChar;
}

/*
 * Function Name: getLineTo
 * Algorithm: finds given text in file, skipping white space if specified, stops searching at given char or length
 * Precondition: file is open with valid file pointer; char buffer has adequate memory for data; stop char and length valid
 * Postcondition: ignores leading white space if specified; captures all printable chars and stores in string buffer
 * Exceptions: returns INCOMPLETE_FILE_ERR if no valid data found;
 *      returns NO_ERR if successful operation
 * Note: none
 */
int getLineTo(FILE *filePtr, int bufferSize, char stopChar, char *buffer, Boolean omitLeadingWhiteSpace, Boolean stopAtNonPrintable) {
    // init vars
        // init char index
        int charIndex = 0;
        // init status return to NO_ERR
        int statusReturn = NO_ERR;
        // init buffer size available flag to True
        Boolean bufferSizeAvailable = True;
        // init other vars
        int charAsInt;
    // get the first char
    charAsInt = fgetc(filePtr);
    // use a loop to consume leading white space, if flagged
    while (omitLeadingWhiteSpace == True
        && charAsInt != (int) stopChar
        && charIndex < bufferSize
        && charAsInt <= (int) SPACE) {
        // get next char (as int)
        charAsInt = fgetc(filePtr);
    }
    // end loop
    // capture string
    // loop while char is not stop char and buffer size is available
    while (charAsInt != (int) stopChar && bufferSizeAvailable == True
        && (stopAtNonPrintable == False || (charAsInt >= ' ' && charAsInt <= '~'))) {
        // check for input failure
        if (isEndOfFile(filePtr) == True) {
            // return incomplete file error
            return INCOMPLETE_FILE_ERR;
        }
        // check for usable printable char
        if (charAsInt >= (int) SPACE) {
            // assign input char to buffer string
            buffer[charIndex] = (char) charAsInt;
            // increment index
            charIndex++;
        }
        // set next buffer element to null char
        buffer[charIndex] = NULL_CHAR;
        // check for not at end of buffer size
        if (charIndex < bufferSize - 1) {
            // get a new char
            charAsInt = fgetc(filePtr);
        }
        // otherwise, assume end of buffer size
        else {
            // set buffer size Boolean to false to end loop
            bufferSizeAvailable = False;
            // set status return to buffer overrun error
            statusReturn = INPUT_BUFFER_OVERRUN_ERR;
        }
    }
    // end loop
    // return status data
    return statusReturn;
}

/*
 * Function Name: isEndOfFile
 * Algorithm: reports end of file, using feof
 * Precondition:file is open with valid file pointer
 * Postcondition: reports end of file
 * Exceptions: none
 * Note: none
 */
Boolean isEndOfFile(FILE *filePtr) {
    // check for feof end of file response
    if (feof(filePtr) != 0) {
        // return true
        return True;
    }
    // assume test failed at this point, return false
    return False;
}
