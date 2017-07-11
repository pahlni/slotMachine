/*
 * draw.c
 *
 *  Created on: Jul 5, 2017
 *      Author: lourw_000
 */

#include "draw.h"
#include "handles.h"
#include "LCDDriver.h"
#include "symbols.h"


#define ICON_HEIGHT 42

// This function initializes the screen, clears it, and sets the rotation
void initScreen()
{
    ST7735_InitR(INITR_REDTAB);
    ST7735_FillScreen(ST7735_WHITE);
    ST7735_SetRotation(2);
}

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
void drawSymbol(int x, int column, int symbolIdx)
{
    if(column == 0){
        ST7735_DrawBitmap(x, COLUMN_ZERO, &SYMBOLS[symbolIdx][0], 42, 38);
    } else if(column == 1){
        ST7735_DrawBitmap(x, COLUMN_ONE, &SYMBOLS[symbolIdx][0], 42, 38);
    } else if(column == 2){
        ST7735_DrawBitmap(x, COLUMN_TWO, &SYMBOLS[symbolIdx][0], 42, 38);
    }
}

// This function will print Victory when the user wins
void drawVictory(int column)
{
    ST7735_SetRotation(3);
    char string[] = "VICTORY!";
    if(column == 0){
        ST7735_DrawString(2, 1, &string[0], ST7735_Color565(0, 255, 0));
    } else if(column == 1){
        ST7735_DrawString(2, 6, &string[0], ST7735_Color565(0, 255, 0));
    } else if(column == 2){
        ST7735_DrawString(2, 10, &string[0], ST7735_Color565(0, 255, 0));
    }
}

// This function will print Failure when the user wins
void drawFailure()
{
    ST7735_SetRotation(3);
    char string[] = "FAILURE!";
    ST7735_DrawString(2, 6, &string[0], ST7735_Color565(0, 255, 0));
}

