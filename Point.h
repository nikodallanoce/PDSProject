//
// Created by niko on 24/08/21.
//

#ifndef PDSPROJECT_POINT_H
#define PDSPROJECT_POINT_H


#include <string>
#include <vector>
#include <queue>
#include <list>
#include <mutex>


class Point {

public:
    typedef std::pair<const Point *, float> pi;

    struct comp {
        constexpr bool operator()(pi &a, pi &b)
        const noexcept {
            return a.second < b.second;
        }
    };

private:
    int ID;
    std::vector<float> coordinates;
public:
    const std::vector<float> &getCoordinates() const;

private:
    std::priority_queue<pi, std::vector<pi>, comp> neighbours;

public:
    Point();

    Point(int ID, float x, float y, int k);

    void insertANeighbour(const Point *p, float d);

    int getId() const;

    std::vector<const Point *> getTopKNeighbours();

    void initializeNeighbours(int k);
};


#endif //PDSPROJECT_POINT_H
