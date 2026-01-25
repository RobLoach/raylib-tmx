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

tmx_object *selected = NULL;

void UpdateCollisons(tmx_object *object, RaylibTMXCollision collision, void* userdata) {
    tmx_map* map = (tmx_map*)userdata;
    if (selected == NULL && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePosition = GetMousePosition();
        switch (object->obj_type)
        {
            case OT_TILE:
            case OT_SQUARE:
            case OT_ELLIPSE: {
                if (CheckCollisionPointRec(mousePosition, collision.rect)) {
                    selected = object;
                }
            } break;
            case OT_POINT: {
                if (CheckCollisionPointCircle(mousePosition, collision.point, 5.0f)) {
                    selected = object;
                }
            } break;
            case OT_POLYGON: {
                double** points      = collision.polygon.points;
                int count            = collision.polygon.count;
                Vector2* checkPoints = malloc(count * sizeof(Vector2));
                for (int i = 0; i < count; i++) {
                    float posX = (float)(object->x + points[i][0]);
                    float posY = (float)(object->y + points[i][1]);
                    checkPoints[i] = (Vector2){posX, posY};
                }
                if (CheckCollisionPointPoly(mousePosition, checkPoints, count)) {
                    selected = object;
                }
                free(checkPoints);
            } break;
	        default: return; break;
        }
    }
}

void DrawCollisons(tmx_object *object, RaylibTMXCollision collision, void* userdata) {
    Vector2* position = (Vector2*)userdata;
    switch (object->obj_type)
    {
	    case OT_SQUARE: {
            collision.rect.x += position->x;
            collision.rect.y += position->y;
            DrawRectangleRec(collision.rect, BLUE);
        } break;
	    case OT_TILE: {
            collision.rect.x += position->x;
            collision.rect.y += position->y;
            DrawRectangleRec(collision.rect, RED);
        } break;
        case OT_POINT: {
            collision.point.x += position->x;
            collision.point.y += position->y;
            int centerX = (int)(collision.point.x + object->width / 2.0);
            int centerY = (int)(collision.point.y + object->height / 2.0);
            DrawCircle(centerX, centerY, 5.0f, YELLOW);
            DrawPixelV(collision.point, PURPLE);
        } break;
        case OT_POLYGON: {
            double** points = collision.polygon.points;
            int count       = collision.polygon.count;
            double offset_x = object->x + position->x;
            double offset_y = object->y + position->y;
            DrawTMXPolygon(offset_x, offset_y, points, count, ORANGE);
        } break;
        case OT_ELLIPSE: {
            int centerX   = (int)(collision.rect.x + position->x);
            int centerY   = (int)(collision.rect.y + position->y);
            float radiusH = (float)(collision.rect.width);
            float radiusV = (float)(collision.rect.height);
            DrawEllipseLines(centerX, centerY, radiusH, radiusV, GREEN);
        } break;
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
        if (IsKeyDown(KEY_LEFT))  position.x += 2;
        if (IsKeyDown(KEY_UP))    position.y += 2;
        if (IsKeyDown(KEY_RIGHT)) position.x -= 2;
        if (IsKeyDown(KEY_DOWN))  position.y -= 2;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drawCollisions = !drawCollisions;
        }
        CollisionsTMXForeach(map, UpdateCollisons, map);
        if (selected != NULL) {
            Vector2 mousePosition = GetMousePosition();
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
