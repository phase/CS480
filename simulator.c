#include "simulator.h"

/**
 * Get the difference between the current time and the start time
 * @param start start time
 * @return time diff
 */
struct timeval timeDiff(struct timeval start) {
    struct timeval end;
    gettimeofday(&end, NULL);
    long secDiff;
    long usecDiff;

    secDiff = end.tv_sec - start.tv_sec;
    usecDiff = end.tv_usec - start.tv_usec;

    // wrap around
    if (usecDiff < 0) {
        usecDiff += 1000000;
        secDiff--;
    }

    end.tv_sec = secDiff;
    end.tv_usec = usecDiff;
    return end;
}

/**
 * run timer from Michael
 */
void runTimer(long milliSeconds) {
    struct timeval startTime, endTime;
    int startSec, startUSec, endSec, endUSec;
    int uSecDiff, mSecDiff, secDiff, timeDiff;
    gettimeofday(&startTime, NULL);

    startSec = startTime.tv_sec;
    startUSec = startTime.tv_usec;
    timeDiff = 0;

    while (timeDiff < milliSeconds) {
        gettimeofday(&endTime, NULL);

        endSec = endTime.tv_sec;
        endUSec = endTime.tv_usec;
        uSecDiff = endUSec - startUSec;

        if (uSecDiff < 0) {
            uSecDiff = uSecDiff + 1000000;
            endSec = endSec - 1;
        }

        mSecDiff = uSecDiff / 1000;
        secDiff = (endSec - startSec) * 1000;
        timeDiff = secDiff + mSecDiff;
    }
}

/**
 * Macro for logging to stdout and/or a file.
 * Make sure logS, logF, and logFile are defined before using this.
 */
#define LOGGER(...) \
    if (logS) { printf(__VA_ARGS__); } \
    if (logF) { fprintf(logFile, __VA_ARGS__); }

/**
 * Create a linked list of process control blocks from the op code list
 * @param metaDataPtr op code list
 * @return linked list of process control blocks
 */
ProcessControlBlock *createProcessControlBlocks(OpCodeType *metaDataPtr) {
    int processCount = 0;
    // get the amount of processes from the op code list
    OpCodeType *currentOp = metaDataPtr;
    while (currentOp != NULL) {
        if (compareString(currentOp->command, "app") == STR_EQ
            && compareString(currentOp->strArg1, "start") == STR_EQ) {
            processCount++;
        }
        currentOp = currentOp->nextNode;
    }

    // no pcbs to allocate
    if (processCount == 0) {
        return NULL;
    }

    // allocate the pcbs
    struct ProcessControlBlock *head = calloc(1, sizeof(struct ProcessControlBlock));
    struct ProcessControlBlock *current = head;
    for (int index = 0; index < processCount; index++) {
        current->id = index;
        current->state = NEW;
        if (index < processCount - 1) {
            current->next = calloc(1, sizeof(struct ProcessControlBlock));
            current = current->next;
        }
    }
    return head;
}

/**
 * Select a process from the list of PCBs using the scheduling code
 */
ProcessControlBlock *selectProcess(int scheduleCode, ProcessControlBlock *pcb) {
    if (scheduleCode == CPU_SCHED_FCFS_N_CODE) {
        ProcessControlBlock *currentPcb = pcb;
        while (currentPcb != NULL) {
            if (currentPcb->state == READY) {
                return currentPcb;
            }
            currentPcb = currentPcb->next;
        }
    }
    return NULL;
}

/**
 * Main method for the timer thread
 */
void *timerThread(void *args) {
    long *time = (long *) args;
    runTimer(*time);
    return 0;
}

/**
 * Spawn a timer thread that runs for the provided time
 */
void spawnTimerThread(long time) {
    pthread_t threadId;
    pthread_create(&threadId, NULL, timerThread, &time);
    pthread_join(threadId, NULL);
}

/**
 * Run a process specific command for a given process
 *
 * @param configDataPtr config data
 * @param startTime start time of the OS simulator
 * @param logFile file pointer to log to
 * @param logF boolean for logging to the file
 * @param logS boolean for logging to stdout
 * @param selectedProcess the currently selected process
 * @param currentOp current op code
 */
void runProcessCommand(
        ConfigDataType *configDataPtr, struct timeval startTime, FILE *logFile, int logF, int logS,
        ProcessControlBlock *selectedProcess, OpCodeType *currentOp) {
    if (compareString(currentOp->command, "dev") == STR_EQ) {
        // we are handling a device op code
        // format: "dev XXX, XXX, #"
        long ioTime = currentOp->intArg2 * configDataPtr->ioCycleRate;
        struct timeval elapsedTime = timeDiff(startTime);
        LOGGER("  %ld.%06ld, Process: %d, %s %sput operation start\n", elapsedTime.tv_sec,
               elapsedTime.tv_usec, selectedProcess->id, currentOp->strArg1, currentOp->inOutArg);

        // run the time in another thread
        spawnTimerThread(ioTime);

        elapsedTime = timeDiff(startTime);
        LOGGER("  %ld.%06ld, Process: %d, %s %sput operation end\n", elapsedTime.tv_sec,
               elapsedTime.tv_usec, selectedProcess->id, currentOp->strArg1, currentOp->inOutArg);
    } else if (compareString(currentOp->command, "cpu") == STR_EQ) {
        // we are handling a cpu op code
        // format: "cpu XXX, #"
        if (compareString(currentOp->strArg1, "process") == STR_EQ) {
            struct timeval elapsedTime = timeDiff(startTime);
            LOGGER("  %ld.%06ld, Process: %d, cpu process operation start\n", elapsedTime.tv_sec,
                   elapsedTime.tv_usec, selectedProcess->id);

            // run the time in another thread
            long processTime = currentOp->intArg2 * configDataPtr->procCycleRate;
            spawnTimerThread(processTime);

            elapsedTime = timeDiff(startTime);
            LOGGER("  %ld.%06ld, Process: %d, cpu process operation end\n", elapsedTime.tv_sec,
                   elapsedTime.tv_usec, selectedProcess->id);
        }
    }
}

/**
 * Runs the simulator
 * @param configDataPtr Config Data
 * @param metaDataPtr Metadata / op code list
 */
void runSim(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr) {
    printf("Simulator Run\n-------------\n\n");
    // TODO: remove for Sim03
    configDataPtr->cpuSchedCode = CPU_SCHED_FCFS_N_CODE;

    ProcessControlBlock *pcb = createProcessControlBlocks(metaDataPtr);
    struct timeval startTime;
    gettimeofday(&startTime, NULL);
    FILE *logFile;

    // booleans for where we need to log
    int logF = configDataPtr->logToCode == LOGTO_BOTH_CODE || configDataPtr->logToCode == LOGTO_FILE_CODE;
    int logS = configDataPtr->logToCode == LOGTO_BOTH_CODE || configDataPtr->logToCode == LOGTO_MONITOR_CODE;

    // if we need to log to a file, open it
    if (logF) {
        logFile = fopen(configDataPtr->logToFileName, "w");
    }
    // let the user know we're outputting to a file
    if (!logS) {
        printf("Simulator running with output to file\n\n");
    }

    ProcessControlBlock *currentProcess = NULL;

    // loop over all the op codes
    OpCodeType *currentOp = metaDataPtr;
    while (currentOp != NULL) {
        struct timeval elapsedTime = timeDiff(startTime);
        if (compareString(currentOp->command, "sys") == STR_EQ) {
            // we are handling a system op code
            // format: "sys XXX"
            if (compareString(currentOp->strArg1, "start") == STR_EQ) {
                LOGGER("  %ld.%06ld, OS: Simulator start\n", elapsedTime.tv_sec, elapsedTime.tv_usec);
                // set all the pcbs to ready
                ProcessControlBlock *selectedPcb = pcb;
                while (selectedPcb != NULL) {
                    selectedPcb->state = READY;
                    LOGGER("  %ld.%06ld, OS: Process %d set to READY state from NEW state\n", elapsedTime.tv_sec,
                           elapsedTime.tv_usec, selectedPcb->id);
                    selectedPcb = selectedPcb->next;
                }
            } else if (compareString(currentOp->strArg1, "end") == STR_EQ) {
                LOGGER("  %ld.%06ld, OS: Simulator end\n", elapsedTime.tv_sec, elapsedTime.tv_usec);
            }
        } else if (compareString(currentOp->command, "app") == STR_EQ) {
            // we are handling an application op code
            // format: "app XXX, ?#"
            if (compareString(currentOp->strArg1, "start") == STR_EQ) {
                currentProcess = selectProcess(configDataPtr->cpuSchedCode, pcb);
                currentProcess->state = RUNNING;
                LOGGER("  %ld.%06ld, OS: Process %d set from READY to RUNNING\n\n", elapsedTime.tv_sec,
                       elapsedTime.tv_usec, currentProcess->id);
            } else if (compareString(currentOp->strArg1, "end") == STR_EQ) {
                if (currentProcess != NULL) {
                    // end the current process
                    currentProcess->state = EXIT;
                    LOGGER("\n  %ld.%06ld, OS: Process %d ended\n", elapsedTime.tv_sec,
                           elapsedTime.tv_usec, currentProcess->id);
                    LOGGER("  %ld.%06ld, OS: Process %d set to EXIT\n", elapsedTime.tv_sec,
                           elapsedTime.tv_usec, currentProcess->id);
                    currentProcess = NULL;
                }
            }
        } else if (compareString(currentOp->command, "dev") == STR_EQ
                   || compareString(currentOp->command, "cpu") == STR_EQ
                   || compareString(currentOp->command, "mem") == STR_EQ) {
            // we are handling a process specific op code
            runProcessCommand(configDataPtr, startTime, logFile, logF, logS, currentProcess, currentOp);
        } else {
            // this should never happen!
            printf("    unhandled op code type %s\n", currentOp->command);
        }
        currentOp = currentOp->nextNode;
    }

    // close the file if we've opened it
    if (logFile != NULL) {
        fclose(logFile);
    }

    // free the pcb
    ProcessControlBlock *tempPcb;
    while (pcb != NULL) {
        tempPcb = pcb;
        pcb = pcb->next;
        free(tempPcb);
    }
}

#undef LOGGER
