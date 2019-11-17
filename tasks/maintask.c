#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "Constants.h"
#include "Prototypes.h"
#include "wireless/address.h"
#include "wireless/comm_lib.h"

int currentState = IDLE;
int previousState = IDLE;
int stateChanged = 1;
Char mainTaskStack[STACKSIZE];

int getState() {
    return currentState;
}

Void setState(int state) { 
    stateChanged = 1;
    previousState = currentState;
    currentState = state; 
}

Void mainTaskFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (currentState == STATE_READING) {
            int gesture = calculateGesture();
            if (gesture) setState(STATE_GESTURE_DETECTED);
        } 
        if (stateChanged) {
            setLcdStateText(currentState);
            stateChanged = 0;
        }
        if (currentState == STATE_GESTURE_DETECTED) {
            buzz(SWEEP_UP);
            // Send network message
            char payload[16] = "I did the thing";
            Send6LoWPAN(IEEE80154_SERVER_ADDR, payload, strlen(payload));
            StartReceive6LoWPAN();
            Task_sleep(1 * 3000000 / Clock_tickPeriod); // show gesture for 3 sec 
            setState(STATE_READING);
        }
        Task_sleep(1 * 10000 / Clock_tickPeriod); // 0.01 sec 
    }
}

Void startMainTask() {
    currentState = IDLE;
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &mainTaskStack; 
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)mainTaskFxn, &params, NULL);
    if (handle == NULL) System_abort("Main task create failed");
}
