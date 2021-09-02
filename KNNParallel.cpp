//
// Created by dllni on 01/09/2021.
//

#include <thread>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "KNNParallel.h"

#define EOS NULL

KNNParallel::KNNParallel(std::vector<std::vector<float>> readPoints) {
    this->readPoints = std::move(readPoints);
    this->adj = std::vector(this->readPoints.size() - 1, std::vector<float>());
}


void KNNParallel::computeDistanceMatrix(std::vector<std::vector<int>> indexes) {

    std::vector<std::thread> procDist = std::vector<std::thread>();
    auto start = std::chrono::high_resolution_clock::now();
    for (auto &index: indexes) {
        procDist.emplace_back(std::thread(&KNNParallel::computeDistancePerRow, this, std::ref(index)));
        //computeDistancePerRow(std::ref(ind));
    }
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
        //std::cout << t.get_id() << std::endl;
    }
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "av: " << std::to_string(elapsed.count()) << std::endl;
    procDist=std::vector<std::thread>();
    indexes.at(indexes.size()-1).push_back((int) readPoints.size() - 1);
    start = std::chrono::high_resolution_clock::now();
    for (auto &index: indexes) {
        procDist.emplace_back(std::thread(&KNNParallel::assignNeighboursReversed, this, std::ref(index)));
    }
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }
    elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "in: " << std::to_string(elapsed.count()) << std::endl;
}

void KNNParallel::computeDistancePerRow(std::vector<int> &rowIndexes) {

    for (int i: rowIndexes) {
        //std::cout << i << std::endl;
        Point *pi = &knn.at(i);
        auto row = std::vector<float>(readPoints.size() - 1 - i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            float d = eucledeanDistance(readPoints.at(i), readPoints.at(j));
            row.at(j - i - 1) = d;
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, d);
            //pj->insertANeighbour(pi, d);
        }
        adj.at(i) = row;
    }
}

void KNNParallel::assignNeighboursReversed(std::vector<int> &rowIndexes) {
    for (int i: rowIndexes) {
        //if (i == 0) { i = (int) readPoints.size() - 1; }
        Point *pi = &knn.at(i);
        int j = 0;
        for (int r = i - 1; r >= 0; r--) {
            auto d = adj.at(r).at(j);
            Point *neigh = &knn.at(r);
            pi->insertANeighbour(neigh, d);
            j = j + 1;
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
    return indexIntervall;
}


float KNNParallel::eucledeanDistance(const std::vector<float> &p1, const std::vector<float> &p2) {
    float d = powf(p2[0] - p1[0], 2) + powf(p2[1] - p1[1], 2);
    return sqrtf(d);
}

void KNNParallel::compute(int k, int nw) {
    auto indexes = distributeIndex(nw);
    initialize(k);
    computeDistanceMatrix(indexes);
    std::vector<std::thread> procNeigh = std::vector<std::thread>();
    /*
    auto f = [this, k](std::vector<int> &index) {
        for (int i: index) {
            getTopKResultPerPoint(&knn[i], k);
        }
    };

    for (auto &index: indexes) {
        procNeigh.emplace_back(std::thread(f, std::ref(index)));
    }
    for (std::thread &t: procNeigh) {                        // await thread termination
        t.join();
    }*/
    //getTopKResultPerPoint(queue.front(), k);
    //std::cout << "" << std::endl;
}

void KNNParallel::initialize(int k) {
    knn = std::vector<Point>(readPoints.size());
    for (int i = 0; i < readPoints.size(); ++i) {
        std::vector<float> coord = std::ref(readPoints[i]);
        knn[i] = Point(i + 1, coord[0], coord[1], k);
    }
}

void KNNParallel::getTopKResultPerPoint(Point *p, int k) {

    auto f = [](Point::pi &p1, Point::pi &p2) {
        return p1.second > p2.second;
    };
    std::vector<Point::pi> topk(k);
    std::make_heap(p->getN()->begin(), p->getN()->end(), f);

    for (int i = 0; i < k; ++i) {
        std::pop_heap(p->getN()->begin(), p->getN()->end(), f);
        topk.at(i) = p->getN()->back();
        p->getN()->pop_back();
    }
    p->setN(&topk);
}


void KNNParallel::matrixAllocation(int nw) {
    int range = readPoints.size() / nw;
    std::vector<std::thread> proc = std::vector<std::thread>(nw);
    int start = 0;
    int end = range;
    for (int i = 0; i < nw; i++) {
        if (i == nw - 1) {
            end = (int) readPoints.size() - 1;
        }
        proc[i] = std::thread(&KNNParallel::matrixRowAllocation, this, start, end, (int) readPoints.size());
        start = end;
        end = start + range;
    }
    for (std::thread &t: proc) {                        // await thread termination
        t.join();
    }
}

void KNNParallel::matrixRowAllocation(int istart, int iend, int len) {
    for (int i = istart; i < iend; ++i) {
        //std::cout << len - i - 1 << std::endl;
        std::vector<float> row = std::vector<float>(len - i - 1);
        adj[i] = row;
    }
}
