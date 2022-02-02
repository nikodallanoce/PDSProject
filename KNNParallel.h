//
// Created by dllni on 01/09/2021.
//

#ifndef PDSPROJECT_KNNPARALLEL_H
#define PDSPROJECT_KNNPARALLEL_H

#include <vector>
#include "Point.h"
#include "KNN.h"
#include <functional>

class KNNParallel : public KNN {

public:
    KNNParallel(std::vector<std::vector<float>> readPoints);

    void compute(int k, int nw);

    void computeNeighForIntevallPoints(int iStart, int iEnd);

    void parallelComputeKNN(int nw);

    void printResultInFile(const std::string &fileName, int nw);

    void storeTopKNeighbours(int iStart, int iEnd, std::string *neigh);
};


#endif //PDSPROJECT_KNNPARALLEL_H
