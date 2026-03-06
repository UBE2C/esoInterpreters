# Minimum CMake version
cmake_minimum_required(VERSION 3.15)

#Project name and language
project(miniStringFuckInterpreter LANGUAGES C)

#Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#Enable warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

#Source files
set(SRC_FILES
    src/msfInterpreter.c
)

#Include directories
include_directories(
    utils
)

#Add executable
add_executable(
    msfInterpreter
    ${SRC_FILES}
)
