#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include "wireless/comm_lib.h"
#include "Constants.h"
#include "Prototypes.h"

Char messageTaskStack[STACKSIZE];

Void messageTaskFxn(UArg arg0, UArg arg1) {
   char payload[16];
   uint16_t senderAddr = IEEE80154_MY_ADDR;
   int32_t result = StartReceive6LoWPAN();
   if(result != true) {
      System_abort("Wireless receive start failed");
   }
   while (true) {
        if (GetRXFlag()) {
           // Receive network message
           memset(payload,0,16);
           Receive6LoWPAN(&senderAddr, payload, 16);
           addLcdWirelessMessage(payload);
           System_printf(payload);
           System_printf(": received \n");
           System_flush();
        }
        // Task_sleep(1 * 500000 / Clock_tickPeriod); // 0.5 sec 
    }
}

Void startMessageTask() {
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &messageTaskStack; 
    params.priority = 1;
    handle = Task_create((Task_FuncPtr)messageTaskFxn, &params, NULL);
    if (handle == NULL) System_abort("Message task create failed");
}
