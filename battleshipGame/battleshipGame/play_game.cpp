#include <windows.h>
#include <dwmapi.h>

#include "play_game.h"
#include "board.h"
#include "app_battleship.h"


POINT play_game::OnLButtonDown(HWND hWindow, POINT clickPoint, int my_or_pc)
{
	int cellSpacing = app_battleship::cellSize + app_battleship::marginBetweenCells;
	int board_x = clickPoint.x - app_battleship::margin + cellSpacing;
	int board_y = clickPoint.y - app_battleship::margin + cellSpacing;

	if (board_x % cellSpacing > app_battleship::cellSize)
		board_x = -7;

	if (board_y % cellSpacing > app_battleship::cellSize)
		board_y = -7;

	if (board_x == -7 || board_y == -7)
	{
		board_x = -7;
		board_y = -7;
		MessageBeep(0);
	}
	else
	{
		board_x = (board_x / cellSpacing) - 1;
		board_y = (board_y / cellSpacing) - 1;
	}

	RECT redrawField;

	redrawField.left = app_battleship::margin + board_x * cellSpacing - 1;
	redrawField.right = redrawField.left + app_battleship::cellSize + 1;
	redrawField.top = app_battleship::margin + board_y * cellSpacing - 1;
	redrawField.bottom = redrawField.top + app_battleship::cellSize + 1;

	InvalidateRect(hWindow, &redrawField, FALSE);

	return POINT(board_x, board_y);
};
