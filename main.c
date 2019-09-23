
#include <stdio.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/mw/display/Display.h>
#include <ti/mw/display/DisplayExt.h>

/* Board Header files */
#include "Board.h"

#include "wireless/comm_lib.h"
#include "sensors/tmp007.h"

/* Task */
#define STACKSIZE 2048
Char labTaskStack[STACKSIZE];
Char commTaskStack[STACKSIZE];
Char helloWorldStack[STACKSIZE];
Char displayStack[STACKSIZE];
Char sensorStack[STACKSIZE];
bool blinking = false;

/* Display */
Display_Handle hDisplay;

// JTKJ: Pin configuration and variables here
// JTKJ: Painonappien konfiguraatio ja muuttujat
static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;

PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE, 
   PIN_TERMINATE // Määritys lopetetaan aina tähän vakioon
};

PIN_Config ledConfig[] = {
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX, 
   PIN_TERMINATE // Määritys lopetetaan aina tähän vakioon
};

Void displayTaskFxn(UArg arg0, UArg arg1) {

   // Alustetaan näyttö nyt taskissa
   Display_Params params;
   Display_Params_init(&params);
   params.lineClearMode = DISPLAY_CLEAR_BOTH;

   // Näyttö käyttöön ohjelmassa
   Display_Handle hDisplayLcd = Display_open(Display_Type_LCD, &params);
    
   // Tulostetaan ruudulle koordinaatteihin (5,3) merkkijono
   if (hDisplayLcd) {
      System_printf("Printing to display!\n");
      char str[9];
      sprintf(str, "%x", *(uint32_t *)IEEE80154_MY_ADDR);
      Display_print0(hDisplayLcd, 5, 3, str);

      // Näytä teksti ruudulla 5s ajan
      Task_sleep(5 * 1000000/Clock_tickPeriod); 

      // Tyhjennä näyttö
      Display_clear(hDisplayLcd);		
   }
}

Void startDisplayTask() {
   Task_Params params;
   Task_Handle handle;
   Task_Params_init(&params);
   params.stackSize = STACKSIZE;
   params.stack = &displayStack; 
   params.priority = 2;
   params.arg0 = 127; 
   params.arg1 = 0xFFFF; 	
   handle = Task_create((Task_FuncPtr)displayTaskFxn, &params, NULL);
   if (handle == NULL) System_abort("Task create failed");
}

Void helloWorldTaskFxn(UArg arg0, UArg arg1) {
   while (1) {
        //System_printf("Mun argumentit ovat %ld ja %ld.. heippa!\n", arg0, arg1);
        //System_flush();
        if (blinking) PIN_setOutputValue( ledHandle, Board_LED1, !PIN_getOutputValue( Board_LED1 ) );
        else PIN_setOutputValue(ledHandle, Board_LED1, 0);
        Task_sleep(100000 / Clock_tickPeriod);

   }
}

Void startHelloWorldTask() {
   Task_Params params;
   Task_Handle handle;
   Task_Params_init(&params);
   params.stackSize = STACKSIZE;
   params.stack = &helloWorldStack; 
   params.priority = 2;
   params.arg0 = 127; 
   params.arg1 = 0xFFFF; 	
   handle = Task_create((Task_FuncPtr)helloWorldTaskFxn, &params, NULL);
   if (handle == NULL) System_abort("Task create failed");
}



Void labTaskFxn(UArg arg0, UArg arg1) {
    /*
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
*/
    /* Create I2C for sensors */
     /*
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
    }
    else {
        System_printf("I2C Initialized!\n");
    }
*/
    // JTKJ: Setup the BMP280 sensor here, before its use
    // JTKJ: Sensorin alustus t�ss� kirjastofunktiolla

    /* Display */
    /*
    Display_Params displayParams;
	displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;
    Display_Params_init(&displayParams);

    hDisplay = Display_open(Display_Type_LCD, &displayParams);
    if (hDisplay == NULL) {
        System_abort("Error initializing Display\n");
    }
    Display_clear(hDisplay);
    Display_print0(hDisplay, 5, 1, "Shall we play");
    Display_print0(hDisplay, 6, 1, "    a game?");
    */
    while (1) {

        // DO SOMETHING HERE..

    	// Once per second
    	Task_sleep(1000000 / Clock_tickPeriod);
    }
}

/* Communication Task */
Void commTaskFxn(UArg arg0, UArg arg1) {

    // Radio to receive mode
    uint16_t senderAddr = (uint16_t) IEEE80154_SERVER_ADDR;
	int32_t result = StartReceive6LoWPAN();
	if(result != true) {
		System_abort("Wireless receive mode failed");
	}
    char payload[16]; // viestipuskuri
    while (1) {

        // If true, we have a message
    	if (GetRXFlag() == true) {
    		// Handle the received message..
    		memset(payload,0,16);
    	    Receive6LoWPAN(&senderAddr, payload, 16);
            // Tulostetaan vastaanotettu viesti konsoli-ikkunaan
            System_printf(payload);
            System_flush();
        }

    	// Absolutely NO Task_sleep in this task!!
    }
}

// JTKJ: Write the interrupt handler for the button
// JTKJ: Laadi painonapille keskeytyksen k�sittelij�funktio

Void blinkListenerFxn(PIN_Handle handle, PIN_Id pinId) {
   // Vaihdetaan led-pinnin tilaa negaatiolla
   //PIN_setOutputValue( ledHandle, Board_LED1, !PIN_getOutputValue( Board_LED1 ) );
    blinking = !blinking;
    char payload[8] = "moiiiiii";
    Send6LoWPAN(IEEE80154_SERVER_ADDR, payload, strlen(payload));
}

Void startBlinkListener() {
   buttonHandle = PIN_open(&buttonState, buttonConfig);
   if (!buttonHandle) System_abort("Error initializing button pins\n");
   ledHandle = PIN_open(&ledState, ledConfig);
   if (!ledHandle) System_abort("Error initializing LED pins\n");
   if (PIN_registerIntCb(buttonHandle, &blinkListenerFxn) != 0) System_abort("Error registering button callback function");
}


Void sensorTaskFxn(UArg arg0, UArg arg1) {
   unsigned int    i;
   float        temperature;
   // 2. i2c-väylän alustus
   // RTOS:n i2c-muuttujat
   I2C_Handle      i2c;
   I2C_Params      i2cParams;
   I2C_Transaction i2cTransaction;
   // Alustetaan i2c
   I2C_Params_init(&i2cParams);
   i2cParams.bitRate = I2C_400kHz;
   i2c = I2C_open(Board_I2C_TMP, &i2cParams);
   if (i2c == NULL) {
      System_abort("Error Initializing I2C\n");
   }
   // 3. Kommunikointi i2c-väylän kautta
   // i2c-viesteille lähetys- ja vastaanottopuskurit
   // Taulukkojen koko riippu siitä kuinka monta tavua ollaan lähettämässä/vastottamassa!
   // Nämä arvot on annettu jokaisen rekisterin kohdalta datakirjassa 
   uint8_t         txBuffer[1]; // Nyt lähetetään yksi tavu
   uint8_t         rxBuffer[2]; // Nyt vastaanotetaan kaksi tavua
   // Luodaan i2c-viesti tietorakenteeseen
   // Vakio: sensorin DATAREKISTERIN osoite TMP007_REG_TEMP
   // Saadaan datakirjasta, ts. täältä kurssimateriaalista   
   // Vakio: laitteen i2c-osoite Board_TMP007_ADDR saadaan Board.h-tiedostosta
   txBuffer[0] = TMP007_REG_TEMP; // Laiterekisteri, mihin operaatio kohdistuu
   i2cTransaction.slaveAddress = Board_TMP007_ADDR; // No nyt laitteen osoite 
   i2cTransaction.writeBuf = txBuffer; // Lähetyspuskuri
   i2cTransaction.writeCount = 1; // Lähetetään yksi tavu
   i2cTransaction.readBuf = rxBuffer; // Vastaanottopuskuri
   i2cTransaction.readCount = 2; // Vastaanotetaan kaksi tavua
   for (i = 0; i < 10; i++) {
      // Lähetetään yllämääritelty i2c-viesti
      if (I2C_transfer(i2c, &i2cTransaction)) {
         // Ok, saatiin vastaus
         // Muunna luettu data rxBufferista lämpötilaksi (datakirjassa kerrotun mukaisesti)
         temperature = 0.03125f * ((rxBuffer[0] << 6) + (rxBuffer[1] >> 2));
         // System_printf("Lämpötila on %.2f C\n", temperature);
         System_printf("rxBuffer0 %d\n", rxBuffer[0]);    
         System_printf("rxBuffer1 %d\n", rxBuffer[1]);    
         //System_printf("Float test %.2f\n", 4.123f);         
         char buf[48];
         snprintf (buf, sizeof(buf), "%f", temperature);
	     System_printf("Lämpötila luettiin...%s\n", buf);         
	     System_flush();
      } else {
         System_printf("I2C Bus fault\n");
      }
      // Taskimme on kohtelias
      Task_sleep(1000000 / Clock_tickPeriod);                                                                                                                                                                                                                
   }
   // 4. i2c-yhteyden sulkeminen
   I2C_close(i2c);	
}

Void startSensorTask() {
   Task_Params params;
   Task_Handle handle;
   Task_Params_init(&params);
   params.stackSize = STACKSIZE;
   params.stack = &sensorStack; 
   params.priority = 2;
   params.arg0 = 127; 
   params.arg1 = 0xFFFF; 	
   handle = Task_create((Task_FuncPtr)sensorTaskFxn, &params, NULL);
   if (handle == NULL) System_abort("Task create failed");
}

Int main(void) {

    // Task variables
	Task_Handle labTask;
	Task_Params labTaskParams;
	Task_Handle commTask;
	Task_Params commTaskParams;

    // Initialize board
    Board_initGeneral();

	// JTKJ: Open and configure the button and led pins here
    // JTKJ: Painonappi- ja ledipinnit k�ytt��n t�ss�
    startBlinkListener();
    startHelloWorldTask();
    startDisplayTask();
    startSensorTask();

	// JTKJ: Register the interrupt handler for the button
    // JTKJ: Rekister�i painonapille keskeytyksen k�sittelij�funktio

    /* Task */
    Task_Params_init(&labTaskParams);
    labTaskParams.stackSize = STACKSIZE;
    labTaskParams.stack = &labTaskStack;
    labTaskParams.priority=2;

    labTask = Task_create(labTaskFxn, &labTaskParams, NULL);
    if (labTask == NULL) {
    	System_abort("Task create failed!");
    }

    /* Communication Task */
    Init6LoWPAN(); // This function call before use!

    Task_Params_init(&commTaskParams);
    commTaskParams.stackSize = STACKSIZE;
    commTaskParams.stack = &commTaskStack;
    commTaskParams.priority=1;

    commTask = Task_create(commTaskFxn, &commTaskParams, NULL);
    if (commTask == NULL) {
    	System_abort("Task create failed!");
    }

    System_printf("Hello again!\n");
    System_flush();
    
    /* Start BIOS */
    BIOS_start();

    return (0);
}

