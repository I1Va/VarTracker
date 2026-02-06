cmake_minimum_required(VERSION 3.20)
project(VarTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(SANITIZE "Enable compiler sanitizers" OFF)
option(BUILD_TESTS "Build unit tests" ON)

if (MSVC)
    add_compile_options(/W4 /WX /Od /d1noelide)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
        -O0                      # disable optimizations
        -fno-elide-constructors  # disable copy elision
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
    add_link_options(
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
endif()

add_executable(${PROJECT_NAME}  
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphvizer.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)