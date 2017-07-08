/*
 * buttonDriver.h
 *
 *  Created on: Jun 22, 2017
 *      Author: Nicholas Pahl and Lourens Willekes
 */

#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_

void buttonInit(void);
void button1ISR(void);
void button2ISR(void);
int isButton1Press(void);
int isButton2Press(void);
void clearButton1Press(void);
void clearButton2Press(void);


#endif /* BUTTONDRIVER_H_ */
