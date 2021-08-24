//
// Created by niko on 24/08/21.
//

#include <cmath>
#include <iostream>
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
    neighbours = std::vector<std::pair<Point, float>>(k);
    for (auto &neighbour : neighbours) {
        neighbour.second = MAXFLOAT;
    }
    std::make_heap(neighbours.begin(), neighbours.end(),
                   [](const std::pair<Point, float> &p1, std::pair<Point, float> &p2) {
                       return p1.second < p2.second;
                   });
}

Point::Point() {
    ID = -1;
    x = -1;
    y = -1;
    neighbours = std::vector<std::pair<Point, float>>(0);
}

void Point::insertANeighbour(const std::pair<Point, float>& neigh) {
    /*auto comp = [](const std::pair<Point, float> &p1, std::pair<Point, float> &p2) {
        return p1.second < p2.second;
    };
    if (neigh.second < neighbours[0].second) {
        //std::pop_heap(neighbours.begin(), neighbours.end(), comp);
        std::pop_heap(neighbours.begin(),neighbours.end(), comp);
        neighbours.pop_back();
        neighbours.push_back(neigh);
        std::push_heap(neighbours.begin(), neighbours.end(), comp);
    }*/
    //neighbours[1] = neigh;
    std::cout<<"# ";
}


