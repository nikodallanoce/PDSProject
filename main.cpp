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

KNN modeSelection(const std::string &mode, const std::vector<std::vector<float>> &rp);

void print(const std::string &str) {
    std::cout << str << std::endl;
}

void generatePoints(const std::string &fileName, int numOfPoints) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 10.0);
    //std::vector<float> randNums = std::vector<float>(numOfPoints);
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
    int num_points = 20000;
    int k = 10;
    int num_workers = 16;
    std::string mode = "par";
    if (argc > 1) {
        mode = argv[1];
        num_points = std::stoi(argv[2]);
        k = std::stoi(argv[3]);
        if (mode != "seq")
            num_workers = std::stoi(argv[4]);
    }
    std::vector<std::vector<float>> rp;
    generatePoints("input/points.txt", num_points);
    {
        utimer reading("reading:");
        rp = readPoints("input/points.txt");
    }

    long time;
    if(mode == "seq"){
        {
            utimer seq(mode + ":", &time);
            KNN kseq(rp);
            kseq.compute(k);
            kseq.printResultInFile("output/results_seq.txt");
        }
    } else if(mode == "par"){
        {
            utimer par(mode + ":", &time);
            KNNParallel kp(rp);
            kp.compute(k, num_workers);
            kp.printResultInFile("output/results_par.txt", num_workers);
        }
    } else if(mode == "ff"){
        {
            utimer ff(mode + ":", &time);
            KNNFF kff(rp);
            kff.compute(k, num_workers);
            kff.printResultInFile("output/results_ff.txt", num_workers);
        }
    }
    return 0;
}

