#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>

struct Obstacle {
    Vector3 position;
    float width;
    float height;
    float length;
    int type;
};

int main() {
    const int screenWidth = 1080;
    const int screenHeight = 920;

    InitWindow(screenWidth, screenHeight, "Endless Runner");
    // Definicja kamery
    Camera camera = { 0 };
    camera.position = Vector3{ 0.0f, 6.0f, 12.0f };
    camera.target = Vector3{ 0.0f, 2.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Definicja gracza
    Vector3 playerPosition = { 0.0f, 1.0f, 2.0f };
    float playerWidth = 1.0f;
    float playerHeight = 2.0f;
    float playerSpeed = 25.0f;
    float gravity = 22.0f;
    float jumpSpeed = 10.0f;
    bool isJumping = false;
    float verticalSpeed = 0.0f;

    int licznik = 0;
    int score = 0;
    bool gameOver = false;

    // Inicjalizacja przeszkód
    Model laser = LoadModel("laser.glb");
    Model beczka = LoadModel("barrel.glb");
    std::vector<Obstacle> obstacles;
    bool isObstacle = false;
    float obstacleTimer = 0.0f;
    float obstacleTimer_zero = 2.0f;
    srand(time(0));
    float zcoorobs = -100.0f;
    while (zcoorobs > -400.0f)
    {
        Obstacle obs;
        obs.position = Vector3{ float(rand() % 3 * 3 - 3), 1.0f, zcoorobs };
        obs.width = 2.0f;
        obs.height = 1.0f;
        obs.length = 2.0f;
        obs.type = rand() % 2;
        obstacles.push_back(obs);
        isObstacle = false;
        zcoorobs -= 50.0f;
    }

    // Dodawanie drogi
    Texture2D texture = LoadTexture("quartercubic.png");
    Mesh cubeMesh = GenMeshCube(15.0f, 1.0f, 100.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    cubeModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;
    std::vector<Obstacle> droga;
    for (int i = 7; i >= -10; i--) {
            Obstacle path;
            path.position = Vector3{ 0.0f, 0.2f, float(i)*100 };
            droga.push_back(path);
    }

    // Dodawanie robota
    Model robot = LoadModel("robot.glb");
    int animsCount = 7;
    unsigned int animIndex = 6;
    unsigned int animCurrentFrame = 0;
    ModelAnimation* modelAnimations = LoadModelAnimations("robot.glb", &animsCount);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (!gameOver) {
            // Generowanie przeszkod
            obstacleTimer += GetFrameTime();
            if (obstacleTimer > obstacleTimer_zero) {
                isObstacle = true;
                obstacleTimer = 0;
            }
            if (isObstacle) {
                Obstacle obs;
                
                obs.position = Vector3{ float(rand() % 3 * 3 - 3), 1.0f, -400.f };
                obs.width = 2.0f;
                obs.height = 1.0f;
                obs.length = 2.0f;
                obs.type = rand() % 2;
                obstacles.push_back(obs);
                isObstacle = false;
                //} while (zcoorobs);
            }

            // Aktualizacja gracza
            if (playerPosition.x > -3) {
                if (IsKeyPressed(KEY_LEFT)) playerPosition.x -= 3;
            }
            if (playerPosition.x < 3) {
                if (IsKeyPressed(KEY_RIGHT)) playerPosition.x += 3;
            }
            if (IsKeyPressed(KEY_SPACE) && !isJumping) {
                isJumping = true;
                verticalSpeed = jumpSpeed;
                animCurrentFrame = 0;
            }
            if (isJumping) {
                verticalSpeed -= gravity * GetFrameTime();
                playerPosition.y += verticalSpeed * GetFrameTime();
                animIndex = 3;
                

                if (playerPosition.y <= 1.0f) {
                    playerPosition.y = 1.0f;
                    isJumping = false;
                    verticalSpeed = 0.0f;
                    animIndex = 6;
                }
            }

            // Animacja robota
            ModelAnimation anim = modelAnimations[animIndex];
            if(animIndex==3)
            {
                if (animCurrentFrame <= anim.frameCount) animCurrentFrame++;

            }
            else  animCurrentFrame = (animCurrentFrame + 2) % anim.frameCount;
            UpdateModelAnimation(robot, anim, animCurrentFrame);

            // aktualizacja wyniku i predkosci
            score++;
            if (score % 500 == 0)
            {
                playerSpeed += 5.0f;
                obstacleTimer_zero *= 0.8f;
            }


            // Aktualizacja przeszkód
            for (auto& obs : obstacles) {
                obs.position.z += playerSpeed * GetFrameTime();

                int pointzero = obs.position.z;
                
                if (playerPosition.z == pointzero && playerPosition.x == obs.position.x && playerPosition.y == obs.position.y)  gameOver = true;
                
                if (obs.position.z > camera.position.z + 10.0f) obstacles.erase(obstacles.begin());           
            }

            //aktualizacja drogi
            for (auto& path : droga) {
                path.position.z += playerSpeed * GetFrameTime();
                if (path.position.z >= camera.position.z + 100.0f) {
                    Obstacle buf;
                    buf.position = Vector3{ 0.0f, 0.2f, droga[17].position.z - 100.0f};
                    droga.push_back(buf);
                    droga.erase(droga.begin());
                }
            }
        }
        // restart
        if (IsKeyPressed(KEY_R)) {
            obstacles.clear();
            score = 0;
            playerSpeed = 25.0f;
            obstacleTimer_zero = 2.0f;
            gameOver = false;
        }

         
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        
        // Rysowanie przeszkód
        for (const auto& obs : obstacles) {
            switch (obs.type) {
            case 0: {
                DrawModelEx(beczka, Vector3{ obs.position.x,  obs.position.y, obs.position.z-0.7f}, Vector3{ 0.0f, 1.0f, 0.0f }, 90.0f, Vector3{ 0.15f, 0.15f ,0.15f }, WHITE);
                break;
            }
            case 1: {
                DrawModelEx(laser, Vector3{ obs.position.x,  obs.position.y + 0.8f, obs.position.z-0.2f }, Vector3{ 1.0f, 0.0f, 0.0f }, 90.0f, Vector3{ 1.0f, 1.0f , 1.8f }, WHITE);
                break;
            }
            }
        }

        // Rysowanie gracza
        DrawModelEx(robot, playerPosition, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 0.3f, 0.3f, 0.3f }, WHITE);

        // Rysowanie drogi
        for (const auto& path : droga) {
            DrawModel(cubeModel, path.position, 1.0f, WHITE);
        }

        EndMode3D();

        // Pokazywanie FPS
        DrawFPS(20, 50);

        // Game Over
        if (gameOver)
        {
            DrawText("GAME OVER", 200, 60, 50, BLACK);
        }
        DrawText("Use LEFT and RIGHT to move, SPACE to jump", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Score : %i", score), 10, 30, 20, DARKGRAY);

        EndDrawing();
    }


    UnloadModel(robot);
    UnloadModel(cubeModel);
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
