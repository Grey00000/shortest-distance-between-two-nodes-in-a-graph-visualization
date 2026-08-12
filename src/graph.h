#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct Edge { int to; int weight; };

class Graph {
public:
    int addNode(const std::string& label);
    bool hasNode(const std::string& label) const;
    int idOf(const std::string& label) const;
    const std::string& labelOf(int id) const;
    void addEdge(const std::string& a, const std::string& b, int weight);
    void lowerEdgeWeight(const std::string& a, const std::string& b, int weight);
    const std::vector<Edge>& neighbors(int id) const;
    int nodeCount() const;

private:
    std::vector<std::string> labels_;
    std::vector<std::vector<Edge>> adj_;
    std::unordered_map<std::string, int> id_of_;
};
