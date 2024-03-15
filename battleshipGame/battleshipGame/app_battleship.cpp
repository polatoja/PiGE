#include "app_battleship.h"
#include "draw_statistics.h"
#include "board.h"
#include "play_game.h"
#include "resource.h"
#include <stdexcept>
#include <windows.h>
#include <Windowsx.h>
#include <dwmapi.h>

std::wstring const app_battleship::s_class_name{ L"Battleship Window" };

DWORD const gameboard_style = WS_OVERLAPPED | WS_CAPTION;

static std::vector<std::vector<int>> my_board;
static std::vector<std::vector<int>> pc_board;

bool app_battleship::register_class()
{
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0)
		return true;
	desc = {
	.cbSize = sizeof(WNDCLASSEXW),
	.lpfnWndProc = window_proc_static,
	.hInstance = m_instance,
	.hIcon = static_cast<HICON>(LoadImageW(m_instance, MAKEINTRESOURCEW(101), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE)),
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	.hbrBackground = CreateSolidBrush(RGB(164,174,196)),
	//.hbrBackground = CreateSolidBrush(RGB(164,64,64)),
	.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}

HWND app_battleship::create_window(DWORD style, HWND parent, DWORD ex_style)
{
	RECT size{ 0, 0, draw_statistics::statWtotal, draw_statistics::statHtotal };
	AdjustWindowRectEx(&size, style, false, 0); // Adjust the window size based on the style

	// Calculate the position of the main window
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = size.right - size.left;
	int windowHeight = size.bottom - size.top;

	int xPos = (screenWidth - windowWidth) / 2; // Center horizontally
	int yPos = screenHeight - (screenHeight / 4) - windowHeight; // 1/4 from the bottom

	return CreateWindowExW(
		ex_style,
		s_class_name.c_str(), // Class name
		L"BATTLESHIPS - STATISTICS", // Title
		style, // Window style
		xPos, // Initial X position
		yPos, // Initial Y position
		size.right - size.left, // Width
		size.bottom - size.top, // Height
		parent, // Parent window
		nullptr,
		m_instance,
		this);
}

HWND app_battleship::create_board_window(DWORD style, HWND board, DWORD ex_style, int whose)
{
	RECT size{ 0, 0, 337, 337 }; // Initial size of the window
	AdjustWindowRectEx(&size, style, false, 0); // Adjust the window size based on the style

	// Calculate the position of the board window
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = size.right - size.left;
	int windowHeight = size.bottom - size.top;

	int xPos = (screenWidth - windowWidth) / 8; // Center horizontally
	int yPos = screenHeight - (screenHeight / 4) - windowHeight; // 1/4 from the bottom

	draw_statistics::counterShip1 = 0;

	HWND hWnd;

	if (whose == 0)
	{
		int rnV1 = board::my_random(1, 10000);
		my_board = board::place_ships(10, rnV1);
		hWnd = CreateWindowExW(
			ex_style,
			s_class_name.c_str(), // Class name
			L"BATTLESHIPS - MY", // Title
			style, // Window style
			xPos, // Initial X position
			yPos, // Initial Y position
			windowWidth,
			windowHeight,
			board, // Parent window
			nullptr,
			m_instance,
			this);
	}
	else
	{
		int rnV2 = board::my_random(10000, 100000000);
		pc_board = board::place_ships(10, rnV2);
		hWnd = CreateWindowExW(
			ex_style,
			s_class_name.c_str(), // Class name
			L"BATTLESHIPS - PC", // Title
			style, // Window style
			7 * xPos, // Initial X position
			yPos, // Initial Y position
			windowWidth,
			windowHeight,
			board, // Parent window
			nullptr,
			m_instance,
			this);
	}

	return hWnd;
}

LRESULT CALLBACK app_battleship::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	app_battleship* app = nullptr;
	if (message == WM_NCCREATE)
	{
		app = static_cast<app_battleship*>(reinterpret_cast<LPCREATESTRUCTW>(lparam)->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	else
		app = reinterpret_cast<app_battleship*>(GetWindowLongPtrW(window, GWLP_USERDATA));

	return app ? app->window_proc(window, message, wparam, lparam) : DefWindowProcW(window, message, wparam, lparam);
}

LRESULT app_battleship::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	case WM_CTLCOLORSTATIC:
		return reinterpret_cast<INT_PTR>(m_field_brush);
	case WM_SYSKEYDOWN:
		// Check if Alt + F4 is pressed and the main window is focused
		if ((wparam == VK_F4) && (lparam & (1 << 29)) && (window == m_main))
		{
			// Close the application
			DestroyWindow(window);
			return 0;
		}
		if ((wparam == VK_SPACE) && (lparam & (1 << 29)) && (window == m_main))
		{
			// Show system menu
			ShowSystemMenu(window);
			return 0;
		}
		break;
	case WM_RBUTTONDOWN:
		// Check if the right mouse button is clicked on the taskbar and the main window is focused
		if ((window == m_main) && (GetForegroundWindow() == window))
		{
			// Show system menu
			ShowSystemMenu(window);
			return 0;
		}
		break;
	case WM_WINDOWPOSCHANGED:
		on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
		return 0;
	case WM_LBUTTONDOWN:
	{
		POINT clickPoint, cellRedraw;

		clickPoint.x = GET_X_LPARAM(lparam);
		clickPoint.y = GET_Y_LPARAM(lparam);
		if (window == pc_popup)
		{
			CellRedraw_pc(window, clickPoint);
		}
		
		if (!success)
		{
			PCMove();
		}

		return 0;
	}
	case WM_TIMER:
	{
		++m_elapsedTime;
		wchar_t title[256];
		swprintf(title, 256, L"BATTLESHIPS - STATISTICS - Elapsed Time: %d seconds", m_elapsedTime);
		SetWindowText(m_main, title);
		break;
	}
	case WM_CREATE:
	{
		// Add menu
		HMENU hMenu = CreateMenu();
		HMENU hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_EASY, L"Easy (10x10)");
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_MEDIUM, L"Medium (15x15)");
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_HARD, L"Hard (20x20)");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Grid Size");
		SetMenu(m_main, hMenu);

		// Set window icon
		HICON hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(103));
		SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		// Start caption timer
		m_elapsedTime = 0;
		if (m_timerID == NULL)
			m_timerID = SetTimer(window, 1, 1000, nullptr);

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		PAINTSTRUCT ps_stat;

		HDC hdc_my = BeginPaint(m_popup, &ps);
		HDC hdc_pc = BeginPaint(pc_popup, &ps);
		HDC hdc_stat = BeginPaint(m_main, &ps_stat);


		// Draw grid cells with content
		DrawGridCells_my(hdc_my);
		DrawGridCells_pc(hdc_pc);
		draw_statistics::Draw(hdc_stat);

		EndPaint(m_popup, &ps);
		EndPaint(pc_popup, &ps);
		EndPaint(m_main, &ps_stat);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wparam);

		RECT sizeE{ 0, 0, 337, 337 };
		RECT sizeM{ 0, 0, 502, 502 };
		RECT sizeH{ 0, 0, 667, 667 };
		int rnV1, rnV2;
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_GRID_EASY:
			// Set board size to 10x10
			rnV1 = board::my_random(1, 10000);
			my_board = board::place_ships(10, rnV1);
			rnV2 = board::my_random(10000, 100000000);
			pc_board = board::place_ships(10, rnV2);

			AdjustWindowRectEx(&sizeE, gameboard_style, false, 0);
			SetBoardSize(m_popup, sizeE.right - sizeE.left, sizeE.bottom - sizeE.top);
			SetBoardSize(pc_popup, sizeE.right - sizeE.left, sizeE.bottom - sizeE.top);
			SaveDifficultyLevel("Easy");
			break;
		case ID_GRID_MEDIUM:
			// Set board size to 15x15
			rnV1 = board::my_random(1, 10000);
			my_board = board::place_ships(15, rnV1);
			rnV2 = board::my_random(10000, 100000000);
			pc_board = board::place_ships(15, rnV2);

			AdjustWindowRectEx(&sizeM, gameboard_style, false, 0);
			SetBoardSize(m_popup, sizeM.right - sizeM.left, sizeM.bottom - sizeM.top);
			SetBoardSize(pc_popup, sizeM.right - sizeM.left, sizeM.bottom - sizeM.top);
			SaveDifficultyLevel("Medium");
			break;
		case ID_GRID_HARD:
			// Set board size to 20x20
			rnV1 = board::my_random(1, 10000);
			my_board = board::place_ships(20, rnV1);
			rnV2 = board::my_random(10000, 100000000);
			pc_board = board::place_ships(20, rnV2);

			AdjustWindowRectEx(&sizeH, gameboard_style, false, 0);
			SetBoardSize(m_popup, sizeH.right - sizeH.left, sizeH.bottom - sizeH.top);
			SetBoardSize(pc_popup, sizeH.right - sizeH.left, sizeH.bottom - sizeH.top);
			SaveDifficultyLevel("Hard");
			break;
			// Handle other menu options as needed
		}
		break;
	}
	}
	return DefWindowProc(window, message, wparam, lparam);
}

app_battleship::app_battleship(HINSTANCE instance)
	: m_instance{ instance }, m_main{}, m_popup{ }, pc_popup{ },
	m_field_brush{},
	m_screen_size{ GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) }
{
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU |
		WS_CAPTION | WS_MINIMIZEBOX;

	m_main = create_window(main_style);

	m_popup = create_board_window(gameboard_style, m_main, WS_EX_LAYERED, 0);
	SetLayeredWindowAttributes(m_popup, 0, 255, LWA_ALPHA);

	pc_popup = create_board_window(gameboard_style, m_main, WS_EX_LAYERED, 1);
	SetLayeredWindowAttributes(pc_popup, 0, 255, LWA_ALPHA);
}

int app_battleship::run(int show_command)
{
	ShowWindow(m_main, show_command);
	ShowWindow(m_popup, SW_SHOWNA);
	ShowWindow(pc_popup, SW_SHOWNA);

	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}

void app_battleship::on_window_move(HWND window, LPWINDOWPOS params)
{
	// Calculate the center of the moved window
	POINT Pw = { params->x, params->y };
	SIZE Sw = { params->cx, params->cy };
	POINT Cw = { Pw.x + Sw.cx / 2, Pw.y + Sw.cy / 2 };

	// Calculate the center of the screen
	POINT Ss_center = { m_screen_size.x / 2, m_screen_size.y / 2 };

	// Calculate the new position of the top-left corner of the moved window
	POINT Po = { Cw.x - Sw.cx / 2, Cw.y - Sw.cy / 2 };

	// Set the new position of the moved window
	SetWindowPos(
		window,
		nullptr,
		Po.x,
		Po.y,
		0,
		0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

	// Update the transparency for the moved window
	if (window == m_main || window == m_popup || window == pc_popup)
	{
		update_transparency();
	}
}

void app_battleship::update_transparency()
{
	RECT main_rc, popup_rc, intersection;

	GetWindowRect(m_main, &main_rc);
	GetWindowRect(m_popup, &popup_rc);

	IntersectRect(&intersection, &main_rc, &popup_rc);
	BYTE a =
		IsRectEmpty(&intersection) ? 255 : 255 * 30 / 100;
	SetLayeredWindowAttributes(m_popup, 0, a, LWA_ALPHA);
}

void app_battleship::SetBoardSize(HWND hWnd, int width, int height) {
	// Set the size of the board window
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	InvalidateRect(hWnd, NULL, TRUE);
}

void app_battleship::DrawGridCells_my(HDC hdc)
{
	// Define the size of each grid cell and the number of rows and columns
	const int cellSize = 30; // Grid cell size: 30px
	const int margin = 5;    // Margin from the edge of the window: 5px
	const int marginBetweenCells = 3; // Margin between grid cells: 3px
	const int roundRadius = 5; // Radius for rounded corners

	int numRows = my_board.size();
	int numCols = numRows;

	// Set the background color of the device context to white
	SetBkColor(hdc, RGB(255, 255, 255)); // White color

	//RECT gridRect = { 0, 0, numCols * (cellSize + marginBetweenCells) + margin, numRows * (cellSize + marginBetweenCells) + margin };
	//FillRect(hdc, &gridRect, (HBRUSH)GetStockObject(WHITE_BRUSH)); // Fill with red color

	// Example: Draw numbers in each grid cell
	for (int row = 0; row < numRows; ++row)
	{
		for (int col = 0; col < numCols; ++col)
		{
			// Calculate the position of the current cell
			int x = margin + row * (cellSize + marginBetweenCells);
			int y = margin + col * (cellSize + marginBetweenCells);

			int shipOnPos = pc_board[row][col];

			if (shipOnPos > 10 && shipOnPos < 20)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(255, 0, 0));
				SetBkColor(hdc, RGB(255, 0, 0));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else if (shipOnPos == 20)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(113, 184, 255));
				SetBkColor(hdc, RGB(113, 184, 255));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else if (shipOnPos == 21)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(247, 213, 116));
				SetBkColor(hdc, RGB(247, 213, 116));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, RGB(255, 255, 255));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}

			// Draw the background of the cell (rounded rectangle)
			RoundRect(hdc, x, y, x + cellSize, y + cellSize, roundRadius, roundRadius);

			// Draw content in the cell (e.g., numbers)
			WCHAR text[2];
			if (shipOnPos != 10)
			{
				
				if (shipOnPos > 0 && shipOnPos < 5)
				{
					swprintf(text, 2, L"%d", shipOnPos); // Example: Row-major numbering
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
				
				if (shipOnPos > 10 && shipOnPos < 20)
				{
					swprintf(text, 2, L"X"); // Example: Row-major numbering
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
				if (shipOnPos == 20)
				{
					swprintf(text, 2, L".");
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
			}
		}
	}
}

void app_battleship::DrawGridCells_pc(HDC hdc)
{
	// Define the size of each grid cell and the number of rows and columns
	const int cellSize = 30; // Grid cell size: 30px
	const int margin = 5;    // Margin from the edge of the window: 5px
	const int marginBetweenCells = 3; // Margin between grid cells: 3px
	const int roundRadius = 5; // Radius for rounded corners

	int numRows = pc_board.size();
	int numCols = numRows;

	// Set the background color of the device context to white
	SetBkColor(hdc, RGB(255, 255, 255)); // White color

	//RECT gridRect = { 0, 0, numCols * (cellSize + marginBetweenCells) + margin, numRows * (cellSize + marginBetweenCells) + margin };
	//FillRect(hdc, &gridRect, (HBRUSH)GetStockObject(WHITE_BRUSH)); // Fill with red color

	// Example: Draw numbers in each grid cell
	for (int row = 0; row < numRows; ++row)
	{
		for (int col = 0; col < numCols; ++col)
		{
			// Calculate the position of the current cell
			int x = margin + row * (cellSize + marginBetweenCells);
			int y = margin + col * (cellSize + marginBetweenCells);

			int shipOnPos = pc_board[row][col];

			if (shipOnPos > 10 && shipOnPos < 20)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(255, 0, 0));
				SetBkColor(hdc, RGB(255, 0, 0));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else if (shipOnPos == 20)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(113, 184, 255));
				SetBkColor(hdc, RGB(113, 184, 255));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else if (shipOnPos == 21)
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(247, 213, 116));
				SetBkColor(hdc, RGB(247, 213, 116));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}
			else
			{
				SelectObject(hdc, GetStockObject(DC_BRUSH));
				SelectObject(hdc, GetStockObject(DC_PEN));

				SetDCBrushColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, RGB(255, 255, 255));
				SetDCPenColor(hdc, RGB(0, 0, 0));
			}


			// Draw the background of the cell (rounded rectangle)
			RoundRect(hdc, x, y, x + cellSize, y + cellSize, roundRadius, roundRadius);

			// Draw content in the cell (e.g., numbers)
			WCHAR text[2];

			if (shipOnPos != 10)
			{
				/*
				if (shipOnPos > 0 && shipOnPos < 5)
				{
					swprintf(text, 2, L"%d", shipOnPos); // Example: Row-major numbering
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
				*/
				if (shipOnPos > 10 && shipOnPos < 20)
				{
					swprintf(text, 2, L"X"); // Example: Row-major numbering
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
				if (shipOnPos == 20)
				{
					swprintf(text, 2, L".");
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
			}
		}
	}
}

void app_battleship::ShowSystemMenu(HWND window)
{
	// Get the handle to the system menu
	HMENU hSysMenu = GetSystemMenu(window, FALSE);
	if (hSysMenu != nullptr)
	{
		// Get the current mouse position
		POINT pt;
		GetCursorPos(&pt);

		// Display the system menu at the mouse position
		TrackPopupMenu(hSysMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			pt.x, pt.y, 0, window, nullptr);
	}
}

// Save the chosen difficulty level to an INI file
void app_battleship::SaveDifficultyLevel(const std::string& difficulty)
{
	WritePrivateProfileStringA("Settings", "DifficultyLevel", difficulty.c_str(), "settings.ini");
}

// Load the chosen difficulty level from the INI file
std::string app_battleship::LoadDifficultyLevel()
{
	const int bufferSize = 256; // Adjust the buffer size as needed
	char buffer[bufferSize];
	GetPrivateProfileStringA("Settings", "DifficultyLevel", "", buffer, bufferSize, "settings.ini");
	return buffer;
}

void app_battleship::search_and_order(int neighborX, int neighborY, HWND window)
{
	if (neighborX >= 0 && neighborX < pc_board.size() && neighborY >= 0 && neighborY < pc_board.size())
	{
		int temp = pc_board[neighborX][neighborY];
		if (temp == 10)
		{
			pc_board[neighborX][neighborY] = temp + 11;

			RECT redrawField;
			int cellSpacing = app_battleship::cellSize + app_battleship::marginBetweenCells;

			redrawField.left = app_battleship::margin + neighborX * cellSpacing - 1;
			redrawField.right = redrawField.left + app_battleship::cellSize + 1;
			redrawField.top = app_battleship::margin + neighborY * cellSpacing - 1;
			redrawField.bottom = redrawField.top + app_battleship::cellSize + 1;

			InvalidateRect(window, &redrawField, FALSE);
		}
	}
}

void app_battleship::Ship1Sunk(POINT position, HWND window)
{
	// our ship position.x position.y
	int offsets[3] = { -1, 0, 1 };

	// Iterate over neighboring cells
	for (int xOffset : offsets)
	{
		for (int yOffset : offsets)
		{
			int neighborX = position.x + xOffset;
			int neighborY = position.y + yOffset;

			// Check if the neighbor cell is within bounds
			search_and_order(neighborX, neighborY, window);
		}
	}

	draw_statistics::counterShip1 += 1;
}

void app_battleship::Ship2Sunk(POINT position, HWND window)
{
	bool isSunk = false;

	// our ship position.x position.y
	int offsets[3] = { -1, 0, 1 };
	POINT position2;

	// Iterate over neighboring cells
	for (int xOffset : offsets)
	{
		for (int yOffset : offsets)
		{
			int neighborX = position.x + xOffset;
			int neighborY = position.y + yOffset;

			// Check if the neighbor cell is within bounds
			if (neighborX >= 0 && neighborX < pc_board.size() && neighborY >= 0 && neighborY < pc_board.size())
			{
				if (!(xOffset == 0 && yOffset == 0))
				{
					int temp = pc_board[neighborX][neighborY];
					if (temp == 12)
					{
						isSunk = true;
						position2.x = neighborX;
						position2.y = neighborY;
					}
				}
			}
		}
	}

	if (isSunk == true)
	{
		for (int xOffset : offsets)
		{
			for (int yOffset : offsets)
			{
				int neighborX = position.x + xOffset;
				int neighborY = position.y + yOffset;

				int neighbor2X = position2.x + xOffset;
				int neighbor2Y = position2.y + yOffset;

				// Check if the neighbor cell is within bounds
				search_and_order(neighborX, neighborY, window);
				search_and_order(neighbor2X, neighbor2Y, window);
			}
		}
	}
}

void app_battleship::Ship3Sunk(POINT position, HWND window)
{
	int counter = 0;
	int offsets[3] = { -1, 0, 1 };
	// our ship position.x position.y
	POINT positions[3];
	positions[0] = position;
	
	int x = position.x;
	int y = position.y;
	int size = pc_board.size();

	if (x + 1 < size && pc_board[x + 1][y] == 13)
	{
		counter++;
		positions[counter].x = x + 1;
		positions[counter].y = y;
	}
	if (x + 2 < size && pc_board[x + 2][y] == 13)
	{
		counter++;
		positions[counter].x = x + 2;
		positions[counter].y = y;
	}

	if (x - 1 >= 0 && pc_board[x - 1][y] == 13)
	{
		counter++;
		positions[counter].x = x - 1;
		positions[counter].y = y;
	}
	if (x - 2 >= 0 && pc_board[x - 2][y] == 13)
	{
		counter++;
		positions[counter].x = x - 2;
		positions[counter].y = y;
	}

	if (y + 1 < size && pc_board[x][y + 1] == 13)
	{
		counter++;
		positions[counter].x = x;
		positions[counter].y = y + 1;
	}
	if (y + 2 < size && pc_board[x][y + 2] == 13)
	{
		counter++;
		positions[counter].x = x;
		positions[counter].y = y + 2;
	}

	if (y - 1 >= 0 && pc_board[x][y - 1] == 13)
	{
		counter++;
		positions[counter].x = x;
		positions[counter].y = y - 1;
	}
	if (y - 2 >= 0 && pc_board[x][y - 2] == 13)
	{
		counter++;
		positions[counter].x = x;
		positions[counter].y = y - 2;
	}

	if (counter == 2)
	{
		for (int xOffset : offsets)
		{
			for (int yOffset : offsets)
			{
				for (int i = 0; i < 3; i++)
				{
					int neighborX = positions[i].x + xOffset;
					int neighborY = positions[i].y + yOffset;
					search_and_order(neighborX, neighborY, window);
				}
			}
		}
	}
}

void app_battleship::Ship4Sunk(POINT position, HWND window)
{
	bool isSunk = false;
	
	int offsets[3] = { -1, 0, 1 };
	POINT positions[4];

	int size = pc_board.size();
	int counter = 0;

	// Iterate over neighboring cells
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int temp = pc_board[i][j];
			if (temp == 14)
			{
				positions[counter].x = i;
				positions[counter].y = j;
				counter++;
			}
			if (counter == 4)
			{
				isSunk = true;
				break;
			}
		}
	}

	if (isSunk == true)
	{
		for (int xOffset : offsets)
		{
			for (int yOffset : offsets)
			{
				for (int i = 0; i < 4; i++)
				{
					int neighborX = positions[i].x + xOffset;
					int neighborY = positions[i].y + yOffset;
					search_and_order(neighborX, neighborY, window);
				}
			}
		}
	}
}

void app_battleship::PCMove()
{
	int size = my_board.size();
	POINT cellRedraw;
	bool goodChoice = false;
	int temp;

	while (goodChoice != true)
	{
		int rnVx = board::my_random(10, 1000);
		int rnVy = board::my_random(1000, 100000);

		cellRedraw.x = rnVx % size;
		cellRedraw.y = rnVy % size;
		temp = my_board[cellRedraw.x][cellRedraw.y];
		if (temp < 11)
		{
			goodChoice = true;
		}
	}

	int cellSpacing = app_battleship::cellSize + app_battleship::marginBetweenCells;
	RECT redrawField;

	redrawField.left = app_battleship::margin + cellRedraw.x * cellSpacing - 1;
	redrawField.right = redrawField.left + app_battleship::cellSize + 1;
	redrawField.top = app_battleship::margin + cellRedraw.y * cellSpacing - 1;
	redrawField.bottom = redrawField.top + app_battleship::cellSize + 1;

	InvalidateRect(m_popup, &redrawField, FALSE);

	my_board[cellRedraw.x][cellRedraw.y] = temp + 10;
	temp = my_board[cellRedraw.x][cellRedraw.y];

	if (temp == 11)
	{
		Ship1Sunk(cellRedraw, m_popup);
	}
	if (temp == 12)
	{
		Ship2Sunk(cellRedraw, m_popup);
	}
	if (temp == 13)
	{
		Ship3Sunk(cellRedraw, m_popup);
	}
	if (temp == 14)
	{
		Ship4Sunk(cellRedraw, m_popup);
	}
	
}

void app_battleship::CellRedraw_pc(HWND window, POINT clickPoint)
{
	POINT cellRedraw = play_game::OnLButtonDown(window, clickPoint, 1);
	if (cellRedraw.x != -7)
	{
		int temp = pc_board[cellRedraw.x][cellRedraw.y];
		if (temp < 11)
		{
			pc_board[cellRedraw.x][cellRedraw.y] = temp + 10;
			temp = pc_board[cellRedraw.x][cellRedraw.y];

			if (temp == 11)
			{
				Ship1Sunk(cellRedraw, pc_popup);
				success = true;
			}
			if (temp == 12)
			{
				Ship2Sunk(cellRedraw, pc_popup);
				success = true;
			}
			if (temp == 13)
			{
				Ship3Sunk(cellRedraw, pc_popup);
				success = true;
			}
			if (temp == 14)
			{
				Ship4Sunk(cellRedraw, pc_popup);
				success = true;
			}
		}
	}
}