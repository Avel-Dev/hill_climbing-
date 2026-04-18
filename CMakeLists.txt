cmake_minimum_required(VERSION 3.16)
project(HillExplorer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(raylib)

set(CORE_SOURCES
    src/core/Application.cpp
    src/core/Simulation.cpp
)

set(TERRAIN_SOURCES
    src/terrain/Terrain.cpp
    src/terrain/HeightFunction.cpp
)

set(AGENT_SOURCES
    src/agent/Agent.cpp
    src/agent/strategies/SimpleHillClimbing.cpp
    src/agent/strategies/SteepestAscent.cpp
    src/agent/strategies/StochasticClimbing.cpp
    src/agent/strategies/RandomRestart.cpp
    src/agent/strategies/StrategyFactory.cpp
)

set(RENDERING_SOURCES
    src/rendering/Renderer.cpp
    src/rendering/CameraController.cpp
)

set(METRICS_SOURCES
    src/metrics/MetricsCollector.cpp
)

set(UTIL_SOURCES
    src/utils/PerlinNoise.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${CORE_SOURCES}
    ${TERRAIN_SOURCES}
    ${AGENT_SOURCES}
    ${RENDERING_SOURCES}
    ${METRICS_SOURCES}
    ${UTIL_SOURCES}
)

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} raylib)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME}
        m
        ${CMAKE_THREAD_LIBS_INIT}
        dl
        rt
        X11
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
