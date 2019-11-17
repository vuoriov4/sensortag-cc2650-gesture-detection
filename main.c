#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include "Board.h"
#include "Prototypes.h"
#include "wireless/comm_lib.h"

Void startTasks() {
    startLcdTask();
    startAccelerationSensorTask();
    startBuzzerTask();
    startMessageTask();
    startMainTask();
}

Int main(void) {
    Board_initGeneral();
    Board_initI2C();
    Init6LoWPAN(); // call this before networking
    startTasks();
    startButtonListener();
    System_printf("Starting BIOS...\n");
    System_flush();
    BIOS_start();
    return (0);
}

