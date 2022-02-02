//
// Created by niko on 24/08/21.
//

#ifndef PDSPROJECT_KNN_H
#define PDSPROJECT_KNN_H

#include <vector>
#include "Point.h"

class KNN {
protected:
    std::vector<Point> knn;
    std::vector<std::vector<float>> readPoints;

    void initialize(int dim);

    static float euclideanDistance(const Point *p1, const Point *p2);

public:
    KNN(std::vector<std::vector<float>> readPoints);
    KNN();
    void compute(int k);

    void computeNeighForNode(int totPoints, int i);

    void printResultInFile(const std::string &fileName);

};


#endif //PDSPROJECT_KNN_H
