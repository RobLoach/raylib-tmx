#include <assert.h>

#include "raylib.h"

#define RAYLIB_TMX_IMPLEMENTATION
#include "raylib-tmx.h"

int main(int argc, char *argv[]) {
    // Initialization
    SetTraceLogLevel(LOG_ALL);
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-tmx-test");
    TraceLog(LOG_INFO, "================================");

    InitWindow(640, 480, "[raylib-tmx] tests");
    assert(IsWindowReady());

    // Make sure we're running in the correct directory.
    assert(argc > 0);
    const char* dir = GetDirectoryPath(argv[0]);
    assert(ChangeDirectory(dir));

    tmx_map* map = LoadTMX("resources/desert.tmx");
    assert(map != NULL);

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawTMX(map, 10, 10, WHITE);
    }
    EndDrawing();

    UnloadTMX(map);

    CloseWindow();
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, "raylib-tmx tests succesful");
    TraceLog(LOG_INFO, "================================");

    return 0;
}
