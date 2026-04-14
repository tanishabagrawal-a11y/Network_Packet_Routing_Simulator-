#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>

// ─────────────────────────────────────────────
//  Edge  –  one connection between two routers
// ─────────────────────────────────────────────
struct Edge {
    int src, dest, weight;
    Edge(int s, int d, int w) : src(s), dest(d), weight(w) {}
    bool operator<(const Edge& o) const { return weight < o.weight; }
};

// ─────────────────────────────────────────────
//  Graph  –  adjacency-list weighted graph
// ─────────────────────────────────────────────
class Graph {
    int V;
    std::vector<std::vector<Edge>> adj;
    std::vector<Edge> edges; // all edges stored once (for Kruskal)

public:
    Graph(int v) : V(v), adj(v) {}

    void addEdge(int s, int d, int w) {
        adj[s].push_back({s, d, w});
        adj[d].push_back({d, s, w});
        edges.push_back({s, d, w});
    }

    int getV() const { return V; }
    const std::vector<Edge>& getEdges()    const { return edges; }
    const std::vector<Edge>& getAdj(int v) const { return adj[v]; }

    void print() const {
        std::cout << "\n  Routers : " << V << "\n";
        for (int i = 0; i < V; i++) {
            std::cout << "  Router" << i << " ->  ";
            for (auto& e : adj[i])
                std::cout << "Router" << e.dest << "(cost=" << e.weight << "ms)  ";
            std::cout << "\n";
        }
    }
};

#endif
