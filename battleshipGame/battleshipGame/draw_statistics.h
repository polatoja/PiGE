#pragma once
#include <wtypes.h>

class draw_statistics
{

private:
	static const int statH = 20; // Grid height for statistics shipsquare
	static const int statW = 15;  // Grid width for statistics shipsquare
	static const int statMargin = 0; // Margin between statistics shipsquare
	static const int shipSizeMargin = 0; // Margin between statistics shipsquare
	static const int boardMargin = 3; // Margin between statistics shipsquare
	static const int statRounded = 5; // Rounding for rectangle corners
	static const int statWgap = 20; // Margin between statistics for 2 players

	static void Draw1playerShips(HDC hdc, int xoffset, int yoffset, int playerID);
	static void DrawShipSize(HDC hdc, int xoffset, int yoffset, int shipSize, int playerID);

public:
	static const int statHtotal = 5 * (statH + statMargin) + 2 * boardMargin; // Total statistics height
	static const int statWtotal = 2 * (1 + 2 + 3 + 4) * (statW + statMargin)
		+ statWgap
		+ 6 * shipSizeMargin
		+ 2 * boardMargin
		; // Total statistics width

	static void Draw(HDC hdc);
};