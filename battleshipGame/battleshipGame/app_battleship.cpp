#include "app_battleship.h"
#include "resource.h"
#include <stdexcept>
#include <windows.h>
#include <dwmapi.h>
std::wstring const app_battleship::s_class_name{ L"2048 Window" };

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
	RECT size{ 0, 0, 600, 250 }; // Initial size of the window
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
	case WM_WINDOWPOSCHANGED:
		on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
		return 0;
	case WM_LBUTTONDOWN:
		// Capture mouse input to the window
		SetCapture(window);
		// Get the current mouse position
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		// Convert screen coordinates to client coordinates
		ScreenToClient(window, &cursorPos);
		// Store the initial mouse position for dragging
		m_dragStartPos = cursorPos;
		return 0;
	case WM_MOUSEMOVE:
		if (wparam & MK_LBUTTON)
		{
			// Calculate the delta movement of the mouse
			POINT currentPos;
			GetCursorPos(&currentPos);
			ScreenToClient(window, &currentPos);
			int deltaX = currentPos.x - m_dragStartPos.x;
			int deltaY = currentPos.y - m_dragStartPos.y;
			// Move the window accordingly
			SetWindowPos(window, nullptr, m_dragStartPos.x + deltaX, m_dragStartPos.y + deltaY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		return 0;
	case WM_LBUTTONUP:
		// Release mouse capture
		ReleaseCapture();
		return 0;
	case WM_TIMER:
	{
		++m_elapsedTime;
		wchar_t title[256];
		swprintf(title, 256, L"BATTLESHIPS - STATISTICS - Elapsed Time: %d seconds", m_elapsedTime);
		SetWindowText(window, title);
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
		SetMenu(window, hMenu);

		// Set window icon
		HICON hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(103));
		SendMessage(window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		// Start caption timer
		m_elapsedTime = 0;
		m_timerID = SetTimer(window, 1, 1000, nullptr);
		break;
	}
	}
	return DefWindowProc(window, message, wparam, lparam);
}


app_battleship::app_battleship(HINSTANCE instance)
	: m_instance{ instance }, m_main{}, m_popup{},
	m_field_brush{},
	m_screen_size{ GetSystemMetrics(SM_CXSCREEN),
	GetSystemMetrics(SM_CYSCREEN) }
{
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU |
		WS_CAPTION | WS_MINIMIZEBOX;
	DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;
	m_main = create_window(main_style);
	m_popup = create_window(popup_style, m_main, WS_EX_LAYERED);
	SetLayeredWindowAttributes(m_popup, 0, 255, LWA_ALPHA);
}

int app_battleship::run(int show_command)
{
	ShowWindow(m_main, show_command);
	ShowWindow(m_popup, SW_SHOWNA);
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
	HWND other = (window == m_main) ? m_popup : m_main;
	RECT other_rc;
	GetWindowRect(other, &other_rc);
	SIZE other_size{
		other_rc.right - other_rc.left,
		other_rc.bottom - other_rc.top
	};

	// Calculate the center of the moved window
	POINT Pw = { params->x, params->y };
	SIZE Sw = { params->cx, params->cy };
	POINT Cw = { Pw.x + Sw.cx / 2, Pw.y + Sw.cy / 2 };

	// Calculate the center of the screen
	POINT Ss_center = { m_screen_size.x / 2, m_screen_size.y / 2 };

	// Calculate the center of the other window
	POINT Co = { Ss_center.x - (Cw.x - Ss_center.x), Ss_center.y - (Cw.y - Ss_center.y) };

	// Calculate the new position of the top-left corner of the other window
	POINT Po = { Co.x - other_size.cx / 2, Co.y - other_size.cy / 2 };

	// Check if the new position is the same as the current position
	if (Po.x == other_rc.left && Po.y == other_rc.top)
		return;

	// Set the new position of the other window
	SetWindowPos(
		other,
		nullptr,
		Po.x,
		Po.y,
		0,
		0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

	update_transparency();
}

void app_battleship::update_transparency()
{
	RECT main_rc, popup_rc, intersection;
	DwmGetWindowAttribute(
		m_main,
		DWMWA_EXTENDED_FRAME_BOUNDS,
		&main_rc,
		sizeof(RECT));
	DwmGetWindowAttribute(
		m_popup,
		DWMWA_EXTENDED_FRAME_BOUNDS,
		&popup_rc,
		sizeof(RECT));
	IntersectRect(&intersection, &main_rc, &popup_rc);
	BYTE a =
		IsRectEmpty(&intersection) ? 255 : 255 * 30 / 100;
	SetLayeredWindowAttributes(m_popup, 0, a, LWA_ALPHA);
}