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
#include "utimer.cpp"

KNN::KNN(std::vector<std::vector<float>> readPoints) {
    this->readPoints = std::move(readPoints);
}

KNN::KNN() {}

void KNN::compute(int k) {

    initialize(k);
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
    {
        utimer writing("writing:");
        MyFile << ris;
        MyFile.close();
    }
}