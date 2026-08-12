#include "graph_io.h"
#include <cctype>
#include <iostream>
#include <sstream>

namespace {

// Returns false when the token is not a well-formed integer. A separate
// bool is required because -1 is itself a valid parse, so no int value can
// serve as the "invalid" sentinel.
bool tryParseInt(const std::string& s, int& out) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-') ? 1 : 0;
    if (i == s.size()) return false;
    for (size_t j = i; j < s.size(); ++j)
        if (!std::isdigit(static_cast<unsigned char>(s[j]))) return false;
    try {
        out = std::stoi(s);
    } catch (const std::exception&) {
        return false;  // out of int range
    }
    return true;
}

}  // namespace

Graph parseGraph(std::istream& in) {
    Graph g;
    std::string line;
    int lineno = 0;
    auto fail = [&](const std::string& msg) -> ParseError {
        return ParseError{msg, lineno};
    };
    while (std::getline(in, line)) {
        ++lineno;
        std::istringstream ls(line);
        std::string directive;
        if (!(ls >> directive)) continue;  // blank
        if (directive[0] == '#') continue; // comment
        if (directive == "N") {
            std::string label;
            if (!(ls >> label)) throw fail("expected 'N <label>'");
            g.addNode(label);
        } else if (directive == "E") {
            std::string a, b, ws;
            if (!(ls >> a >> b >> ws))
                throw fail("expected 'E <from> <to> <weight>'");
            if (a == b)
                throw fail("self-loop edge '" + a + " - " + a + "' is not allowed");
            int w = 0;
            if (!tryParseInt(ws, w))
                throw fail("weight must be a non-negative integer, got '" + ws + "'");
            if (w < 0)
                throw fail("negative weight " + ws + " (Dijkstra requires non-negative weights)");
            bool dup = false;
            if (g.hasNode(a) && g.hasNode(b)) {
                for (const auto& e : g.neighbors(g.idOf(a)))
                    if (e.to == g.idOf(b)) { dup = true; break; }
            }
            if (dup) {
                std::cerr << "warning: duplicate edge " << a << " - " << b
                          << ", keeping the smaller weight\n";
                g.lowerEdgeWeight(a, b, w);
            } else {
                g.addEdge(a, b, w);
            }
        } else {
            throw fail("unknown directive '" + directive + "'");
        }
    }
    return g;
}
