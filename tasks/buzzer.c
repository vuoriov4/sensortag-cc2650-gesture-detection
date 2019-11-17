#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Prototypes.h"
#include "Constants.h"

int longDelay = 700;
int shortDelay = 50;
int buzzDelay = 100;
int buzzing = 0;
int buzzType = SWEEP_DOWN; // 0: short->long, 1: long->short
Char buzzerTaskStack[STACKSIZE];
static PIN_Handle buzzerHandle;
static PIN_State buzzerState;
PIN_Config buzzerConfig[] = { Board_BUZZ | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX, PIN_TERMINATE };

Void buzzerTaskFxn(UArg arg0, UArg arg1) {
    while (1) {
        if (!buzzing) {
            Task_sleep(1 * 10000 / Clock_tickPeriod); // 0.01 sec 
            continue;
        }
        PIN_setOutputValue( buzzerHandle, Board_BUZZ, !PIN_getOutputValue( Board_BUZZ ) );
        if (buzzType == SWEEP_DOWN) {
            if (buzzDelay >= longDelay) { buzzDelay = shortDelay; buzzing = false; }
            buzzDelay++;
        } else /* SWEEP_UP */ {
            if (buzzDelay <= shortDelay) { buzzDelay = longDelay; buzzing = false; }
            buzzDelay--;
        }
        Task_sleep(buzzDelay / Clock_tickPeriod); 
    }
}

Void buzz(int bType) {
    if (bType == SWEEP_DOWN) buzzDelay = shortDelay;
    else /* SWEEP_UP */ buzzDelay = longDelay; 
    buzzing = 1;
    buzzType = bType;
}

Void startBuzzerTask() {
    buzzerHandle = PIN_open(&buzzerState, buzzerConfig);
    if (!buzzerHandle) System_abort("Error initializing BUZZER pins\n");
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &buzzerTaskStack; 
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)buzzerTaskFxn, &params, NULL);
    if (handle == NULL) System_abort("Blink task create failed");
}
