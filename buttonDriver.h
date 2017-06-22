/*
 * buttonDriver.h
 *
 *  Created on: Jun 22, 2017
 *      Author: Nicholas Pahl and Lourens Willekes
 */

#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_

void buttonInit(void);
void buttonISR(void);
int isButtonPress(void);
void clearButtonPress(void);


#endif /* BUTTONDRIVER_H_ */
