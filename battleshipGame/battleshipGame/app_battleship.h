#pragma once
#include "window_board.h"
#include <windows.h>
#include <string>

#define ID_GRID_EASY 101
#define ID_GRID_MEDIUM 102
#define ID_GRID_HARD 103

class app_battleship
{
private:
	bool register_class();
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	HWND create_window(
		DWORD style,
		HWND parent = nullptr,
		DWORD ex_style = 0);
	
	HINSTANCE m_instance;
	HWND m_main, m_popup;

	board m_board;
	HBRUSH m_field_brush;
	POINT m_screen_size;

	void on_window_move(HWND window, LPWINDOWPOS params);
	void update_transparency();

	POINT m_dragStartPos;

	UINT_PTR m_timerID; // Timer ID variable
	int m_elapsedTime; // Elapsed time variable

public:
	app_battleship(HINSTANCE instance);
	int run(int show_command);
};