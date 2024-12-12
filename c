#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 20 
#define CELL_SIZE 27  
#define BOMBS_COUNT 10  

typedef struct {
    bool isBomb;
    bool revealed;
} Cell;

typedef struct {
    Cell board[BOARD_SIZE][BOARD_SIZE];
    int lives;
    bool gameOver;
} GameState;

GameState gameState;
const char *saveFileName = "campo_minado_save.csv";

typedef enum {
    MENU,
    GAME,
    PAUSE
} GameScreen;

GameScreen currentScreen = MENU;

void InitGame(bool loadSavedGame) {
    gameState.lives = 3;
    gameState.gameOver = false;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameState.board[i][j].isBomb = false;
            gameState.board[i][j].revealed = false;
        }
    }

    if (loadSavedGame) {
        FILE *file = fopen(saveFileName, "r");
        if (file) {
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    int isBomb, revealed;
                    fscanf(file, "%d,%d", &isBomb, &revealed);
                    gameState.board[i][j].isBomb = isBomb;
                    gameState.board[i][j].revealed = revealed;
                }
            }
            fscanf(file, "%d", &gameState.lives);
            fclose(file);
        }
    } else {
        srand(time(NULL));
        for (int i = 0; i < BOMBS_COUNT;) {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            if (!gameState.board[x][y].isBomb) {
                gameState.board[x][y].isBomb = true;
                i++;
            }
        }
    }
}

void SaveGame() {
    FILE *file = fopen(saveFileName, "w");
    if (file) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                fprintf(file, "%d,%d\n", gameState.board[i][j].isBomb, gameState.board[i][j].revealed);
            }
        }
        fprintf(file, "%d\n", gameState.lives);
        fclose(file);
    }
}

int CountBombsAround(int x, int y) {
    int count = 0;
    for (int i = -4; i <= 4; i++) {
        for (int j = -4; j <= 4; j++) {
            int nx = x + i;
            int ny = y + j;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (gameState.board[nx][ny].isBomb) {
                    count++;
                }
            }
        }
    }
    return count;
}

void TextoMenu(const char *opcoes[], int quantidadeOpcoes, Font fonte, int larguraTela, int alturaTela, int *selecionada) {
    Vector2 pontoMouse = GetMousePosition();

    for (int i = 0; i < quantidadeOpcoes; i++) {
        Vector2 tamanhoTexto = MeasureTextEx(fonte, opcoes[i], 40, 2);
        Vector2 posicaoTexto = (Vector2){(larguraTela - tamanhoTexto.x) / 2, 330 + i * 60};
        Rectangle areaTexto = {posicaoTexto.x, posicaoTexto.y, tamanhoTexto.x, tamanhoTexto.y};

        if (CheckCollisionPointRec(pontoMouse, areaTexto)) {
            *selecionada = i;
            Vector2 posicaoTriangulo = {posicaoTexto.x - 30, posicaoTexto.y + tamanhoTexto.y / 2};
            DrawTriangle(
                (Vector2){posicaoTriangulo.x + 10, posicaoTriangulo.y - 10},
                (Vector2){posicaoTriangulo.x + 10, posicaoTriangulo.y + 10},
                (Vector2){posicaoTriangulo.x + 20, posicaoTriangulo.y},
                WHITE
            );
        }

        DrawTextEx(fonte, opcoes[i], posicaoTexto, 40, 2, WHITE);
    }
}

void menu() {

    Texture2D robo1 = LoadTexture("./img/robo1.png");
    Texture2D robo2 = LoadTexture("./img/robo2.png");
    Texture2D robo3 = LoadTexture("./img/robo3.png");
    Texture2D fundo = LoadTexture("./img/fundo.png");
    Texture2D nuvem = LoadTexture("./img/nuvem.png");
    Texture2D titulo = LoadTexture("./img/titulo.png");


    Texture2D roboAtual = robo1;
    int roboFrame = 0;
    float frameTime = 0.5f;
    float roboTimer = 0.0f;
    float nuvemX = 0.0f, nuvemSpeed = 20.0f;

    Font fontePadrao = GetFontDefault();
    const char *opcoesMenu[] = {"New Game", "Load Game", "Exit"};
    int quantidadeOpcoes = sizeof(opcoesMenu) / sizeof(opcoesMenu[0]);
    int selecionada = -1;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        nuvemX -= nuvemSpeed * deltaTime;
        if (nuvemX <= -nuvem.width) nuvemX += nuvem.width;

        roboTimer += deltaTime;
        if (roboTimer >= frameTime) {
            roboFrame = (roboFrame + 1) % 3;
            roboAtual = (roboFrame == 0) ? robo1 : (roboFrame == 1) ? robo2 : robo3;
            roboTimer = 0.0f;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(fundo, 0, 0, WHITE);
        DrawTexture(nuvem, (int)nuvemX, -15, WHITE);
        DrawTexture(nuvem, (int)(nuvemX + nuvem.width), -15, WHITE);
        DrawTexture(roboAtual, 100, 400, WHITE);
        DrawTexture(titulo, (1280 - titulo.width) / 2, 90, WHITE);

        TextoMenu(opcoesMenu, quantidadeOpcoes, fontePadrao, 1280, 620, &selecionada);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && selecionada != -1) {
            if (selecionada == 0) { 
                InitGame(false);
                currentScreen = GAME;
                return;
            }
            if (selecionada == 1) { 
                InitGame(true);
                currentScreen = GAME;
                return;
            }
           
            if (selecionada == 2) {
                CloseWindow();
                return;
            }
        }

        EndDrawing();
    }

    UnloadTexture(robo1);
    UnloadTexture(robo2);
    UnloadTexture(robo3);
    UnloadTexture(fundo);
    UnloadTexture(nuvem);
    UnloadTexture(titulo);
}

void TextoPauseMenu(const char *opcoes[], int quantidadeOpcoes, Font fonte, int larguraTela, int alturaTela, int *selecionada) {
    Vector2 pontoMouse = GetMousePosition();

    for (int i = 0; i < quantidadeOpcoes; i++) {
        Vector2 tamanhoTexto = MeasureTextEx(fonte, opcoes[i], 40, 2);
        Vector2 posicaoTexto = (Vector2){(larguraTela - tamanhoTexto.x) / 2, 330 + i * 60};
        Rectangle areaTexto = {posicaoTexto.x, posicaoTexto.y, tamanhoTexto.x, tamanhoTexto.y};

        if (CheckCollisionPointRec(pontoMouse, areaTexto)) {
            *selecionada = i;
            Vector2 posicaoTriangulo = {posicaoTexto.x - 30, posicaoTexto.y + tamanhoTexto.y / 2};
            DrawTriangle(
                (Vector2){posicaoTriangulo.x + 10, posicaoTriangulo.y - 10},
                (Vector2){posicaoTriangulo.x + 10, posicaoTriangulo.y + 10},
                (Vector2){posicaoTriangulo.x + 20, posicaoTriangulo.y},
                WHITE
            );
        }

        DrawTextEx(fonte, opcoes[i], posicaoTexto, 40, 2, WHITE);
    }
}

void menuPause() {
    Texture2D fundo = LoadTexture("./img/fundo.png");

    Font fontePadrao = GetFontDefault();
    const char *opcoesPause[] = {"Return", "Save and Return to Menu"};
    int quantidadeOpcoes = sizeof(opcoesPause) / sizeof(opcoesPause[0]);
    int selecionada = -1;

    while (currentScreen == PAUSE && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        DrawTexture(fundo, 0, 0, WHITE);
        TextoPauseMenu(opcoesPause, quantidadeOpcoes, fontePadrao, 1280, 720, &selecionada);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && selecionada != -1) {
            if (selecionada == 0) { 
                currentScreen = GAME;
            }
            if (selecionada == 1) {  
                SaveGame();
                currentScreen = MENU;
            }
        }

        EndDrawing();
    }

    UnloadTexture(fundo);
}



int main() {
    InitWindow(1280, 620, "Blast Escape");
    SetExitKey(0); 

    int offsetX = (GetScreenWidth() - (BOARD_SIZE * CELL_SIZE)) / 2;
    int offsetY = (GetScreenHeight() - (BOARD_SIZE * CELL_SIZE)) / 2;

    while (!WindowShouldClose()) {
        switch (currentScreen) {
            case MENU:
                menu();
                break;

            case GAME: {
Texture2D bombaImg = LoadTexture("./img/bomba.png");
if (IsKeyPressed(KEY_ESCAPE)) {
    currentScreen = PAUSE; 
}

Vector2 mousePos = GetMousePosition();
int cellX = (mousePos.x - offsetX) / CELL_SIZE;
int cellY = (mousePos.y - offsetY) / CELL_SIZE;

if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (cellX >= 0 && cellX < BOARD_SIZE && cellY >= 0 && cellY < BOARD_SIZE) {
        if (!gameState.board[cellX][cellY].revealed) {
            gameState.board[cellX][cellY].revealed = true;

            if (gameState.board[cellX][cellY].isBomb) {
                // Cálculo da posição para a bomba
                float xPos = offsetX + cellX * CELL_SIZE;
                float yPos = offsetY + cellY * CELL_SIZE;
                
                // Desenhando a bomba na célula
                DrawTextureEx(bombaImg, (Vector2){xPos, yPos}, 0.0f, 1.0f, WHITE);
                
                gameState.lives--;
                if (gameState.lives <= 0) {
                    gameState.gameOver = true;
                }
            }
        }
    }
}

Texture2D fundojogo = LoadTexture("./img/fundojogo.jpg");
BeginDrawing();
ClearBackground(RAYWHITE);
DrawTexture(fundojogo, 0, 0, WHITE);

for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
        Rectangle cell = { offsetX + i * CELL_SIZE, offsetY + j * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        if (gameState.board[i][j].revealed) {
            if (gameState.board[i][j].isBomb) {
                DrawRectangleRec(cell, (Color){255, 140, 0, 255}); // Cor da célula com bomba
            } else {
                DrawRectangleRec(cell, (Color){139, 69, 19, 255}); // Cor de célula não-bomba
            }
        } else {
            DrawRectangleRec(cell, (Color){154, 205, 50, 255}); // Cor das células não reveladas
        }
        DrawRectangleLines(cell.x, cell.y, cell.width, cell.height, (Color){0, 100, 0, 255}); // Borda das células
    }
}

                if (cellX >= 0 && cellX < BOARD_SIZE && cellY >= 0 && cellY < BOARD_SIZE) {
                    int bombsAround = CountBombsAround(cellX, cellY);
                    DrawText(TextFormat("Bombas ao redor: %d", bombsAround), 10, BOARD_SIZE * CELL_SIZE + 10, 20, BLACK);
                }

                Texture2D vidaImg = LoadTexture("./img/coracao.png");
                for (int i = 0; i < gameState.lives; i++) {
                    DrawTexture(vidaImg, 10 + i * (vidaImg.width + 10), 10, WHITE);
                }

                if (gameState.gameOver) {
                    DrawText("GAME OVER", 300, 200, 100, RED);
                }

                EndDrawing();
                UnloadTexture(fundojogo);
                break;
            }

            case PAUSE:
                menuPause();
                break;

            default:
                break;
        }
    }
    SaveGame();
    CloseWindow();

    return 0;
}

