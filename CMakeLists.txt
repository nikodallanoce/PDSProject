cmake_minimum_required(VERSION 3.20)
project(Project)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS_DEBUG  "-O3 -g")
SET(CMAKE_CXX_FLAGS -pthread)


add_executable(Project main.cpp Point.cpp Point.h KNN.cpp KNN.h KNNParallel.cpp KNNParallel.h KNNFF.cpp KNNFF.h)
