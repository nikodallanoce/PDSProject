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
#include <fstream>
#include "string.h"

KNN::KNN(std::vector<std::vector<float>> readPoints) {
    this->readPoints = std::move(readPoints);
}

/*void KNN::computeLessMem(int k) {
    initialize(k);
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            Point *pj = &knn.at(j);
            auto distance = eucledeanDistance(&{pi->getX(), pi->getY()}, &{pj->getX(), pj->getY()});
            pi->insertANeighbour(pj, distance);
            pj->insertANeighbour(pi, distance);
        }
    }
}*/

/*void KNN::compute(int k) {
    initialize(k);
    auto m = computeNeighbours();
    for (int i = 0; i < m.size(); ++i) {
        Point *pi = &knn.at(i);
        auto start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < m[i].size(); ++j) {
            Point *pj = &knn.at(j + i + 1);
            pi->insertANeighbour(pj, m[i][j]);
            pj->insertANeighbour(pi, m[i][j]);
        }
        auto elapsed = std::chrono::start<double>(std::chrono::high_resolution_clock::now() - start);
        //std::cout<<std::endl;
        //std::cout << " i=" << i << " -> " << "len=" << m[i].size() << " t=" << std::to_string(elapsed.count())
                  //<< std::endl;
    }

    //std::cout << getTopKResultPerPoint();
}*/

void KNN::compute(int k) {
    initialize(k);
    //forward();
    //backward();
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            Point *pj = &knn.at(j);
            float dist = eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates());
            pi->insertANeighbour(pj, dist);
            pj->insertANeighbour(pi, dist);
        }
    }
}

void KNN::initialize(int k) {
    knn = std::vector<Point>(readPoints.size());
    for (int i = 0; i < readPoints.size(); ++i) {
        std::vector<float> *coord = &readPoints.at(i);
        knn.at(i) = Point(i + 1, coord->at(0), coord->at(1), k);
    }
}

/*
std::vector<std::vector<float>> KNN::computeDistanceMatrix() {
    std::vector<std::vector<float>> adj = std::vector(readPoints.size() - 1, std::vector<float>());
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        std::vector<float> row = std::vector<float>(readPoints.size() - 1 - i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            row[j - i - 1] = eucledeanDistance(&readPoints[i], &readPoints[j]);
        }
        adj[i] = row;
    }
    return adj;
}*/

float KNN::eucledeanDistance(const std::vector<float> *p1, const std::vector<float> *p2) {
    float d = powf(p2->at(0) - p1->at(0), 2) + powf(p2->at(1) - p1->at(1), 2);
    return d;
}

void KNN::forward() {
    for (int i = 0; i < knn.size(); ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < knn.size(); ++j) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
        }
    }
}

void KNN::backward() {
    for (int i = (int) knn.size() - 1; i >= 0; i--) {
        Point *pi = &knn.at(i);
        for (int j = i - 1; j >= 0; j--) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
        }
    }
}

std::string KNN::getTopKResultPerPoint() {
    std::string ris;
    for (auto p: knn) {
        auto topk = p.getTopKNeighbours();
        ris += std::to_string(p.getId()) + "-> ";
        for (auto ptk: topk) {
            ris = ris.append(std::to_string(ptk->getId()) + " ");
        }
        ris = ris.append("\n");
    }

    return ris;
}

void KNN::storeTopKNeighbours(int workerID, int workLoad, std::string *neigh) {
    int bound = workerID * workLoad + workLoad;
    bound > knn.size() ? bound = (int) knn.size() : bound;
    std::string ris;
    for (int i = workerID * workLoad; i < bound; i++) {
        Point *p = &knn.at(i);
        auto topk = p->getTopKNeighbours();
        ris += std::to_string(p->getId()) + "-> ";
        for (auto ptk: topk) {
            ris = ris.append(std::to_string(ptk->getId()) + " ");
        }
        neigh[i] = ris;
        ris = "";
    }
}

void KNN::printResultInFile(const std::string &fileName, const int nw) {
    std::string ris;
    std::ofstream MyFile(fileName);
    auto neigh = new std::string[knn.size()];
    //neigh.reserve(knn.size());
    std::vector<std::thread> procDist = std::vector<std::thread>();
    int workLoad = (int) knn.size() / nw + 1;
    for (int i = 0; i < nw; ++i) {
        procDist.emplace_back(&KNN::storeTopKNeighbours, this, i, workLoad, neigh);
    }

    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }

    for (int i = 0; i < knn.size(); ++i) {
        ris = ris.append(neigh[i].append("\n"));
    }

    MyFile << ris;
    MyFile.close();
}
