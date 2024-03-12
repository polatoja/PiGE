#pragma once
#include <windows.h>
#include <string>

#define ID_GRID_EASY 101
#define ID_GRID_MEDIUM 102
#define ID_GRID_HARD 103

class app_battleship {
public:
    app_battleship(HINSTANCE instance);
    int run(int show_command);

    HWND getMainWindow() const { return m_main; }

private:
    static LRESULT CALLBACK window_proc_static(
        HWND window, 
        UINT message, 
        WPARAM wparam, 
        LPARAM lparam
    );

    LRESULT window_proc(
        HWND window, 
        UINT message, 
        WPARAM wparam, 
        LPARAM lparam
    );

    bool register_class();
    HWND create_window(DWORD style, HWND parent = nullptr, DWORD ex_style = 0);

    static const std::wstring s_class_name;

    HINSTANCE m_instance;
    HWND m_main;
    HWND m_popup;
    UINT_PTR m_timerID; // Timer ID variable
    int m_elapsedTime; // Elapsed time variable

    void on_window_move(HWND window, LPWINDOWPOS params);
    void update_transparency();
};
