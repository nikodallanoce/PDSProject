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
    this->x = x;
    this->y = y;
    initializeNeighbours(k);
}

int Point::getId() const {
    return ID;
}

float Point::getX() const {
    return x;
}

float Point::getY() const {
    return y;
}

std::string Point::toString() {
    std::string s =
            "ID: " + std::to_string(getId()) + " | x=" + std::to_string(getX()) + " , y=" + std::to_string(getY());
    return s;
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
    x = -1;
    y = -1;
}

void Point::insertANeighbour(const Point *p, const float d) {
    auto front = &neighbours.top();
    if (d < front->second) {
        pi neigh = std::make_pair(p, d);
        neighbours.pop();
        neighbours.push(neigh);
    }
}

void Point::insertANeighbourParallel(const Point *p, float d) {


    n.emplace_back(p, d);

    /*{
        std::lock_guard<std::mutex> m(mtx);
        insertANeighbour(p,d);
    }*/
}

std::vector<const Point *> Point::getTopKNeighbours() {
    auto topKPoints = std::vector<const Point *>();
    while (!neighbours.empty()) {
        topKPoints.push_back(neighbours.top().first);
        neighbours.pop();
    }
    return topKPoints;
}

std::vector<Point::pi> *Point::getN() {
    return &n;
}

void Point::setN(const std::vector<pi> *neigh) {
    this->n = *neigh;
}

Point::Point(int ID, float x, float y, std::vector<std::vector<float>> &readPoints) {
    this->ID = ID;
    this->x = x;
    this->y = y;
    n.reserve(readPoints.size());
}




