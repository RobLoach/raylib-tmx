# raylib-tmx

Load [Tiled](https://www.mapeditor.org) `.tmx` files for tile maps in [raylib](https://www.raylib.com), with [TMX C Loader](https://github.com/baylej/tmx).

![examples/raylib-tmx-example.png](examples/raylib-tmx-example.png)

## Usage

In your project, make sure to link its dependencies:
- [raylib](https://www.raylib.com/)
- [tmx](https://github.com/baylej/tmx)
- [libxml2](http://xmlsoft.org)
- [zlib](http://zlib.net/) (optional)

If you're using CMake, these come packed in.

### Example

``` c
#include "raylib.h"

#define RAYLIB_TMX_IMPLEMENTATION
#include "raylib-tmx.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "[raylib-tmx] example");

    tmx_map* map = LoadTMX("resources/desert.tmx");

    while(!WindowShouldClose()) {

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawTMX(map, 0, 0, WHITE);
        }
        EndDrawing();
    }

    UnloadTMX(map);

    CloseWindow();
    return 0;
}

```

See the [examples directory](examples) for more demonstrations of how to use *raylib-tmx*.

### API

``` c
tmx_map* LoadTMX(const char* fileName);
void UnloadTMX(tmx_map* map);
Color ColorFromTMX(uint32_t color);
void DrawTMX(tmx_map *map, int posX, int posY, Color tint);
void DrawTMXLayer(tmx_map *map, tmx_layer *layers, int posX, int posY, Color tint);
void DrawTMXTile(tmx_tile* tile, int posX, int posY, Color tint);
```

## Development

To build the example locally, and run tests, use [cmake](https://cmake.org/).

``` bash
git submodule update --init
mkdir build
cd build
cmake ..
make
cd examples
./raylib-tmx-example
```

This uses the [TMX C Loader](https://github.com/baylej/tmx), which is licensed under the [BSD 2-Clause "Simplified" License](https://github.com/baylej/tmx/blob/master/COPYING). Thank you to [Bayle Jonathan](https://github.com/baylej) for putting it together, and the [tmx example](https://github.com/baylej/tmx/blob/master/examples/raylib/raylib.c) this was inspired from.

## License

*raylib-tmx* is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
