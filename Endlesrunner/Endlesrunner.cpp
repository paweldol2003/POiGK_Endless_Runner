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
};

int main() {
    const int screenWidth = 700;
    const int screenHeight = 800;

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
    float playerSpeed = 15.0f;
    float gravity = 20.0f;
    float jumpSpeed = 8.0f;
    bool isJumping = false;
    float verticalSpeed = 0.0f;
    
    int score = 0;

    // Inicjalizacja przeszkód
    std::vector<Obstacle> obstacles;
    bool isObstacle = false;
    float obstacleTimer = 0.0f;
    srand(time(0));

    //Dodawanie drogi
    Texture2D texture = LoadTexture("cubicmap_atlas.png");
    Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model cubeModel = LoadModelFromMesh(cubeMesh);
    cubeModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = texture;
    std::vector<Obstacle> droga;
    for (int i = 7; i >= -100; i--)
    {
        for (int j = -4; j <= 4; j++)
        {
            Obstacle path;
            path.position = Vector3{ float(j), 0.2f, float(i) };
            path.width = 1.0f;
            path.height = 1.0f;
            path.length = 1.0f;
            droga.push_back(path);
        }
    }



    //Dodawanie robota
    Model robot = LoadModel("robot.glb");
    int animsCount = 7;
    unsigned int animIndex = 6;
    unsigned int animCurrentFrame = 0;
    ModelAnimation* modelAnimations = LoadModelAnimations("robot.glb", &animsCount);

    SetTargetFPS(60);
    

    while (!WindowShouldClose()) {

        //Generowanie przeszkod
        obstacleTimer += GetFrameTime();
        if (obstacleTimer > 2.0f)
        {
            isObstacle = true;
            obstacleTimer = 0;
        }
        if (isObstacle)
        {
            Obstacle obs;
            obs.position = Vector3{ float(rand() % 3 * 3 - 3), 1.0f, -100.0f };
            obs.width = 2.0f;
            obs.height = 2.0f;
            obs.length = 2.0f;
            obstacles.push_back(obs);
            isObstacle = false;
        }

        // Aktualizacja gracza
        if (playerPosition.x > -3)
        {
            if (IsKeyPressed(KEY_LEFT)) playerPosition.x -= 3;
        }
        if (playerPosition.x < 3)
        {
            if (IsKeyPressed(KEY_RIGHT)) playerPosition.x += 3;
        }
        if (IsKeyPressed(KEY_SPACE) && !isJumping) {
            isJumping = true;
            verticalSpeed = jumpSpeed;
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

        // Animacja robota
        ModelAnimation anim = modelAnimations[animIndex];
        animCurrentFrame = (animCurrentFrame + 2) % anim.frameCount;
        UpdateModelAnimation(robot, anim, animCurrentFrame);

        //aktualizacja wyniku
        score++;

        //restart
        if (IsKeyPressed(KEY_R))
        {
            obstacles.clear();
            DisableEventWaiting();
            score = 0;
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

        //aktualizacja drogi //COS TU SIE SYPIE I WYWALA PROGRAM, ZEBY DZIALALO TRZEBA ZAKOMENTOWAC
        //for (auto& path : droga) {
        //    path.position.z += playerSpeed * GetFrameTime()/1.3;

        //    if (path.position.z > camera.position.z + 10.0f) {
        //        droga.push_back(path);
        //        //droga.erase(droga.begin());
        //    }
        //}


        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // Rysowanie gracza
        DrawModelEx(robot, playerPosition, Vector3{ 0.0f, 1.0f, 0.0f }, 180.0f, Vector3{ 0.3f, 0.3f, 0.3f }, WHITE);

        // Rysowanie przeszkód
        for (const auto& obs : obstacles) 
        {
            DrawCube(obs.position, obs.width, obs.height, obs.length, RED);
            DrawCubeWires(obs.position, obs.width, obs.height, obs.length, BLACK);
        }

        //Rysowanie drogi
        for (const auto& path : droga) 
        {
            DrawModel(cubeModel, path.position, 1.0f, WHITE);
        }
      /*  DrawModel(cubeModel, Vector3{ 0.0f, 0.15f, 1.0f }, 1.0f, WHITE);
        DrawModel(cubeModel, Vector3{ -3.0f, 0.2f, 1.0f }, 1.0f, WHITE);
        DrawModel(cubeModel, Vector3{ 3.0f, 0.1f, 1.0f }, 1.0f, WHITE);*/

        //Pokazywanie FPS mi nie działa :(
        DrawFPS(20, 20);

        EndMode3D();

        DrawText("Ue LEFT and RIGHT to move, SPACE to jump", 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Score : %i", score), 10, 30, 20, DARKGRAY);

        EndDrawing();
    }


    UnloadModel(robot);
    UnloadModel(cubeModel);
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}