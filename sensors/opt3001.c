/*
 * opt3001.c
 *
 *  Created on: 22.7.2016
 *  Author: Teemu Leppanen / UBIComp / University of Oulu
 *
 *  Datakirja: http://www.ti.com/lit/ds/symlink/opt3001.pdf
 */

#include <string.h>
#include <math.h>

#include <xdc/runtime/System.h>

#include "opt3001.h"
#include "Board.h"

I2C_Transaction i2cTransaction;
char itxBuffer[4];
char irxBuffer[2];

void opt3001_setup(I2C_Handle *i2c) {

    i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
    itxBuffer[0] = OPT3001_REG_CONFIG;
    itxBuffer[1] = 0xCE; // continuous mode s.22
    itxBuffer[2] = 0x02;
    i2cTransaction.writeBuf = itxBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    if (I2C_transfer(*i2c, &i2cTransaction)) {

        System_printf("OPT3001: Config write ok\n");
    } else {
        System_printf("OPT3001: Config write failed!\n");
    }
    System_flush();

}

double opt3001_get_data(I2C_Handle *i2c) {

	uint16_t e=0;

	/* Read sensor state */
	i2cTransaction.slaveAddress = Board_OPT3001_ADDR;
	itxBuffer[0] = OPT3001_REG_CONFIG;
	i2cTransaction.writeBuf = itxBuffer;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = irxBuffer;
	i2cTransaction.readCount = 2;

	if (I2C_transfer(*i2c, &i2cTransaction)) {

		e = (irxBuffer[0] << 8) | irxBuffer[1];

	} else {

		e = 0;
		System_printf("OPT3001: Config read failed!\n");
		System_flush();
	}

	/*
	uint8_t txBuffer[ ?? ];
	uint8_t rxBuffer[ ?? ];
	*/

	/* Data available? */
	if (e & OPT3001_DATA_READY) {

		/* FILL OUT THIS DATA STRUCTURE TO GET LUX DATA
	    i2cTransaction.slaveAddress = ...
	    i2cTransaction.writeBuf = ...
	    i2cTransaction.writeCount = ...
	    i2cTransaction.readBuf = ...
	    i2cTransaction.readCount = ...
		*/

		if (I2C_transfer(*i2c, &i2cTransaction)) {

			// HERE YOU GET THE HUMIDITY VALUE FROM RXBUFFER
			// ACCORDING TO LECTURE MATERIAL

		} else {

			System_printf("OPT3001: Data read failed!\n");
			System_flush();
		}
	}

	// FIX THIS
	return -10000;
}
