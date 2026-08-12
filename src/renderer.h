#pragma once
#include "layout.h"
#include "scene.h"
#include <string>

const char* ansi_for(Style s);
std::string render_to_string(const Scene& s, Canvas c, bool color);

class TerminalRenderer {
public:
    TerminalRenderer(Canvas c, bool color) : canvas_(c), color_(color) {}
    void draw(const Scene& s) const;
    void clear() const;

private:
    Canvas canvas_;
    bool color_;
};
