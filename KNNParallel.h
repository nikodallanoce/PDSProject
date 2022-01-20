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
protected:
    //float* adj[];

    void forward(std::vector<int> *rowIndexes);

    void backward(std::vector<int> *rowIndexes);

    std::vector<std::vector<int>> distributeIndex(int nw) const;

public:
    KNNParallel(std::vector<std::vector<float>> readPoints);

    void compute(int k, int nw);

    void computeNeighbours(std::vector<std::vector<int>> indexes);

    void parallelDistances(std::vector<std::vector<int>> &indexes, bool forw, float* adj[]);

    void forwardWithMatrix(std::vector<int> *rowIndexes, float* adj[]);

    void backwardWithMatrix(std::vector<int> *rowIndexes, float* adj[]);

    std::vector<std::vector<int>> parallelDistributeIndex(int nw) const;

    void backwardNew(int workerID, int totWorkers);

    void forwardNew(int workerID, int totWorkers);

    void computeNeighbours(int nw, bool forw);

    void parallelDistancesNew(bool forw, int nw);

    void computeNeighboursNew(int nw);

    void computeAllDistances(int workerID, int workLoad);

    void parallelDistancesAll(int nw);

    void insertCoordinates(int workerID, int nw, int k);

    void initializeParallel(int nw, int k);
};


#endif //PDSPROJECT_KNNPARALLEL_H
