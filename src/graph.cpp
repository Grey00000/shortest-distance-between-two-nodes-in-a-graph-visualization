#include "graph.h"

int Graph::addNode(const std::string& label) {
    auto it = id_of_.find(label);
    if (it != id_of_.end()) return it->second;
    int id = static_cast<int>(labels_.size());
    id_of_.emplace(label, id);
    labels_.push_back(label);
    adj_.emplace_back();
    return id;
}

bool Graph::hasNode(const std::string& label) const {
    return id_of_.find(label) != id_of_.end();
}

int Graph::idOf(const std::string& label) const {
    return id_of_.at(label);
}

const std::string& Graph::labelOf(int id) const {
    return labels_[static_cast<size_t>(id)];
}

void Graph::addEdge(const std::string& a, const std::string& b, int weight) {
    int ia = addNode(a);
    int ib = addNode(b);
    adj_[static_cast<size_t>(ia)].push_back(Edge{ib, weight});
    adj_[static_cast<size_t>(ib)].push_back(Edge{ia, weight});
}

void Graph::lowerEdgeWeight(const std::string& a, const std::string& b, int weight) {
    int ia = idOf(a);
    int ib = idOf(b);
    bool found = false;
    // Both directions must stay in sync, so update each list rather than
    // returning after the first hit.
    for (auto& e : adj_[static_cast<size_t>(ia)])
        if (e.to == ib) {
            if (weight < e.weight) e.weight = weight;
            found = true;
        }
    for (auto& e : adj_[static_cast<size_t>(ib)])
        if (e.to == ia) {
            if (weight < e.weight) e.weight = weight;
            found = true;
        }
    if (!found) addEdge(a, b, weight);
}

const std::vector<Edge>& Graph::neighbors(int id) const {
    return adj_[static_cast<size_t>(id)];
}

int Graph::nodeCount() const {
    return static_cast<int>(labels_.size());
}
