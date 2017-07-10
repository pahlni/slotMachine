/*
 * buttonDriver.c
 *
 *  Created on: Jun 22, 2017
 *      Author: Nicholas Pahl and Lourens Willekes
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "msp.h"

#include "buttonDriver.h"

int button1State = 0;
int button2State = 0;

void buttonInit(void)
{
    /* Configuring P5.7 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN7);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN7);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    MAP_Interrupt_enableInterrupt(INT_PORT5);

    /* Configuring P5.7 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    MAP_Interrupt_enableInterrupt(INT_PORT3);
}

// ISR for debouncing button 1
void button1ISR(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    if (status & GPIO_PIN7)
    {
        uint32_t debounce = 0;
        int i;
        for (i = 0; i < 10; i++)
        {
            debounce = debounce << 1;
            if (P5->IN & BIT7)
            {
                debounce |= 1;
            }
            else
            {
                break;
            }
        }

        if (debounce == 0x3ff)
        {
            button1State = 1;
        }
    }
}

// ISR for debouncing button 1
void button2ISR(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if (status & GPIO_PIN7)
    {
        uint32_t debounce = 0;
        int i;
        for (i = 0; i < 10; i++)
        {
            debounce = debounce << 1;
            if (P3->IN & BIT7)
            {
                debounce |= 1;
            }
            else
            {
                break;
            }
        }

        if (debounce == 0x3ff)
        {
            button2State = 1;
        }
    }
}

int isButton1Press(void)
{
    return button1State;
}

void clearButton1Press(void)
{
    button1State = 0;
}

int isButton2Press(void)
{
    return button2State;
}

void clearButton2Press(void)
{
    button2State = 0;
}
