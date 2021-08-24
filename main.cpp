#include <iostream>
#include <cstdio>
#include <fstream>
#include <list>
#include <random>
#include <chrono>
#include "Point.h"
#include "KNN.h"

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
    std::list<std::vector<float>> points;
    while (getline(MyReadFile, line)) {
        int index = static_cast<int>(line.find(','));
        std::vector<std::string> split = {line.substr(0, index), line.substr(index + 1, line.length())};
        points.push_back({std::stof(split[0]), std::stof(split[1])});
    }
    MyReadFile.close();
    return std::vector<std::vector<float>>(points.begin(), points.end());
}

float eucledeanDistance(std::vector<float> p1, std::vector<float> p2) {
    float d = powf(p2[0] - p1[0], 2) + powf(p2[1] - p1[1], 2);
    return sqrtf(d);
}

std::vector<std::vector<float>> buildAdjacencyMatrix(std::vector<std::vector<float>> points) {
    std::vector<std::vector<float>> adj = std::vector(points.size() - 1, std::vector<float>());
    for (int i = 0; i < points.size() - 1; ++i) {
        std::vector<float> row = std::vector<float>(points.size() - 1 - i);
        for (int j = i + 1; j < points.size(); ++j) {
            row[j - i - 1] = eucledeanDistance(points[i], points[j]);
        }
        adj[i] = row;
    }
    return adj;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    generatePoints("points.txt", 50);
    KNN gr(readPoints("points.txt"));
    auto start = std::chrono::high_resolution_clock::now();
    gr.compute(3);
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout<< std::to_string(elapsed.count()) <<std::endl;
    return 0;
}
