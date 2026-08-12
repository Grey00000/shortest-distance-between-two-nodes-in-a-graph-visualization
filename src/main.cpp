#include "dijkstra.h"
#include "graph.h"
#include "graph_io.h"
#include "layout.h"
#include "renderer.h"
#include "scene.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

struct Options {
    std::string file = "graphs/sample.txt";
    bool color = true;
    bool animate = true;
};

bool parseArgs(int argc, char** argv, Options& opt) {
    bool file_seen = false;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--no-color") {
            opt.color = false;
        } else if (a == "--no-animate") {
            opt.animate = false;
        } else if (a.rfind("--", 0) == 0) {
            std::cerr << "unknown flag '" << a << "'\n"
                      << "usage: graphviz [FILE] [--no-color] [--no-animate]\n";
            return false;
        } else if (!file_seen) {
            opt.file = a;
            file_seen = true;
        } else {
            std::cerr << "unexpected extra argument '" << a << "'\n";
            return false;
        }
    }
    return true;
}

Scene baseScene(const Graph& g, const std::vector<Point>& pos) {
    Scene s;
    for (int u = 0; u < g.nodeCount(); ++u) {
        for (const Edge& e : g.neighbors(u)) {
            if (e.to < u) continue;  // draw each undirected edge once
            s.edges.push_back(SceneEdge{
                pos[static_cast<size_t>(u)].x, pos[static_cast<size_t>(u)].y,
                pos[static_cast<size_t>(e.to)].x, pos[static_cast<size_t>(e.to)].y,
                e.weight, Style::Dimmed});
        }
        s.nodes.push_back(SceneNode{g.labelOf(u),
                                    pos[static_cast<size_t>(u)].x,
                                    pos[static_cast<size_t>(u)].y,
                                    Style::Normal});
    }
    return s;
}

bool onPath(const std::vector<int>& path, int a, int b) {
    for (size_t i = 0; i + 1 < path.size(); ++i)
        if ((path[i] == a && path[i + 1] == b) || (path[i] == b && path[i + 1] == a))
            return true;
    return false;
}

Scene pathScene(const Graph& g, const std::vector<Point>& pos,
                const DijkstraResult& r, int source, int target) {
    Scene s = baseScene(g, pos);
    int idx = 0;
    for (int u = 0; u < g.nodeCount(); ++u) {
        for (const Edge& e : g.neighbors(u)) {
            if (e.to < u) continue;
            if (onPath(r.path, u, e.to)) s.edges[static_cast<size_t>(idx)].style = Style::Highlighted;
            ++idx;
        }
    }
    for (SceneNode& n : s.nodes) {
        int id = g.idOf(n.label);
        if (id == source || id == target) n.style = Style::Highlighted;
    }
    if (r.total_cost < 0) {
        s.caption = "No path from " + g.labelOf(source) + " to " + g.labelOf(target);
    } else {
        std::string p;
        for (size_t i = 0; i < r.path.size(); ++i) {
            if (i) p += " -> ";
            p += g.labelOf(r.path[i]);
        }
        s.caption = "Shortest path: " + p + "   cost " + std::to_string(r.total_cost);
    }
    return s;
}

Scene stepScene(const Graph& g, const std::vector<Point>& pos,
                const DijkstraResult& r, size_t upto) {
    Scene s = baseScene(g, pos);
    std::vector<Style> node_style(static_cast<size_t>(g.nodeCount()), Style::Normal);
    int relax_a = -1, relax_b = -1;
    for (size_t i = 0; i <= upto && i < r.trace.size(); ++i) {
        const Step& st = r.trace[i];
        if (st.kind == Step::Kind::Settle) {
            node_style[static_cast<size_t>(st.node)] = Style::Visited;
        } else if (st.kind == Step::Kind::Relax || st.kind == Step::Kind::Push) {
            if (node_style[static_cast<size_t>(st.node)] != Style::Visited)
                node_style[static_cast<size_t>(st.node)] = Style::Frontier;
            if (i == upto) { relax_a = st.from; relax_b = st.node; }
        }
    }
    for (SceneNode& n : s.nodes)
        n.style = node_style[static_cast<size_t>(g.idOf(n.label))];
    if (relax_a >= 0) {
        int idx = 0;
        for (int u = 0; u < g.nodeCount(); ++u)
            for (const Edge& e : g.neighbors(u)) {
                if (e.to < u) continue;
                if ((u == relax_a && e.to == relax_b) || (u == relax_b && e.to == relax_a))
                    s.edges[static_cast<size_t>(idx)].style = Style::Highlighted;
                ++idx;
            }
    }
    s.caption = "step " + std::to_string(upto + 1) + " / " + std::to_string(r.trace.size());
    return s;
}

bool promptNode(const Graph& g, const std::string& which, int& out) {
    while (true) {
        std::cout << which;
        std::cout.flush();
        std::string label;
        if (!(std::cin >> label)) return false;  // EOF
        if (g.hasNode(label)) {
            out = g.idOf(label);
            return true;
        }
        std::cout << "No node named '" << label << "'. Available:";
        for (int i = 0; i < g.nodeCount(); ++i)
            std::cout << (i ? ", " : " ") << g.labelOf(i);
        std::cout << "\n";
    }
}

}  // namespace

int main(int argc, char** argv) {
    Options opt;
    if (!parseArgs(argc, argv, opt)) return 2;

    std::ifstream in(opt.file);
    if (!in) {
        std::cerr << opt.file << ": cannot open file\n";
        return 1;
    }

    Graph g;
    try {
        g = parseGraph(in);
    } catch (const ParseError& e) {
        std::cerr << opt.file << ":" << e.line << ": " << e.message << "\n";
        return 1;
    }
    if (g.nodeCount() == 0) {
        std::cerr << opt.file << ": graph is empty\n";
        return 1;
    }

    Canvas canvas = canvasFromEnv();
    std::vector<Point> pos = circularLayout(g, canvas);
    TerminalRenderer renderer(canvas, opt.color);

    renderer.clear();
    renderer.draw(baseScene(g, pos));

    while (true) {
        int source = 0, target = 0;
        if (!promptNode(g, "Start node: ", source)) break;
        if (!promptNode(g, "End node:   ", target)) break;

        DijkstraResult r = dijkstra(g, source, target);

        bool animate = false;
        if (opt.animate) {
            std::cout << "Animate? [y/N] ";
            std::cout.flush();
            std::string ans;
            if (!(std::cin >> ans)) break;
            animate = (ans == "y" || ans == "Y");
        }

        if (animate) {
            for (size_t i = 0; i < r.trace.size(); ++i) {
                renderer.clear();
                renderer.draw(stepScene(g, pos, r, i));
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
            }
        }

        renderer.clear();
        renderer.draw(pathScene(g, pos, r, source, target));
    }

    std::cout << "\n";
    return 0;
}
