//
// Created by dllni on 21/09/2021.
//

#include "KNNFF.h"
#include <thread>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>

using namespace ff;

KNNFF::KNNFF(std::vector<std::vector<float>> readPoints) : KNN(std::move(readPoints)) {
}

void KNNFF::computeAllDistances(int workerID, int workLoad) {
    int bound = workerID * workLoad + workLoad;
    bound > knn.size() ? bound = (int) knn.size() : bound;

    for (int i = workerID * workLoad; i < bound; i++) {
        Point *pi = &knn.at(i);
        for (int j = 0; j < knn.size(); ++j) {
            if (j != i) {
                Point *pj = &knn.at(j);
                pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
            }
        }
    }
}

void KNNFF::computeDistances(int i) {
    Point *pi = &knn.at(i);
    for (int j = 0; j < knn.size(); ++j) {
        if (j != i) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
        }
    }
}

void KNNFF::compute(int k, int nw) {
    initialize(k);
    int workLoad = (int) knn.size() / nw + 1;
    int limit = (int) knn.size();
    ParallelFor pf(nw);
    pf.parallel_for(0, limit, 1, 0, [this, workLoad](const int i) {
        //computeAllDistances(i, workLoad);
        computeDistances(i);
        //std::cout << i << " " << workLoad;
    }, nw);
}