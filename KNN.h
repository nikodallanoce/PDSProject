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

    void backward();
    void forward();
    void insertNeighbour(Point *pi, Point *pj, float dist);
    static float eucledeanDistance(const std::vector<float> *p1, const std::vector<float> *p2);
public:
    KNN(std::vector<std::vector<float>> readPoints);
    std::string getTopKResultPerPoint();
    void compute(int k);
    std::vector<std::vector<float>> computeDistanceMatrix();

    void printResultInFile(const std::string& fileName, const int nw);

    void storeTopKNeighbours(int workerID, int workLoad, std::string *neigh);
};


#endif //PDSPROJECT_KNN_H
