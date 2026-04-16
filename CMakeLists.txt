cmake_minimum_required(VERSION 3.12)
project(WFCRP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

add_executable(
    wfcrp
    src/main.cpp
    src/instance.cpp
    src/instance_handler.cpp
    src/solution.cpp
    src/mst.cpp
    src/sweep.cpp
    src/metaheuristics/base.cpp
    src/metaheuristics/ils.cpp
    src/metaheuristics/sa.cpp
    src/metaheuristics/ts.cpp
    src/metaheuristics/vns.cpp
)
