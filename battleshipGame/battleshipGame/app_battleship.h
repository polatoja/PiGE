#pragma once
#include <windows.h>
#include <string>

#define ID_GRID_EASY 101
#define ID_GRID_MEDIUM 102
#define ID_GRID_HARD 103

class app_battleship
{
private:
	bool register_class();
	//bool register_popup_class();
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

	HWND create_board_window(
		DWORD style,
		HWND parent = nullptr,
		DWORD ex_style = 0,
		int whose = 0);

	HINSTANCE m_instance;
	HWND m_main, m_popup, pc_popup;

	HBRUSH m_field_brush;
	POINT m_screen_size;

	void on_window_move(HWND window, LPWINDOWPOS params);
	void update_transparency();
	void SetBoardSize(HWND hWnd, int width, int height);
	void DrawGridCells(HDC hdc, int numRows, int numCols);
	
	void ShowSystemMenu(HWND window);
	void SaveDifficultyLevel(const std::string& difficulty);
	std::string LoadDifficultyLevel();

	POINT m_dragStartPos;

	UINT_PTR m_timerID; // Timer ID variable
	int m_elapsedTime; // Elapsed time variable

public:
	app_battleship(HINSTANCE instance);
	int run(int show_command);
};