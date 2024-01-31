#include "raylib.h"
#include <stdio.h>

#define SQUARE_SIZE 30
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define GAME_WIDTH (SCREEN_WIDTH / SQUARE_SIZE)
#define GAME_HEIGHT (SCREEN_HEIGHT / SQUARE_SIZE)
#define GAME_OFFSET_X ((SCREEN_WIDTH - GAME_WIDTH * SQUARE_SIZE) / 2)
#define GAME_OFFSET_Y ((SCREEN_HEIGHT - GAME_HEIGHT * SQUARE_SIZE) / 2)

int board[GAME_HEIGHT][GAME_WIDTH] = {0}; // 0 represents empty, 1 represents snake, 2 represents food
int score=0;

typedef struct {
    int x;
    int y;
} Position;

Position snake[GAME_WIDTH * GAME_HEIGHT] = {{0, 0}, {0, 1}, {0, 2}}; // Initial snake position and size
int snakeSize = 3;
Position food;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

Direction direction = DIR_RIGHT;

void DrawRoundedRectangle(int x, int y, int width, int height, float roundness, Color color)
{
    // Draw rounded rectangle with 4 corner circles
    DrawCircle(x + roundness, y + roundness, roundness, color);
    DrawCircle(x + width - roundness, y + roundness, roundness, color);
    DrawCircle(x + roundness, y + height - roundness, roundness, color);
    DrawCircle(x + width - roundness, y + height - roundness, roundness, color);
    DrawRectangle(x + roundness, y, width - 2 * roundness, height, color);
    DrawRectangle(x, y + roundness, roundness, height - 2 * roundness, color);
    DrawRectangle(x + width - roundness, y + roundness, roundness, height - 2 * roundness, color);
}

void PlaceFood()
{
    while (1)
    {
        int x = GetRandomValue(0, GAME_WIDTH - 1);
        int y = GetRandomValue(0, GAME_HEIGHT - 1);
        
        if (board[y][x] == 0)
        {
            food.x = x;
            food.y = y;
            board[y][x] = 2;
            break;
        }
    }
}

void MoveSnake()
{
    // Move body
    for (int i = snakeSize - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    
    // Move head
    switch (direction)
    {
        case DIR_UP:
            snake[0].y--;
            break;
        case DIR_DOWN:
            snake[0].y++;
            break;
        case DIR_LEFT:
            snake[0].x--;
            break;
        case DIR_RIGHT:
            snake[0].x++;
            break;
    }
    
    // Wrap around screen
    if (snake[0].x < 0)
        snake[0].x = GAME_WIDTH - 1;
    if (snake[0].x >= GAME_WIDTH)
        snake[0].x = 0;
    if (snake[0].y < 0)
        snake[0].y = GAME_HEIGHT - 1;
    if (snake[0].y >= GAME_HEIGHT)
        snake[0].y = 0;
    
    // Check for collision with food
    if (snake[0].x == food.x && snake[0].y == food.y)
    {
        Sound fxWav = LoadSound("eating.wav"); 
        PlaySound(fxWav);  
        score++;
        snakeSize++;
        PlaceFood();
    }
    
    // Check for collision with snake body
    if (board[snake[0].y][snake[0].x] == 1)
    {
        Sound fxWav = LoadSound("GameOver.wav"); 
        PlaySound(fxWav);
        BeginDrawing();
        DrawText("GAME OVER", 155, 330, 80, RED);
        char score_text[20];
        sprintf(score_text, "YOUR SCORE : %d", score);
        DrawText(score_text, 180, 400, 50, RED);
        EndDrawing();
        sleep(7);
        score=0;
        snakeSize = 3;
        snake[0] = (Position){0, 0};
        snake[1] = (Position){0, 1};
        snake[2] = (Position){0, 2};
        direction = DIR_RIGHT;
        PlaceFood();
    }
    
    // Update board
    for (int i = 0; i < GAME_HEIGHT; i++)
    {
        for (int j = 0; j < GAME_WIDTH; j++)
        {
            board[i][j] = 0;
        }
    }
    for (int i = 0; i < snakeSize; i++)
    {
        board[snake[i].y][snake[i].x] = 1;
    }
    board[food.y][food.x] = 2;
}

int main()
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
    InitAudioDevice();      // Initialize audio device
    Sound fxWav = LoadSound("intro.wav"); 
    PlaySound(fxWav);  
    SetTargetFPS(8);

    // Display menu and wait for user input
    while (1)
    {
        BeginDrawing();
        Texture2D gifTexture = LoadTexture("snake_background.gif");
        DrawTexture(gifTexture, 0, 0, WHITE);
        ClearBackground(DARKGRAY);
        Texture2D image = LoadTexture("snake.png");
        Rectangle imagerect = { 0, 0, image.width, image.height };
        DrawTextureRec(image, imagerect, (Vector2){ 230, 310 }, WHITE);
        DrawText("SNAKE GAME", 130, 100, 80, GREEN);
        DrawText("Press ENTER to start", 170, 200, 40, RAYWHITE);
        DrawText("Press ESC to quit", 220,250,40, RAYWHITE);

        EndDrawing();

        if (IsKeyPressed(KEY_ENTER))
        {
            UnloadSound(fxWav);
            break;
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
            return 0;
        }
    }

    PlaceFood();

    // Main game loop
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_UP) && direction != DIR_DOWN)
            direction = DIR_UP;
        if (IsKeyPressed(KEY_DOWN) && direction != DIR_UP)
            direction = DIR_DOWN;
        if (IsKeyPressed(KEY_LEFT) && direction != DIR_RIGHT)
            direction = DIR_LEFT;
        if (IsKeyPressed(KEY_RIGHT) && direction != DIR_LEFT)
            direction = DIR_RIGHT;

        MoveSnake();

        // Draw
        //-----------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);
        char score_text[20];
        sprintf(score_text, "Score: %d", score);
        DrawText(score_text, 10, 10, 30, WHITE);
    
        for (int i = 0; i < GAME_HEIGHT; i++)
        {
            for (int j = 0; j < GAME_WIDTH; j++)
            {
                if (board[i][j] == 1)
                {
                    DrawRoundedRectangle(j * SQUARE_SIZE + GAME_OFFSET_X, i * SQUARE_SIZE + GAME_OFFSET_Y, SQUARE_SIZE, SQUARE_SIZE, 10, GREEN);
                }
                else if (board[i][j] == 2)
                {
                    //Texture2D image = LoadTexture("fruit.png");
                    //Rectangle imagerect = { 0, 0, image.width, image.height };
                    //DrawTextureRec(image, imagerect, (Vector2){ j * SQUARE_SIZE + GAME_OFFSET_X, i * SQUARE_SIZE + GAME_OFFSET_Y, 310 }, WHITE);
                    DrawRoundedRectangle(j * SQUARE_SIZE + GAME_OFFSET_X, i * SQUARE_SIZE + GAME_OFFSET_Y, SQUARE_SIZE, SQUARE_SIZE,15, ORANGE);
                }
            }
        }
        EndDrawing();
        //-----------------------------------------------------
    }
    UnloadSound(fxWav);  
    CloseAudioDevice();
   
    CloseWindow();

    return 0;
}
