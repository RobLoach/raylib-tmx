# raylib-tmx-example
add_executable(raylib-tmx-example raylib-tmx-example.c)
target_link_libraries(raylib-tmx-example PUBLIC
    raylib
    raylib-tmx
)

# Copy the resources
file(GLOB resources resources/*)
set(test_resources)
list(APPEND test_resources ${resources})
file(COPY ${test_resources} DESTINATION "resources/")
