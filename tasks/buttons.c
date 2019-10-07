#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Constants.h"

static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;
PIN_Config buttonConfig[] = { Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,  PIN_TERMINATE };
PIN_Config ledConfig[] = { Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX, PIN_TERMINATE };

Void buttonListenerFxn(PIN_Handle handle, PIN_Id pinId) {
    PIN_setOutputValue( ledHandle, Board_LED1, !PIN_getOutputValue( Board_LED1 ) );
    int state = getState();
    if (state == SLEEP) setState(READING);
    else setState(SLEEP);
}

Void startButtonListener() {
    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if (!buttonHandle) System_abort("Error initializing button pins\n");
    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) System_abort("Error initializing LED pins\n");
    if (PIN_registerIntCb(buttonHandle, &buttonListenerFxn) != 0) System_abort("Error registering button callback function");
}
