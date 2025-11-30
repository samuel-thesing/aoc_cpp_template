set(PROJECT_NAME "aocREPLACE_YEAR_REPLACE_DAY")
project(${PROJECT_NAME})

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_SOURCE_DIR "${PROJECT_DIR}")
set(PROJECT_ARTIFACT_DIR "${WORKSPACE_ARTIFACT_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(PROJECT_BINARY_DIR "${WORKSPACE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
# ================================ Part 1 ================================

add_executable("${PROJECT_NAME}_1"
        part1.cpp
        ../../utils/Logger.cpp
        ../../utils/utils.cpp
        ../../utils/utils.h
        ../../utils/matrix.h
)

target_link_libraries("${PROJECT_NAME}_1" spdlog::spdlog)
target_include_directories("${PROJECT_NAME}_1" PUBLIC ../../utils)

set_target_properties("${PROJECT_NAME}_1" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ARTIFACT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ARTIFACT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

# ================================ Part 2 ================================

add_executable("${PROJECT_NAME}_2"
        part2.cpp
        ../../utils/Logger.cpp
        ../../utils/utils.cpp
        ../../utils/utils.h
        ../../utils/matrix.h
)

target_link_libraries("${PROJECT_NAME}_2" spdlog::spdlog)
target_include_directories("${PROJECT_NAME}_2" PUBLIC ../../utils)

set_target_properties("${PROJECT_NAME}_2" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ARTIFACT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ARTIFACT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)