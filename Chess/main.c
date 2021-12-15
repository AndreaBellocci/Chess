#include <stdio.h>
#include <crtdbg.h>

#include "loader.h" 
#include "board.h"

#define INVALID -1
#define LEFT_BTN	0
#define RIGHT_BTN	1
#define MIDDLE_BTN	2

/* Assume board is centered on the window */
void GetCursorPositionOnBoard(
	int screen_width, int screen_height,
	int cell_width, int cell_height,
	int offsetX, int offsetY,
	int* row, int* column)
{
	const int x = GetMouseX();
	const int y = GetMouseY();

	/* process mouse input only if cursor is hovering on the board */
	if (screen_width > 0 && screen_height > 0 && cell_width > 0 && cell_height > 0 && row && column &&
		(offsetX < x && x < screen_width - offsetX) && (offsetY < y && y < screen_height - offsetY))
	{
		int i;
		int sel_x = INVALID;
		int sel_y = INVALID;

		for (i = 0; i <= cell_width * BOARD_SIDE; i += cell_width)
		{
			if (i + offsetX >= x)
			{
				sel_x = i;
				sel_x /= cell_width;
				break;
			}
		}

		for (i = 0; i <= cell_height * BOARD_SIDE; i += cell_height)
		{
			if (i + offsetY >= y)
			{
				sel_y = i;
				sel_y /= cell_height;
				break;
			}
		}

		*row	= sel_x - 1;	/* feel free to comment out -1 if you're more human than machine */
		*column	= sel_y - 1;	/* feel free to comment out -1 if you're more human than machine */
	}
} /* WhereCursor */

/* Assume row and col start from 0, not from 1, because today I'm feeling a very good programmer */
void ProcessMouseEvents(
	int cell_width, int cell_height,
	int offsetX, int offsetY,
	int row, int col)
{
	static int selection = INVALID;
	static int old_sel_x = INVALID;
	static int old_sel_y = INVALID;

	if (IsMouseButtonPressed(LEFT_BTN))
	{	
		if (selection != INVALID)
		{
			/* reset selection only if left-clicking on the same cell */
			if (old_sel_x == row && old_sel_y == col)
			{
				selection = INVALID;
				old_sel_x = INVALID;
				old_sel_y = INVALID;
			}
		}
		else
		{
			/* select new cell  */
			selection = 0;
			old_sel_x = row;
			old_sel_y = col;
		}
	} /* left */

	if (IsMouseButtonPressed(RIGHT_BTN))
	{
		/* wherever the cursor is, reset selection */
		selection = INVALID;
		old_sel_x = INVALID;
		old_sel_y = INVALID;
	} /* right */

	if (selection != INVALID && old_sel_x != INVALID && old_sel_y != INVALID)
	{
		Rectangle rect = { 
			offsetX + old_sel_x * cell_width,
			offsetY + old_sel_y * cell_height,
			cell_width,
			cell_height };

		DrawRectangleLinesEx(rect, 5.0f, RED);
	}
}
	
#define S_TO_MILLIS(seconds) seconds * 1000.0
typedef double millis;
void UpdateAndDrawGame(int FPS, Board board, Texture2D* textures)
{
	static long frames = 0;

	// Do not update the scene if not enough time has passed since last update
	const millis timePerFrame = (FPS == 0 ? 0 : 1000.0 / FPS);
	static double oldNow = 0;

	// Get the app's time
	const double now = S_TO_MILLIS(GetTime());

	if (!timePerFrame != 0 || (now - oldNow > timePerFrame))
	{
		oldNow = now;

		/* Update app */
		int row = INVALID, col = INVALID;
		const int screen_width = GetScreenWidth();
		const int screen_height = GetScreenHeight();

		const int step = screen_height / BOARD_SIDE; /* step is the lenght of the side of each square */
		const int start = (screen_width - (step * 8)) / 2; /* start is an offset we calculate to center the board in the window */

		GetCursorPositionOnBoard(screen_width, screen_height, step, step, start, 0, &row, &col);


		/* Update window's title once per second (i.e.: 1000 ms) */
		static double lastStatsUpdate = 0;

		if (now - lastStatsUpdate >= 1000)
		{
			static char title[64];
			sprintf_s(title, 64, "Chess - %d fps", frames);
			SetWindowTitle(title);

			lastStatsUpdate = now;
			frames = 0;
		}
		else
			frames++;


		/* draw app */
		BeginDrawing();
		ClearBackground(PURPLE);

		PrintBoard(board, textures); /* displays the board on a GUI */
		ProcessMouseEvents(step, step, start, 0, row, col);
		EndDrawing();
	}
}

int main(int argc, char* argv[]) 
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	InitWindow(640, 480, "Chess"); /* inizialize the window and the OpenGL contex */
	SetTargetFPS(30);

	Board board = InitBoard(); /* create a an empty game board in memory */

	int step = GetScreenHeight() / BOARD_SIDE; /* create and resize the textures and load them in vram */
	Texture2D* textures = LoadAssets(step);

	if (board == NULL || textures == NULL)
	{
		/* the board initalization or the texture loading failed */
		if (board) 
			DestroyBoard(board);

		if (textures)
			DestroyAssets(textures);

		CloseWindow();
		return 1;
	}

	LoadFenString(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); /* this is the initial state */
	PrintBoardOnTerminal(board); /* for debug porpuses */

	/* game loop */
	while (!WindowShouldClose())
	{
		UpdateAndDrawGame(30, board, textures);
	}

	DestroyBoard(board); /* deallocate the board */
	DestroyAssets(textures); /* deallocate the textures  */
	CloseWindow();
	return 0;
}