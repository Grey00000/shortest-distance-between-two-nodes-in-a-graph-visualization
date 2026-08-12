#pragma once
#include <vector>

struct Point {
    int x;
    int y;
};

inline bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

std::vector<Point> line(Point a, Point b);
