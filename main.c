/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
 *******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "msp.h"

#include "buttonDriver.h"
#include "LCDTest.h"
#include "ClockSystem.h"

void ClkInit(void)
{
    /* Configuring pins for peripheral/crystal usage and LED for output */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_PJ,
            GPIO_PIN3 | GPIO_PIN2,
            GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Just in case the user wants to use the getACLK, getMCLK, etc. functions,
     * let's set the clock frequency in the code.
     */
    CS_setExternalClockSourceFrequency(32000, 48000000);

    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false);

    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    //Clock_Init48MHz();
    ClkInit();

    uint32_t smclk = MAP_CS_getSMCLK();

    P6->DIR |= BIT6;
    P6->OUT |= BIT6;

    int i;

    ST7735_InitR(INITR_REDTAB);
    ST7735_FillRect(20, 20, 2, 2, ST7735_YELLOW);
    for (i = 0; i < 80; i++)
    {
        ST7735_FillRect(i, 2 * i, 2, 2, ST7735_MAGENTA);
    }
    buttonInit();

    /* Configuring P4.5 as output (on shield LED) */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    while (1)
    {
        if (isButtonPress())
        {
            clearButtonPress();
            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN5);
        }
    }
}
