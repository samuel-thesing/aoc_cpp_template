cmake_minimum_required(VERSION 3.29)
project(aoc_template)

include(FetchContent)

set(CMAKE_CXX_STANDARD 26)

set(WORKSPACE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(WORKSPACE_DEPS_DIR "${WORKSPACE_DIR}/deps/")
set(WORKSPACE_ARTIFACT_DIR "${WORKSPACE_DIR}/bin-int/")
set(WORKSPACE_BINARY_DIR "${WORKSPACE_DIR}/bin/")

# prevent installing to system directories.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${WORKSPACE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WORKSPACE_BINARY_DIR})

set(CMAKE_INSTALL_PREFIX "${WORKSPACE_BINARY_DIR}" CACHE INTERNAL "")

#===========================================================================================
# spdlog

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.16.0
)

FetchContent_GetProperties(spdlog)
FetchContent_MakeAvailable(spdlog)

#===========================================================================================

foreach(DAY RANGE 1 12)
    if (DAY LESS 10)
        set(DAY_NAME "0${DAY}")
    else()
        set(DAY_NAME "${DAY}")
    endif()
    set(DAY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/${DAY_NAME}")
    if(IS_DIRECTORY ${DAY_PATH})
        add_subdirectory("src/${DAY_NAME}")
    endif()
endforeach()




