//
// Created by niko on 24/08/21.
//

#ifndef PROJECT_KNN_H
#define PROJECT_KNN_H


#include <vector>
#include "Point.h"

class KNN {
private:
    std::vector<Point> knn;
    std::vector<std::vector<float>> readPoints;
    void initialize(int dim);
    static float eucledeanDistance(std::vector<float> p1, std::vector<float> p2);
public:
    KNN(std::vector<std::vector<float>> readPoints);
    void compute(int k);
    void computeLessMem(int k);
    std::vector<std::vector<float>> computeMatrix();
};


#endif //PROJECT_KNN_H
