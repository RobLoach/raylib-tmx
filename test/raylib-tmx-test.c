#include <assert.h>

#include "raylib.h"

#define RAYLIB_TMX_IMPLEMENTATION
#include "raylib-tmx.h"

void RaylibTMXTrace(const char* text) {
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, text);
    TraceLog(LOG_INFO, "================================");
}

Texture* AssertTMXTexture(tmx_map* map)
{
    assert(map->ts_head != NULL);
    assert(map->ts_head->tileset != NULL);
    tmx_tileset* tileset = map->ts_head->tileset;
    Texture *image = NULL;
    if (tileset->image->resource_image) {
        image = (Texture*) tileset->image->resource_image;
        if (IsTextureValid(*image)) return image;
    }

    assert(tileset->tiles != NULL);
    tmx_tile* tile = tileset->tiles;
    if (tile->image) {
        if (tile->image->resource_image) {
            image = (Texture*) tile->image->resource_image;
            if (IsTextureValid(*image)) return image;
        }
    }

    return NULL;
}

void AssertRenderOrder(tmx_map* map)
{
    RaylibTMXTrace("Testing Render Order");
    const int orders[] = {R_NONE, R_RIGHTDOWN, R_RIGHTUP, R_LEFTDOWN, R_LEFTUP};
    const char *labels[] = {"R_NONE", "R_RIGHTDOWN", "R_RIGHTUP", "R_LEFTDOWN", "R_LEFTUP"};
    for (size_t i = 0; i < sizeof(orders)/sizeof(orders[0]); i++) {
	    RaylibTMXTrace(TextFormat("Draw %s", labels[i]));
	    map->renderorder = (enum tmx_map_renderorder) orders[i];
	    BeginDrawing();
	    {
            ClearBackground(RAYWHITE);
            DrawTMX(map, 10, 10, WHITE);
            DrawTMXLayer(map, map->ly_head, 10, 10, WHITE);
	    }
	    EndDrawing();
    }
}

int main(int argc, char *argv[]) {
    // Initialization
    SetTraceLogLevel(LOG_ALL);
    RaylibTMXTrace("raylib-tmx-test");

    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(640, 480, "[raylib-tmx] tests");
    assert(IsWindowReady());

    // Make sure we're running in the correct directory.
    assert(argc > 0);
    const char* dir = GetDirectoryPath(argv[0]);
    assert(ChangeDirectory(dir));

    tmx_map* map = NULL;
    map = LoadTMX("resources/desert.tmx");
    assert(map != NULL);
    assert(AssertTMXTexture(map) != NULL);
    UnloadTMX(map);
    map = NULL;
    assert(map == NULL);
    map = LoadTMXFromMemory("resources/desert.tmx");
    assert(map != NULL);
    assert(AssertTMXTexture(map) == NULL);
    UnloadTMX(map);
    map = NULL;
    assert(map == NULL);
    ChangeDirectory("resources");
    map = LoadTMXFromMemory("desert.tmx");
    ChangeDirectory(GetApplicationDirectory());
    assert(AssertTMXTexture(map) != NULL);

    AssertRenderOrder(map);

    UnloadTMX(map);

    CloseWindow();
    RaylibTMXTrace("raylib-tmx tests succesful");

    return 0;
}
