cmake_minimum_required(VERSION 3.16)

project(CommandPredictor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(CmdPredictor)
add_subdirectory(PredictProtocol)
add_subdirectory(demo)
