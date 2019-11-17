#include <stdio.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include "Board.h"
#include "Constants.h"
#include "Prototypes.h"
#include "sensors/bmp280.h"
#include "sensors/mpu9250.h"

Char accelerationSensorTaskStack[STACKSIZE];

static PIN_Handle hMpuPin;
static PIN_State MpuPinState;
static PIN_Config MpuPinConfig[] = { Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX, PIN_TERMINATE };
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = { .pinSDA = Board_I2C0_SDA1, .pinSCL = Board_I2C0_SCL1 };

Void accelerationSensorTaskFxn(UArg arg0, UArg arg1) {
    // initialize sensor
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) System_abort("Pin open failed!");
	I2C_Handle i2cMPU; 
	I2C_Params i2cMPUParams;
	float ax, ay, az, gx, gy, gz;
    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) System_abort("Error Initializing I2CMPU\n");
    PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);
	Task_sleep(100000 / Clock_tickPeriod); // wait for power up
    System_printf("MPU9250: Power ON\n");
    System_flush();
	System_printf("MPU9250: Setup and calibration...\n");
	System_flush();
	mpu9250_setup(&i2cMPU);
	System_printf("MPU9250: Setup and calibration OK\n");
	System_flush();
    I2C_close(i2cMPU);
    // read
	while (1) {
	    int s = getState();
        if (!(s == STATE_READING || s == STATE_RECORDING || s == STATE_GESTURE_DETECTED)) {
            Task_sleep(10000 / Clock_tickPeriod); // 0.1s
            continue;
        }
	    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
	    if (i2cMPU == NULL) System_abort("Error Initializing I2CMPU\n");
		mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
		if (false) {
		    char sax[16]; char say[16]; char saz[16];
            snprintf (sax, sizeof(sax), "%f", ax);
            snprintf (say, sizeof(say), "%f", ay);
            snprintf (saz, sizeof(saz), "%f", az);
            printf("%s,%s,%s\n", sax, say, saz);
		}
		uint32_t timestamp = Clock_tickPeriod * Clock_getTicks(); 
		if (s == STATE_READING || s == STATE_GESTURE_DETECTED) recordData(ax, ay, az, gx, gy, gz, timestamp / 1000000.0f); // timestamp seconds
        else if (s == STATE_RECORDING) recordTrainingData(ax, ay, az, gx, gy, gz, timestamp / 1000000.0f); // timestamp seconds
        setLcdAccelerationText(ax, ay, az);
        setLcdGyroText(gx, gy, gz);
	    I2C_close(i2cMPU);
    	Task_sleep(ACC_SENSOR_SAMPLING_DELAY / Clock_tickPeriod); 
	}
	// power off
    // PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_OFF); 
}

Void startAccelerationSensorTask() {
   initializeData();
   Task_Params params;
   Task_Handle handle;
   Task_Params_init(&params);
   params.stackSize = STACKSIZE;
   params.stack = &accelerationSensorTaskStack; 
   params.priority = 2;
   params.arg0 = NULL; 
   params.arg1 = NULL; 	
   handle = Task_create((Task_FuncPtr)accelerationSensorTaskFxn, &params, NULL);
   if (handle == NULL) System_abort("Acceleration sensor task create failed");
}
