#include "app_battleship.h"
#include <stdexcept>

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
        .lpszClassName = s_class_name.c_str()
    };
    return RegisterClassExW(&desc) != 0;
}

HWND app_battleship::create_window(DWORD style, HWND parent)
{
    RECT size{ 0, 0, 600, 250 };
    AdjustWindowRectEx(&size, style, false, 0);
    return CreateWindowExW(
        0,
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
        break;
    }
    case WM_CLOSE:
        DestroyWindow(m_main);
        break;
    case WM_DESTROY:
        if (window == m_main)
            PostQuitMessage(EXIT_SUCCESS);
        break;
    default:
        return DefWindowProcW(window, message, wparam, lparam);
    }
    return 0;
}

app_battleship::app_battleship(HINSTANCE instance)
    : m_instance{ instance }, m_main{}, m_popup{}
{
    register_class();
    DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    m_main = create_window(main_style);
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
