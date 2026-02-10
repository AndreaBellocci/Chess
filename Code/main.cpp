#include <iostream>
#include <crtdbg.h>
#include <raylib.h>

#include "Chess.h"
#include "ChessApp.h"
#include "../Libraries/Logger.h"
using namespace ENGINE_NAMESPACE;

#define APP_FILENAME "../../ChessInit.xml"

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	// Initialize Raylib
	InitWindow(640, 480, "Chess"); // Inizialize the window and the OpenGL contex

	// Instantiate and initialize the app
	ChessApp app(APP_FILENAME);
	app.OnInitialize();

	// Main game loop
	while (!WindowShouldClose())
	{
		// Check for input
		app.OnInput();

		// Update and draw game state
		app.OnUpdate({});
		app.OnRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} // Main loop
	
	// Release Raylib resources
	CloseWindow();
	return 0;
} // main