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

    void computeNeighbours(std::vector<std::vector<int>> indexes);

    void parallelDistances(std::vector<std::vector<int>> &indexes, bool forw, float *adj[]);

    void forwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]);

    void backwardWithMatrix(std::vector<int> *rowIndexes, float *adj[]);

    void computeNeighForIntevallPoints(int iStart, int iEnd);

    void parallelComputeKNN(int nw);

    std::vector<std::vector<int>> distributeIndex(int nw) const;

    void printResultInFile(const std::string &fileName, const int nw);

    void storeTopKNeighbours(int iStart, int iEnd, std::string *neigh);
};


#endif //PDSPROJECT_KNNPARALLEL_H
