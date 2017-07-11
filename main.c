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
#include "draw.h"
#include "ADC.h"

#define SPEED 8

void ClkInit(void);

void updateIcons(int column);

int getStoppedIcons(int column);

typedef enum slotState_e
{
    SLOT_IDLE,
    SLOT_SPIN = 1,
    SLOT_ONE = 2,
    SLOT_TWO = 3,
    SLOT_DONE = 4
} slotState_t;

typedef enum handleState_e
{
    HANDLE_IDLE, HANDLE_PULL, HANDLE_UP
} handleState_t;

// position of slot machine reel
int g_slotPosition = 0;
// slot icon values for all reels
int g_slotIcons[3][4] = { { 0, 1, 2, 0 }, { 3, 4, 5, 3 }, { 6, 7, 8, 6 } };
// state of slot machine
slotState_t g_slotState = SLOT_IDLE;
// values of stopped slot machine icons
int g_stoppedIcons[3][3];

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    handleState_t handleState = HANDLE_IDLE;
    int handlePosition = 0;
    int stopFlag = 0;

    //Clock_Init48MHz();
    ClkInit();

    //Initialize ADC for rand seeding
    initAdc();

    // Initialize LCD screen
    initScreen();

    drawDividers();

    //print all icons first
    drawSymbol(ICON_HEIGHT * 2, 0, g_slotIcons[0][0]);
    drawSymbol(ICON_HEIGHT, 0, g_slotIcons[0][1]);
    drawSymbol(0, 0, g_slotIcons[0][2]);

    drawSymbol(ICON_HEIGHT * 2, 1, g_slotIcons[1][0]);
    drawSymbol(ICON_HEIGHT, 1, g_slotIcons[1][1]);
    drawSymbol(0, 1, g_slotIcons[1][2]);

    drawSymbol(ICON_HEIGHT * 2, 2, g_slotIcons[2][0]);
    drawSymbol(ICON_HEIGHT, 2, g_slotIcons[2][1]);
    drawSymbol(0, 2, g_slotIcons[2][2]);

    drawHandle(0);

    srand(readAdc());

    buttonInit();

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    while (1)
    {
        // handle pull button check
        if (isButton1Press() && handleState == HANDLE_IDLE
                && g_slotState == SLOT_IDLE)
        {
            clearButton1Press();
            handleState = HANDLE_PULL;
            clearButton2Press();
        }

        // stop reels button check
        if (isButton2Press()
                && (g_slotState == SLOT_SPIN || g_slotState == SLOT_ONE
                        || g_slotState == SLOT_TWO))
        {
            clearButton2Press();
            stopFlag = 1;
        }

        // stops the reels in the correct spots on position for icons
        if (stopFlag)
        {
            if(getStoppedIcons(g_slotState - 1)){
                stopFlag = 0;
                g_slotState++;
            }
        }

        // checks for victory or failure
        if(g_slotState == SLOT_DONE){
            int i;
            for(i = 0; i < 3; i++){
                // check if the stopped icons are the same
                if(g_stoppedIcons[0][i] == g_stoppedIcons[1][i] && g_stoppedIcons[1][i] == g_stoppedIcons[2][i]){
                    drawVictory(i);
                    return 0;
                }
            }

            drawFailure();
            return 0;

        }

        // animates the handle pulling and returning
        if (handleState != HANDLE_IDLE && g_slotState == SLOT_IDLE)
        {
            handlePosition =
                    (handleState == HANDLE_PULL) ?
                            ++handlePosition : --handlePosition;
            drawHandle(handlePosition);

            if (handlePosition >= 5)
            {
                handleState = HANDLE_UP;
            }
            if (handlePosition <= 0)
            {
                handleState = HANDLE_IDLE;
                g_slotState = SLOT_SPIN;
            }
        }

        // spins the reels
        if (g_slotState == SLOT_SPIN)
        {
            updateIcons(0);
        }

        if (g_slotState == SLOT_SPIN || g_slotState == SLOT_ONE)
        {
            updateIcons(1);
        }

        if (g_slotState == SLOT_SPIN || g_slotState == SLOT_ONE
                || g_slotState == SLOT_TWO)
        {
            updateIcons(2);
            // advances the slot position
            g_slotPosition += SPEED;
        }

    }
}

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

// moves the reels and randomizes the upcoming icon
void updateIcons(int column)
{
    // prints all the icons depending on the slot position
    // (slotPosition + Icon height) % 170 (high of screen and one icon) - Icon height
    drawSymbol(((g_slotPosition + ICON_HEIGHT * 3) % 170) - ICON_HEIGHT, column,
               g_slotIcons[column][0]);
    drawSymbol(((g_slotPosition + ICON_HEIGHT * 2) % 170) - ICON_HEIGHT, column,
               g_slotIcons[column][1]);
    drawSymbol(((g_slotPosition + ICON_HEIGHT * 1) % 170 - ICON_HEIGHT), column,
               g_slotIcons[column][2]);
    drawSymbol((g_slotPosition % 170) - ICON_HEIGHT, column,
               g_slotIcons[column][3]);

    // randomize the upcoming icon when it is off screen
    if ((g_slotPosition % 170) <= SPEED)
    {
        g_slotIcons[column][3] = rand() % 9;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 1) % 170 <= SPEED)
    {
        g_slotIcons[column][2] = rand() % 9;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 2) % 170 <= SPEED)
    {
        g_slotIcons[column][1] = rand() % 9;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 3) % 170 <= SPEED)
    {
        g_slotIcons[column][0] = rand() % 9;
    }
}

// stores the stopped icons when they are in the correct positions
int getStoppedIcons(int column)
{
    // this is when icon 3 is off screen so store 0, 1, and 2
    if ((g_slotPosition % 170) <= SPEED)
    {
        g_stoppedIcons[column][0] = g_slotIcons[column][0];
        g_stoppedIcons[column][1] = g_slotIcons[column][1];
        g_stoppedIcons[column][2] = g_slotIcons[column][2];
        return 1;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 1) % 170 <= SPEED)
    {
        g_stoppedIcons[column][0] = g_slotIcons[column][3];
        g_stoppedIcons[column][1] = g_slotIcons[column][0];
        g_stoppedIcons[column][2] = g_slotIcons[column][1];
        return 1;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 2) % 170 <= SPEED)
    {
        g_stoppedIcons[column][0] = g_slotIcons[column][2];
        g_stoppedIcons[column][1] = g_slotIcons[column][3];
        g_stoppedIcons[column][2] = g_slotIcons[column][0];
        return 1;
    }
    else if ((g_slotPosition + ICON_HEIGHT * 3) % 170 <= SPEED)
    {
        g_stoppedIcons[column][0] = g_slotIcons[column][1];
        g_stoppedIcons[column][1] = g_slotIcons[column][2];
        g_stoppedIcons[column][2] = g_slotIcons[column][3];
        return 1;
    }
    return 0;
}
