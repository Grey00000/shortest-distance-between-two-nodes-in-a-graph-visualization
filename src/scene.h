#pragma once
#include <string>
#include <vector>

enum class Style { Normal, Dimmed, Highlighted, Visited, Frontier };

struct SceneNode {
    std::string label;
    int x;
    int y;
    Style style;
};

struct SceneEdge {
    int x1;
    int y1;
    int x2;
    int y2;
    int weight;
    Style style;
};

struct Scene {
    std::vector<SceneNode> nodes;
    std::vector<SceneEdge> edges;
    std::string caption;
};
