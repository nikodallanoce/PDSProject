//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
#include <functional>
#include <memory>
#include "Point.h"

std::mutex mtx;

Point::Point(int ID, float x, float y, int k) {
    this->ID = ID;
    this->coordinates = {x, y};
    initializeNeighbours(k);
}

int Point::getId() const {
    return ID;
}

void Point::initializeNeighbours(int k) {
    const Point *p = new Point();
    std::pair<const Point *, float> ins = std::make_pair(p, std::numeric_limits<float>::max());
    for (int i = 0; i < k; ++i) {
        neighbours.push(ins);
    }
}

Point::Point() {
    ID = -1;
    this->coordinates = std::vector<float>();
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
    while (!neighbours.empty()) {
        topKPoints.push_back(neighbours.top().first);
        neighbours.pop();
    }
    return topKPoints;
}

const std::vector<float> &Point::getCoordinates() const {
    return coordinates;
}




