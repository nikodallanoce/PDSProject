//
// Created by dllni on 21/09/2021.
//

#ifndef PDSPROJECT_KNNFF_H
#define PDSPROJECT_KNNFF_H

#include <vector>
#include "Point.h"
#include "KNN.h"
#include <functional>
class KNNFF : public KNN{

    void computeDistances(int i);

public:
    KNNFF(std::vector<std::vector<float>> readPoints);

    void compute(int k, int nw);

    void printResultInFile(const std::string &fileName, const int nw);

    void storeTopKNeighbours(int nw, std::string *neigh);
};


#endif //PDSPROJECT_KNNFF_H
