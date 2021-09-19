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
    std::string getTopKResultPerPoint();
    void backward();
    void forward();
    void insertNeighbour(Point *pi, int j);
    static float eucledeanDistance(const std::vector<float> *p1, const std::vector<float> *p2);
public:
    KNN(std::vector<std::vector<float>> readPoints);
    void compute(int k);
    std::vector<std::vector<float>> computeDistanceMatrix();
};


#endif //PDSPROJECT_KNN_H
