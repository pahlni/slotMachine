/*
 * draw.h
 *
 *  Created on: Jul 2, 2017
 *      Author: lourw_000
 */

#ifndef DRAW_H_
#define DRAW_H_

#define ICON_HEIGHT 42

#define COLUMN_ZERO 37
#define COLUMN_ONE 78
#define COLUMN_TWO 119

void drawDividers();
void drawEdges();
void drawHandle(int handleIdx);
void drawSymbol(int x, int column, int symbolIdx);
void drawVictory(int column);
void drawFailure();


#endif /* SYMBOLS_H_ */
