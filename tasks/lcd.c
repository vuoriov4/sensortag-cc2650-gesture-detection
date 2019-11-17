#include <stdio.h>
#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>
#include <ti/sysbios/knl/Clock.h>
#include "Constants.h"
#include "Prototypes.h"
#include <ti/mw/grlib/grlib.h>
#include <stdint.h>

Display_Handle hDisplayLcd;

Char lcdTaskStack[STACKSIZE];
char lcdStateText[16] = "";
char lcdAccelerationXText[16] = "N/A";
char lcdAccelerationYText[16] = "N/A";
char lcdAccelerationZText[16] = "N/A";
char lcdWirelessMessageText[16] = "";
char lcdGyroXText[16] = "N/A";
char lcdGyroYText[16] = "N/A";
char lcdGyroZText[16] = "N/A";
char lcdMessageArray[8][16];

int lcdMessageArrayCount = 0;
int lcdTextChanged = 0;
uint32_t imgPalette[] = {0, 0xFFFFFF};

// TI Logo bitmap - Copied from TI Resource explorer / Display examples
const uint8_t imgData[402] = {
    0x33, 0x2C, 0x91, 0x2C, 0x91, 0x2C, 0x91, 0x2C, 0x91, 0x2C, 0x91, 0x2C, 0x19,
    0x10, 0x65, 0x2C, 0x19, 0x10, 0x65, 0x2C, 0x19, 0x10, 0x65, 0x2C, 0x17, 0x12,
    0x65, 0x2C, 0x17, 0x12, 0x65, 0x2C, 0x17, 0x10, 0x67, 0x2C, 0x17, 0x10, 0x67,
    0x2C, 0x17, 0x10, 0x67, 0x2C, 0x15, 0x12, 0x67, 0x2C, 0x91, 0x2C, 0x91, 0x2C,
    0x91, 0x2C, 0x91, 0x38, 0x09, 0x10, 0x07, 0x24, 0x3B, 0x38, 0x07, 0x12, 0x07,
    0x24, 0x3B, 0x38, 0x07, 0x10, 0x07, 0x26, 0x3B, 0x38, 0x07, 0x10, 0x07, 0x26,
    0x3B, 0x38, 0x07, 0x10, 0x07, 0x26, 0x3B, 0x36, 0x09, 0x10, 0x07, 0x26, 0x3B,
    0x36, 0x07, 0x12, 0x07, 0x26, 0x3B, 0x36, 0x07, 0x10, 0x07, 0x28, 0x3B, 0x36,
    0x07, 0x10, 0x07, 0x28, 0x3B, 0x34, 0x09, 0x10, 0x07, 0x28, 0x3B, 0x34, 0x07,
    0x12, 0x07, 0x28, 0x3B, 0x34, 0x07, 0x12, 0x07, 0x28, 0x3B, 0x26, 0x15, 0x12,
    0x13, 0x1C, 0x3B, 0x26, 0x15, 0x10, 0x15, 0x1C, 0x3B, 0x26, 0x15, 0x10, 0x15,
    0x1C, 0x3B, 0x26, 0x13, 0x12, 0x15, 0x1E, 0x39, 0x26, 0x13, 0x12, 0x15, 0x1E,
    0x39, 0x24, 0x15, 0x12, 0x13, 0x20, 0x39, 0x24, 0x15, 0x10, 0x15, 0x22, 0x05,
    0x56, 0x15, 0x10, 0x15, 0x22, 0x05, 0x56, 0x15, 0x10, 0x15, 0x24, 0x03, 0x54,
    0x15, 0x12, 0x15, 0x26, 0x01, 0x54, 0x15, 0x10, 0x15, 0x8E, 0x07, 0x10, 0x07,
    0x9A, 0x09, 0x10, 0x07, 0x9A, 0x09, 0x10, 0x07, 0x9A, 0x07, 0x12, 0x07, 0x38,
    0x01, 0x5E, 0x07, 0x10, 0x07, 0x3A, 0x03, 0x5C, 0x07, 0x10, 0x07, 0x3A, 0x05,
    0x58, 0x09, 0x10, 0x07, 0x3A, 0x07, 0x56, 0x07, 0x12, 0x07, 0x3A, 0x0B, 0x52,
    0x07, 0x12, 0x07, 0x3A, 0x0D, 0x50, 0x07, 0x12, 0x07, 0x3A, 0x0D, 0x50, 0x07,
    0x10, 0x07, 0x3C, 0x0F, 0x4E, 0x07, 0x10, 0x07, 0x3C, 0x11, 0x4A, 0x09, 0x10,
    0x07, 0x3C, 0x13, 0x48, 0x07, 0x12, 0x07, 0x3C, 0x13, 0x48, 0x07, 0x10, 0x07,
    0x3E, 0x15, 0x46, 0x07, 0x10, 0x07, 0x3E, 0x15, 0x46, 0x07, 0x10, 0x07, 0x3E,
    0x15, 0x44, 0x09, 0x10, 0x09, 0x3C, 0x17, 0x42, 0x31, 0x30, 0x17, 0x42, 0x2F,
    0x32, 0x19, 0x40, 0x2F, 0x2C, 0x1F, 0x1C, 0x0B, 0x14, 0x31, 0x26, 0x27, 0x16,
    0x11, 0x12, 0x31, 0x20, 0x2D, 0x14, 0x15, 0x10, 0x2F, 0x1E, 0x33, 0x12, 0x15,
    0x10, 0x2F, 0x1A, 0x39, 0x0E, 0x19, 0x10, 0x2D, 0x16, 0x41, 0x06, 0x1F, 0x0E,
    0x2D, 0x14, 0x6B, 0x10, 0x2B, 0x12, 0x6F, 0x10, 0x29, 0x0E, 0x73, 0x18, 0x1F,
    0x0E, 0x77, 0x44, 0x79, 0x42, 0x7D, 0x3E, 0x7F, 0x3C, 0x83, 0x3A, 0x83, 0x38,
    0x85, 0x36, 0x89, 0x34, 0x8B, 0x30, 0x8D, 0x30, 0x8F, 0x2E, 0x8F, 0x2C, 0x93,
    0x2A, 0x93, 0x2A, 0x95, 0x28, 0x97, 0x26, 0x97, 0x26, 0x99, 0x24, 0x9B, 0x22,
    0x9D, 0x22, 0x9D, 0x20, 0x9F, 0x1E, 0xA3, 0x1A, 0xA7, 0x18, 0xA9, 0x10,
};
const tImage image = {
    .BPP = IMAGE_FMT_1BPP_COMP_RLE7,
    .NumColors = 2,
    .XSize = 96,
    .YSize = 96,
    .pPalette = imgPalette,
    .pPixel = imgData
};

Void lcdTaskFxn(UArg arg0, UArg arg1) {
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;
    hDisplayLcd = Display_open(Display_Type_LCD, &params);
    if (hDisplayLcd) {
        while (1) {
            int state = getState();
            if (lcdTextChanged) {
                if (state == STATE_IDLE) {
                    Display_print0(hDisplayLcd, 0, 0, "Welcome"); 
                    Display_print0(hDisplayLcd, 3, 0, "Navigate with");
                    Display_print0(hDisplayLcd, 4, 0, "top button");
                    Display_print0(hDisplayLcd, 6, 0, "Power off with");
                    Display_print0(hDisplayLcd, 7, 0, "bottom button");
                    Display_print0(hDisplayLcd, 10, 0, "-----STATE-----"); 
                    Display_print0(hDisplayLcd, 11, 0, lcdStateText); 
                } else if (state == STATE_READING) {
                    Display_print0(hDisplayLcd, 0, 0, "Acceleration"); // handle, y, x, message
                    Display_print0(hDisplayLcd, 1, 0, lcdAccelerationXText);
                    Display_print0(hDisplayLcd, 2, 0, lcdAccelerationYText);
                    Display_print0(hDisplayLcd, 3, 0, lcdAccelerationZText);
                    Display_print0(hDisplayLcd, 5, 0, "Gyroscope"); 
                    Display_print0(hDisplayLcd, 6, 0, lcdGyroXText); 
                    Display_print0(hDisplayLcd, 7, 0, lcdGyroYText); 
                    Display_print0(hDisplayLcd, 8, 0, lcdGyroZText); 
                    Display_print0(hDisplayLcd, 10, 0, "-----STATE-----"); 
                    Display_print0(hDisplayLcd, 11, 0, lcdStateText); 
                } else if (state == STATE_RECORDING) {
                    Display_print0(hDisplayLcd, 0, 0, "Acceleration"); // handle, y, x, message
                    Display_print0(hDisplayLcd, 1, 0, lcdAccelerationXText);
                    Display_print0(hDisplayLcd, 2, 0, lcdAccelerationYText);
                    Display_print0(hDisplayLcd, 3, 0, lcdAccelerationZText);
                    Display_print0(hDisplayLcd, 5, 0, "Gyroscope"); 
                    Display_print0(hDisplayLcd, 6, 0, lcdGyroXText); 
                    Display_print0(hDisplayLcd, 7, 0, lcdGyroYText); 
                    Display_print0(hDisplayLcd, 8, 0, lcdGyroZText); 
                    Display_print0(hDisplayLcd, 10, 0, "-----STATE-----"); 
                    Display_print0(hDisplayLcd, 11, 0, lcdStateText); 
                } else if (state == STATE_GESTURE_DETECTED) {
                    Display_print0(hDisplayLcd, 0, 0, "GREAT JOB"); // handle, y, x, message
                    Display_print0(hDisplayLcd, 1, 0, "you did");
                    Display_print0(hDisplayLcd, 2, 0, "the thing");
                    Display_print0(hDisplayLcd, 3, 0, "good job");
                    Display_print0(hDisplayLcd, 10, 0, "-----STATE-----"); 
                    Display_print0(hDisplayLcd, 11, 0, lcdStateText); 
                } else if (state == STATE_MAILBOX) {
                    Display_print0(hDisplayLcd, 0, 0, "YOUVE GOT MAIL:"); // handle, y, x, message
                    int i = 0; for (i = 0; i < lcdMessageArrayCount; i++) {
                        Display_print0(hDisplayLcd, i+1, 0, lcdMessageArray[i]);
                    }
                    Display_print0(hDisplayLcd, 10, 0, "-----STATE-----"); 
                    Display_print0(hDisplayLcd, 11, 0, lcdStateText); 
                }
                lcdTextChanged = 0;
            }
            if (state == STATE_IMAGE) {
                tContext *pContext = DisplayExt_getGrlibContext(hDisplayLcd);
                GrImageDraw(pContext, &image, 0, 0);
                GrFlush(pContext);
            }
            Task_sleep(1 * 100000 / Clock_tickPeriod); // 0.1 sec 
        }
    }
}

Void startLcdTask() {
    int i; for (i = 0; i < 8; i++) strncpy(lcdMessageArray[i], "", sizeof(lcdMessageArray[i]));
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
    System_flush();
}

Void setLcdStateText(int state) {
    Display_clear(hDisplayLcd);	
    switch (state) {
        case STATE_IDLE:
            strncpy(lcdStateText, "Idle", sizeof(lcdStateText));
            break;
        case STATE_READING:
            strncpy(lcdStateText, "Gesture", sizeof(lcdStateText));
            break;
        case STATE_RECORDING:
            strncpy(lcdStateText, "Recording", sizeof(lcdStateText));
            break;
        case STATE_GESTURE_DETECTED:
            strncpy(lcdStateText, "Gesture", sizeof(lcdStateText));
            break;
        case STATE_IMAGE:
            strncpy(lcdStateText, "Image", sizeof(lcdStateText));
            break;
        case STATE_MAILBOX:
            strncpy(lcdStateText, "Mailbox", sizeof(lcdStateText));
            break;
    }   
    lcdTextChanged = 1;
}

Void setLcdAccelerationText(float ax, float ay, float az) {
    char sax[16]; char say[16]; char saz[16];
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
    strncpy(lcdGyroXText, sgx, sizeof(sgx));
    strncpy(lcdGyroYText, sgy, sizeof(sgy));
    strncpy(lcdGyroZText, sgz, sizeof(sgz));
    lcdTextChanged = 1;
}  

Void addLcdWirelessMessage(char* msg) {
    if (lcdMessageArrayCount < 8) {
        strncpy(lcdMessageArray[lcdMessageArrayCount], msg, sizeof(lcdMessageArray[lcdMessageArrayCount]));
    }
    else {
        //shift
        int i; for (i = 7; i > 0; i--) strncpy(lcdMessageArray[i], lcdMessageArray[i-1], sizeof(lcdMessageArray[i]));
        strncpy(lcdMessageArray[0], msg, sizeof(lcdMessageArray[0]));
    }
    lcdMessageArrayCount++;
    lcdTextChanged = 1;
}

Void closeLcd() {
    Display_clear(hDisplayLcd);
    Display_close(hDisplayLcd);
}

