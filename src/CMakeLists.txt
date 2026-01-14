# raylib
find_package(raylib QUIET GLOBAL)
if (NOT raylib_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG c1ab645ca298a2801097931d1079b10ff7eb9df8
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    endif()
endif()

# tmx
find_package(tmx QUIET GLOBAL)
if (NOT tmx_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        tmx
        GIT_REPOSITORY https://github.com/baylej/tmx.git
        GIT_TAG 11ffdcd
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(tmx)
    if (NOT tmx_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(tmx)
    endif()
endif()

option(RAYLIB_TMX_SHARED "raylib-tmx-shared" OFF)

if (RAYLIB_TMX_SHARED)
    if (raylib_FOUND)
        get_target_property(RAYLIB_PIC raylib POSITION_INDEPENDENT_CODE)
        if (NOT RAYLIB_PIC)
            message(FATAL_ERROR "raylib added externally does not have PIC. Recompile raylib as Shared library")
        endif ()
    endif ()
    if (tmx_FOUND)
        get_target_property(TMX_PIC tmx POSITION_INDEPENDENT_CODE)
        if (NOT TMX_PIC)
            message(FATAL_ERROR "tmx added externally does not have PIC. Recompile tmx as Shared library")
        endif ()
    endif ()
    add_library(raylib-tmx SHARED)
else ()
    add_library(raylib-tmx STATIC)
endif ()

target_sources(raylib-tmx
    PRIVATE raylib-tmx.c
    PUBLIC
       FILE_SET HEADERS
       FILES ../include/raylib-tmx.h
       BASE_DIRS ../include
)
target_include_directories(raylib-tmx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

set(RAYLIB_TMX_DEPS raylib tmx)
if(UNIX)
    list(APPEND RAYLIB_TMX_DEPS m)
endif()
if(WIN32)
    list(APPEND RAYLIB_TMX_DEPS gdi32 winmm)
    target_compile_options(raylib-tmx PRIVATE "/NODEFAULTLIB:libcmt")
endif()
target_link_libraries(raylib-tmx PUBLIC ${RAYLIB_TMX_DEPS})


configure_file(raylib-tmxConfig.cmake.in raylib-tmxConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/raylib-tmxConfig.cmake" DESTINATION lib/cmake/raylib-tmx)

install(TARGETS raylib-tmx
        EXPORT "raylib-tmxTargets"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        FILE_SET HEADERS DESTINATION include
)

install(EXPORT "raylib-tmxTargets"
        FILE "raylib-tmxTargets.cmake"
        DESTINATION lib/cmake/raylib-tmx
)
