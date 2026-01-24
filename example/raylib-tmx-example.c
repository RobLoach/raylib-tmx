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

void DrawCollisons(tmx_object *object, RaylibTMXCollision collision, void* userdata) {
    bool *draw = (bool*)userdata;
    if (!*draw) return;
    switch (object->obj_type)
    {
	    case OT_SQUARE: DrawRectangleRec(collision.rect, BLUE); break;
	    case OT_TILE:   DrawRectangleRec(collision.rect, RED);  break;
	    default: return; break;
    }
}

int main(int argc, char *argv[]) {
    // Initialization
    //--------------------------------------------------------------------------------------
    // Make sure we're running in the correct directory.
    ChangeDirectory(GetDirectoryPath(argv[0]));

    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "[raylib-tmx] example");
    SetTargetFPS(60);

    tmx_map* map = LoadTMX(argc > 1 ? argv[1] : "resources/desert.tmx");
    Vector2 position = {0, 0};
    bool drawCollisions = false;
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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) drawCollisions = !drawCollisions;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawTMX(map, position.x, position.y, WHITE);
            HandleCollisions(map, DrawCollisons, &drawCollisions);
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
