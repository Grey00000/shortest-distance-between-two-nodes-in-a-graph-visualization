#include "layout.h"
#include <cmath>
#include <cstdlib>

Canvas canvasFromEnv() {
    Canvas c{100, 30};
    if (const char* cols = std::getenv("COLUMNS")) {
        int v = std::atoi(cols);
        if (v > 0) c.width = v;
    }
    if (const char* lines = std::getenv("LINES")) {
        int v = std::atoi(lines);
        if (v > 0) c.height = v;
    }
    if (c.width < 40) c.width = 40;
    if (c.height < 15) c.height = 15;
    return c;
}

std::vector<Point> circularLayout(const Graph& g, Canvas c) {
    const int n = g.nodeCount();
    std::vector<Point> out;
    out.reserve(static_cast<size_t>(n));
    const int cx = c.width / 2;
    const int cy = c.height / 2;
    if (n == 1) {
        out.push_back(Point{cx, cy});
        return out;
    }
    const double rx = c.width / 2.0 - 4.0;
    const double ry = c.height / 2.0 - 2.0;
    // M_PI is a POSIX extension, not guaranteed under strict -std=c++17, so
    // the full turn is spelled out as a literal instead.
    const double tau = 6.283185307179586;
    for (int i = 0; i < n; ++i) {
        double t = tau * static_cast<double>(i) / static_cast<double>(n);
        int x = cx + static_cast<int>(std::lround(rx * std::cos(t)));
        int y = cy + static_cast<int>(std::lround(ry * std::sin(t)));
        out.push_back(Point{x, y});
    }
    return out;
}
