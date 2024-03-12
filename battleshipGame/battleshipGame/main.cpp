#define NOMINMAX
#include <windows.h>
#include "app_battleship.h"
int WINAPI wWinMain(HINSTANCE instance,
	HINSTANCE /*prevInstance*/,
	LPWSTR /*command_line*/,
	int show_command)
{
	app_battleship app{ instance };
	return app.run(show_command);
}