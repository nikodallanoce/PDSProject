cmake_minimum_required(VERSION 3.20)
project(Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project main.cpp Point.cpp Point.h KNN.cpp KNN.h KNNParallel.cpp KNNParallel.h)
