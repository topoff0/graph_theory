cmake_minimum_required(VERSION 3.10)
project(graph_theory)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable for lsp normal work
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include .h files
include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(leaks
    COMMAND leaks --atExit -- ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running with leaks detector"
    USES_TERMINAL
)
