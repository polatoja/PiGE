#include "draw_statistics.h"

void draw_statistics::DrawShipSize(HDC hdc, int xoffset, int yoffset, int shipSize, int playerID)
{
	for (int row = 0; row + shipSize < 5; row++)
	{
		for (int i = 0; i < shipSize; i++)
		{
			// Choose infosquare color based on: playerID, shipSize, row, i=vertical position in a ship
			// TODO
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SelectObject(hdc, GetStockObject(DC_PEN));
			// niebieski (113,184,255)
			// zolty (247, 213, 116)
			SetDCBrushColor(hdc, RGB(113, 184, 255));
			SetDCPenColor(hdc, RGB(247, 213, 116));

			RoundRect(hdc,
				xoffset + statMargin,
				yoffset + statMargin + i * (statMargin + statH),
				xoffset + statW,
				yoffset + statH + i * (statMargin + statH),
				statRounded, statRounded);
		}

		xoffset += statMargin + statW;
	}
}

void draw_statistics::Draw1playerShips(HDC hdc, int xoffset, int yoffset, int playerID)
{
	for (int shipSize = 4; shipSize > 0; shipSize--)
	{
		DrawShipSize(hdc, xoffset, yoffset, shipSize, playerID);
		xoffset += (5 - shipSize) * (statMargin + statW) + shipSizeMargin;
	}
};

void draw_statistics::Draw(HDC hdc)
{
	Draw1playerShips(hdc, boardMargin, boardMargin, 0);
	Draw1playerShips(hdc, 10 * (statW + statMargin) + 3 * shipSizeMargin + statWgap + boardMargin, boardMargin, 1);
};