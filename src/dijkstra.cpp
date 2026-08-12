#include "dijkstra.h"
#include <algorithm>
#include <queue>
#include <utility>

DijkstraResult dijkstra(const Graph& g, int source, int target) {
    const size_t n = static_cast<size_t>(g.nodeCount());
    DijkstraResult r;
    r.dist.assign(n, INF);
    r.prev.assign(n, -1);
    r.total_cost = -1;

    using Item = std::pair<int, int>;  // (dist, node)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    std::vector<char> done(n, 0);

    r.dist[static_cast<size_t>(source)] = 0;
    pq.push({0, source});
    r.trace.push_back(Step{Step::Kind::Push, source, -1, 0, r.dist});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (done[static_cast<size_t>(u)]) {
            r.trace.push_back(Step{Step::Kind::Skip, u, -1, d, r.dist});
            continue;
        }
        done[static_cast<size_t>(u)] = 1;
        r.trace.push_back(Step{Step::Kind::Settle, u, r.prev[static_cast<size_t>(u)], d, r.dist});

        for (const Edge& e : g.neighbors(u)) {
            if (done[static_cast<size_t>(e.to)]) continue;
            int nd = d + e.weight;
            if (nd < r.dist[static_cast<size_t>(e.to)]) {
                r.dist[static_cast<size_t>(e.to)] = nd;
                r.prev[static_cast<size_t>(e.to)] = u;
                pq.push({nd, e.to});
                r.trace.push_back(Step{Step::Kind::Relax, e.to, u, nd, r.dist});
            }
        }
    }

    if (r.dist[static_cast<size_t>(target)] != INF) {
        r.total_cost = r.dist[static_cast<size_t>(target)];
        for (int at = target; at != -1; at = r.prev[static_cast<size_t>(at)])
            r.path.push_back(at);
        std::reverse(r.path.begin(), r.path.end());
    }
    return r;
}
