#pragma once
#include "bresenham.h"
#include "graph.h"
#include <vector>

struct Canvas {
    int width;
    int height;
};

Canvas canvasFromEnv();
std::vector<Point> circularLayout(const Graph& g, Canvas c);
