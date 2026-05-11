cmake_minimum_required(VERSION 3.10)
project(lab6_dictionary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/app.cpp
    src/io.cpp
    src/menu_.cpp
    src/format.cpp
    src/text_utils.cpp
    src/rb_tree_dictionary.cpp
    src/hash_dictionary.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(leaks
    COMMAND leaks --atExit -- ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running with leaks detector"
    USES_TERMINAL
)
