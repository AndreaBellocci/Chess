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
	auto now = std::chrono::high_resolution_clock::now();
	auto prev = now;
	while (!WindowShouldClose() && !app.ShouldQuit())
	{
		// Check for input
		app.OnInput();

		// Update and draw game state
		app.OnUpdate(now - prev);
		app.OnRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
		// Update timings
		prev = now;
		now = std::chrono::high_resolution_clock::now();
	} // Main loop

	// Wait for 10 seconds just to show a little more the board
	if (app.ShouldQuit())
	{
		const auto start = std::chrono::high_resolution_clock::now();
		auto now = start;
		while ((now - start) < std::chrono::seconds(10))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			app.OnRender();

			now = std::chrono::high_resolution_clock::now();
		}
	}
	
	// Release Raylib resources
	CloseWindow();
	return 0;
} // main