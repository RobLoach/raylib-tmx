/**********************************************************************************************
*
*   raylib-tmx - Tiled TMX Loader for tile maps in raylib.
*
*   Copyright 2021 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       raylib https://www.raylib.com/
*       tmx https://github.com/baylej/tmx
*
*   LICENSE: zlib/libpng
*
*   raylib-tmx is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef INCLUDE_RAYLIB_TMX_H_
#define INCLUDE_RAYLIB_TMX_H_

#include "raylib.h" // NOLINT
#include "tmx.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

// TMX functions
tmx_map* LoadTMX(const char* fileName);                             // Load a Tiled .tmx tile map
void UnloadTMX(tmx_map* map);                                       // Unload the given Tiled map
Color ColorFromTMX(uint32_t color);                                 // Convert a Tiled color number to a raylib Color
void DrawTMX(tmx_map *map, int posX, int posY, Color tint);         // Render the given Tiled map to the screen
void DrawTMXLayers(tmx_map *map, tmx_layer *layers, int posX, int posY, Color tint); // Render all the given map layers to the screen
void DrawTMXLayer(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint); // Render a single map layer on the screen
void DrawTMXTile(tmx_tile* tile, int posX, int posY, Color tint);   // Render the given tile to the screen

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_RAYLIB_TMX_H_

#ifdef RAYLIB_TMX_IMPLEMENTATION
#ifndef RAYLIB_TMX_IMPLEMENTATION_ONCE
#define RAYLIB_TMX_IMPLEMENTATION_ONCE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert the given Tiled ARGB color to a raylib Color.
 *
 * @param color The Tiled color number in ARGB form.
 *
 * @return The raylib Color representation.
 */
Color ColorFromTMX(uint32_t color) {
	tmx_col_bytes res = tmx_col_to_bytes(color);
	return *((Color*)&res);
}

/**
 * Loads the provided path as a Texture for use with TMX.
 *
 * @param fileName The file path of the image to load.
 *
 * @return A void pointer representation of the Texture.
 *
 * @see UnloadTMXImage()
 *
 * @internal
 */
void *LoadTMXImage(const char *fileName) {
	Texture2D *returnValue = MemAlloc(sizeof(Texture2D));
	*returnValue = LoadTexture(fileName);
	return returnValue;
}

/**
 * Unload the provided Texture pointer.
 *
 * @internal
 */
void UnloadTMXImage(void *ptr) {
    if (ptr != NULL) {
        UnloadTexture(*((Texture2D *) ptr));
        MemFree(ptr);
    }
}

/**
 * Reallocate memory function callback for TMX.
 *
 * @internal
 */
void* MemReallocTMX(void* address, size_t len) {
    return MemRealloc(address, (int)len);
}

/**
 * Loads given .tmx Tiled file.
 *
 * @param fileName The .tmx file to load.
 *
 * @return A TMX Tiled map object pointer.
 *
 * @see UnloadTMX()
 * @todo Add LoadTMXFromMemory() to allow loading through a buffer: https://github.com/baylej/tmx/pull/58
 */
tmx_map* LoadTMX(const char* fileName) {
    // Register the TMX callbacks.
    tmx_alloc_func = MemReallocTMX;
    tmx_free_func = MemFree;
    tmx_img_load_func = LoadTMXImage;
	tmx_img_free_func = UnloadTMXImage;

    // Load the TMX file.
    tmx_map* map = tmx_load(fileName);
    if (!map) {
        TraceLog(LOG_ERROR, "TMX: Failed to load TMX file %s", fileName);
        return NULL;
    }
    TraceLog(LOG_INFO, "TMX: Loaded %ix%i map", map->width, map->height);
    return map;

    // TODO: Load using a buffer instead: https://github.com/baylej/tmx/pull/58
    // const char* fileText = LoadFileText(fileName);
    // tmx_map* map = tmx_load_buffer_path(fileText, TextLength(fileText), fileName);
    // if (!map) {
    //     TraceLog(LOG_ERROR, "TMX: Failed to load TMX file %s", fileName);
    //     return NULL;
    // }
    // TraceLog(LOG_INFO, "TMX: Loaded %ix%i map", map->width, map->height);
    // return map;
}

/**
 * Unloads the given TMX map.
 *
 * @param map The map to unload.
 */
void UnloadTMX(tmx_map* map) {
    if (!map) {
        tmx_map_free(map);
        TraceLog(LOG_INFO, "TMX: Unloaded map");
    }
}

#ifndef RAYLIB_TMX_LINE_THICKNESS
#define RAYLIB_TMX_LINE_THICKNESS 3.0f
#endif

/**
 * @internal
 */
void DrawTMXPolyline(double offset_x, double offset_y, double **points, int points_count, Color color) {
	for (int i = 1; i < points_count; i++) {
		DrawLineEx((Vector2){(float)(offset_x + points[i-1][0]), (float)(offset_y + points[i-1][1])},
		           (Vector2){(float)(offset_x + points[i][0]), (float)(offset_y + points[i][1])},
		           RAYLIB_TMX_LINE_THICKNESS, color);
	}
}

/**
 * @internal
 */
void DrawTMXPolygon(double offset_x, double offset_y, double **points, int points_count, Color color) {
	DrawTMXPolyline(offset_x, offset_y, points, points_count, color);
	if (points_count > 2) {
		DrawLineEx((Vector2){(float)(offset_x + points[0][0]), (float)(offset_y + points[0][1])},
		           (Vector2){(float)(offset_x + points[points_count-1][0]), (float)(offset_y + points[points_count-1][1])},
		           RAYLIB_TMX_LINE_THICKNESS, color);
	}
}

/**
 * @internal
 */
void DrawTMXText(tmx_text* text, Rectangle dest, Color tint) {
    float fontSize = (float)text->pixelsize;
    const char* message = text->text;
    Font font = GetFontDefault();
    // TODO: Figure out the correct spacing.
    float spacing = (float)text->kerning * fontSize / 12.0f;
    Vector2 position = {dest.x, dest.y};

    if (text->wrap == 0) {
        Vector2 textSize = MeasureTextEx(font, message, fontSize, spacing);
        if (text->halign == HA_CENTER) {
            position.x = dest.x + dest.width / 2.0f - textSize.x / 2.0f;
        }
        else if (text->halign == HA_RIGHT) {
            position.x = dest.x + dest.width - textSize.x;
        }
        if (text->valign == VA_CENTER) {
            position.y = dest.y + dest.height / 2.0f - textSize.y / 2.0f;
        }
        else if (text->valign == VA_BOTTOM) {
            position.y = dest.y + dest.height - textSize.y;
        }
        DrawTextEx(font, message, position, fontSize, spacing, tint);
    }
    else {
        Vector2 origin = {0.0f, 0.0f};
        DrawTextPro(font, message, position, origin, 0.0f, fontSize, spacing, tint);
    }
}

/**
 * @internal
 */
void DrawTMXLayerObjects(tmx_map *map, tmx_object_group *objgr, int posX, int posY, Color tint) {
	tmx_object *head = objgr->head;
	Color color = ColorFromTMX(objgr->color);
    // TODO: Merge the tint

	while (head) {
		if (head->visible) {
            Rectangle dest = (Rectangle) {
                (float)posX + (float)head->x,
                (float)posY + (float)head->y,
                (float)head->width,
                (float)head->height
            };
            switch (head->obj_type) {
                case OT_SQUARE:
				    DrawRectangleLinesEx(dest, (int)RAYLIB_TMX_LINE_THICKNESS, color);
                    break;
                case OT_POLYGON:
                    DrawTMXPolygon(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
                    break;
                case OT_POLYLINE:
                    DrawTMXPolyline(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
                    break;
                case OT_ELLIPSE:
                    DrawEllipseLines(dest.x + head->width / 2.0, dest.y + head->height / 2.0, head->width / 2.0f, head->height / 2.0f, color);
                    break;
                case OT_TILE: {
                    int gid = head->content.gid;
                    if (map->tiles[gid] != NULL) {
                        DrawTMXTile(map->tiles[gid], dest.x, dest.y - dest.height, tint);
                    }
                } break;
                case OT_TEXT: {
                    tmx_text* text = head->content.text;
                    Color textColor = ColorFromTMX(text->color);
                    // TODO: Fix application of the tint.
                    textColor.a = tint.a;
                    DrawTMXText(text, dest, textColor);
                } break;
                case OT_POINT:
                    DrawCircle(dest.x + head->width / 2.0, dest.y + head->height / 2.0, 5, color);
                    break;
            }
		}
		head = head->next;
	}
}

/**
 * @internal
 */
void DrawTMXLayerImage(tmx_image *image, int posX, int posY, Color tint) {
    if (image->resource_image) {
        Texture2D *texture = (Texture2D*)image->resource_image;
        DrawTexture(*texture, posX, posY, tint);
    }
}

/**
 * Render a single TMX tile on the screen.
 *
 * @param tile Which tile to render on the screen.
 * @param posX The X position of the tile.
 * @param posY The Y position of the tile.
 * @param tint How to tint the tile when rendering.
 */
void DrawTMXTile(tmx_tile* tile, int posX, int posY, Color tint) {
    Texture* image;
    Rectangle srcRect;
    Vector2 position;
    position.x = (float)posX;
    position.y = (float)posY;

#ifdef RAYLIB_TMX_SUPPORT_ANIMATIONS
    // TODO: Process the animation https://github.com/baylej/tmx/pull/64
    if(tile->animation) {
        int tile_id = tile->animation[tile->current_animation_frame].tile_id;
        tile = &tile->tileset->tiles[tile_id];
    }
#endif

    srcRect.x  = tile->ul_x;
    srcRect.y  = tile->ul_y;
    srcRect.width  = tile->tileset->tile_width;
    srcRect.height = tile->tileset->tile_height;

    // Find the image
    tmx_image *im = tile->image;

    if (im && im->resource_image) {
        image = (Texture*)im->resource_image;
    }
    else if (tile->tileset->image->resource_image) {
        image = (Texture*)tile->tileset->image->resource_image;
    }

    if (image) {
        DrawTextureRec(*image, srcRect, position, tint);
    }
}

/**
 * @internal
 */
void DrawTMXLayerTiles(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint) {
	unsigned int gid, baseGid; //, flags;
	tmx_tileset *ts;
    Color newTint = ColorAlpha(tint, (float)layer->opacity);

    switch (map->renderorder) {
        case R_NONE:
        case R_RIGHTDOWN:
            for (int y = 0; y < map->height; y++) {
                for (int x = 0; x < map->width; x++) {
                    baseGid = layer->content.gids[(y * map->width) + x];
                    gid = (baseGid) & TMX_FLIP_BITS_REMOVAL;
                    // TODO: Add the flags of the tile to Draw.
                    // flags = baseGid & ~TMX_FLIP_BITS_REMOVAL;
                    if (map->tiles[gid] != NULL) {
                        ts = map->tiles[gid]->tileset;
                        DrawTMXTile(map->tiles[gid], posX + x * ts->tile_width, posY + y * ts->tile_height, newTint);
                    }
                }
            }
        break;
        case R_RIGHTUP:
            for (int y = map->height - 1; y >= 0; y--) {
                for (int x = 0; x < map->width; x++) {
                    baseGid = layer->content.gids[(y * map->width) + x];
                    gid = (baseGid) & TMX_FLIP_BITS_REMOVAL;
                    // TODO: Add the flags of the tile to Draw.
                    // flags = baseGid & ~TMX_FLIP_BITS_REMOVAL;
                    if (map->tiles[gid] != NULL) {
                        ts = map->tiles[gid]->tileset;
                        DrawTMXTile(map->tiles[gid], posX + x * ts->tile_width, posY + y * ts->tile_height, newTint);
                    }
                }
            }
        break;
        case R_LEFTDOWN:
            for (int y = 0; y < map->height; y++) {
                for (int x = map->width - 1; x >= 0; x--) {
                    baseGid = layer->content.gids[(y * map->width) + x];
                    gid = (baseGid) & TMX_FLIP_BITS_REMOVAL;
                    // TODO: Add the flags of the tile to Draw.
                    // flags = baseGid & ~TMX_FLIP_BITS_REMOVAL;
                    if (map->tiles[gid] != NULL) {
                        ts = map->tiles[gid]->tileset;
                        DrawTMXTile(map->tiles[gid], posX + x * ts->tile_width, posY + y * ts->tile_height, newTint);
                    }
                }
            }
        break;
        case R_LEFTUP:
            for (int y = map->height - 1; y >= 0; y--) {
                for (int x = map->width - 1; x >= 0; x--) {
                    baseGid = layer->content.gids[(y * map->width) + x];
                    gid = (baseGid) & TMX_FLIP_BITS_REMOVAL;
                    // TODO: Add the flags of the tile to Draw.
                    // flags = baseGid & ~TMX_FLIP_BITS_REMOVAL;
                    if (map->tiles[gid] != NULL) {
                        ts = map->tiles[gid]->tileset;
                        DrawTMXTile(map->tiles[gid], posX + x * ts->tile_width, posY + y * ts->tile_height, newTint);
                    }
                }
            }
        break;
    }

}

/**
 * Render the given layer to the screen.
 *
 * @param map The TMX map that holds the layer.
 * @param layer The layer to render on the screen.
 * @param posX The X position of the screen.
 * @param posY The Y position of the screen.
 * @param tint How to tint the rendering of the layer.
 */
void DrawTMXLayer(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint) {
    switch (layer->type) {
        case L_GROUP:
            DrawTMXLayers(map, layer->content.group_head, posX + layer->offsetx, posY + layer->offsety, tint); // recursive call
            break;
        case L_OBJGR:
            DrawTMXLayerObjects(map, layer->content.objgr, posX + layer->offsetx, posY + layer->offsety, tint);
            break;
        case L_IMAGE:
            DrawTMXLayerImage(layer->content.image, posX + layer->offsetx, posY + layer->offsety, tint);
            break;
        case L_LAYER:
            DrawTMXLayerTiles(map, layer, posX + layer->offsetx, posY + layer->offsety, tint);
            break;
        case L_NONE:
            // Nothing.
            break;
    }
}

/**
 * Draws all of the given TMX map layers to the screen.
 *
 * @param map The TMX map that holds the layer.
 * @param layers The layer to render on the screen.
 * @param posX The X position of the screen.
 * @param posY The Y position of the screen.
 * @param tint How to tint the rendering of the layer.
 */
void DrawTMXLayers(tmx_map *map, tmx_layer *layers, int posX, int posY, Color tint) {
	while (layers) {
		if (layers->visible) {
            DrawTMXLayer(map, layers, posX, posY, tint);
		}
		layers = layers->next;
	}
}

/**
 * Render the given map to the screen.
 *
 * @param map The TMX map to render to the screen.
 * @param posX The X position of the screen.
 * @param posY The Y position of the screen.
 * @param tint How to tint the rendering of the layer.
 */
void DrawTMX(tmx_map *map, int posX, int posY, Color tint) {
    Color background = ColorFromTMX(map->backgroundcolor);
    // TODO: Apply the tint to the background color.
    DrawRectangle(posX, posY, map->width, map->height, background);
	DrawTMXLayers(map, map->ly_head, posX, posY, tint);
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_TMX_IMPLEMENTATION_ONCE
#endif  // RAYLIB_TMX_IMPLEMENTATION
