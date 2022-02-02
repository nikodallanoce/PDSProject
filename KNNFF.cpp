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
#include <fstream>
#include "utimer.cpp"

using namespace ff;

KNNFF::KNNFF(std::vector<std::vector<float>> readPoints) : KNN(std::move(readPoints)) {
}

void KNNFF::computeDistances(int i) {
    Point *pi = &knn.at(i);
    pi->initializeNeighbours();
    for (int j = 0; j < knn.size(); ++j) {
        if (j != i) {
            Point *pj = &knn.at(j);
            pi->insertANeighbour(pj, euclideanDistance(pi, pj));
        }
    }
}

void KNNFF::storeTopKNeighbours(int nw, std::string *neigh) {
    int limit = (int) knn.size();
    ParallelFor pf(nw);

    pf.parallel_for(0, limit, 1, 0, [this, &neigh](const int i) {
        std::string ris;
        Point *p = &knn.at(i);
        auto topk = p->getTopKNeighbours();
        ris = ris.append(std::to_string(p->getId()) + "-> ");
        for (auto ptk: topk) {
            ris = ris.append(std::to_string(ptk->getId()) + " ");
        }
        neigh[i] = ris;
        ris = "";
    }, nw);

}

void KNNFF::printResultInFile(const std::string &fileName, const int nw) {
    std::ofstream MyFile(fileName);
    auto neigh = new std::string[knn.size()];

    storeTopKNeighbours(nw, neigh);
    std::string ris;

    {
        utimer writeFF("writing:");
        for (int i = 0; i < knn.size(); ++i) {
            ris = ris.append(neigh[i].append("\n"));
        }

        MyFile << ris;
        MyFile.close();
    }
}

void KNNFF::compute(int k, int nw) {
    initialize(k);
    int limit = (int) knn.size();
    ParallelFor pf(nw);
    pf.parallel_for(0, limit, 1, 0, [this](const int i) {
        computeDistances(i);
    }, nw);
}