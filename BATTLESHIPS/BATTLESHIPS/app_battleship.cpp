#include "app_battleship.h"
#include <stdexcept>
#include <windows.h> 
#include <dwmapi.h> 

const std::wstring app_battleship::s_class_name{ L"BattleshipsStatisticsWindow" };

bool app_battleship::register_class()
{
    WNDCLASSEXW desc{};
    if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
        return true;

    desc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = window_proc_static,
        .hInstance = m_instance,
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .hbrBackground = CreateSolidBrush(RGB(164,174,196)),
        .lpszClassName = s_class_name.c_str()
    };
    return RegisterClassExW(&desc) != 0;
}


HWND app_battleship::create_window(DWORD style, HWND parent, DWORD ex_style)
{
    RECT size{ 0, 0, 600, 250 };
    AdjustWindowRectEx(&size, style, false, 0);
    return CreateWindowExW(
        ex_style,
        s_class_name.c_str(),
        L"BATTLESHIPS - STATISTICS",
        style,
        (GetSystemMetrics(SM_CXSCREEN) - (size.right - size.left)) / 2,
        (GetSystemMetrics(SM_CYSCREEN) * 3) / 4 - (size.bottom - size.top) / 2,
        size.right - size.left,
        size.bottom - size.top,
        parent,
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
    case WM_CLOSE:
        DestroyWindow(m_main);
        break;
    case WM_DESTROY:
        if (window == m_main)
            PostQuitMessage(EXIT_SUCCESS);
        break;
    case WM_TIMER:
    {
        ++m_elapsedTime;
        wchar_t title[256];
        swprintf(title, 256, L"BATTLESHIPS - STATISTICS - Elapsed Time: %d seconds", m_elapsedTime);
        SetWindowText(window, title);
        break;
    }
    case WM_WINDOWPOSCHANGED:
        on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
        return 0;
    default:
        return DefWindowProcW(window, message, wparam, lparam);
    }
    return 0;
}

app_battleship::app_battleship(HINSTANCE instance)
    : m_instance{ instance }, m_main{}, m_popup{}, m_timerID{}, m_elapsedTime{}
{
    register_class();
    DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;

    m_main = create_window(main_style);
    m_popup = create_window(
        popup_style, m_main, WS_EX_LAYERED);
    SetLayeredWindowAttributes(m_popup, 0, 255, LWA_ALPHA);
}

int app_battleship::run(int show_command)
{
    ShowWindow(m_main, show_command);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return msg.wParam;
}

void app_battleship::on_window_move(
    HWND window,
    LPWINDOWPOS params)
{
    HWND other = (window == m_main) ? m_popup : m_main;
    RECT other_rc;
    GetWindowRect(other, &other_rc);
    SIZE other_size{
    .cx = other_rc.right - other_rc.left,
    .cy = other_rc.bottom -other_rc.top };
    POINT new_pos{
        /*calculate the new position of the other window*/
    };
    if (new_pos.x == other_rc.left &&
        new_pos.y == other_rc.top)
        return;
    SetWindowPos(
        other,
        nullptr,
        new_pos.x,
        new_pos.y,
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