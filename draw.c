/*
 * draw.c
 *
 *  Created on: Jul 5, 2017
 *      Author: lourw_000
 */

#include "draw.h"
#include "handles.h"
#include "LCDTest.h"
#include "symbols.h"

// This function draws in the lines that create the columns in the slot machine
void drawDividers()
{
    int i;
    for(i = 0; i < 3; i++){
        ST7735_DrawFastHLine( 0, 38 + i, 128, ST7735_BLACK);
        ST7735_DrawFastHLine( 0, 79 + i, 128, ST7735_BLACK);
        ST7735_DrawFastHLine( 0, 120 + i, 128, ST7735_BLACK);
    }
}

// This function draws the top and bottom edges of the machine as it scrolls
void drawEdges()
{
    int i;
    for(i = 0; i < 4; i++)
    {
        ST7735_DrawFastVLine(  0 + i, 159, 126, ST7735_BLACK);
        ST7735_DrawFastVLine(123 + i, 159, 126, ST7735_BLACK);
    }
}

// This function draws the bitmap of the handle
void drawHandle(int handleIdx)
{
    ST7735_DrawBitmap(0, 156, &HANDLES[handleIdx][0], 128, 34);
}

// This function draws the corresponding symbol at the given location
void drawSymbol(int x, int y, int symbolIdx)
{
    ST7735_DrawBitmap(x, y, &SYMBOLS[symbolIdx][0], 40, 38);
}

// This function will print Victory when the user wins
void drawVictory()
{
    // get/setRotation()
}


