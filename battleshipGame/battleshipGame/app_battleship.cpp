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
	.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}

HWND app_battleship::create_window(DWORD style, HWND parent, DWORD ex_style)
{
	RECT size{ 0, 0, draw_statistics::statWtotal, draw_statistics::statHtotal };
	AdjustWindowRectEx(&size, style, false, 0);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = size.right - size.left;
	int windowHeight = size.bottom - size.top;

	int xPos = (screenWidth - windowWidth) / 2;
	int yPos = screenHeight - (screenHeight / 4) - windowHeight; 

	HWND window = CreateWindowExW(
		ex_style | WS_EX_LAYERED,
		s_class_name.c_str(),
		L"BATTLESHIPS - STATISTICS",
		style, 
		xPos,
		yPos,
		size.right - size.left, 
		size.bottom - size.top, 
		parent,
		nullptr,
		m_instance,
		this);

	SetLayeredWindowAttributes(window, 0, (255 * 30) / 100, LWA_ALPHA);
	
	return window;
}

HWND app_battleship::create_board_window(DWORD style, HWND board, DWORD ex_style, int whose)
{
	RECT size{ 0, 0, 337, 337 };
	AdjustWindowRectEx(&size, style, false, 0);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = size.right - size.left;
	int windowHeight = size.bottom - size.top;

	int xPos = (screenWidth - windowWidth) / 8;
	int yPos = screenHeight - (screenHeight / 4) - windowHeight;

	HWND hWnd;

	if (whose == 0)
	{
		int rnV1 = board::my_random(1, 10000);
		my_board = board::place_ships(10, rnV1);
		hWnd = CreateWindowExW(
			ex_style,
			s_class_name.c_str(),
			L"BATTLESHIPS - MY",
			style,
			xPos,
			yPos,
			windowWidth,
			windowHeight,
			board,
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
			s_class_name.c_str(),
			L"BATTLESHIPS - PC",
			style,
			7 * xPos, 
			yPos, 
			windowWidth,
			windowHeight,
			board,
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
		// Alt + F4
		if ((wparam == VK_F4) && (lparam & (1 << 29)) && (window == m_main))
		{
			DestroyWindow(window);
			return 0;
		}
		// Alt + space
		if ((wparam == VK_SPACE) && (lparam & (1 << 29)) && (window == m_main))
		{
			ShowSystemMenu(window);
			return 0;
		}
		break;
	case WM_RBUTTONDOWN:
	{
		// right mouse button
		if ((window == m_main) && (GetForegroundWindow() == window))
		{
			ShowSystemMenu(window);
			return 0;
		}
		break;
	}
	case WM_WINDOWPOSCHANGED:
		on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
		return 0;
	case WM_LBUTTONDOWN:
	{
		if (!winner)
		{
			POINT clickPoint, cellRedraw;

			clickPoint.x = GET_X_LPARAM(lparam);
			clickPoint.y = GET_Y_LPARAM(lparam);
			if (window == pc_popup)
			{
				CellRedraw_pc(window, clickPoint);
			}
			pc_success = true;
			if (!my_success && my_valid_move)
			{
				while (pc_success)
				{
					PCMove();
				}
			}

			bool finished = GameFinished();
			if (finished)
			{
				winner = WinningGame();
			}

			return 0;
		}
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
		HMENU hMenu = CreateMenu();
		HMENU hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_EASY, L"Easy (10x10)");
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_MEDIUM, L"Medium (15x15)");
		AppendMenuW(hSubMenu, MF_STRING, ID_GRID_HARD, L"Hard (20x20)");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Grid Size");
		SetMenu(m_main, hMenu);

		HICON hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(103));
		SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		m_elapsedTime = 0;
		if (m_timerID == NULL)
			m_timerID = SetTimer(window, 1, 1000, nullptr);

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		PAINTSTRUCT ps_stat;

		InvalidateRect(m_main, NULL, TRUE);

		HDC hdc_my = BeginPaint(m_popup, &ps);
		HDC hdc_pc = BeginPaint(pc_popup, &ps);
		HDC hdc_stat = BeginPaint(m_main, &ps_stat);

		DrawGridCells_my(hdc_my);
		DrawGridCells_pc(hdc_pc);
		draw_statistics::Draw(hdc_stat);

		if (GameFinished())
		{
			if (winner)
			{
				HDC hdc_popup = GetDC(pc_popup);
				HDC greenbitmap = CreateCompatibleDC(hdc_popup);

				HBITMAP hBitmap = CreateCompatibleBitmap(hdc_popup, 500, 500);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(greenbitmap, hBitmap);

				SelectObject(greenbitmap, GetStockObject(DC_BRUSH));
				SelectObject(greenbitmap, GetStockObject(DC_PEN));
				SetBkColor(greenbitmap, RGB(0, 255, 0)); // Green color
				SetDCBrushColor(greenbitmap, RGB(0, 255, 0));
				SetDCPenColor(greenbitmap, RGB(0, 0, 0));

				RoundRect(greenbitmap, 0, 0, 500, 500, 1, 1);

				BLENDFUNCTION blendFunction;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.BlendFlags = 0;
				blendFunction.SourceConstantAlpha = 200;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;

				AlphaBlend(hdc_popup,
					0, 0, 1000, 1000,
					greenbitmap,
					0, 0, 500, 500,
					blendFunction
				);

				SetTextColor(hdc_popup, RGB(0, 0, 0));
				SetBkMode(hdc_popup, TRANSPARENT);

				HFONT hFont = CreateFont(35, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
					CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
				
				SelectObject(hdc_popup, hFont);

				SIZE textSize;
				GetTextExtentPoint32(hdc_popup, L"Congratulations!", 17, &textSize);

				int size = my_board.size() * 30 + (my_board.size() - 1) * 3 + 10;
				
				int textX = (size - textSize.cx) / 2;
				int textY = (size - textSize.cy) / 2;
				TextOut(hdc_popup, textX, textY, L"Congratulations!", 17);

				SelectObject(greenbitmap, hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(greenbitmap);
				ReleaseDC(pc_popup, hdc_popup);
			}

			else
			{
				HDC hdc_popup = GetDC(pc_popup);
				HDC greenbitmap = CreateCompatibleDC(hdc_popup);

				HBITMAP hBitmap = CreateCompatibleBitmap(hdc_popup, 500, 500);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(greenbitmap, hBitmap);

				SelectObject(greenbitmap, GetStockObject(DC_BRUSH));
				SelectObject(greenbitmap, GetStockObject(DC_PEN));
				SetBkColor(greenbitmap, RGB(0, 0, 255));
				SetDCBrushColor(greenbitmap, RGB(0, 0, 255));
				SetDCPenColor(greenbitmap, RGB(0, 0, 0));

				RoundRect(greenbitmap, 0, 0, 500, 500, 1, 1);

				BLENDFUNCTION blendFunction;
				blendFunction.BlendOp = AC_SRC_OVER;
				blendFunction.BlendFlags = 0;
				blendFunction.SourceConstantAlpha = 200;
				blendFunction.AlphaFormat = AC_SRC_ALPHA;

				AlphaBlend(hdc_popup,
					0, 0, 1000, 1000,
					greenbitmap,
					0, 0, 500, 500,
					blendFunction
				);

				SetTextColor(hdc_popup, RGB(0, 0, 0));
				SetBkMode(hdc_popup, TRANSPARENT);

				HFONT hFont = CreateFont(35, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
					CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

				SelectObject(hdc_popup, hFont);

				SIZE textSize;
				textSize.cx = 80;
				textSize.cy = 10;

				int size = my_board.size() * 30 + (my_board.size() - 1) * 3 + 10;
				// Calculate the position to center the text
				int textX = (size - textSize.cx) / 2;
				int textY = (size - textSize.cy) / 2;
				TextOut(hdc_popup, textX, textY, L"Loser", 17);

				// Release resources
				SelectObject(greenbitmap, hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(greenbitmap);
				ReleaseDC(pc_popup, hdc_popup);
			}
		}

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

		switch (wmId)
		{
		case ID_GRID_EASY:
			// board size: 10x10
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
			// board size: 15x15
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
			// board size: 20x20
			rnV1 = board::my_random(1, 10000);
			my_board = board::place_ships(20, rnV1);
			rnV2 = board::my_random(10000, 100000000);
			pc_board = board::place_ships(20, rnV2);

			AdjustWindowRectEx(&sizeH, gameboard_style, false, 0);
			SetBoardSize(m_popup, sizeH.right - sizeH.left, sizeH.bottom - sizeH.top);
			SetBoardSize(pc_popup, sizeH.right - sizeH.left, sizeH.bottom - sizeH.top);
			SaveDifficultyLevel("Hard");
			break;
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
	POINT Pw = { params->x, params->y };
	SIZE Sw = { params->cx, params->cy };
	POINT Cw = { Pw.x + Sw.cx / 2, Pw.y + Sw.cy / 2 };

	POINT Ss_center = { m_screen_size.x / 2, m_screen_size.y / 2 };

	POINT Po = { Cw.x - Sw.cx / 2, Cw.y - Sw.cy / 2 };

	SetWindowPos(
		window,
		nullptr,
		Po.x,
		Po.y,
		0,
		0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	
	if (window == m_main || window == m_popup || window == pc_popup)
	{
		update_transparency();
	}
	
}

void app_battleship::update_transparency()
{
	SetLayeredWindowAttributes(m_main, 0, 255 * 70 / 100, LWA_ALPHA);
}

void app_battleship::SetBoardSize(HWND hWnd, int width, int height) {
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	InvalidateRect(hWnd, NULL, TRUE);
}

void app_battleship::DrawGridCells_my(HDC hdc)
{
	const int cellSize = 30;
	const int margin = 5;
	const int marginBetweenCells = 3;
	const int roundRadius = 5;

	int numRows = my_board.size();
	int numCols = numRows;

	SetBkColor(hdc, RGB(255, 255, 255)); 

	for (int row = 0; row < numRows; ++row)
	{
		for (int col = 0; col < numCols; ++col)
		{
			int x = margin + row * (cellSize + marginBetweenCells);
			int y = margin + col * (cellSize + marginBetweenCells);

			int shipOnPos = my_board[row][col];

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

			RoundRect(hdc, x, y, x + cellSize, y + cellSize, roundRadius, roundRadius);

			WCHAR text[2];
			if (shipOnPos != 10)
			{
				
				if (shipOnPos > 0 && shipOnPos < 5)
				{
					swprintf(text, 2, L"%d", shipOnPos);
					TextOut(hdc, x + cellSize / 2 - 5, y + cellSize / 2 - 5, text, lstrlen(text));
				}
				
				if (shipOnPos > 10 && shipOnPos < 20)
				{
					swprintf(text, 2, L"X");
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
	const int cellSize = 30;
	const int margin = 5;
	const int marginBetweenCells = 3;
	const int roundRadius = 5;

	int numRows = pc_board.size();
	int numCols = numRows;

	SetBkColor(hdc, RGB(255, 255, 255));

	for (int row = 0; row < numRows; ++row)
	{
		for (int col = 0; col < numCols; ++col)
		{
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

			RoundRect(hdc, x, y, x + cellSize, y + cellSize, roundRadius, roundRadius);

			WCHAR text[2];

			if (shipOnPos != 10)
			{	
				if (shipOnPos > 10 && shipOnPos < 20)
				{
					swprintf(text, 2, L"X");
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
	HMENU hSysMenu = GetSystemMenu(window, FALSE);
	if (hSysMenu != nullptr)
	{
		POINT pt;
		GetCursorPos(&pt);

		TrackPopupMenu(hSysMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			pt.x, pt.y, 0, window, nullptr);
	}
}

void app_battleship::SaveDifficultyLevel(const std::string& difficulty)
{
	WritePrivateProfileStringA("Settings", "DifficultyLevel", difficulty.c_str(), "settings.ini");
}

std::string app_battleship::LoadDifficultyLevel()
{
	const int bufferSize = 256;
	char buffer[bufferSize];
	GetPrivateProfileStringA("Settings", "DifficultyLevel", "", buffer, bufferSize, "settings.ini");
	return buffer;
}

void app_battleship::search_and_order(int neighborX, int neighborY, HWND window)
{
	int size = pc_board.size();
	int temp = 1;

	if (neighborX >= 0 && neighborX < size && neighborY >= 0 && neighborY < size)
	{
		if(window == pc_popup)
			temp = pc_board[neighborX][neighborY];
		if(window == m_popup)
			temp = my_board[neighborX][neighborY];

		if (temp == 10)
		{
			if (window == pc_popup)
				pc_board[neighborX][neighborY] = temp + 11;
			if (window == m_popup)
				my_board[neighborX][neighborY] = temp + 11;

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
	int offsets[3] = { -1, 0, 1 };

	for (int xOffset : offsets)
	{
		for (int yOffset : offsets)
		{
			int neighborX = position.x + xOffset;
			int neighborY = position.y + yOffset;

			search_and_order(neighborX, neighborY, window);
		}
	}
}

void app_battleship::Ship2Sunk(POINT position, HWND window)
{
	bool isSunk = false;

	// our ship position.x position.y
	int offsets[3] = { -1, 0, 1 };
	POINT position2;
	int temp = 1;

	for (int xOffset : offsets)
	{
		for (int yOffset : offsets)
		{
			int neighborX = position.x + xOffset;
			int neighborY = position.y + yOffset;

			if (neighborX >= 0 && neighborX < pc_board.size() && neighborY >= 0 && neighborY < pc_board.size())
			{
				if (!(xOffset == 0 && yOffset == 0))
				{
					if (window == pc_popup)
						temp = pc_board[neighborX][neighborY];
					if (window == m_popup)
						temp = my_board[neighborX][neighborY];

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
	if (window == pc_popup)
	{
		if (x + 1 < size && pc_board[x + 1][y] == 13)
		{
			counter++;
			positions[counter].x = x + 1;
			positions[counter].y = y;

			if (x + 2 < size && pc_board[x + 2][y] == 13)
			{
				counter++;
				positions[counter].x = x + 2;
				positions[counter].y = y;
			}
		}

		if (x - 1 >= 0 && pc_board[x - 1][y] == 13)
		{
			counter++;
			positions[counter].x = x - 1;
			positions[counter].y = y;

			if (x - 2 >= 0 && pc_board[x - 2][y] == 13)
			{
				counter++;
				positions[counter].x = x - 2;
				positions[counter].y = y;
			}
		}

		if (y + 1 < size && pc_board[x][y + 1] == 13)
		{
			counter++;
			positions[counter].x = x;
			positions[counter].y = y + 1;

			if (y + 2 < size && pc_board[x][y + 2] == 13)
			{
				counter++;
				positions[counter].x = x;
				positions[counter].y = y + 2;
			}
		}

		if (y - 1 >= 0 && pc_board[x][y - 1] == 13)
		{
			counter++;
			positions[counter].x = x;
			positions[counter].y = y - 1;

			if (y - 2 >= 0 && pc_board[x][y - 2] == 13)
			{
				counter++;
				positions[counter].x = x;
				positions[counter].y = y - 2;
			}
		}
	}
	
	if (window == m_popup)
	{
		if (x + 1 < size && my_board[x + 1][y] == 13)
		{
			counter++;
			positions[counter].x = x + 1;
			positions[counter].y = y;

			if (x + 2 < size && my_board[x + 2][y] == 13)
			{
				counter++;
				positions[counter].x = x + 2;
				positions[counter].y = y;
			}
		}

		if (x - 1 >= 0 && my_board[x - 1][y] == 13)
		{
			counter++;
			positions[counter].x = x - 1;
			positions[counter].y = y;

			if (x - 2 >= 0 && my_board[x - 2][y] == 13)
			{
				counter++;
				positions[counter].x = x - 2;
				positions[counter].y = y;
			}
		}

		if (y + 1 < size && my_board[x][y + 1] == 13)
		{
			counter++;
			positions[counter].x = x;
			positions[counter].y = y + 1;

			if (y + 2 < size && my_board[x][y + 2] == 13)
			{
				counter++;
				positions[counter].x = x;
				positions[counter].y = y + 2;
			}
		}

		if (y - 1 >= 0 && my_board[x][y - 1] == 13)
		{
			counter++;
			positions[counter].x = x;
			positions[counter].y = y - 1;

			if (y - 2 >= 0 && my_board[x][y - 2] == 13)
			{
				counter++;
				positions[counter].x = x;
				positions[counter].y = y - 2;
			}
		}
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
	int temp;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (window == pc_popup)
				temp = pc_board[i][j];
			if (window == m_popup)
				temp = my_board[i][j];

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
	pc_success = false;

	if (temp == 11)
	{
		Ship1Sunk(cellRedraw, m_popup);
		pc_success = true;
	}
	if (temp == 12)
	{
		Ship2Sunk(cellRedraw, m_popup);
		pc_success = true;
	}
	if (temp == 13)
	{
		Ship3Sunk(cellRedraw, m_popup);
		pc_success = true;
	}
	if (temp == 14)
	{
		Ship4Sunk(cellRedraw, m_popup);
		pc_success = true;
	}
	
}

void app_battleship::CellRedraw_pc(HWND window, POINT clickPoint)
{
	POINT cellRedraw = play_game::OnLButtonDown(window, clickPoint, 1);
	my_success = false;
	my_valid_move = false;

	if (cellRedraw.x != -7)
	{
		int temp = pc_board[cellRedraw.x][cellRedraw.y];
		if (temp < 11)
		{
			my_valid_move = true;
			pc_board[cellRedraw.x][cellRedraw.y] = temp + 10;
			temp = pc_board[cellRedraw.x][cellRedraw.y];

			if (temp == 11)
			{
				Ship1Sunk(cellRedraw, pc_popup);
				my_success = true;
			}
			if (temp == 12)
			{
				Ship2Sunk(cellRedraw, pc_popup);
				my_success = true;
			}
			if (temp == 13)
			{
				Ship3Sunk(cellRedraw, pc_popup);
				my_success = true;
			}
			if (temp == 14)
			{
				Ship4Sunk(cellRedraw, pc_popup);
				my_success = true;
			}
		}
	}
}

bool app_battleship::GameFinished()
{
	bool finished = false;
	int my_left = 0, pc_left = 0;
	int size = my_board.size();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int my_temp = my_board[i][j];
			int pc_temp = pc_board[i][j];

			if (my_temp < 5)
			{
				pc_left++;
			}
			if (pc_temp < 5)
			{
				my_left++;
			}
		}
	}

	if (my_left == 0 || pc_left == 0)
		finished = true;

	return finished;
}

bool app_battleship::WinningGame()
{
	bool won = false;
	int my_left = 0, pc_left = 0;
	int size = my_board.size();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int my_temp = my_board[i][j];
			int pc_temp = pc_board[i][j];

			if (my_temp < 5)
			{
				pc_left++;
			}
			if (pc_temp < 5)
			{
				my_left++;
			}
		}
	}

	if (my_left == 0 && pc_left > 0)
		won = true;

	return won;
}

static std::vector<std::vector<std::vector<int>>> statisticStatus;
static std::vector<std::vector<std::vector<int>>> statisticStatus_my;

void app_battleship::scanShipSize1()
{
	int shipNumber = 0;

	for (int x = 0; x < pc_board.size(); x++)
	{
		for (int y = 0; y < pc_board.size(); y++)
		{
			int cellValue = pc_board[x][y];
			int fire = -1;

			if (pc_board[x][y] == 11)
			{
				fire = 1;
				statisticStatus[1][shipNumber][0] = 1;
				shipNumber++;
			}

		}
	}

}

void app_battleship::scanShipSize1_my()
{
	int shipNumber = 0;

	for (int x = 0; x < my_board.size(); x++)
	{
		for (int y = 0; y < my_board.size(); y++)
		{
			int cellValue = my_board[x][y];
			int fire = -1;

			if(my_board[x][y] == 11)
			{
				fire = 1;
				statisticStatus_my[1][shipNumber][0] = 1;
				shipNumber++;
			}

		}
	}

}

void app_battleship::scanVertical_pc_size(int shipSize)
{
	int shipNumber = -1;
	int shipSegment = -1;
	int newFire = -1;

	for (int x = 0; x < pc_board.size(); x++)
	{
		shipSegment = 0;
		newFire = 1;

		for (int y = 0; y < pc_board.size(); y++)
		{
			int cellValue = pc_board[x][y];
			int fire = -1;

			if (
				(pc_board[x][y] == (10 + shipSize)) &&
				(
					(y + 1 < pc_board.size()) &&
					((pc_board[x][y + 1] == (10 + shipSize)) || (pc_board[x][y + 1] == (shipSize)))
					||
					(y - 1 > 0) &&
					((pc_board[x][y - 1] == (10 + shipSize)) || (pc_board[x][y - 1] == (shipSize)))
					)
				)
				fire = 1;
			else
				fire = 0;


			if (fire == 1)
			{
				if (newFire == 1)
				{
					newFire = 0;
					shipNumber++;
				}

				statisticStatus[shipSize][shipNumber][shipSegment] = fire;
				shipSegment++;
			}
			else
			{
				newFire = 1;
				shipSegment = 0;
			}

		}
	}

}

void app_battleship::scanVertical_my_size(int shipSize)
{
	int shipNumber = -1;
	int shipSegment = -1;
	int newFire = -1;

	for (int x = 0; x < my_board.size(); x++)
	{
		shipSegment = 0;
		newFire = 1;

		for (int y = 0; y < my_board.size(); y++)
		{
			int cellValue = my_board[x][y];
			int fire = -1;

			if (
				(my_board[x][y] == (10 + shipSize)) &&
				(
					(y + 1 < my_board.size()) &&
					((my_board[x][y + 1] == (10 + shipSize)) || (my_board[x][y + 1] == (shipSize)))
					||
					(y - 1 > 0) &&
					((my_board[x][y - 1] == (10 + shipSize)) || (my_board[x][y - 1] == (shipSize)))
					)
				)
				fire = 1;
			else
				fire = 0;


			if (fire == 1)
			{
				if (newFire == 1)
				{
					newFire = 0;
					shipNumber++;
				}

				statisticStatus_my[shipSize][shipNumber][shipSegment] = fire;
				shipSegment++;
			}
			else
			{
				newFire = 1;
				shipSegment = 0;
			}

		}
	}

}

void app_battleship::scanVertical_pc()
{

	scanVertical_pc_size(4);
	scanVertical_pc_size(3);
	scanVertical_pc_size(2);
}

void app_battleship::scanVertical_my()
{

	scanVertical_my_size(4);
	scanVertical_my_size(3);
	scanVertical_my_size(2);
}

void app_battleship::scanHorizontal_pc_size(int shipSize)
{
	int shipNumber = -1;
	int shipSegment = -1;
	int newFire = -1;

	for (int y = 0; y < pc_board.size(); y++)
	{
		shipSegment = 0;
		newFire = 1;

		for (int x = 0; x < pc_board.size(); x++)
		{
			int cellValue = pc_board[x][y];
			int fire = -1;

			if (
				(pc_board[x][y] == (10 + shipSize)) &&
				(
					(x + 1 < pc_board.size()) &&
					((pc_board[x + 1][y] == (10 + shipSize)) || (pc_board[x + 1][y] == (shipSize)))
					||
					(x - 1 > 0) &&
					((pc_board[x - 1][y] == (10 + shipSize)) || (pc_board[x - 1][y] == (shipSize)))
					)
				)
				fire = 1;
			else
				fire = 0;


			if (fire == 1)
			{
				if (newFire == 1)
				{
					newFire = 0;
					shipNumber++;

					while (statisticStatus[shipSize][shipNumber][shipSegment] == 1)
						shipNumber++;
				}

				statisticStatus[shipSize][shipNumber][shipSegment] = fire;
				shipSegment++;
			}
			else
			{
				newFire = 1;
				shipSegment = 0;
			}

		}
	}

}

void app_battleship::scanHorizontal_my_size(int shipSize)
{
	int shipNumber = -1;
	int shipSegment = -1;
	int newFire = -1;

	for (int y = 0; y < my_board.size(); y++)
	{
		shipSegment = 0;
		newFire = 1;

		for (int x = 0; x < my_board.size(); x++)
		{
			int cellValue = my_board[x][y];
			int fire = -1;

			if (
				(my_board[x][y] == (10 + shipSize)) &&
				(
					(x + 1 < my_board.size()) &&
					((my_board[x + 1][y] == (10 + shipSize)) || (my_board[x + 1][y] == (shipSize)))
					||
					(x - 1 > 0) &&
					((my_board[x - 1][y] == (10 + shipSize)) || (my_board[x - 1][y] == (shipSize)))
					)
				)
				fire = 1;
			else
				fire = 0;


			if (fire == 1)
			{
				if (newFire == 1)
				{
					newFire = 0;
					shipNumber++;

					while (statisticStatus_my[shipSize][shipNumber][shipSegment] == 1)
						shipNumber++;
				}

				statisticStatus_my[shipSize][shipNumber][shipSegment] = fire;
				shipSegment++;
			}
			else
			{
				newFire = 1;
				shipSegment = 0;
			}

		}
	}

}

void app_battleship::scanHorizontal_pc()
{

	scanHorizontal_pc_size(4);
	scanHorizontal_pc_size(3);
	scanHorizontal_pc_size(2);
}

void app_battleship::scanHorizontal_my()
{

	scanHorizontal_my_size(4);
	scanHorizontal_my_size(3);
	scanHorizontal_my_size(2);
}

void app_battleship::scanStatistics()
{
	statisticStatus = std::vector<std::vector<std::vector<int>>>
		(5, std::vector<std::vector<int>>(5, std::vector<int>(5, 0)));

	scanVertical_pc();
	scanHorizontal_pc();
	scanShipSize1();
}

void app_battleship::scanMyStatistics()
{
	statisticStatus_my = std::vector<std::vector<std::vector<int>>>
		(5, std::vector<std::vector<int>>(5, std::vector<int>(5, 0)));

	scanVertical_my();
	scanHorizontal_my();
	scanShipSize1_my();
}

int app_battleship::getStatValue(int shipSize, int shipRow, int shipSegment)
{
	return statisticStatus[shipSize][shipRow][shipSegment];
}

int app_battleship::getMyStatValue(int shipSize, int shipRow, int shipSegment)
{
	return statisticStatus_my[shipSize][shipRow][shipSegment];
}