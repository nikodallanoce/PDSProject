//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
#include <functional>
#include <memory>
#include "limits"
#include "Point.h"


Point::Point(int ID, float x, float y, int k) {
    this->ID = ID;
    this->coordinates = {x, y};
    this->k = k;
    //initializeNeighbours();
}

int Point::getId() const {
    return ID;
}

const float Point::getX() const{
    return coordinates.at(0);
}

const float Point::getY() const{
    return coordinates.at(1);
}


void Point::initializeNeighbours() {
    const Point *p = new Point();
    std::pair<const Point *, float> ins = std::make_pair(p, std::numeric_limits<float>::max());
    for (int i = 0; i < k; ++i) {
        neighbours.emplace(ins);
    }
}

Point::Point() {
    ID = -1;
    this->coordinates = std::vector<float>();
    this->k = 0;
}

void Point::insertANeighbour(const Point *p, const float d) {
    auto front = &neighbours.top();
    if (d < front->second) {
        std::pair<const Point *, float> neigh = std::make_pair(p, d);
        neighbours.pop();
        neighbours.push(neigh);
    }
}

std::vector<const Point *> Point::getTopKNeighbours() {
    auto topKPoints = std::vector<const Point *>();
    topKPoints.reserve(neighbours.size());
    while (!neighbours.empty()) {
        topKPoints.push_back(neighbours.top().first);
        neighbours.pop();
    }
    return topKPoints;
}




