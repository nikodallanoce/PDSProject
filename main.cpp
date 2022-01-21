#include <iostream>
#include <cstdio>
#include <fstream>
#include <list>
#include <random>
#include <chrono>
#include "Point.h"
#include "KNN.h"
#include "KNNParallel.h"
#include "KNNFF.h"
#include "utimer.cpp"

void print(const std::string &str) {
    std::cout << str << std::endl;
}

void generatePoints(const std::string &fileName, int numOfPoints) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 10.0);
    std::vector<float> randNums = std::vector<float>(numOfPoints);
    std::ofstream MyFile(fileName);
    std::string out;

    for (int i = 0; i < numOfPoints; ++i) {
        out = out.append(std::to_string(dist(mt)))
                .append(",")
                .append(std::to_string(dist(mt)))
                .append("\n");
    }
    MyFile << out;
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

int main(int argc, char **argv) {
    std::cout << "Hello, World!" << std::endl;
    int num_points =1000;
    int k = 20;
    int num_workers = 16;
    if (argc == 4) {
        num_points = std::stoi(argv[1]);
        k = std::stoi(argv[2]);
        num_workers = std::stoi(argv[3]);
    }
    generatePoints("points.txt", num_points);
    auto rp = readPoints("points.txt");
    KNNParallel kp(rp);
    KNN ks(rp);
    KNNFF kff(rp);


    /*long tff;
    {
        utimer parallel("ff:", &tff);
        kff.compute(k, num_workers);
        kff.printResultInFile("results_ff.txt", num_workers);
    }*/


    long tpar;
    {
        utimer parallel("parallel:", &tpar);
        kp.compute(k, num_workers);
        kp.printResultInFile("results_parallel.txt", num_workers);
    }

    long tseq;
    {
        utimer sequential("sequential:", &tseq);
        ks.compute(k);
        ks.printResultInFile("results_seq.txt", 1);
    }

    std::cout << "spdup: " << std::to_string((long double) tseq / tpar) << std::endl;

    return 0;
}
