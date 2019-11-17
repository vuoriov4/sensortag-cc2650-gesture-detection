#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Prototypes.h"
#include "Constants.h"
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

static PIN_Handle hButtonMenu;
static PIN_State bStateMenu;
static PIN_Handle ledHandle;
static PIN_State ledState;
static PIN_Handle hButtonShut;
static PIN_State bStateShut;
PIN_Config buttonMenu[] = { Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,  PIN_TERMINATE };
PIN_Config buttonShut[] = { Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE, PIN_TERMINATE };
PIN_Config buttonWake[] = { Board_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PINCC26XX_WAKEUP_NEGEDGE, PIN_TERMINATE };
PIN_Config ledConfig[] = { Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX, PIN_TERMINATE };

Void buttonMenuFxn(PIN_Handle handle, PIN_Id pinId) {
    int state = getState();
    if (state == STATE_IDLE) {
        setState(STATE_READING);
        PIN_setOutputValue( ledHandle, Board_LED1, !PIN_getOutputValue( Board_LED1 ) );
    } else if (state == STATE_READING) {
        setState(STATE_IMAGE);
        PIN_setOutputValue( ledHandle, Board_LED1, !PIN_getOutputValue( Board_LED1 ) );
    } 
    else if (state == STATE_IMAGE) setState(STATE_MAILBOX);
    else setState(STATE_IDLE);
}

Void buttonShutFxn(PIN_Handle handle, PIN_Id pinId) {
    int state = getState();
    if (state == STATE_READING) {
        setState(STATE_RECORDING);
    } else if (state == STATE_RECORDING) {
        setState(STATE_READING);
    } else {
        closeLcd();
        Task_sleep(100000 / Clock_tickPeriod);
        PIN_close(hButtonShut);
        PINCC26XX_setWakeup(buttonWake);
        Power_shutdown(NULL,0);
    }
}

Void startButtonListener() {
    hButtonMenu = PIN_open(&bStateMenu, buttonMenu);
    if (!hButtonMenu) System_abort("Error initializing button pins\n");
    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) System_abort("Error initializing LED pins\n");
    if (PIN_registerIntCb(hButtonMenu, &buttonMenuFxn) != 0) System_abort("Error registering button callback function");
    hButtonShut = PIN_open(&bStateShut, buttonShut);
    if (!hButtonShut) System_abort("Error initializing button shut pins\n");
    if (PIN_registerIntCb(hButtonShut, &buttonShutFxn) != 0) System_abort("Error registering button callback function");
}
