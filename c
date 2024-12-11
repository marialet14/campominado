#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

// Função para inicializar o jogo
void InitGame(bool loadSavedGame) {
    gameState.lives = 3;
    gameState.gameOver = false;

    // Inicializa o tabuleiro
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameState.board[i][j].isBomb = false;
            gameState.board[i][j].revealed = false;
        }
    }

    if (loadSavedGame) {
        // Carregar o jogo salvo
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
        // Distribuir bombas aleatoriamente
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

// Função para salvar o jogo
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

// Função para calcular bombas no raio de 4 casas
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

// Função para desenhar o menu principal
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
    const char *opcoesMenu[] = {"New Game", "Load Game", "Options", "Exit"};
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
            if (selecionada == 2) { }
            if (selecionada == 3) {
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

// Função para desenhar o menu de pausa
void DrawPauseMenu() {
    ClearBackground(LIGHTGRAY);
    DrawText("Jogo Pausado", 100, 50, 30, BLACK);
    DrawRectangle(100, 150, 200, 50, GRAY);
    DrawRectangle(100, 250, 200, 50, GRAY);
    DrawText("Retornar", 140, 165, 20, BLACK);
    DrawText("Sair", 160, 265, 20, BLACK);
}

// Função principal
int main() {
     InitWindow(1280, 720, "Blast Escape");
    //InitWindow(BOARD_SIZE * CELL_SIZE, BOARD_SIZE * CELL_SIZE + 50, "Campo Minado");
    
    SetExitKey(0); // Desativa o comportamento padrão do ESC para fechar a janela

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if (currentScreen == MENU) {
            menu();

        } else if (currentScreen == GAME) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentScreen = PAUSE; // Pausa o jogo
            }

            Vector2 mousePos = GetMousePosition();
            int cellX = mousePos.x / CELL_SIZE;
            int cellY = mousePos.y / CELL_SIZE;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (cellX >= 0 && cellX < BOARD_SIZE && cellY >= 0 && cellY < BOARD_SIZE) {
                    if (!gameState.board[cellX][cellY].revealed) {
                        gameState.board[cellX][cellY].revealed = true;

                        if (gameState.board[cellX][cellY].isBomb) {
                            gameState.lives--;
                            if (gameState.lives <= 0) {
                                gameState.gameOver = true;
                            }
                        }
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    Rectangle cell = { i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                    if (gameState.board[i][j].revealed) {
                        if (gameState.board[i][j].isBomb) {
                            DrawRectangleRec(cell, RED);
                        } else {
                            DrawRectangleRec(cell, LIGHTGRAY);
                        }
                    } else {
                        DrawRectangleRec(cell, GRAY);
                    }
                    DrawRectangleLines(cell.x, cell.y, cell.width, cell.height, BLACK);
                }
            }

            if (cellX >= 0 && cellX < BOARD_SIZE && cellY >= 0 && cellY < BOARD_SIZE) {
                int bombsAround = CountBombsAround(cellX, cellY);
                DrawText(TextFormat("Bombas ao redor: %d", bombsAround), 10, BOARD_SIZE * CELL_SIZE + 10, 20, DARKGRAY);
            }

            DrawText(TextFormat("Vidas restantes: %d", gameState.lives), 10, BOARD_SIZE * CELL_SIZE + 30, 20, DARKGRAY);

            if (gameState.gameOver) {
                DrawText("GAME OVER", 150, 200, 40, RED);
            }

            EndDrawing();
        } else if (currentScreen == PAUSE) {
            BeginDrawing();
            DrawPauseMenu();
            EndDrawing();

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){100, 150, 200, 50})) {
                    currentScreen = GAME; // Retorna ao jogo
                } else if (CheckCollisionPointRec(mousePos, (Rectangle){100, 250, 200, 50})) {
                    SaveGame();
                    currentScreen = MENU; // Volta ao menu principal
                }
            }
        }
    }

    SaveGame();
    CloseWindow();

    return 0;
}
