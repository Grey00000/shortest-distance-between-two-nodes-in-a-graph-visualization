#include "bresenham.h"
#include <cstdlib>

std::vector<Point> line(Point a, Point b) {
    std::vector<Point> out;
    int dx = std::abs(b.x - a.x);
    int dy = std::abs(b.y - a.y);
    int sx = a.x < b.x ? 1 : -1;
    int sy = a.y < b.y ? 1 : -1;
    int err = dx - dy;
    int x = a.x;
    int y = a.y;
    while (true) {
        out.push_back(Point{x, y});
        if (x == b.x && y == b.y) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx)  { err += dx; y += sy; }
    }
    return out;
}
