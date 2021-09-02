//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
#include <chrono>
#include <functional>
#include <utility>
#include "KNN.h"
#include <thread>

KNN::KNN(std::vector<std::vector<float>> readPoints) {
    this->readPoints = std::move(readPoints);
}

void KNN::computeLessMem(int k) {
    initialize(k);
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            Point *pj = &knn.at(j);
            auto distance = eucledeanDistance({pi->getX(), pi->getY()}, {pj->getX(), pj->getY()});
            pi->insertANeighbour(pj, distance);
            pj->insertANeighbour(pi, distance);
        }
    }
}

void KNN::compute(int k) {
    initialize(k);
    auto m = computeDistanceMatrix();
    for (int i = 0; i < m.size(); ++i) {
        Point *pi = &knn.at(i);
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < m[i].size(); ++j) {
            Point *pj = &knn.at(j + i + 1);
            pi->insertANeighbour(pj, m[i][j]);
            pj->insertANeighbour(pi, m[i][j]);
        }
        auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
        //std::cout<<std::endl;
        /*std::cout << " i=" << i << " -> " << "len=" << m[i].size() << " t=" << std::to_string(elapsed.count())
                  << std::endl;*/
    }

    //std::cout << getTopKResultPerPoint();
}

void KNN::initialize(int k) {
    knn = std::vector<Point>(readPoints.size());
    for (int i = 0; i < readPoints.size(); ++i) {
        std::vector<float> coord = std::ref(readPoints[i]);
        knn[i] = Point(i + 1, coord[0], coord[1], k);
    }
}

std::vector<std::vector<float>> KNN::computeDistanceMatrix() {
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

float KNN::eucledeanDistance(const std::vector<float> &p1, const std::vector<float> &p2) {
    float d = powf(p2[0] - p1[0], 2) + powf(p2[1] - p1[1], 2);
    return sqrtf(d);
}

std::string KNN::getTopKResultPerPoint() {
    std::string ris;
    for (auto p: knn) {
        auto topk = p.getTopKNeighbours();
        ris += std::to_string(p.getId()) + "-> ";
        std::for_each(topk.begin(), topk.end(), [&ris](auto ptk) { ris += std::to_string(ptk->getId()) + " "; });
        ris += "\n";
    }

    return ris;
}
