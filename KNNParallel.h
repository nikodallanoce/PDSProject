//
// Created by dllni on 01/09/2021.
//

#ifndef PDSPROJECT_KNNPARALLEL_H
#define PDSPROJECT_KNNPARALLEL_H
#include <vector>
#include "Point.h"
#include "KNN.h"
#include <functional>

class KNNParallel: public KNN{
protected:
    std::vector<std::vector<float>> adj;
    void matrixRowAllocation(int istart, int iend, int len);
    void getTopKResultPerPoint(Point* p, int k);
    void computeDistances(std::vector<int>* rowIndexes, bool forw);
    void computeDistancesRev(std::vector<int> *rowIndexes);
    void computeDistancesFor(std::vector<int> *rowIndexes);
    void assignNeighboursReversed(std::vector<int> *rowIndexes);
    std::vector<std::vector<int>> distributeIndex(int nw) const;

public:
    KNNParallel(std::vector<std::vector<float>> readPoints);
    void compute(int k, int nw);
    void computeDistanceMatrix(std::vector<std::vector<int>> indexes);

    void matrixAllocation(int nw);

    void parallelDistances(std::vector<std::vector<int>> &indexes, bool forw);
};


#endif //PDSPROJECT_KNNPARALLEL_H
