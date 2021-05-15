/*******************************************************************************************
*
*   [raylib-tmx] example - TMX Tiled Map editor loader for raylib.
*
*   This example has been created using raylib 3.7 (www.raylib.com)
*   raylib-tmx is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Example by Rob Loach (@RobLoach)
*
*   Copyright (c) 2021 Rob Loach (@RobLoach)
*
********************************************************************************************/

#include "raylib.h"

#define RAYLIB_TMX_IMPLEMENTATION
#include "raylib-tmx.h"

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "[raylib-tmx] example");
    SetTargetFPS(60);

    tmx_map* map = LoadTMX("resources/desert.tmx");
    Vector2 position = {0, 0};
    //--------------------------------------------------------------------------------------

    while(!WindowShouldClose()) {

        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_LEFT)) {
            position.x += 2;
        }
        if (IsKeyDown(KEY_UP)) {
            position.y += 2;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            position.x -= 2;
        }
        if (IsKeyDown(KEY_DOWN)) {
            position.y -= 2;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawTMX(map, position.x, position.y, WHITE);
            DrawFPS(10, 10);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTMX(map);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}
