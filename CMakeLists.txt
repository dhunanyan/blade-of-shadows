cmake_minimum_required(VERSION 3.19)
project(pp2_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CLI_TARGET_NAME game_cli)

set(GAME_CORE_HEADERS
    include/game/core/direction.h
    include/game/core/enemy.h
    include/game/core/engine.h
    include/game/core/object.h
    include/game/core/player.h
    include/game/core/position.h
    include/game/core/stage.h
)

set(GAME_CORE_SOURCES
    src/core/enemy.cpp
    src/core/engine.cpp
    src/core/object.cpp
)

add_library(game_core ${GAME_CORE_HEADERS} ${GAME_CORE_SOURCES})
target_include_directories(game_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    add_executable(${CLI_TARGET_NAME} src/app/cli/main.cpp "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
else()
    add_executable(${CLI_TARGET_NAME} src/app/cli/main.cpp)
endif()

target_link_libraries(${CLI_TARGET_NAME} PRIVATE game_core)

add_custom_target(run
    COMMAND ${CLI_TARGET_NAME}
    DEPENDS ${CLI_TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

find_program(VALGRIND NAMES valgrind)
if (VALGRIND)
    add_custom_target(valgrind_${CLI_TARGET_NAME} DEPENDS ${CLI_TARGET_NAME}
        COMMAND ${VALGRIND} --leak-check=full ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CLI_TARGET_NAME}
    )
endif()

add_subdirectory(tests)
add_subdirectory(src/app/qt)
