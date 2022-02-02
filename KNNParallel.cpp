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
        utimer writePar("writing:");
        for (int i = 0; i < knn.size(); ++i) {
            ris = ris.append(neigh[i].append("\n"));
        }

        MyFile << ris;
        MyFile.close();
    }


}

void KNNParallel::compute(int k, int nw) {
    initialize(k);
    parallelComputeKNN(nw);
}