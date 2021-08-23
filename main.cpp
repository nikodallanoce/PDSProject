#include <iostream>
#include <cstdio>
#include <fstream>
#include <list>
#include <random>

void print(const std::string &str) {
    std::cout << str << std::endl;
}

void generatePoints(const std::string &fileName, int numOfPoints) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 100.0);
    std::vector<float> randNums = std::vector<float>(numOfPoints);
    std::ofstream MyFile(fileName);

    for (int i = 0; i < numOfPoints; ++i) {
        MyFile << dist(mt) << "," << dist(mt) << std::endl;
    }
    MyFile.close();
}

std::list<std::vector<float>> readPoints(const std::string &fileName) {
    std::ifstream MyReadFile(fileName);
    std::string line;
    std::list<std::vector<float>> points;
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
    generatePoints("points.txt", 10);
    readPoints("points.txt");
    print("end");
    return 0;
}
