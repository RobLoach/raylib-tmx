# raylib-tmx-example
add_executable(raylib-tmx-example raylib-tmx-example.c)
target_link_libraries(raylib-tmx-example PUBLIC
    raylib-tmx
)

# Copy the resources
if(MSVC)
    add_custom_command(TARGET raylib-tmx-example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:raylib-tmx-example>/resources"
    )
else()
    file(GLOB resources resources/*)
    set(test_resources)
    list(APPEND test_resources ${resources})
    file(COPY ${test_resources} DESTINATION "resources/")
endif()
