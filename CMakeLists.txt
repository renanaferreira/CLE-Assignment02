cmake_minimum_required(VERSION 3.25)
project(CLE_Assignment02 C)

set(CMAKE_C_STANDARD 11)

include_directories(prog1)
include_directories(prog2)

include_directories(/home/renan/mpich-install/include)

add_executable(CLE_Assignment02
        prog1/prog1.c
        prog1/textProcessing.c
        prog1/textProcessing.h
        prog2/prog2.c
        prog2/sorting.c
        prog2/sorting.h
        )
