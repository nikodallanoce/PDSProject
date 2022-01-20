cmake_minimum_required(VERSION 2.8)
project(Project)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g -DNO_DEFAULT_MAPPING")
SET(CMAKE_CXX_FLAGS_RELEASE  "-O3 -g -DNO_DEFAULT_MAPPING")
SET(CMAKE_CXX_FLAGS -pthread)


add_executable(Project main.cpp utimer.cpp Point.cpp Point.h KNN.cpp KNN.h KNNParallel.cpp KNNParallel.h KNNFF.cpp KNNFF.h )
