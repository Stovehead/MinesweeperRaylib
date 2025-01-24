#include "raylib.h"
#include "game.h"

int main() {
    InitWindow(WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE, "Minesweeper");
    SetTargetFPS(60);
    InitGame();
    while (!WindowShouldClose()) {
        UpdateGame();
        BeginDrawing();
        DrawGame();
        DrawToScreen();
        EndDrawing();
    }
}