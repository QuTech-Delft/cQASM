target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/function.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/generator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)
add_subdirectory(v1x)
add_subdirectory(v3x)
