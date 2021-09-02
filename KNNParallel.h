//
// Created by dllni on 01/09/2021.
//

#ifndef PDSPROJECT_KNNPARALLEL_H
#define PDSPROJECT_KNNPARALLEL_H
#include <vector>
#include "Point.h"

class KNNParallel {
private:
    std::vector<Point> knn;
    std::vector<std::vector<float>> readPoints;
    std::vector<std::vector<float>> adj;
    void initialize(int dim);
    void matrixRowAllocation(int istart, int iend, int len);
    void getTopKResultPerPoint(Point* p, int k);
    static float eucledeanDistance(const std::vector<float> &p1, const std::vector<float> &p2);
    void computeDistancePerRow(std::vector<int> &rowIndexes);
    void assignNeighboursReversed(std::vector<int> &rowIndexes);
public:
    KNNParallel(std::vector<std::vector<float>> readPoints);
    void compute(int k, int nw);
    void computeDistanceMatrix(std::vector<std::vector<int>> indexes);

    void matrixAllocation(int nw);

    std::vector<std::vector<int>> distributeIndex(int nw) const;
};


#endif //PDSPROJECT_KNNPARALLEL_H
