//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
#include <chrono>
#include "KNN.h"

KNN::KNN(std::vector<std::vector<float>> readPoints) {
    this->readPoints = readPoints;

}

/*void KNN::compute(int k) {
    initialize(k);
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        for (int j = i + 1; j < readPoints.size(); ++j) {
            auto distance = knn[i].distanceTo(knn[j]);
            knn[i].insertANeighbour(std::pair(knn[j], distance));
            knn[j].insertANeighbour(std::pair(knn[i], distance));
        }

    }
}*/

void KNN::compute(int k) {
    initialize(k);
    auto m = computeMatrix();
    for (int i = 0; i < m.size(); ++i) {
        Point pi = knn[i];
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < m[i].size(); ++j) {
            Point pj = knn[j + 1];
            knn[i].insertANeighbour(std::pair(knn[j + 1], m[i][j]));
            knn[j + 1].insertANeighbour(std::pair(knn[i], m[i][j]));
        }
        auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
        //std::cout<<std::endl;
        std::cout << i << "-> " << "j= " << m[i].size() << " " << std::to_string(elapsed.count()) << std::endl;
    }

}

void KNN::initialize(int k) {
    knn = std::vector<Point>(readPoints.size());
    for (int i = 0; i < readPoints.size(); ++i) {
        knn[i] = Point(i + 1, readPoints[i][0], readPoints[i][1], k);
    }
}

std::vector<std::vector<float>> KNN::computeMatrix() {
    std::vector<std::vector<float>> adj = std::vector(readPoints.size() - 1, std::vector<float>());
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        std::vector<float> row = std::vector<float>(readPoints.size() - 1 - i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            row[j - i - 1] = eucledeanDistance(readPoints[i], readPoints[j]);
        }
        adj[i] = row;
    }
    return adj;
}

float KNN::eucledeanDistance(std::vector<float> p1, std::vector<float> p2) {
    float d = powf(p2[0] - p1[0], 2) + powf(p2[1] - p1[1], 2);
    return sqrtf(d);
}
