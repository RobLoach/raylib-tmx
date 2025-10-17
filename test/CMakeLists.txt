# raylib-tmx-test
add_executable(raylib-tmx-test raylib-tmx-test.c)

if(WIN32)
target_compile_options(raylib-tmx-test PRIVATE -Wall)
else()
target_compile_options(raylib-tmx-test PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
endif()

target_link_libraries(raylib-tmx-test PUBLIC
    raylib-tmx
)

# Copy the resources
if(MSVC)
    add_custom_command(TARGET raylib-tmx-test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:raylib-tmx-test>/resources"
    )
else()
    file(GLOB resources resources/*)
    set(test_resources)
    list(APPEND test_resources ${resources})
    file(COPY ${test_resources} DESTINATION "resources/")
endif()

# Set up the test
add_test(NAME raylib-tmx-test COMMAND raylib-tmx-test)
