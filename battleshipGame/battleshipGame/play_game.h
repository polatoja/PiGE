#pragma once

#include <windows.h>

class play_game
{
public:

	//static void OnLButtonDown(HWND hWindow);
	static void OnLButtonDown(HWND hWindow
		, POINT clickPoint
		, int my_or_pc
	);

};
