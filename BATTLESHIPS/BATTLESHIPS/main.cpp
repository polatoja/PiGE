#define NOMINMAX
#include <windows.h>
#include "app_battleship.h"
#include "board_window.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE /*prevInstance*/, LPWSTR /*command_line*/, int show_command)
{
    // Create the main application instance
    app_battleship app(instance);

    // Run the main application
    int result = app.run(show_command);

    // Create and display Board windows
    BoardWindow boardMy(app.m_main, 10); // For "BATTLESHIPS - MY"
    BoardWindow boardPC(app.m_main, 10); // For "BATTLESHIPS - PC"

    // Run the message loop for Board windows
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return result;
}
