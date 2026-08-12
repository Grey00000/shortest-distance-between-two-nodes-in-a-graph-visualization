#pragma once
#include "graph.h"
#include <limits>
#include <vector>

constexpr int INF = std::numeric_limits<int>::max();

struct Step {
    enum class Kind { Push, Settle, Relax, Skip } kind;
    int node;
    int from;
    int new_dist;
    std::vector<int> dist_snapshot;
};

struct DijkstraResult {
    std::vector<int> dist;
    std::vector<int> prev;
    std::vector<int> path;
    int total_cost;
    std::vector<Step> trace;
};

DijkstraResult dijkstra(const Graph& g, int source, int target);
