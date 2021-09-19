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
    //this->adj = std::vector(this->readPoints.size() - 1, std::vector<float>());
}

void KNNParallel::computeDistanceMatrix(std::vector<std::vector<int>> indexes) {
    auto start = std::chrono::high_resolution_clock::now();
    parallelDistances(indexes, true);
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "av: " << std::to_string(elapsed.count()) << std::endl;

    indexes.at(indexes.size() - 1).push_back((int) readPoints.size() - 1);

    start = std::chrono::high_resolution_clock::now();
    parallelDistances(indexes, false);
    elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "in: " << std::to_string(elapsed.count()) << std::endl;
}

void KNNParallel::parallelDistances(std::vector<std::vector<int>> &indexes, bool forw) {
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
            insertNeighbour(pi, j);
        }
    }
}

void KNNParallel::backward(std::vector<int> *rowIndexes) {
    for (int i: *rowIndexes) {
        Point *pi = &knn.at(i);
        for (int j = i - 1; j >= 0; j--) {
            insertNeighbour(pi, j);
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
    for (auto &e: indexIntervall) {
        printf("%d\n", e.size());
    }
    return indexIntervall;
}

void KNNParallel::compute(int k, int nw) {
    auto indexes = distributeIndex(nw);
    initialize(k);
    computeDistanceMatrix(indexes);
}