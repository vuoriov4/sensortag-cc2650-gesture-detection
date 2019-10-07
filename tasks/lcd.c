#include <stdio.h>
#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/sysbios/knl/Clock.h>
#include "Constants.h"
#include "Prototypes.h"

Char lcdTaskStack[STACKSIZE];
char lcdStateText[16] = "";
char lcdAccelerationXText[16] = "N/A";
char lcdAccelerationYText[16] = "N/A";
char lcdAccelerationZText[16] = "N/A";
char lcdGyroXText[16] = "N/A";
char lcdGyroYText[16] = "N/A";
char lcdGyroZText[16] = "N/A";
int lcdTextChanged = 0;
    
Void lcdTaskFxn(UArg arg0, UArg arg1) {
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;
    Display_Handle hDisplayLcd = Display_open(Display_Type_LCD, &params);
    if (hDisplayLcd) {
        while (1) {
            if (lcdTextChanged) {
                // Display_clear(hDisplayLcd);	
                Display_print0(hDisplayLcd, 0, 0, "-Acceleration-"); // handle, y, x, message
                Display_print0(hDisplayLcd, 1, 0, lcdAccelerationXText);
                Display_print0(hDisplayLcd, 2, 0, lcdAccelerationYText);
                Display_print0(hDisplayLcd, 3, 0, lcdAccelerationZText);
                Display_print0(hDisplayLcd, 4, 0, "-Gyroscope-"); 
                Display_print0(hDisplayLcd, 5, 0, lcdGyroXText); 
                Display_print0(hDisplayLcd, 6, 0, lcdGyroYText); 
                Display_print0(hDisplayLcd, 7, 0, lcdGyroZText); 
                Display_print0(hDisplayLcd, 8, 0, "-State-"); 
                Display_print0(hDisplayLcd, 9, 0, lcdStateText); 
                lcdTextChanged = 0;
            }
            Task_sleep(1 * 10000 / Clock_tickPeriod); // 0.01 sec 
        }
    }
}

Void startLcdTask() {
    Task_Params params;
    Task_Handle handle;
    Task_Params_init(&params);
    params.stackSize = STACKSIZE;
    params.stack = &lcdTaskStack; 
    params.arg0 = 1;
    params.arg1 = 2;
    params.priority = 2;
    handle = Task_create((Task_FuncPtr)lcdTaskFxn, &params, NULL);
    if (handle == NULL) System_abort("Lcd task create failed");
}

Void setLcdStateText(int state) {
    printf("setDisplayStateMessage\n");
    switch (state) {
        case SLEEP:
            strncpy(lcdStateText, "Sleeping", sizeof(lcdStateText));
            break;
        case READING:
            strncpy(lcdStateText, "Reading", sizeof(lcdStateText));
            break;
    }   
    lcdTextChanged = 1;
}

Void setLcdAccelerationText(float ax, float ay, float az) {
    char sax[48]; char say[48]; char saz[48];
    snprintf (sax, sizeof(sax), "%f", ax);
    snprintf (say, sizeof(say), "%f", ay);
    snprintf (saz, sizeof(saz), "%f", az);
    strncpy(lcdAccelerationXText, sax, sizeof(lcdAccelerationXText));
    strncpy(lcdAccelerationYText, say, sizeof(lcdAccelerationYText));
    strncpy(lcdAccelerationZText, saz, sizeof(lcdAccelerationZText));
    lcdTextChanged = 1;
}

Void setLcdGyroText(float gx, float gy, float gz) {
    char sgx[16]; char sgy[16]; char sgz[16];
    snprintf (sgx, sizeof(sgx), "%f", gx);
    snprintf (sgy, sizeof(sgy), "%f", gy);
    snprintf (sgz, sizeof(sgz), "%f", gz);
    strncpy(lcdGyroXText, sgx, sizeof(lcdGyroXText));
    strncpy(lcdGyroYText, sgy, sizeof(lcdGyroYText));
    strncpy(lcdGyroZText, sgz, sizeof(lcdGyroZText));
    lcdTextChanged = 1;
}
