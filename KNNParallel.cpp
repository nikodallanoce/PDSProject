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
    this->adj = std::vector(this->readPoints.size() - 1, std::vector<float>());
}

void KNNParallel::computeDistanceMatrix(std::vector<std::vector<int>> indexes) {
    auto start = std::chrono::high_resolution_clock::now();
    auto f1 = [](Point *p1, Point *p2, float d) { p1->insertANeighbour(p2, d); };
    auto f2 = [](Point *p1, Point *p2, float d) { p2->insertANeighbour(p1, d); };

    parallelDistances(indexes, true);
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "av: " << std::to_string(elapsed.count()) << std::endl;

    //indexes.at(indexes.size() - 1).push_back((int) readPoints.size() - 1);

    start = std::chrono::high_resolution_clock::now();
    parallelDistances(indexes, false);
    elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "in: " << std::to_string(elapsed.count()) << std::endl;
}

void KNNParallel::parallelDistances(std::vector<std::vector<int>> &indexes, bool forw) {
    std::vector<std::thread> procDist = std::vector<std::thread>();
    procDist.reserve(indexes.size());
    for (auto &index: indexes) {
        procDist.emplace_back(&KNNParallel::computeDistances, this, &index, forw);
    }
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }
}

void KNNParallel::computeDistances(std::vector<int>* rowIndexes, bool forw) {
    for (int i: *rowIndexes) {
        //std::cout << i << std::endl;
        Point *pi = &knn.at(i);
        //auto row = std::vector<float>(readPoints.size() - 1 - i);
        for (int j = i + 1; j < readPoints.size(); ++j) {
            float d = eucledeanDistance(&readPoints.at(i), &readPoints.at(j));
            //row.at(j - i - 1) = d;
            Point *pj = &knn.at(j);
            if (forw) {
                pi->insertANeighbour(pj, d);
                //pj->insertANeighbour(pi, d);
            } else { pj->insertANeighbour(pi, d); }
        }
        //adj.at(i) = row;
    }
}

void KNNParallel::assignNeighboursReversed(std::vector<int> *rowIndexes) {
    for (int i: *rowIndexes) {
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
