#include "renderer.h"
#include "bresenham.h"
#include <cstdio>
#include <string>
#include <vector>

namespace {

struct Cell {
    char ch = ' ';
    Style style = Style::Normal;
};

bool inBounds(int x, int y, Canvas c) {
    return x >= 0 && x < c.width && y >= 0 && y < c.height;
}

void put(std::vector<std::vector<Cell>>& grid, Canvas c, int x, int y,
         char ch, Style st) {
    if (!inBounds(x, y, c)) return;
    grid[static_cast<size_t>(y)][static_cast<size_t>(x)] = Cell{ch, st};
}

void putText(std::vector<std::vector<Cell>>& grid, Canvas c, int x, int y,
             const std::string& text, Style st) {
    for (size_t i = 0; i < text.size(); ++i)
        put(grid, c, x + static_cast<int>(i), y, text[i], st);
}

char edgeGlyph(Style s) {
    return s == Style::Dimmed ? '.' : '#';
}

}  // namespace

const char* ansi_for(Style s) {
    switch (s) {
        case Style::Dimmed:      return "\033[2;90m";
        case Style::Highlighted: return "\033[1;93m";
        case Style::Visited:     return "\033[36m";
        case Style::Frontier:    return "\033[35m";
        case Style::Normal:      break;
    }
    return "\033[0m";
}

std::string render_to_string(const Scene& s, Canvas c, bool color) {
    std::vector<std::vector<Cell>> grid(
        static_cast<size_t>(c.height),
        std::vector<Cell>(static_cast<size_t>(c.width)));

    // Pass 1: edges.
    for (const SceneEdge& e : s.edges)
        for (const Point& p : line(Point{e.x1, e.y1}, Point{e.x2, e.y2}))
            put(grid, c, p.x, p.y, edgeGlyph(e.style), e.style);

    // Pass 2: weight labels at the midpoint, nudged one row off the line.
    for (const SceneEdge& e : s.edges) {
        int mx = (e.x1 + e.x2) / 2;
        int my = (e.y1 + e.y2) / 2;
        int ny = (e.y1 == e.y2) ? my - 1 : my;
        putText(grid, c, mx, ny, std::to_string(e.weight), e.style);
    }

    // Pass 3: nodes on top.
    for (const SceneNode& n : s.nodes) {
        bool endpoint = (n.style == Style::Highlighted);
        std::string glyph = (endpoint ? "[" : "(") + n.label + (endpoint ? "]" : ")");
        putText(grid, c, n.x - 1, n.y, glyph, n.style);
    }

    // Serialize, emitting an escape only when the style changes.
    std::string out;
    for (const auto& row : grid) {
        Style cur = Style::Normal;
        bool open = false;
        for (const Cell& cell : row) {
            if (color && cell.ch != ' ' && (!open || cell.style != cur)) {
                out += ansi_for(cell.style);
                cur = cell.style;
                open = true;
            }
            out += cell.ch;
        }
        if (color && open) out += "\033[0m";
        out += '\n';
    }
    if (!s.caption.empty()) out += s.caption + "\n";
    return out;
}

void TerminalRenderer::draw(const Scene& s) const {
    std::fputs(render_to_string(s, canvas_, color_).c_str(), stdout);
    std::fflush(stdout);
}

void TerminalRenderer::clear() const {
    if (color_) std::fputs("\033[2J\033[H", stdout);
}
