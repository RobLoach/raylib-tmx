# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/raylib ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/raylib)
endif()

# tmx
find_package(tmx QUIET)
if (NOT tmx_FOUND)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/tmx ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/tmx)
endif()

option(RAYLIB_TMX "raylib-tmx" ON)
option(RAYLIB_TMX_SHARED "raylib-tmx-shared" OFF)

set(RAYLIB_TMX_DEPS raylib tmx)

if(RAYLIB_TMX)
    add_library(raylib-tmx STATIC raylib-tmx.c)
    target_include_directories(raylib-tmx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_link_libraries(raylib-tmx PUBLIC ${RAYLIB_TMX_DEPS})
endif()

if(RAYLIB_TMX_SHARED)
    add_library(raylib-tmx-shared SHARED raylib-tmx.c)
    target_include_directories(raylib-tmx-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_link_libraries(raylib-tmx-shared PUBLIC ${RAYLIB_TMX_DEPS})
endif()
