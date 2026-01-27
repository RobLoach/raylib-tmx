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

Vector2 mousePosition = {0};
tmx_object *selected = NULL;

void UpdateCollisons(tmx_object *object, RaylibTMXCollision collision, void* userdata) {
    tmx_map* map = (tmx_map*)userdata;
    if (selected == NULL && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch (collision.type)
        {
            case COLLISION_RECT:
            case COLLISION_ELLIPSE: {
                if (CheckCollisionPointRec(mousePosition, collision.rect)) {
                    selected = object;
                }
            } break;
            case COLLISION_POINT: {
                if (CheckCollisionPointCircle(mousePosition, collision.point, 5.0f)) {
                    selected = object;
                }
            } break;
            case COLLISION_POLYGON:
            case COLLISION_POLYLINE: {
                double** points = collision.polygon.points;
                int count       = collision.polygon.count;
                Vector2* check  = malloc(count * sizeof(Vector2));
                int threshold   = RAYLIB_TMX_LINE_THICKNESS;
                for (int i = 0; i < count; i++) {
                    float posX = (float)(object->x + points[i][0]);
                    float posY = (float)(object->y + points[i][1]);
                    check[i] = (Vector2){posX, posY};
                }
                if (object->obj_type == OT_POLYGON) {
                    if (CheckCollisionPointPoly(mousePosition, check, count)) {
                        selected = object;
                    }
                }
                if (object->obj_type == OT_POLYLINE) {
                    if (CheckCollisionPointLine(mousePosition, check[0], check[1], threshold)) {
                        selected = object;
                    }
                }
                free(check);
            } break;
        }
    }
}

void DrawCollisons(tmx_object *object, RaylibTMXCollision collision, void* userdata) {
    Vector2* position = (Vector2*)userdata;
    switch (collision.type)
    {
        case COLLISION_RECT: {
            Color color = object->obj_type == OT_TILE ? RED : BLUE;
            collision.rect.x += position->x;
            collision.rect.y += position->y;
            DrawRectangleRec(collision.rect, color);
        } break;
        case COLLISION_POINT: {
            collision.point.x += position->x;
            collision.point.y += position->y;
            int centerX = (int)(collision.point.x + object->width / 2.0);
            int centerY = (int)(collision.point.y + object->height / 2.0);
            DrawCircle(centerX, centerY, 5.0f, YELLOW);
        } break;
        case COLLISION_POLYGON:
        case COLLISION_POLYLINE: {
            double** points = collision.polygon.points;
            int count       = collision.polygon.count;
            double offset_x = object->x + position->x;
            double offset_y = object->y + position->y;
            if (object->obj_type == OT_POLYGON) {
                DrawTMXPolygon(offset_x, offset_y, points, count, ORANGE);
            }
            if (object->obj_type == OT_POLYLINE) {
                DrawTMXPolyline(offset_x, offset_y, points, count, PURPLE);
            }
        } break;
        case COLLISION_ELLIPSE: {
            int centerX   = (int)(collision.rect.x + position->x);
            int centerY   = (int)(collision.rect.y + position->y);
            float radiusH = (float)(collision.rect.width);
            float radiusV = (float)(collision.rect.height);
            DrawEllipseLines(centerX, centerY, radiusH, radiusV, GREEN);
        } break;
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
        mousePosition = GetMousePosition();
        mousePosition.x -= position.x;
        mousePosition.y -= position.y;
        if (IsKeyDown(KEY_LEFT))  position.x += 2;
        if (IsKeyDown(KEY_UP))    position.y += 2;
        if (IsKeyDown(KEY_RIGHT)) position.x -= 2;
        if (IsKeyDown(KEY_DOWN))  position.y -= 2;
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            drawCollisions = !drawCollisions;
        }
        CollisionsTMXForeach(map, UpdateCollisons, map);
        if (selected != NULL) {
            selected->x           = mousePosition.x;
            selected->y           = mousePosition.y;
        }
        if (selected != NULL && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            selected = NULL;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawTMX(map, position.x, position.y, WHITE);
            if (drawCollisions) CollisionsTMXForeach(map, DrawCollisons, &position);
            DrawFPS(10, 10);
            DrawText("Move arround with arrow keys. ", 10, 40, 20, DARKGRAY);
            DrawText("Select an collision and its object with left click and move it arround with mouse position.", 10, 60, 20, DARKGRAY);
            DrawText("Unselect with right click and leave it in the current mouse position. ", 10, 80, 20, DARKGRAY);
            DrawText("Click middle mouse to toggle collisions drawing. ", 10, 100, 20, DARKGRAY);
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
