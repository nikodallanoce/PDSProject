//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
#include <functional>
#include "Point.h"

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

float Point::distanceTo(const Point &otherPoint) {
    float d = powf(otherPoint.getX() - getX(), 2) + powf(otherPoint.getY() - getY(), 2);
    return sqrtf(d);
}

std::string Point::toString() {
    std::string s =
            "ID: " + std::to_string(getId()) + " | x=" + std::to_string(getX()) + " , y=" + std::to_string(getY());
    return s;
}

void Point::initializeNeighbours(int k) {
    for (int i = 0; i < k; ++i) {
        neighbours.push(std::pair(Point(), MAXFLOAT));
    }
}

Point::Point() {
    ID = -1;
    x = -1;
    y = -1;
}

void Point::insertANeighbour(const std::pair<Point, float> &neigh, std::string print) {
    auto front = neighbours.top();
    std::cout << print;
    if (neigh.second < front.second) {
        neighbours.pop();
        neighbours.push(neigh);
        std::cout << "#";
    }
    /*for (int i = 0; i < 5000; ++i) {
        auto g = neighbours.top();
        neighbours.pop();
        neighbours.push(neigh);
    }*/
}


