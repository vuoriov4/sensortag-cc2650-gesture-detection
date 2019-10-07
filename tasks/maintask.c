#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "Constants.h"

int currentState;
int stateChanged = 1;
Char mainTaskStack[STACKSIZE];

int getState() {
    return currentState;
}

Void setState(int state) { 
    stateChanged = 1;
    currentState = state; 
}

Void mainTaskFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (stateChanged) {
            setLcdStateText(currentState);
            switch (currentState) {
                case SLEEP:
                    //currentState = IDLE;
                    break;
                case READING:
                    //currentState = SLEEP;
                    break;
            }   
            stateChanged = 0;
        }
        Task_sleep(1 * 100000 / Clock_tickPeriod); // 0.1 sec 
    }
}

Void startMainTask() {
    currentState = SLEEP;
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &mainTaskStack; 
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)mainTaskFxn, &params, NULL);
    if (handle == NULL) System_abort("Main task create failed");
}