#include "raylib.h"
#include "game.h"
#include "bg.h"
#include "mario.h"
#include <time.h>
#include <stdlib.h>

void InitGrid() {
    // Init grid and nametable
    // Put some mines at the beginning
    // Each bit in the grid represents something
    // Bit 0 = is a mine
    // Bit 1 = is opened
    // Bit 2 = is flagged
    // Bit 3 = is clicked mine
    // Bit 4-7 = number of adjacent mines
    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int j = 0; j < GRID_WIDTH; j++) {
            if(i * GRID_WIDTH + j < numMines)
                grid[i][j] = 1;
            else
                grid[i][j] = 0;
            nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = 75;
            nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = 76;
            nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = 77;
            nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = 78;
        }
    }
    // Shuffle grid
    for(int i = GRID_HEIGHT * GRID_WIDTH - 1; i >= 1; i--) {
        const int swapIndex = rand() % (i + 1);
        const int swapX = swapIndex % GRID_WIDTH;
        const int swapY = swapIndex / GRID_WIDTH;
        const int temp = grid[swapY][swapX];
        grid[swapY][swapX] = grid[i / GRID_WIDTH][i % GRID_WIDTH];
        grid[i / GRID_WIDTH][i % GRID_WIDTH] = temp;
    }
    // Mark number of mines for the inner grid
    for(int i = 1; i < GRID_HEIGHT - 1; i++) {
        for(int j = 0; j < GRID_WIDTH - 1; j++) {
            if(grid[i][j] & 1)
                continue;
            grid[i][j] = ((grid[i - 1][j] & 1) + (grid[i - 1][j - 1] & 1) + (grid[i][j - 1] & 1) + (grid[i + 1][j - 1] & 1) + (grid[i + 1][j] & 1) + (grid[i + 1][j + 1] & 1) + (grid[i][j + 1] & 1) + (grid[i - 1][j + 1] & 1)) << 4;
        }
    }
    // Mark number of mines for the top and bottom
    for(int j = 1; j < GRID_WIDTH - 1; j++) {
        if(!(grid[0][j] & 1))
            grid[0][j] = ((grid[0][j - 1] & 1) + (grid[1][j - 1] & 1) + (grid[1][j] & 1) + (grid[1][j + 1] & 1) + (grid[0][j + 1] & 1)) << 4;
        if(!(grid[GRID_HEIGHT - 1][j] & 1))
            grid[GRID_HEIGHT - 1][j] = ((grid[GRID_HEIGHT - 2][j] & 1) + (grid[GRID_HEIGHT - 2][j - 1] & 1) + (grid[GRID_HEIGHT - 1][j - 1] & 1) + (grid[GRID_HEIGHT - 1][j + 1] & 1) + (grid[GRID_HEIGHT - 2][j + 1] & 1)) << 4;
    }
    // Mark number of mines for the left and right
    for(int i = 1; i < GRID_HEIGHT - 1; i++) {
        if(!(grid[i][0] & 1))
            grid[i][0] = ((grid[i - 1][0] & 1) + (grid[i + 1][0] & 1) + (grid[i + 1][1] & 1) + (grid[i][1] & 1) + (grid[i - 1][1] & 1)) << 4;
        if(!(grid[i][GRID_WIDTH - 1] & 1))
            grid[i][GRID_WIDTH - 1] = ((grid[i - 1][GRID_WIDTH - 1] & 1) + (grid[i - 1][GRID_WIDTH - 2] & 1) + (grid[i][GRID_WIDTH - 2] & 1) + (grid[i + 1][GRID_WIDTH - 2] & 1) + (grid[i + 1][GRID_WIDTH - 1] & 1)) << 4;
    }
    // Mark number of mines for the corners
    if(!(grid[0][0] & 1))
        grid[0][0] = ((grid[0][1] & 1) + (grid[1][1] & 1) + (grid[1][0] & 1)) << 4;
    if(!(grid[0][GRID_WIDTH - 1] & 1))
        grid[0][GRID_WIDTH - 1] = ((grid[0][GRID_WIDTH - 2] & 1) + (grid[1][GRID_WIDTH - 2] & 1) + (grid[1][GRID_WIDTH - 1] & 1)) << 4;
    if(!(grid[GRID_HEIGHT - 1][0] & 1))
        grid[GRID_HEIGHT - 1][0] = ((grid[GRID_HEIGHT - 2][0] & 1) + (grid[GRID_HEIGHT - 2][1] & 1) + (grid[GRID_HEIGHT - 1][1] & 1)) << 4;
    if(!(grid[GRID_HEIGHT - 1][GRID_WIDTH - 1] & 1))
        grid[GRID_HEIGHT - 1][GRID_WIDTH - 1] = ((grid[GRID_HEIGHT - 2][GRID_WIDTH - 1] & 1) + (grid[GRID_HEIGHT - 2][GRID_WIDTH - 2] & 1) + (grid[GRID_HEIGHT - 1][GRID_WIDTH - 2] & 1)) << 4;\
    openedTiles = 0;
    numFlags = 0;
    secondsElapsed = 0;
    gameStarted = false;
    UpdateMineCount();
    UpdateTimeDisplay();
    /*
    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int j = 0; j < GRID_WIDTH; j++) {
            if(grid[i][j] & 1) {
                nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = 116;
                nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = 117;
                nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = 118;
                nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = 119;
            }
            else if(grid[i][j] >> 4 > 0) {
                nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = (grid[i][j] >> 2) + 80;
                nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = (grid[i][j] >> 2) + 81;
                nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = (grid[i][j] >> 2) + 82;
                nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = (grid[i][j] >> 2) + 83;
            }
        }
    }
    */
}

void InitGame() {
    srand(time(NULL));
    numMines = 15;
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    inverted = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderPosition = {0.0f, 0.0f};
    Image bgImage, marioImage;
    bgImage.format = BG_FORMAT;
    bgImage.height = BG_HEIGHT;
    bgImage.width = BG_WIDTH;
    bgImage.data = BG_DATA;
    bgImage.mipmaps = 1;
    marioImage.format = MARIO_FORMAT;
    marioImage.height = MARIO_HEIGHT;
    marioImage.width = MARIO_WIDTH;
    marioImage.data = MARIO_DATA;
    marioImage.mipmaps = 1;
    bgTileset = LoadTextureFromImage(bgImage);
    marioTexture = LoadTextureFromImage(marioImage);
    mouseState = UP;
    mouseDownX = 0;
    mouseDownY = 0;
    gameOver = false;
    gameWon = false;
    InitGrid();
}

void OpenTile(int x, int y) {
    if(((grid[y][x] >> 1) & 1) || ((grid[y][x] >> 2) & 1) || x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT)
        return;
    grid[y][x] |= 0b00000010;
    if(!gameStarted) {
        startTime = GetTime();
        gameStarted = true;
    }
    if(grid[y][x] & 1) {
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X] = 120;
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X + 1] = 121;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X] = 122;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X + 1] = 123;
        grid[y][x] |= 0b00001000;
        gameOver = true;
        return;
    }
    openedTiles += 1;
    if(grid[y][x] >> 4 > 0) {
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X] = (grid[y][x] >> 2) + 80;
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X + 1] = (grid[y][x] >> 2) + 81;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X] = (grid[y][x] >> 2) + 82;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X + 1] = (grid[y][x] >> 2) + 83;
    }
    else {
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X] = 44;
        nametable[y * 2 + GRID_Y][x * 2 + GRID_X + 1] = 32;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X] = 30;
        nametable[y * 2 + GRID_Y + 1][x * 2 + GRID_X + 1] = 83;
        OpenTile(x, y - 1);
        OpenTile(x - 1, y - 1);
        OpenTile(x - 1, y);
        OpenTile(x - 1, y + 1);
        OpenTile(x, y + 1);
        OpenTile(x + 1, y + 1);
        OpenTile(x + 1, y);
        OpenTile(x + 1, y - 1);
    }
}

void DrawDigit(int x, int y, int digit) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 2; j++) {
            nametable[y + i][x + j] = digitTiles[digit][i][j];
        }
    } 
}

void UpdateMineCount() {
    const int numMinesLeft = numMines - numFlags;
    if(numMinesLeft >= 0) {
        if(numMinesLeft > 999) {
            DrawDigit(6, 2, 9);
            DrawDigit(4, 2, 9);
            DrawDigit(2, 2, 9);
        }
        else {
            DrawDigit(6, 2, numMinesLeft % 10);
            DrawDigit(4, 2, numMinesLeft / 10);
            DrawDigit(2, 2, numMinesLeft / 100);
        }
    }
    else {
        nametable[2][2] = 1; nametable[2][3] = 128;
        nametable[3][2] = 7; nametable[3][3] = 8;
        nametable[4][2] = 24; nametable[4][3] = 11;
        nametable[5][2] = 12; nametable[5][3] = 129;
        if(numMinesLeft < -99) {
            DrawDigit(6, 2, 9);
            DrawDigit(4, 2, 9);
        }
        else {
            DrawDigit(6, 2, -numMinesLeft % 10);
            DrawDigit(4, 2, -numMinesLeft / 10);
        }
    }
}

void UpdateTimeDisplay() {
    if(secondsElapsed > 999) {
        DrawDigit(28, 2, 9);
        DrawDigit(26, 2, 9);
        DrawDigit(24, 2, 9);
    }
    else {
        DrawDigit(28, 2, secondsElapsed % 10);
        DrawDigit(26, 2, secondsElapsed / 10);
        DrawDigit(24, 2, secondsElapsed / 100);
    }
}

void UpdateTime() {
    const int newSecondsElapsed = (int)(GetTime() - startTime);
    if(newSecondsElapsed > secondsElapsed) {
        secondsElapsed = newSecondsElapsed;
        UpdateTimeDisplay();
    }
}

void CheckGameState() {
    if(GRID_HEIGHT * GRID_WIDTH - numMines == openedTiles) {
        gameWon = true;
        numFlags = numMines;
        UpdateMineCount();
    }
    if(gameOver) {
        for(int i = 0; i < GRID_HEIGHT; i++) {
            for(int j = 0; j < GRID_WIDTH; j++) {
                if((grid[i][j] & 1) && !((grid[i][j] >> 3) & 1)) {
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = 116;
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = 117;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = 118;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = 119;
                }
                else if(!(grid[i][j] & 1) && ((grid[i][j] >> 2) & 1)) {
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = 124;
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = 125;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = 126;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = 127;
                }
            }
        }
    }
    else if(gameWon) {
        for(int i = 0; i < GRID_HEIGHT; i++) {
            for(int j = 0; j < GRID_WIDTH; j++) {
                if((grid[i][j] & 1)) {
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X] = 79;
                    nametable[i * 2 + GRID_Y][j * 2 + GRID_X + 1] = 80;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X] = 81;
                    nametable[i * 2 + GRID_Y + 1][j * 2 + GRID_X + 1] = 82;
                }
            }
        }
    }
}

void CheckMouseInput() {
    if(!mouseState && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
        const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
        const int positionX = (screenMousePositionX / 16) - GRID_X / 2;
        const int positionY = (screenMousePositionY / 16) - GRID_Y / 2;
        if(positionX < 0 || positionX >= GRID_WIDTH || positionY < 0 || positionY >= GRID_HEIGHT) {
            if(screenMousePositionX < 145 && screenMousePositionX >= 111 && screenMousePositionY < 49 && screenMousePositionY >= 15) {
                mouseState = DOWNONBUTTON;
                hoveringOnButton = true;
                nametable[2][14] = 56; nametable[3][14] = 58; nametable[4][14] = 58; nametable[5][14] = 58; 
                nametable[2][15] = 57; nametable[3][15] = 44; nametable[4][15] = 44; nametable[5][15] = 44; 
                nametable[2][16] = 57; nametable[3][16] = 44; nametable[4][16] = 44; nametable[5][16] = 44; 
                nametable[2][17] = 57; nametable[3][17] = 44; nametable[4][17] = 44; nametable[5][17] = 44; 
            }
        }
        else if(IsKeyDown(KEY_LEFT_SHIFT))
            mouseState = SHIFTDOWN;
        else 
            mouseState = DOWN;
    }
    else if(mouseState == DOWN && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mouseState = UP;
        if(mouseDownX >= 0 && mouseDownX < GRID_WIDTH && mouseDownY >= 0 && mouseDownY < GRID_HEIGHT && !((grid[mouseDownY][mouseDownX] >> 1) & 1) && !((grid[mouseDownY][mouseDownX] >> 2) & 1) && !gameOver && !gameWon) {
            OpenTile(mouseDownX, mouseDownY);
            CheckGameState();
        }
    }
    else if(mouseState == SHIFTDOWN && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        mouseState = UP;
        if(mouseDownX >= 0 && mouseDownX < GRID_WIDTH && mouseDownY >= 0 && mouseDownY < GRID_HEIGHT && ((grid[mouseDownY][mouseDownX] >> 1) & 1) && !((grid[mouseDownY][mouseDownX] >> 2) & 1) && !gameOver && !gameWon) {
            int flagCount = 0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    if((mouseDownX + j) >= 0 && (mouseDownX + j) < GRID_WIDTH && (mouseDownY + i) >= 0 && (mouseDownY + i) < GRID_HEIGHT) {
                        flagCount += ((grid[mouseDownY + i][mouseDownX + j] >> 2) & 1);
                    }
                }
            }
            if(flagCount == (grid[mouseDownY][mouseDownX] >> 4)) {
                OpenTile(mouseDownX, mouseDownY - 1);
                OpenTile(mouseDownX - 1, mouseDownY - 1);
                OpenTile(mouseDownX - 1, mouseDownY);
                OpenTile(mouseDownX - 1, mouseDownY + 1);
                OpenTile(mouseDownX, mouseDownY + 1);
                OpenTile(mouseDownX + 1, mouseDownY + 1);
                OpenTile(mouseDownX + 1, mouseDownY);
                OpenTile(mouseDownX + 1, mouseDownY - 1);
                CheckGameState();
            }
            else
                goto resetTiles;
        }
        else if(!gameOver && !gameWon) {
            resetTiles:
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    if((mouseDownX + j) >= 0 && (mouseDownX + j) < GRID_WIDTH && (mouseDownY + i) >= 0 && (mouseDownY + i) < GRID_HEIGHT && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 1) & 1) && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 2) & 1)) {
                        nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X] = 75;
                        nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X + 1] = 76;
                        nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X] = 77;
                        nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X + 1] = 78;
                    }
                }
            }
        }
    }
    else if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !gameOver && !gameWon) {
        const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
        const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
        const int flagPositionX = (screenMousePositionX / 16) - GRID_X / 2;
        const int flagPositionY = (screenMousePositionY / 16) - GRID_Y / 2;
        if(flagPositionX >= 0 && flagPositionX < GRID_WIDTH && flagPositionY >= 0 && flagPositionY < GRID_HEIGHT) {
            if(((grid[flagPositionY][flagPositionX] >> 2) & 1)) {
                grid[flagPositionY][flagPositionX] &= 0b11111011;
                nametable[flagPositionY * 2 + GRID_Y][flagPositionX * 2 + GRID_X] = 75;
                nametable[flagPositionY * 2 + GRID_Y][flagPositionX * 2 + GRID_X + 1] = 76;
                nametable[flagPositionY * 2 + GRID_Y + 1][flagPositionX * 2 + GRID_X] = 77;
                nametable[flagPositionY * 2 + GRID_Y + 1][flagPositionX * 2 + GRID_X + 1] = 78;
                numFlags -= 1;
                UpdateMineCount();
            }
            else if(!((grid[flagPositionY][flagPositionX] >> 1) & 1)) {
                grid[flagPositionY][flagPositionX] |= 0b00000100;
                nametable[flagPositionY * 2 + GRID_Y][flagPositionX * 2 + GRID_X] = 79;
                nametable[flagPositionY * 2 + GRID_Y][flagPositionX * 2 + GRID_X + 1] = 80;
                nametable[flagPositionY * 2 + GRID_Y + 1][flagPositionX * 2 + GRID_X] = 81;
                nametable[flagPositionY * 2 + GRID_Y + 1][flagPositionX * 2 + GRID_X + 1] = 82;
                numFlags += 1;
                UpdateMineCount();
            }
        }
    }
    else if(mouseState == DOWNONBUTTON) {
        const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
        const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if(hoveringOnButton) {
                if(screenMousePositionX >= 145 || screenMousePositionX < 111 || screenMousePositionY >= 49 || screenMousePositionY < 15) {
                    hoveringOnButton = false;
                    nametable[2][14] = 45; nametable[3][14] = 51; nametable[4][14] = 51; nametable[5][14] = 53; 
                    nametable[2][15] = 46; nametable[3][15] = 44; nametable[4][15] = 44; nametable[5][15] = 54; 
                    nametable[2][16] = 46; nametable[3][16] = 44; nametable[4][16] = 44; nametable[5][16] = 54; 
                    nametable[2][17] = 47; nametable[3][17] = 52; nametable[4][17] = 52; nametable[5][17] = 55; 
                }
            }
            else {
                if(screenMousePositionX < 145 && screenMousePositionX >= 111 && screenMousePositionY < 49 && screenMousePositionY >= 15) {
                    hoveringOnButton = true;
                    nametable[2][14] = 56; nametable[3][14] = 58; nametable[4][14] = 58; nametable[5][14] = 58; 
                    nametable[2][15] = 57; nametable[3][15] = 44; nametable[4][15] = 44; nametable[5][15] = 44; 
                    nametable[2][16] = 57; nametable[3][16] = 44; nametable[4][16] = 44; nametable[5][16] = 44; 
                    nametable[2][17] = 57; nametable[3][17] = 44; nametable[4][17] = 44; nametable[5][17] = 44; 
                }
            }
        }
        else {
            mouseState = UP;
            nametable[2][14] = 45; nametable[3][14] = 51; nametable[4][14] = 51; nametable[5][14] = 53; 
            nametable[2][15] = 46; nametable[3][15] = 44; nametable[4][15] = 44; nametable[5][15] = 54; 
            nametable[2][16] = 46; nametable[3][16] = 44; nametable[4][16] = 44; nametable[5][16] = 54; 
            nametable[2][17] = 47; nametable[3][17] = 52; nametable[4][17] = 52; nametable[5][17] = 55; 
            if(screenMousePositionX < 145 && screenMousePositionX >= 111 && screenMousePositionY < 49 && screenMousePositionY >= 15) {
                InitGrid();
                gameOver = false;
                gameWon = false;
            }
        }
    }
}

void UpdateGame() {
    CheckMouseInput();
    if(!gameOver && !gameWon) {
        if(mouseState == DOWN) {
            if(mouseDownX >= 0 && mouseDownX < GRID_WIDTH && mouseDownY >= 0 && mouseDownY < GRID_HEIGHT && !((grid[mouseDownY][mouseDownX] >> 1) & 1) && !((grid[mouseDownY][mouseDownX] >> 2) & 1)) {
                nametable[mouseDownY * 2 + GRID_Y][mouseDownX * 2 + GRID_X] = 75;
                nametable[mouseDownY * 2 + GRID_Y][mouseDownX * 2 + GRID_X + 1] = 76;
                nametable[mouseDownY * 2 + GRID_Y + 1][mouseDownX * 2 + GRID_X] = 77;
                nametable[mouseDownY * 2 + GRID_Y + 1][mouseDownX * 2 + GRID_X + 1] = 78;
            }
            const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
            const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
            mouseDownX = (screenMousePositionX / 16) - GRID_X / 2;
            mouseDownY = (screenMousePositionY / 16) - GRID_Y / 2;
            if(mouseDownX >= 0 && mouseDownX < GRID_WIDTH && mouseDownY >= 0 && mouseDownY < GRID_HEIGHT && !((grid[mouseDownY][mouseDownX] >> 1) & 1) && !((grid[mouseDownY][mouseDownX] >> 2) & 1)) {
                nametable[mouseDownY * 2 + GRID_Y][mouseDownX * 2 + GRID_X] = 44;
                nametable[mouseDownY * 2 + GRID_Y][mouseDownX * 2 + GRID_X + 1] = 32;
                nametable[mouseDownY * 2 + GRID_Y + 1][mouseDownX * 2 + GRID_X] = 30;
                nametable[mouseDownY * 2 + GRID_Y + 1][mouseDownX * 2 + GRID_X + 1] = 83;
            }
        }
        else if(mouseState == SHIFTDOWN) {
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    if((mouseDownX + j) >= 0 && (mouseDownX + j) < GRID_WIDTH && (mouseDownY + i) >= 0 && (mouseDownY + i) < GRID_HEIGHT && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 1) & 1) && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 2) & 1)) {
                        nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X] = 75;
                        nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X + 1] = 76;
                        nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X] = 77;
                        nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X + 1] = 78;
                    }
                }
            }
            const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
            const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
            mouseDownX = (screenMousePositionX / 16) - GRID_X / 2;
            mouseDownY = (screenMousePositionY / 16) - GRID_Y / 2;
            if(mouseDownX >= 0 && mouseDownX < GRID_WIDTH && mouseDownY >= 0 && mouseDownY < GRID_HEIGHT) {
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        if((mouseDownX + j) >= 0 && (mouseDownX + j) < GRID_WIDTH && (mouseDownY + i) >= 0 && (mouseDownY + i) < GRID_HEIGHT && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 1) & 1) && !((grid[(mouseDownY + i)][(mouseDownX + j)] >> 2) & 1)) {
                            nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X] = 44;
                            nametable[(mouseDownY + i) * 2 + GRID_Y][(mouseDownX + j) * 2 + GRID_X + 1] = 32;
                            nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X] = 30;
                            nametable[(mouseDownY + i) * 2 + GRID_Y + 1][(mouseDownX + j) * 2 + GRID_X + 1] = 83;
                        }
                    }
                }
            }
        }
    }
    if(gameStarted && !gameOver && !gameWon)
        UpdateTime();
}

void DrawBackground() {
    Rectangle source, destination;
    source.height = TILE_SIZE;
    source.width = TILE_SIZE;
    source.y = 0;
    destination.height = TILE_SIZE;
    destination.width = TILE_SIZE;
    for(int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++) {
        for(int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++) {
            source.x = nametable[i][j] * TILE_SIZE;
            destination.x = j * TILE_SIZE;
            destination.y = i * TILE_SIZE;
            DrawTexturePro(bgTileset, source, destination, renderPosition, 0.0f, WHITE);
        }
    }
}

void DrawMario() {
    Rectangle source, destination;
    source.height = 32;
    source.width = 32;
    source.y = 0;
    destination.height = 32;
    destination.width = 32;
    if(mouseState == DOWNONBUTTON && hoveringOnButton) {
        const int screenMousePositionX = GetMousePosition().x / WINDOW_SCALE;
        const int screenMousePositionY = GetMousePosition().y / WINDOW_SCALE;
        source.x = 0;
        destination.x = 113;
        destination.y = 17;
    }
    else {
        destination.x = 112;
        destination.y = 16;
        if(gameOver) {
            source.x = 64;
        }
        else if(gameWon) {
            source.x = 96;
        }
        else if(mouseState == DOWN || mouseState == SHIFTDOWN) {
            source.x = 32;
        }
        else {
            source.x = 0;
        }
    }
    DrawTexturePro(marioTexture, source, destination, renderPosition, 0.0f, WHITE);
}

void DrawGame() {
    BeginTextureMode(target);
    ClearBackground(BLACK);
    DrawBackground();
    DrawMario();
}

void DrawToScreen() {
    EndTextureMode();
    BeginTextureMode(inverted);
    DrawTexture(target.texture, 0, 0, WHITE);
    EndTextureMode();
    DrawTextureEx(inverted.texture, renderPosition, 0.0f, (float)WINDOW_SCALE, WHITE);
}