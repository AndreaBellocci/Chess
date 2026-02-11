# Chess (C++ / Raylib)

## Overview
This is a C++ chess game built with an engine-style architecture: a clean game state core, event-driven input and logic flow, and a Raylib-powered 2D view. The rules layer implements full move generation for every piece, with validation for check safety, castling, en passant, and promotion. The UI includes board rendering, move highlighting, and a dedicated promotion picker.

I personally wrote every single line of code in this repository. No AI tools were used (except for this very readme), so if there are any bugs, it's a totally intended feature :)

## Key Features
- Event-driven game loop with decoupled input, logic, and rendering.
- Full move-generation and rule validation per piece.
- Check safety, castling (including Chess960 notes), en passant, and pawn promotion.
- FEN parsing for initial game state setup.
- Simple animation/process system for visual transitions.

## Architecture Highlights
- Core state in `Chess` and `ChessGame` (board, pieces, turn, castling rights, en passant, move counters).
- Rules layer in `Code/Pieces/*.cpp` with shared helpers in `Code/Pieces/piece_utilities.cpp`.
- Event system in `Libraries/EventManager.*` with a queue-based dispatcher.
- Presentation in `Code/GameViews.*` using Raylib and a promotion overlay.
- Tooling: XML logger and thread-safe utilities in `Libraries/`.

## Project Structure
- `Code/` gameplay, rules, and views
- `Libraries/` engine helpers (events, processes, logging, concurrency)
- `assets/` graphics and other assets

## Background
The original version started in December 2021 as a fun challenge with a friend, but soon evolved into a self-learning project inspired by the book "Game Coding Complete (4th Edition)". The original code is included for reference in branch `master`, and besides a few changes, it has remained as it was five years ago.

The current development branch (`new_dev`) focuses on refactoring the rule algorithms, improving separation of concerns, and modernizing the gameplay layer. This code is the result of university-level and professional-level experience that I did not have back in 2021, and is much cleaner and easier to read, maintain and extend. As of February 2026, the code allows to play and entire match, hopefully without bugs :). More features, as shown below, may come in the future.

## Roadmap
- More testing and debugging
- UI menu and in-game settings.
- Save/load games and replay support.
- Undo/redo move history.
- Sound effects.
- Improved AI (alpha-beta pruning) and possible RL experiments.
- Optional networking for multiplayer.


## Dependencies
- Raylib (graphics and input)

## Acknowledgments
- Game Coding Complete, 4th Edition by Mike McShaffry and David Graham.
- Raylib project for the rendering layer.
