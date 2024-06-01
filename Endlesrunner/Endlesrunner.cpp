#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct Obstacle {
    Vector3 position;
    float width;
    float height;
    float length;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Endless Runner");

    // Definicja kamery
    Camera camera = { 0 };
    camera.position = Vector3{ 0.0f, 6.0f, 9.0f };
    camera.target = Vector3{ 0.0f, 2.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Definicja gracza
    Vector3 playerPosition = { 0.0f, 1.0f, 0.0f };
    float playerWidth = 1.0f;
    float playerHeight = 2.0f;
    float playerSpeed = 15.0f;
    float gravity = 20.0f;
    float jumpSpeed = 8.0f;
    bool isJumping = false;
    float verticalSpeed = 0.0f;
    bool isObstacle = false;
    int score = 0;

    // Inicjalizacja przeszkód
    std::vector<Obstacle> obstacles;
    srand(time(0));


    SetTargetFPS(60);
    float obstacleTimer = 0.0f;

    while (!WindowShouldClose()) {

        //Generowanie przeszkod
        obstacleTimer += GetFrameTime();
        if (obstacleTimer>2.0f)
        {
            isObstacle = true;
            obstacleTimer = 0;
        }
        if (isObstacle)
        {
            Obstacle obs;
            obs.position = Vector3{ float(rand() % 3 * 4 - 4), 1.0f, -100.0f};
            obs.width = 2.0f;
            obs.height = 2.0f;
            obs.length = 2.0f;
            obstacles.push_back(obs);
            isObstacle = false;
        }
        // Aktualizacja gracza

        if (playerPosition.x >-4)
        {
            if (IsKeyPressed(KEY_LEFT)) playerPosition.x -= 4;
        }
        if (playerPosition.x < 4)
        {
            if (IsKeyPressed(KEY_RIGHT)) playerPosition.x += 4;
        }

        if (IsKeyPressed(KEY_SPACE) && !isJumping) {
            isJumping = true;
            verticalSpeed = jumpSpeed;
        }

        //aktualizacja wyniku
        score++;
        

        //restart
        if (IsKeyPressed(KEY_R))
        {
            obstacles.clear();
            DisableEventWaiting();
            score = 0;
        }

        if (isJumping) {
            verticalSpeed -= gravity * GetFrameTime();
            playerPosition.y += verticalSpeed * GetFrameTime();

            if (playerPosition.y <= 1.0f) {
                playerPosition.y = 1.0f;
                isJumping = false;
                verticalSpeed = 0.0f;
            }
        }

        BeginDrawing();

        // Aktualizacja przeszkód
        for (auto& obs : obstacles) {
            obs.position.z += playerSpeed * GetFrameTime();

            int pointzero = obs.position.z;
            if (playerPosition.z == pointzero && playerPosition.x == obs.position.x)
            {
                DrawText("GAME OVER", 100, 100, 50, BLACK);
                EnableEventWaiting();
            }
            if (obs.position.z > camera.position.z + 10.0f) {
                obstacles.erase(obstacles.begin());
            }
        }

        
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Rysowanie gracza
        DrawCube(playerPosition, playerWidth, playerHeight, playerWidth, BLUE);
        DrawCubeWires(playerPosition, playerWidth, playerHeight, playerWidth, DARKBLUE);

        // Rysowanie przeszkód
        for (const auto& obs : obstacles) {
            DrawCube(obs.position, obs.width, obs.height, obs.length, RED);
            DrawCubeWires(obs.position, obs.width, obs.height, obs.length, BLACK);
        }

        //Pokazywanie FPS mi nie działa :(
        DrawFPS(20, 20);

        EndMode3D();

        DrawText("Ue LEFT and RIGHT to move, SPACE to jump", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Score : %i",score), 10, 30, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}