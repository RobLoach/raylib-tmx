# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# tmx
find_package(tmx QUIET)
if (NOT tmx_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        tmx
        GIT_REPOSITORY https://github.com/RobLoach/tmx.git
        GIT_TAG f88a433
    )
    FetchContent_GetProperties(tmx)
    if (NOT tmx_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(tmx)
        add_subdirectory(${tmx_SOURCE_DIR} ${tmx_BINARY_DIR})
    endif()
endif()

option(RAYLIB_TMX "raylib-tmx" ON)
option(RAYLIB_TMX_SHARED "raylib-tmx-shared" OFF)

set(RAYLIB_TMX_DEPS raylib tmx)
if(UNIX)
    list(APPEND RAYLIB_TMX_DEPS m)
endif()

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
