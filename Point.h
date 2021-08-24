//
// Created by niko on 24/08/21.
//

#ifndef PROJECT_POINT_H
#define PROJECT_POINT_H


#include <string>
#include <vector>

class Point {

private:
    int ID;
    float x;
    float y;
    std::vector<std::pair<Point, float>> neighbours = std::vector<std::pair<Point, float>>(0);

public:
    Point();

    Point(int ID, float x, float y, int k);

    float distanceTo(const Point& otherPoint);

    void insertANeighbour(const std::pair<Point, float>& neigh);

    int getId() const;

    float getX() const;

    float getY() const;

    std::string toString();

    void initializeNeighbours(int k);
};


#endif //PROJECT_POINT_H
