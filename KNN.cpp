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

KNN::KNN() {}

/*void KNN::computeLessMem(int k) {
    initialize(k);
    for (int i = 0; i < readPoints.size() - 1; ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            Point *pj = &knn.at(j);
            auto distance = euclideanDistance(&{pi->getX(), pi->getY()}, &{pj->getX(), pj->getY()});
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

/*void KNN::compute(int k) {
    initialize(k);
    forward();
    backward();
}*/

void KNN::compute(int k) {
    initialize(k);
    /*for (int i = 0; i < readPoints.size() - 1; ++i) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            Point *pj = &knn.at(j);
            auto distance = euclideanDistance(&pi->getCoordinates(), &pj->getCoordinates());
            pi->insertANeighbour(pj, distance);
            pj->insertANeighbour(pi, distance);
        }
    }*/
    int totPoints = (int) knn.size();
    for (int i = 0; i < totPoints; ++i) {
        computeNeighForNode(totPoints, i);
    }
}

void KNN::computeNeighForNode(int totPoints, int i) {
    Point *pi = &knn.at(i);
    pi->initializeNeighbours();
    for (int j = 0; j < totPoints; ++j) {
        if (j != i) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, euclideanDistance(pi, pj));
        }
    }
}

void KNN::initialize(int k) {
    knn = std::vector<Point>();
    knn.reserve(readPoints.size());
    for (int i = 0; i < readPoints.size(); ++i) {
        std::vector<float> *coord = &readPoints.at(i);
        knn.emplace_back(Point(i + 1, coord->at(0), coord->at(1), k));
    }
}

float KNN::euclideanDistance(const Point *p1, const Point *p2) {
    float d = powf(p2->getX() - p1->getX(), 2) + powf(p2->getY() - p1->getY(), 2);
    return d;
}


void KNN::printResultInFile(const std::string &fileName){
    std::string ris;
    std::ofstream MyFile(fileName);
    for (int i = 0; i < knn.size(); i++) {
        Point *p = &knn.at(i);
        auto topk = p->getTopKNeighbours();
        ris = ris.append(std::to_string(p->getId()) + "-> ");
        for (auto ptk: topk) {
            ris = ris.append(std::to_string(ptk->getId()) + " ");
        }
        ris = ris.append("\n");
    }
    MyFile << ris;
    MyFile.close();
}

/*void KNN::printResultInFile(const std::string &fileName, const int nw) {
    std::string ris;
    std::ofstream MyFile(fileName);
    auto neigh = new std::string[knn.size()];
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
}*/
