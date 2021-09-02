#include <iostream>
#include <cstdio>
#include <fstream>
#include <list>
#include <random>
#include <chrono>
#include "Point.h"
#include "KNN.h"
#include "KNNParallel.h"

void print(const std::string &str) {
    std::cout << str << std::endl;
}

void generatePoints(const std::string &fileName, int numOfPoints) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 10.0);
    std::vector<float> randNums = std::vector<float>(numOfPoints);
    std::ofstream MyFile(fileName);

    for (int i = 0; i < numOfPoints; ++i) {
        MyFile << dist(mt) << "," << dist(mt) << std::endl;
    }
    MyFile.close();
}

std::vector<std::vector<float>> readPoints(const std::string &fileName) {
    std::ifstream MyReadFile(fileName);
    std::string line;
    std::vector<std::vector<float>> points;
    while (getline(MyReadFile, line)) {
        int index = static_cast<int>(line.find(','));
        std::vector<std::string> split = {line.substr(0, index), line.substr(index + 1, line.length())};
        points.push_back({std::stof(split[0]), std::stof(split[1])});
    }
    MyReadFile.close();
    return points;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    generatePoints("points.txt", 8);
    KNNParallel kp(readPoints("points.txt"));
    KNN k(readPoints("points.txt"));
    auto start = std::chrono::high_resolution_clock::now();
    kp.compute(5, 3);
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "par: " << std::to_string(elapsed.count()) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    k.compute(5);
    elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "seq: " << std::to_string(elapsed.count()) << std::endl;

    return 0;
}
