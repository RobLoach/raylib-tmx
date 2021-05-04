/**********************************************************************************************
*
*   raylib-tmx - Tiled TMX Loader for raylib.
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
tmx_map* LoadTMX(const char* fileName);
void UnloadTMX(tmx_map* map);
Color ColorFromTMX(uint32_t color);
void DrawTMX(tmx_map *map, int posX, int posY, Color tint);

#ifdef __cplusplus
}
#endif

#endif  // INCLUDE_RAYLIB_TMX_H_

#ifdef RAYLIB_TMX_IMPLEMENTATION
#ifndef RAYLIB_TMX_IMPLEMENTATION_ONCE
#define RAYLIB_TMX_IMPLEMENTATION_ONCE

#include "raylib.h"
#include <tmx.h>

#ifdef __cplusplus
extern "C" {
#endif

Color ColorFromTMX(uint32_t color) {
	tmx_col_bytes res = tmx_col_to_bytes(color);
	return *((Color*)&res);
}

void *LoadTMXImage(const char *path) {
	Texture2D *returnValue = MemAlloc(sizeof(Texture2D));
	*returnValue = LoadTexture(path);
	return returnValue;
}

void UnloadTMXImage(void *ptr) {
    UnloadTexture(*((Texture2D *) ptr));
    MemFree(ptr);
}

void* MemReallocTMX(void* address, size_t len) {
    return MemRealloc(address, (int)len);
}

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
}

void UnloadTMX(tmx_map* map) {
    if (!map) {
        tmx_map_free(map);
        TraceLog(LOG_INFO, "TMX: Unloaded map");
    }
}

#ifndef RAYLIB_TMX_LINE_THICKNESS
#define RAYLIB_TMX_LINE_THICKNESS 2.5f
#endif

void DrawTMXPolyline(double offset_x, double offset_y, double **points, int points_count, Color color) {
	int i;
	for (i=1; i<points_count; i++) {
		DrawLineEx((Vector2){(float)(offset_x + points[i-1][0]), (float)(offset_y + points[i-1][1])},
		           (Vector2){(float)(offset_x + points[i][0]), (float)(offset_y + points[i][1])},
		           RAYLIB_TMX_LINE_THICKNESS, color);
	}
}

void DrawTMXPolygon(double offset_x, double offset_y, double **points, int points_count, Color color) {
	DrawTMXPolyline(offset_x, offset_y, points, points_count, color);
	if (points_count > 2) {
		DrawLineEx((Vector2){(float)(offset_x + points[0][0]), (float)(offset_y + points[0][1])},
		           (Vector2){(float)(offset_x + points[points_count-1][0]), (float)(offset_y + points[points_count-1][1])},
		           RAYLIB_TMX_LINE_THICKNESS, color);
	}
}

void DrawTMXLayerObjects(tmx_object_group *objgr, int posX, int posY, Color tint) {
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
			if (head->obj_type == OT_SQUARE) {
				DrawRectangleLinesEx(dest, (int)RAYLIB_TMX_LINE_THICKNESS, color);
			}
			else if (head->obj_type  == OT_POLYGON) {
				DrawTMXPolygon(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
			}
			else if (head->obj_type == OT_POLYLINE) {
				DrawTMXPolyline(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
			}
			else if (head->obj_type == OT_ELLIPSE) {
				DrawEllipseLines(dest.x + head->width / 2.0f, dest.y + head->height / 2.0f, head->width / 2.0f, head->height / 2.0f, color);
            } else if (head->obj_type == OT_TEXT) {
                tmx_text* text = head->content.text;
                int size = text->pixelsize;
                const char* msg = text->text;
                Color color = ColorFromTMX(text->color);
                color.a = tint.a;
                // TODO: Wrap the text
                DrawText(msg, (int)dest.x, (int)dest.y, size, color);
            }
		}
		head = head->next;
	}
}

void DrawTMXLayerImage(tmx_image *image, int posX, int posY, Color tint) {
	Texture2D *texture = (Texture2D*)image->resource_image;
	DrawTexture(*texture, posX, posY, tint);
}

void DrawTMXTile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               int dx, int dy, float opacity, unsigned int flags, Color tint) {
    Texture2D *texture = (Texture2D*)image;
    Color newTint = ColorAlpha(tint, opacity);
    DrawTextureRec(*texture, (Rectangle) {sx, sy, sw, sh}, (Vector2) {dx, dy}, newTint);
}

void DrawTMXLayerTiles(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint) {
	unsigned long i, j;
	unsigned int gid, x, y, w, h, flags;
	float op;
	tmx_tileset *ts;
	tmx_image *im;
	void* image;
	op = layer->opacity;
	for (i=0; i<map->height; i++) {
		for (j=0; j<map->width; j++) {
			gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
			if (map->tiles[gid] != NULL) {
				ts = map->tiles[gid]->tileset;
				im = map->tiles[gid]->image;
				x  = map->tiles[gid]->ul_x;
				y  = map->tiles[gid]->ul_y;
				w  = ts->tile_width;
				h  = ts->tile_height;
				if (im) {
                    image = im->resource_image;
				}
				else {
                    image = ts->image->resource_image;
				}
				flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
                DrawTMXTile(image, x, y, w, h, j*ts->tile_width + posX, i*ts->tile_height + posY, op, flags, tint);
			}
		}
	}
}

void DrawTMXLayer(tmx_map *map, tmx_layer *layers, int posX, int posY, Color tint) {
	while (layers) {
		if (layers->visible) {
			if (layers->type == L_GROUP) {
				DrawTMXLayer(map, layers->content.group_head, posX, posY, tint); // recursive call
			}
			else if (layers->type == L_OBJGR) {
				DrawTMXLayerObjects(layers->content.objgr, posX, posY, tint);
			}
			else if (layers->type == L_IMAGE) {
                DrawTMXLayerImage(layers->content.image, layers->offsetx + posX, layers->offsety + posY, tint);
			}
			else if (layers->type == L_LAYER) {
				DrawTMXLayerTiles(map, layers, posX, posY, tint);
			}
		}
		layers = layers->next;
	}
}

void DrawTMX(tmx_map *map, int posX, int posY, Color tint) {
    Color background = ColorFromTMX(map->backgroundcolor);
    // TODO: Apply the tint to the background color.
    DrawRectangle(posX, posY, map->width, map->height, background);
	DrawTMXLayer(map, map->ly_head, posX, posY, tint);
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_TMX_IMPLEMENTATION_ONCE
#endif  // RAYLIB_TMX_IMPLEMENTATION
