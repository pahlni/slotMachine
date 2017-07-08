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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "msp.h"

#include "buttonDriver.h"
#include "LCDDriver.h"
#include "ClockSystem.h"
#include "draw.h"
#include "ADC.h"

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

typedef enum slotState_e
{
    SLOT_IDLE, SLOT_SPIN, SLOT_ONE, SLOT_TWO, SLOT_VICTORY, SLOT_FAILURE
} slotState_t;

typedef enum handleState_e
{
    HANDLE_IDLE, HANDLE_PULL, HANDLE_UP
} handleState_t;

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    slotState_t slotState = SLOT_IDLE;
    handleState_t handleState = HANDLE_IDLE;
    int handlePosition = 0;
    int slotPosition = 0;
    int slotIcons[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2 };

    //Clock_Init48MHz();
    ClkInit();

    uint32_t smclk = MAP_CS_getSMCLK();

    P6->DIR |= BIT6;
    P6->OUT |= BIT6;

    initAdc();

    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(ST7735_WHITE);

    ST7735_SetRotation(2);

    drawDividers();

    drawSymbol(82, COLUMN_TWO, slotIcons[11]);
    drawSymbol(82, COLUMN_ONE, slotIcons[10]);
    drawSymbol(82, COLUMN_ZERO, slotIcons[9]);
    drawSymbol(41, COLUMN_TWO, slotIcons[8]);
    drawSymbol(41, COLUMN_ONE, slotIcons[7]);
    drawSymbol(41, COLUMN_ZERO, slotIcons[6]);
    drawSymbol(0, COLUMN_TWO, slotIcons[5]);
    drawSymbol(0, COLUMN_ONE, slotIcons[4]);
    drawSymbol(0, COLUMN_ZERO, slotIcons[3]);

    drawHandle(0);

    srand(readAdc());

    buttonInit();

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    while (1)
    {
        if (isButton1Press() && handleState == HANDLE_IDLE
                && slotState == SLOT_IDLE)
        {
            clearButton1Press();
            handleState = HANDLE_PULL;
        }

        if (isButton2Press()
                && (slotState == SLOT_SPIN || slotState == SLOT_ONE
                        || slotState == SLOT_TWO))
        {
            clearButton2Press();
            slotState++;

        }

        if (handleState != HANDLE_IDLE && slotState == SLOT_IDLE)
        {
            handlePosition =
                    (handleState == HANDLE_PULL) ?
                            ++handlePosition : --handlePosition;
            drawHandle(handlePosition);
            handleState = (handlePosition >= 5) ? HANDLE_UP : handleState;
            handleState = (handlePosition <= 0) ? HANDLE_IDLE : handleState;
            slotState = (handleState == HANDLE_IDLE) ? SLOT_SPIN : slotState;
        }

        if (slotState == SLOT_SPIN)
        {
            drawSymbol(((slotPosition + 123) % 169) - 41, COLUMN_ZERO,
                       slotIcons[9]);
            drawSymbol(((slotPosition + 82) % 169) - 41, COLUMN_ZERO,
                       slotIcons[6]);
            drawSymbol(((slotPosition + 41) % 169 - 41), COLUMN_ZERO,
                       slotIcons[3]);
            drawSymbol((slotPosition % 169) - 41, COLUMN_ZERO, slotIcons[0]);

            if ((slotPosition % 169) <= 8)
            {
                slotIcons[0] = rand() % 9;
            }
            else if ((slotPosition + 40) % 169 <= 8)
            {
                slotIcons[3] = rand() % 9;
            }
            else if ((slotPosition + 80) % 169 <= 8)
            {
                slotIcons[6] = rand() % 9;
            }
            else if ((slotPosition + 120) % 169 <= 8)
            {
                slotIcons[9] = rand() % 9;
            }
        }

        if (slotState == SLOT_SPIN || slotState == SLOT_ONE)
        {
            drawSymbol(((slotPosition + 123) % 169) - 41, COLUMN_ONE,
                       slotIcons[10]);
            drawSymbol(((slotPosition + 82) % 169) - 41, COLUMN_ONE,
                       slotIcons[7]);
            drawSymbol(((slotPosition + 41) % 169) - 41, COLUMN_ONE,
                       slotIcons[4]);
            drawSymbol((slotPosition % 169) - 41, COLUMN_ONE, slotIcons[1]);

            slotPosition += 8;
            if ((slotPosition % 169) <= 8)
            {
                slotIcons[1] = rand() % 9;
            }
            else if ((slotPosition + 40) % 169 <= 8)
            {
                slotIcons[4] = rand() % 9;
            }
            else if ((slotPosition + 80) % 169 <= 8)
            {
                slotIcons[7] = rand() % 9;
            }
            else if ((slotPosition + 120) % 169 <= 8)
            {
                slotIcons[10] = rand() % 9;
            }
        }

        if (slotState == SLOT_SPIN || slotState == SLOT_ONE || slotState == SLOT_TWO)
        {
            drawSymbol(((slotPosition + 123) % 169) - 41, COLUMN_TWO,
                       slotIcons[11]);
            drawSymbol(((slotPosition + 82) % 169) - 41, COLUMN_TWO,
                       slotIcons[8]);
            drawSymbol(((slotPosition + 41) % 169) - 41, COLUMN_TWO,
                       slotIcons[5]);
            drawSymbol((slotPosition % 169) - 41, COLUMN_TWO, slotIcons[2]);

            slotPosition += 8;

            if ((slotPosition % 169) <= 8)
            {
                slotIcons[2] = rand() % 9;
            }
            else if ((slotPosition + 40) % 169 <= 8)
            {
                slotIcons[5] = rand() % 9;
            }
            else if ((slotPosition + 80) % 169 <= 8)
            {
                slotIcons[8] = rand() % 9;
            }
            else if ((slotPosition + 120) % 169 <= 8)
            {
                slotIcons[11] = rand() % 9;
            }
        }

    }
}
