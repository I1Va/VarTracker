cmake_minimum_required(VERSION 3.20)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(SANITIZE "Enable compiler sanitizers" OFF)
option(BUILD_TESTS "Build unit tests" ON)

if (MSVC)
    add_compile_options(/W4 /WX /Od /d1noelide)
else()
    add_compile_options(
        -O0                      
        -fno-elide-constructors 
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
    add_link_options(
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
endif()

add_executable(${PROJECT_NAME}  
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/node.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../inc)