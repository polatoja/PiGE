#include "draw_statistics.h"
#include "app_battleship.h"

void draw_statistics::DrawShipSize(HDC hdc, int xoffset, int yoffset, int shipSize, int playerID)
{
	for (int row = 0; row + shipSize < 5; row++)
	{
		for (int y = 0; y < shipSize; y++)
		{
			// Choose infosquare color based on: playerID, shipSize, row, y=vertical position in a ship
			// TODO: data is here:
			// app_battleship::statisticStatus[shipSize][row][y] 0/1

			int statValue = 0;

			if (playerID == 1)
				statValue = app_battleship::getStatValue(shipSize, row, y);

			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SelectObject(hdc, GetStockObject(DC_PEN));

			if (statValue == 0)
			{
				SetDCBrushColor(hdc, RGB(113, 184, 255));
			}
			else
			{
				SetDCBrushColor(hdc, RGB(212, 12, 12));
			}

			SetDCPenColor(hdc, RGB(247, 213, 116));

			RoundRect(hdc,
				xoffset + statMargin,
				yoffset + statMargin + y * (statMargin + statH),
				xoffset + statW,
				yoffset + statH + y * (statMargin + statH),
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
	app_battleship::scanStatistics();

	Draw1playerShips(hdc, boardMargin, boardMargin, 0);
	Draw1playerShips(hdc, 10 * (statW + statMargin) + 3 * shipSizeMargin + statWgap + boardMargin, boardMargin, 1);
};