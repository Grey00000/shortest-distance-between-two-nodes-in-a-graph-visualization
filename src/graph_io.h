#pragma once
#include "graph.h"
#include <istream>
#include <stdexcept>
#include <string>

struct ParseError {
    std::string message;
    int line;
};

Graph parseGraph(std::istream& in);
