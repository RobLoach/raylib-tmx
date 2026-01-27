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

#include <stdlib.h>
#include <math.h>

#include "raylib.h" // NOLINT
#include "tmx.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

// TMX structs
typedef struct AnimationState {
    int currentFrame;
    float frameCounter;
} AnimationState;

typedef union {
    Rectangle rect;
    Vector2   point;
    struct {
        double** points;
        int count;
    } polygon;
} RaylibTMXCollision;

typedef void (*tmx_collision_functor)(tmx_object *object, RaylibTMXCollision collision, void* userdata);

// TMX functions
tmx_map* LoadTMX(const char* fileName);                                                                // Load a Tiled .tmx tile map
void UnloadTMX(tmx_map* map);                                                                          // Unload the given Tiled map
Color ColorFromTMX(uint32_t color);                                                                    // Convert a Tiled color number to a raylib Color
void DrawTMX(tmx_map *map, int posX, int posY, Color tint);                                            // Render the given Tiled map to the screen
void DrawTMXLayers(tmx_map *map, tmx_layer *layers, int posX, int posY, Color tint);                   // Render all the given map layers to the screen
void DrawTMXLayer(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint);                     // Render a single map layer on the screen
void DrawTMXTile(tmx_tile* tile, unsigned int baseGid, int posX, int posY, Color tint);                // Render the given tile to the screen
void DrawTMXObjectTile(tmx_tile* tile, int baseGid, Rectangle destRect, float rotation, Color tint);   // Render the tile of a given object to the screen
void UpdateTMXTileAnimation(tmx_map* map, tmx_tile** tile);                                            // Controls the animation state of a tile and return the LID of the current animation
void CollisionsTMXForeach(tmx_map *map, tmx_collision_functor callback, void* userdata);               // Returns each tmx_object on a given map and their collisions on a callback
RaylibTMXCollision HandleTMXCollision(tmx_object* object);                                              // Returns a single RaylibTMXCollision for an given object pointer

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
    return MemRealloc(address, (unsigned int)len);
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
 * Unload animations withing layers.
 *
 * @internal
 */
void UnloadAnimations(tmx_map* map) {
    for (tmx_layer *layer = (map->ly_head); layer != NULL; layer = layer->next) {
	    switch (layer->type)
	    {
		    default: continue; break;
		    case L_LAYER: {
		        for (unsigned int y = 0; y < map->height; y++) {
			        for (unsigned int x = 0; x < map->width; x++) {
			            unsigned int index = (y * map->width) + x;
			            unsigned int baseGid = layer->content.gids[index];
			            unsigned int gid = baseGid & TMX_FLIP_BITS_REMOVAL;
			            if (map->tiles[gid] != NULL) {
				            tmx_tile* tile = map->tiles[gid];
				            if(tile->animation) {
				                if (tile->user_data.pointer != NULL) {
					                MemFree(tile->user_data.pointer);
				                }
				            }
			            }
			        }
		        }
		    } break;
	    }
	}
}

/**
 * Unloads the given TMX map.
 *
 * @param map The map to unload.
 */
void UnloadTMX(tmx_map* map) {
    if (map) {
        UnloadAnimations(map);
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
            switch (head->obj_type)
            {
                case OT_SQUARE:
				    DrawRectangleLinesEx(dest, (int)RAYLIB_TMX_LINE_THICKNESS, color);
                    break;
                case OT_POLYGON:
                    DrawTMXPolygon(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
                    break;
                case OT_POLYLINE:
                    DrawTMXPolyline(dest.x, dest.y, head->content.shape->points, head->content.shape->points_len, color);
                    break;
                case OT_ELLIPSE: {
                    int centerX   = (int)(dest.x + head->width  / 2.0);
                    int centerY   = (int)(dest.y + head->height / 2.0);
                    float radiusH = (float)head->width / 2.0f;
                    float radiusV = (float)head->height / 2.0f;
                    DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
                } break;
		        case OT_TILE: {
		            int baseGid = head->content.gid;
                    int gid = baseGid & TMX_FLIP_BITS_REMOVAL;
		            if (!map->tiles[gid]) continue;
			        tmx_tile *tile = map->tiles[gid];
                    if (tile->animation) UpdateTMXTileAnimation(map, &tile);
			        DrawTMXObjectTile(tile, baseGid, dest, (float)head->rotation, tint);
		        } break;
                case OT_TEXT: {
                    tmx_text* text = head->content.text;
                    Color textColor = ColorFromTMX(text->color);
                    // TODO: Fix application of the tint.
                    textColor.a = tint.a;
                    DrawTMXText(text, dest, textColor);
                } break;
                case OT_POINT: {
                    int centerX = (int)(dest.x + head->width  / 2.0);
                    int centerY = (int)(dest.y + head->height / 2.0);
                    DrawCircle(centerX, centerY, 5.0f, color);
                } break;
		        case OT_NONE: break;
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
 * @internal
 */
void UpdateTMXTileAnimation(tmx_map* map, tmx_tile** tile){
    tmx_tile* currentTile = *tile;
    AnimationState* animState = (AnimationState*) currentTile->user_data.pointer;
    if (!animState) {
        animState = malloc(sizeof(AnimationState));
        animState->currentFrame = 0;
        animState->frameCounter = 0.0f;
	    currentTile->user_data.pointer = animState;
    }
    int*   currentFrame      = &animState->currentFrame;
    float* frameCounter      = &animState->frameCounter;
    int    animationLength   = (int)currentTile->animation_len;
    tmx_anim_frame animation = currentTile->animation[*currentFrame];
    float  frameThreshold    = (float)(GetFPS() * (int)animation.duration) / 1000.0f;    

    (*frameCounter)++;
    if (*frameCounter >= (frameThreshold)) {
	    *frameCounter = 0;
	    (*currentFrame)++;
	    if (*currentFrame >= animationLength) *currentFrame = 0;
    }

    tmx_tileset_list* tileset_list = map->ts_head;
    while (tileset_list) {
        if (currentTile->tileset == tileset_list->tileset) {
            unsigned int gid = tileset_list->firstgid + animation.tile_id;
            if (map->tiles[gid]) {
                *tile = map->tiles[gid];
                return;
            }
        }
        tileset_list = tileset_list->next;
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
void DrawTMXTile(tmx_tile* tile, unsigned int baseGid, int posX, int posY, Color tint) {
    Texture* image = NULL;
    Rectangle srcRect;
    Vector2 origin = {0};
    float rotation = 0;

    srcRect.x      = (float)tile->ul_x;
    srcRect.y      = (float)tile->ul_y;
    srcRect.width  = (float)tile->tileset->tile_width;
    srcRect.height = (float)tile->tileset->tile_height;

    Rectangle destRect = srcRect;
    destRect.x = (float)posX;
    destRect.y = (float)posY;

    int flags = (int)baseGid & ~TMX_FLIP_BITS_REMOVAL;
    if  (flags) {
        int is_diagonally_fliped   = (baseGid & TMX_FLIPPED_DIAGONALLY);
        int is_horizontally_fliped = (int)(baseGid & TMX_FLIPPED_HORIZONTALLY);
        int is_vertically_fliped   = (baseGid & TMX_FLIPPED_VERTICALLY);
	    if (is_diagonally_fliped) {
            if (is_horizontally_fliped && is_vertically_fliped) {
                srcRect.height = (float) -fabs(srcRect.height);
                rotation = 270.0f;
            } else if (is_horizontally_fliped) {
                rotation = 90.0f;
            } else if (is_vertically_fliped) {
                rotation = 270.0f;
            } else {
                srcRect.height = (float) -fabs(srcRect.height);
                rotation = -270.0f;
            }
            origin.x    = destRect.width  * 0.5f;
            origin.y    = destRect.height * 0.5f;
            destRect.x += origin.x;
            destRect.y += origin.y;
	    } else {
            if (is_horizontally_fliped) {
		        srcRect.width =  (float) -fabs(srcRect.width);
	        }
            if (is_vertically_fliped) {
		        srcRect.height = (float) -fabs(srcRect.height);
	        }
        }
    }

    // Find the image
    tmx_image *im = tile->image;    
    if (im && im->resource_image) {
        image = (Texture*)im->resource_image;
    }
    else if (tile->tileset->image->resource_image) {
        image = (Texture*)tile->tileset->image->resource_image;
    }

    if (image) {
        DrawTexturePro(*image, srcRect, destRect, origin, rotation, tint);
    }
}

/**
 * Render a single TMX tile of a TMX Object on the screen.
 *
 * @param tile      Which tile to render on the screen.
 * @param gid       The GID of the object used to calculate it flags
 * @param destRect  The destination of the tile on screen
 * @param rotation  The rotation of the tile on screen
 * @param tint      How to tint the tile when rendering.
 */
void DrawTMXObjectTile(tmx_tile* tile, int baseGid, Rectangle destRect, float rotation, Color tint) {
    Texture* image = NULL;
    Vector2 origin = {0};
    Rectangle srcRect = (Rectangle) {
        .x      = (float)tile->ul_x,
        .y      = (float)tile->ul_y,
        .width  = (float)tile->width,
        .height = (float)tile->height
    };

    switch (tile->tileset->objectalignment)
    {
        case OA_TOPLEFT: /*RAYLIB DEFAULT*/ break;
        case OA_NONE:
        case OA_BOTTOMLEFT: { /* TILED DEFAULT */
            destRect.y -= destRect.height;
        } break;
        case OA_TOP:         /* TODO */ break;
        case OA_LEFT:        /* TODO */ break;
        case OA_BOTTOM:      /* TODO */ break;
        case OA_RIGHT:       /* TODO */ break;
        case OA_TOPRIGHT:    /* TODO */ break;
        case OA_BOTTOMRIGHT: /* TODO */ break;
        case OA_CENTER:      /* TODO */ break;
    }

    int flags = baseGid & ~TMX_FLIP_BITS_REMOVAL;
    if  (flags) {
        int is_horizontally_fliped = (int)((unsigned int)baseGid & TMX_FLIPPED_HORIZONTALLY);
        if (is_horizontally_fliped) {
		    srcRect.width = (float) -fabs(srcRect.width);
	    }
        int is_vertically_fliped = baseGid & TMX_FLIPPED_VERTICALLY;
        if (is_vertically_fliped) {
		    srcRect.height = (float) -fabs(srcRect.height);
	    }
    }

    // Find the image
    tmx_image *im = tile->image;

    if (im && im->resource_image) {
        image = (Texture*)im->resource_image;
    } else if (tile->tileset->image->resource_image) {
        image = (Texture*)tile->tileset->image->resource_image;
    }

    if (image) DrawTexturePro(*image, srcRect, destRect, origin, rotation, tint);
}

/**
 * @internal
 */
void DrawTMXLayerTiles(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint)
{
    Color newTint = ColorAlpha(tint, (float)layer->opacity);
    for (unsigned int y = 0; y < map->height; y++) {
        for (unsigned int x = 0; x < map->width; x++) {
	        unsigned int cellIndex = (y * map->width) + x;
            unsigned int baseGid = layer->content.gids[cellIndex];
            unsigned int gid = (baseGid) & TMX_FLIP_BITS_REMOVAL;
            if (!map->tiles[gid]) continue;
	        tmx_tile* tile = map->tiles[gid];
            if (tile->animation) UpdateTMXTileAnimation(map, &tile);
	        int drawX = (int)((unsigned int)posX + x * tile->width);
	        int drawY = (int)((unsigned int)posY + y * tile->height);
	        DrawTMXTile(tile, baseGid, drawX, drawY, newTint);
        }
    }
}

/**
 * @internal
 */
void HandleTMXLayerRenderOrder(tmx_map *map, tmx_layer *layer, int posX, int posY, Color tint) {
    switch (map->renderorder)
    {
	    case R_NONE: {} break;
        case R_RIGHTDOWN: {} break;
        case R_RIGHTUP: {
            // Reverse Y axis
	        posY = posY - (int)((map->height - 1) * map->tile_height);
            break;
	    }
        case R_LEFTDOWN: {
            // Reverse X axis
	        posX = posX - (int)((map->width - 1) * map->tile_width);
            break;
	    }
        case R_LEFTUP: {
            // Reverse both axes
	        posX = posX - (int)((map->width - 1) * map->tile_width);
	        posY = posY - (int)((map->height - 1) * map->tile_height);
            break;
	    }
    }
    DrawTMXLayerTiles(map, layer, posX, posY, tint);
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
	        HandleTMXLayerRenderOrder(map, layer, posX + layer->offsetx, posY + layer->offsety, tint);
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
    do {
		if (layers->visible) DrawTMXLayer(map, layers, posX, posY, tint);
	} while ((layers = layers->next));
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
    DrawRectangle(posX, posY, (int)map->width, (int)map->height, background);
	DrawTMXLayers(map, map->ly_head, posX, posY, tint);
}

/**
 * Returns an RaylibTMXCollision shape relative to object type
 *
 * @param object  The object pointer which the collision will be returned.
 */
RaylibTMXCollision HandleTMXCollision(tmx_object* object) {
    RaylibTMXCollision collision = {0};
    switch (object->obj_type)
    {
	    case OT_SQUARE:
	    case OT_TILE: {
            collision.rect = (Rectangle) {
                .x      = (float) object->x,
                .y      = (float) object->y,
                .width  = (float) object->width,
                .height = (float) object->height
            };
	    } break;
        case OT_POINT: {
            collision.point = (Vector2) {
                .x = (float)object->x,
                .y = (float)object->y
            };
        } break;
        case OT_POLYLINE:
        case OT_POLYGON: {
            collision.polygon.points = object->content.shape->points;
            collision.polygon.count  = object->content.shape->points_len;
        } break;
        case OT_ELLIPSE: {
            collision.rect = (Rectangle) {
                .x         = (float) (object->x + object->width  / 2.0f),
                .y         = (float) (object->y + object->height / 2.0f),
                .width     = (float) (object->width  / 2.0f),
                .height    = (float) (object->height / 2.0f)
            };
        } break;
        case OT_NONE:
        case OT_TEXT: {
            TraceLog(LOG_ERROR, "Unreachable: OT_TEXT and OT_NONE dont have collisions");
            abort();
        } break;
    }
    return collision;
}

/**
 * Returns each tmx_object on a given map and their collisions on a callback
 *
 * @param map       The map where collisions will be collected and calculated.
 * @param callback  The callback function that the user wants to receive the collisions.
 * @param userdata  The userdata that the user wnats to utilize within the callback.
 */
void CollisionsTMXForeach(tmx_map *map, tmx_collision_functor callback, void* userdata) {
    tmx_layer *layer = map->ly_head;
    do {
        if (!layer->visible) continue;
        switch (layer->type)
        {
            case L_LAYER: {
                for (unsigned int y = 0; y < map->height; y++) {
                    for (unsigned int x = 0; x < map->width; x++) {
                        unsigned int index   = (y * map->width) + x;
                        unsigned int baseGid = layer->content.gids[index];
                        unsigned int gid     = baseGid & TMX_FLIP_BITS_REMOVAL;
                        tmx_tile* tile       = map->tiles[gid];
                        if (!tile || !tile->collision) continue;
                        tmx_object *collision = tile->collision;
                        do {
                            tmx_object copy = *collision;
                            copy.x += (x * tile->width);
                            copy.y += (y * tile->height);
                            callback(collision, HandleTMXCollision(&copy), userdata);
                        } while ((collision = collision->next));
                    }
                }
            } break;
            case L_OBJGR: {
                tmx_object *object = layer->content.objgr->head;
                if (!object) continue;
                do {
                    if (object->obj_type == OT_TEXT || object->obj_type == OT_NONE) continue;
                    RaylibTMXCollision raylibCollision = HandleTMXCollision(object);
                    if (object->obj_type != OT_TILE) {
                        callback(object, raylibCollision, userdata);
                        continue;
                    }
                    int baseGid      = object->content.gid;
                    unsigned int gid = baseGid & TMX_FLIP_BITS_REMOVAL;
                    tmx_tile* tile   = map->tiles[gid];
                    if (!tile) {
                        callback(object, raylibCollision, userdata);
                        continue;
                    }
                    switch (tile->tileset->objectalignment)
                    {
                        case OA_TOPLEFT: /*RAYLIB DEFAULT*/ break;
                        case OA_NONE:
                        case OA_BOTTOMLEFT: { /* TILED DEFAULT */
                            raylibCollision.rect.y -= (float) object->height;
                        } break;
                        case OA_TOP:         /* TODO */ break;
                        case OA_LEFT:        /* TODO */ break;
                        case OA_BOTTOM:      /* TODO */ break;
                        case OA_RIGHT:       /* TODO */ break;
                        case OA_TOPRIGHT:    /* TODO */ break;
                        case OA_BOTTOMRIGHT: /* TODO */ break;
                        case OA_CENTER:      /* TODO */ break;
                    }
                    callback(object, raylibCollision, userdata);

                    tmx_object *collision = tile->collision;
                    if (!collision) continue;
                    do {
                        tmx_object copy = *collision;
                        copy.x += object->x;
                        copy.y += object->y;
                        callback(collision, HandleTMXCollision(&copy), userdata);
                    } while ((collision = collision->next));
                } while ((object = object->next));
            } break;
            
            default: continue;
        }
    } while ((layer = layer->next));
}

#ifdef __cplusplus
}
#endif

#endif  // RAYLIB_TMX_IMPLEMENTATION_ONCE
#endif  // RAYLIB_TMX_IMPLEMENTATION
