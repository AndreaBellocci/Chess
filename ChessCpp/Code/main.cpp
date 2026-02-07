#include <stdio.h>
#include <crtdbg.h>
#include <raylib.h>

#include "Chess.h"
#include "Libraries/Logger.h"
using namespace ENGINE_NAMESPACE;

#define APP_FILENAME "ChessInit.xml"

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	LoggerWrapper logger(L"Log_file.xml");

	InitWindow(640, 480, "Chess"); // Inizialize the window and the OpenGL contex
	LOG_MESSAGE(LOGS::L_INFO, "Game Window has been created");


#define S_TO_MILLIS(seconds) float(seconds * 1000.0f)
typedef float millis;

	ChessGame app(APP_FILENAME);
	app.OnInitialize();

	long frames = 0;
	milliseconds oldNow = 0;
	milliseconds mspf = .0f;

	// TODO: fps file depending
	const int FPS = 60;

	while (!WindowShouldClose())
	{
		// Do not update the scene if not enough time has passed since last update
		const millis timePerFrame = (FPS == 0 ? 0 : 1000.0f / FPS);
		
		// Get the app's time
		const float now = S_TO_MILLIS(GetTime());
		const auto dt = now - oldNow;

		// Update app with correct frequency
		if (!timePerFrame != 0 || (dt > timePerFrame))
		{
			oldNow = now;

			// Check for input
			app.OnInput();

			// Update and draw game state	
			app.OnUpdate(ENGINE_NAMESPACE::MilliToNano(dt));
			app.OnRender();

			mspf += S_TO_MILLIS(GetTime()) - now;

			// Update window's title once per second (i.e.: 1000 ms)
			static double lastStatsUpdate = 0;

			if (now - lastStatsUpdate >= 1000)
			{
				static char title[64];
				sprintf_s(title, 64, "Chess - %d fps %f mspf", frames, mspf / frames);
				SetWindowTitle(title);

				lastStatsUpdate = now;
				frames = 0;
				mspf = 0;
			}
			else
				frames++;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	} // Main loop
	
	CloseWindow();
	LOG_MESSAGE(LOGS::L_INFO, "Game Window has been released");
	return 0;
} // main