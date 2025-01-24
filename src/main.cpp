#include "raylib.h"
#include "game.h"

int main() {
    Image bg = LoadImage("C:/Users/Levent/Documents/Projects/Minesweeper/assets/bg.png");
    Image mario = LoadImage("C:/Users/Levent/Documents/Projects/Minesweeper/assets/mario.png");
    ExportImageAsCode(bg, "bg.h");
    ExportImageAsCode(mario, "mario.h");
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