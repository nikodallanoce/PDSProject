//
// Created by dllni on 01/09/2021.
//

#include <thread>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include "KNNParallel.h"

#define EOS NULL

KNNParallel::KNNParallel(std::vector<std::vector<float>> readPoints) : KNN(std::move(readPoints)) {
    //adj = std::vector<std::vector<float>>(this->readPoints.size(), std::vector<float>(0));
}

void KNNParallel::computeNeighbours(std::vector<std::vector<int>> indexes) {
    float *m[this->readPoints.size() - 1];
    auto start = std::chrono::high_resolution_clock::now();
    parallelDistances(indexes, true, m);
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "av: " << std::to_string(elapsed.count()) << std::endl;

    indexes.at(indexes.size() - 1).push_back((int) readPoints.size() - 1);

    start = std::chrono::high_resolution_clock::now();
    parallelDistances(indexes, false, m);
    elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "in: " << std::to_string(elapsed.count()) << std::endl;
}

void KNNParallel::parallelDistances(std::vector<std::vector<int>> &indexes, bool forw, float *adj[]) {
    std::vector<std::thread> procDist = std::vector<std::thread>();
    procDist.reserve(indexes.size());
    if (forw) {
        for (auto &index: indexes) {
            procDist.emplace_back(&KNNParallel::forward, this, &index);
        }
    } else {
        for (auto &index: indexes) {
            procDist.emplace_back(&KNNParallel::backward, this, &index);
        }
    }
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }
}

void KNNParallel::forward(std::vector<int> *rowIndexes) {
    for (int i: *rowIndexes) {
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < knn.size(); ++j) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
        }
    }
}

void KNNParallel::backward(std::vector<int> *rowIndexes) {
    for (int i: *rowIndexes) {
        Point *pi = &knn.at(i);
        for (int j = i - 1; j >= 0; j--) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates()));
        }
    }
}

void KNNParallel::forwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]) {
    for (int i: *rowIndexes) {
        auto rowDist = new float[knn.size() - 1 - i];
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < knn.size(); ++j) {
            Point *pj = &knn.at(j);
            auto d = eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates());
            rowDist[j - i - 1] = d;
            pi->insertANeighbour(pj, d);
        }
        adj[i]=rowDist;
    }
}

void KNNParallel::backwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]) {
    for (int i: *rowIndexes) {
        Point *pi = &knn.at(i);
        int j = 0;
        for (i = i - 1; i >= 0; --i) {
            Point *pj = &knn.at(i);
            float d = adj[i][j];
            //auto d = eucledeanDistance(&pi->getCoordinates(), &pj->getCoordinates());
            pi->insertANeighbour(pj, d);
            j++;
        }
    }
}

std::vector<std::vector<int>> KNNParallel::distributeIndex(int nw) const {
    std::vector<std::vector<int>> indexIntervall = std::vector<std::vector<int>>(nw);
    int size = (int) readPoints.size();
    for (int i = 0; i < size / 2; ++i) {
        indexIntervall[i % indexIntervall.size()].emplace_back(i);
        int rev = size - 2 - i;
        if (rev != i) {
            indexIntervall[i % indexIntervall.size()].emplace_back(rev);
        }
    }
    auto cmp = [](std::vector<int> &v1, std::vector<int> &v2) { return v1.size() > v2.size(); };
    std::sort(indexIntervall.begin(), indexIntervall.end(), cmp);
    for (int i = 0; i < indexIntervall.size(); ++i) {
        auto q1 = &indexIntervall.at(i);
        auto q2 = &indexIntervall.at(indexIntervall.size() - i - 1);
        if (q1->size() - 1 > q2->size()) {
            auto e = &(q1->at(q1->size() - 1));
            q1->pop_back();
            q2->push_back(*e);
        } else {
            break;
        }
    }
    return indexIntervall;
}

void KNNParallel::compute(int k, int nw) {
    auto indexes = distributeIndex(nw);
    initialize(k);
    computeNeighbours(indexes);
}