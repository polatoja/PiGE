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

	void DrawGridCells_my(HDC hdc);
	void DrawGridCells_pc(HDC hdc);
	void CellRedraw_pc(HWND window, POINT clickPoint);

	void search_and_order(int x, int y, HWND window);
	void Ship1Sunk(POINT position, HWND window);
	void Ship2Sunk(POINT position, HWND window);
	void Ship3Sunk(POINT position, HWND window);
	void Ship4Sunk(POINT position, HWND window);
	bool success = false;

	void ShowSystemMenu(HWND window);
	void SaveDifficultyLevel(const std::string& difficulty);
	std::string LoadDifficultyLevel();

	void PCMove();

	POINT m_dragStartPos;

	UINT_PTR m_timerID = NULL; // Timer ID variable
	int m_elapsedTime; // Elapsed time variable

public:
	app_battleship(HINSTANCE instance);
	int run(int show_command);

	// Define the size of each grid cell and the number of rows and columns
	static const int cellSize = 30; // Grid cell size: 30px
	static const int margin = 5;    // Margin from the edge of the window: 5px
	static const int marginBetweenCells = 3; // Margin between grid cells: 3px
	static const int roundRadius = 5; // Radius for rounded corners
};
