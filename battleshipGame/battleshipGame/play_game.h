#pragma once

#include <windows.h>

class play_game
{
public:

	static POINT OnLButtonDown(HWND hWindow
		, POINT clickPoint
		, int my_or_pc
	);

};
