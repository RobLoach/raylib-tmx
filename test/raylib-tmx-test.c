#include <assert.h>

#include "raylib.h"

#define RAYLIB_TMX_IMPLEMENTATION
#include "raylib-tmx.h"

void trace(const char* text) {
    TraceLog(LOG_INFO, "================================");
    TraceLog(LOG_INFO, text);
    TraceLog(LOG_INFO, "================================");
}

int main(int argc, char *argv[]) {
    // Initialization
    SetTraceLogLevel(LOG_ALL);
    trace("raylib-tmx-test");

    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(640, 480, "[raylib-tmx] tests");
    assert(IsWindowReady());

    // Make sure we're running in the correct directory.
    assert(argc > 0);
    const char* dir = GetDirectoryPath(argv[0]);
    assert(ChangeDirectory(dir));

    tmx_map* map = LoadTMX("resources/desert.tmx");
    assert(map != NULL);

    const int orders[] = {R_NONE, R_RIGHTDOWN, R_RIGHTUP, R_LEFTDOWN, R_LEFTUP};
    const char *labels[] = {"R_NONE", "R_RIGHTDOWN", "R_RIGHTUP", "R_LEFTDOWN", "R_LEFTUP"};
    for (size_t i = 0; i < sizeof(orders)/sizeof(orders[0]); i++) {
	const char* test_text = ("Draw %s", labels[i]);
	trace(test_text);
	map->renderorder = orders[i];
	BeginDrawing();
	{
            ClearBackground(RAYWHITE);
            DrawTMX(map, 10, 10, WHITE);
            DrawTMXLayer(map, map->ly_head, 10, 10, WHITE);
	}
	EndDrawing();
    }

    UnloadTMX(map);

    CloseWindow();
    trace("raylib-tmx tests succesful");

    return 0;
}
