cmake_minimum_required(VERSION 3.11)
project (raylib-tmx
    VERSION 0.0.1
    DESCRIPTION "raylib-tmx"
    HOMEPAGE_URL "https://github.com/robloach/raylib-tmx"
    LANGUAGES C)

# Include Directory
add_subdirectory(include)
add_subdirectory(src)

# Examples
option(RAYLIB_TMX_BUILD_EXAMPLE "Build Example" ON)
if(RAYLIB_TMX_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

# Testing
include(CTest)
enable_testing()
if(BUILD_TESTING AND RAYLIB_TMX_BUILD_EXAMPLE)
    set(CTEST_CUSTOM_TESTS_IGNORE
        pkg-config--static
    )
    add_subdirectory(test)
endif()
