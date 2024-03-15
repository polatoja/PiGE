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
	bool my_success = false;
	bool pc_success = false;
	bool my_valid_move = false;

	void ShowSystemMenu(HWND window);
	void SaveDifficultyLevel(const std::string& difficulty);
	std::string LoadDifficultyLevel();

	void PCMove();
	bool GameFinished();
	bool WinningGame();
	bool winner = false;

	POINT m_dragStartPos;

	UINT_PTR m_timerID = NULL; // Timer ID variable
	int m_elapsedTime; // Elapsed time variable

	static void scanHorizontal_pc_size(int shipSize);
	static void scanHorizontal_pc();
	static void scanVertical_pc_size(int shipSize);
	static void scanVertical_pc();
	static void scanShipSize1();

	static void scanHorizontal_my_size(int shipSize);
	static void scanHorizontal_my();
	static void scanVertical_my_size(int shipSize);
	static void scanVertical_my();
	static void scanShipSize1_my();

public:
	app_battleship(HINSTANCE instance);
	int run(int show_command);

	static const int cellSize = 30; // grid cell size: 30px
	static const int margin = 5;    // margin from the edge of the window: 5px
	static const int marginBetweenCells = 3; // margin between grid cells: 3px
	static const int roundRadius = 5; // radius for rounded corners

	static void scanStatistics();
	static void scanMyStatistics();

	static int getStatValue(int shipSize, int shipRow, int shipSegment);
	static int getMyStatValue(int shipSize, int shipRow, int shipSegment);
};
