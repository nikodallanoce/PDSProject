//
// Created by dllni on 01/09/2021.
//

#include <thread>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include <future>
#include <fstream>
#include "KNNParallel.h"
#include "utimer.cpp"

#define EOS NULL

KNNParallel::KNNParallel(std::vector<std::vector<float>> readPoints) : KNN(std::move(readPoints)) {
    //adj = std::vector<std::vector<float>>(this->readPoints.size(), std::vector<float>(0));
}

void KNNParallel::computeNeighbours(std::vector<std::vector<int>> indexes) {
    //float *m[this->readPoints.size()];
    int dim = (int) knn.size();
    float *m[dim];
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
            procDist.emplace_back(&KNNParallel::forwardWithMatrix, this, &index, adj);
            //procDist.emplace_back(&KNNParallel::forward, this, &index);
        }
    } else {
        for (auto &index: indexes) {
            procDist.emplace_back(&KNNParallel::backwardWithMatrix, this, &index, adj);
            //procDist.emplace_back(&KNNParallel::backward, this, &index);
        }
    }
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }
}

void KNNParallel::computeNeighForIntevallPoints(int iStart, int iEnd) {

    int totPoints = (int) knn.size();
    for (int i = iStart; i < iEnd; i++) {
        computeNeighForNode(totPoints, i);
    }
}


void KNNParallel::parallelComputeKNN(int nw) {
    std::vector<std::thread> procDist = std::vector<std::thread>();
    procDist.reserve(nw);
    int iStart = 0;
    int reminder = (int) readPoints.size() % nw;
    int workload = (int) readPoints.size() / nw;
    for (int i = 0; i < nw - 1; ++i) {
        int iEnd = iStart + workload;
        if (reminder > 0) {
            iEnd = iEnd + 1;
            reminder--;
        }
        procDist.emplace_back(&KNNParallel::computeNeighForIntevallPoints, this, iStart, iEnd);
        iStart = iEnd;
    }
    computeNeighForIntevallPoints(iStart, (int) readPoints.size());
    for (std::thread &t: procDist) {                        // await thread termination
        t.join();
    }
}

void KNNParallel::forwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]) {
    for (int i: *rowIndexes) {
        int dim = (int) knn.size() - 1 - i;
        auto rowDist = new float[dim];
        Point *pi = &knn.at(i);
        for (int j = i + 1; j < knn.size(); ++j) {
            Point *pj = &knn.at(j);
            auto d = euclideanDistance(pi, pj);
            rowDist[j - i - 1] = d;
            pi->insertANeighbour(pj, d);
        }
        adj[i] = rowDist;
    }
}

void KNNParallel::backwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]) {
    for (int i: *rowIndexes) {
        Point *pi = &knn.at(i);
        int j = 0;
        for (i = i - 1; i >= 0; --i) {
            Point *pj = &knn.at(i);
            float d = adj[i][j];
            //auto d = euclideanDistance(&pi->getCoordinates(), &pj->getCoordinates());
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

void KNNParallel::storeTopKNeighbours(int iStart, int iEnd, std::string *neigh) {
    std::string ris;
    for (int i = iStart; i < iEnd; i++) {
        Point *p = &knn.at(i);
        auto topk = p->getTopKNeighbours();
        ris = ris.append(std::to_string(p->getId()) + "-> ");
        for (auto ptk: topk) {
            ris = ris.append(std::to_string(ptk->getId()) + " ");
        }
        neigh[i] = ris;
        ris = "";
    }
}

void KNNParallel::printResultInFile(const std::string &fileName, const int nw) {
    std::string ris;
    std::ofstream MyFile(fileName);
    auto neigh = new std::string[knn.size()];
    std::vector<std::thread> procDist = std::vector<std::thread>();
    procDist.reserve(nw);
    int iStart = 0;
    int reminder = (int) readPoints.size() % nw;
    int workload = (int) readPoints.size() / nw;
    for (int i = 0; i < nw - 1; ++i) {
        int iEnd = iStart + workload;
        if (reminder > 0) {
            iEnd = iEnd + 1;
            reminder--;
        }
        procDist.emplace_back(&KNNParallel::storeTopKNeighbours, this, iStart, iEnd, neigh);
        iStart = iEnd;
    }
    storeTopKNeighbours(iStart, (int) readPoints.size(), neigh);
    for (std::thread &t: procDist) {
        t.join();
    }

    {
        utimer writing("writing:");
        for (int i = 0; i < knn.size(); ++i) {
            ris = ris.append(neigh[i].append("\n"));
        }

        MyFile << ris;
        MyFile.close();
    }
}

void KNNParallel::compute(int k, int nw) {
    //auto indexes = distributeIndex(nw);
    initialize(k);
    parallelComputeKNN(nw);
    //computeNeighbours(indexes);
}