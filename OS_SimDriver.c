#include "OS_SimDriver.h"

int main(int argc, char** argv) {
    // initialize program
    // initialize variables
    Boolean programRunFlag = False;
    Boolean configDisplayFlag = False;
    Boolean configUploadFlag = False;
    Boolean mdDisplayFlag = False;
    Boolean runSimFlag = False;
    Boolean infoFlag = False;
    int argIndex = 1;
    int lastFourLetters = 4;
    int fileStrLen, fileStrSubLoc;
    char fileName[STD_STR_LEN];
    char errorMessage[MAX_STR_LEN];
    ConfigDataType *configDataPtr = NULL;
    OpCodeType *metaDataPtr = NULL;
    // show title
    printf("\nSimulator Program\n");
    printf("=================\n\n");

    // check for only one arg
    if (argc < 2 ) {
        // show error message, end program
        showProgramFormat();
        // unset program run flag
        programRunFlag = False;
        // set info flag
        infoFlag = True;
    }

    // initialize file name to empty
    fileName[0] = NULL_CHAR;

    // loop across args and program run flag
    while (programRunFlag == False && argIndex < argc) {
        // find lengths to verify file name
        fileStrLen = getStringLength(argv[argIndex]);
        fileStrSubLoc = findSubString(argv[argIndex], ".cnf");

        // check for -dc display config flag
        if (compareString(argv[argIndex], "-dc") == STR_EQ) {
            // set config upload flag
            configUploadFlag = True;
            // set config display flag
            configDisplayFlag = True;
        }
        // otherwise, check for -dm display metadata flag
        else if (compareString(argv[argIndex], "-dm") == STR_EQ) {
            // set config upload flag
            configUploadFlag = True;
            // set meta data display flag
            mdDisplayFlag = True;
        }
        // otherwise, check for -rs run simulator
        else if (compareString(argv[argIndex], "-rs") == STR_EQ) {
            // set config upload flag
            configUploadFlag = True;
            // set run simulator flag
            runSimFlag = True;
        }
        // otherwise, check for file name, ending in .cnf
        // must be last flag
        else if (fileStrSubLoc != SUBSTRING_NOT_FOUND && fileStrSubLoc == fileStrLen - lastFourLetters) {
            // set file name to variable
            copyString(fileName, argv[argIndex]);
            // set program run flag
            programRunFlag = True;
        }
        // update arg index
        argIndex++;
    }
    // end arg loop

    // check for command line failure
    if (programRunFlag == False && infoFlag == False) {
        // show error message, end program
        printf("Incorrect argument line format, program aborted.\n\n");
        // provide command line info to user
        showProgramFormat();
    }

    // check for program run and config upload flag
    if (programRunFlag == True && configUploadFlag == True) {
        // upload config file, check for success
        if (getStringLength(fileName) > 0 && getConfigData(fileName, &configDataPtr, errorMessage) == True) {
            // check config display flag
            if (configDisplayFlag == True) {
                // display config data
                displayConfigData(configDataPtr);
            }
        }
        // otherwise, assume config file upload failure
        else {
            // show error message, end program
            printf("\nConfig Upload Error: %s, program aborted.\n\n", errorMessage);
            // unset program run flag
            programRunFlag = False;
        }
    }
    // check for program run flag
    if (programRunFlag == True && (mdDisplayFlag == True || runSimFlag == True))  {
        // upload meta data file, check for success
        if (getMetaData(configDataPtr->metaDataFileName, &metaDataPtr, errorMessage) == True) {
            // check meta data display flag
            if (mdDisplayFlag == True) {
                // display meta data
                displayMetaData(metaDataPtr);
            }
            // check run simulator flag
            if (runSimFlag == True) {
                // run simulator
                runSim(configDataPtr, metaDataPtr);
            }
        }
        // otherwise, assume meta data upload failure
        else {
            // show error message, end program
            printf("\nMetadata Upload Error: %s, program aborted.\n", errorMessage);
        }
    }
    // clean up config data as needed
    configDataPtr = clearConfigData(configDataPtr);
    // clean up metadata as needed
    metaDataPtr = clearMetaDataList(metaDataPtr);
    // show program end
    printf("\nSimulator Program End.\n");
    // return success
    return 0;
}

/*
 * name: showProgramFormat
 * process: displays command line argument requirements for this program
 * method input/parameters: none
 * method output/parameters: none
 * method output/returned: none
 * device input/keyboard: none
 * device output/monitor: instructions provided as specified
 * dependencies: none
 */
void showProgramFormat() {
    // print out command line argument instructions
    printf("Program Format:\n");
    printf("     sim [-dc] [-dm] [-rs] <config file name>\n");
    printf("     -dc [optional] displays configuration data\n");
    printf("     -dm [optional] displays meta data\n");
    printf("     -rs [optional] runs simulator\n");
    printf("     config file name is required\n");
}
