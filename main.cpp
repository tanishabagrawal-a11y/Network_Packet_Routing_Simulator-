// ============================================================
//  Network Packet Routing Simulator
//  DSA Course Project
//
//  HOW TO COMPILE (Windows / Linux / Mac):
//      g++ main.cpp -o main
//
//  HOW TO RUN:
//      Windows : .\main
//      Linux   : ./main
// ============================================================

#include "graph.h"        // Graph + Edge (header-only)
#include "union_find.cpp" // UnionFind
#include "kruskal.cpp"    // Kruskal MST
#include "prim.cpp"       // Prim MST
#include "dijkstra.cpp"   // Dijkstra shortest path

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

// ────────────────────────────────────────────────────────────
//  Load graph from data.txt
//  Format:  first line = number of routers
//           next lines = src dest weight
//           lines starting with # are comments
// ────────────────────────────────────────────────────────────
Graph loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "  ERROR: Cannot open " << path << "\n";
        std::cerr << "  Make sure data.txt is in the same folder as main.cpp\n";
        exit(1);
    }

    std::string line;
    int n = 0;

    // Skip comment lines to find the router count
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        n = std::stoi(line);
        break;
    }

    Graph g(n);
    int s, d, w;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        if (ss >> s >> d >> w) g.addEdge(s, d, w);
    }
    return g;
}

// ────────────────────────────────────────────────────────────
//  Pause until user presses Enter
// ────────────────────────────────────────────────────────────
void pause() {
    std::cout << "\n  [Press Enter to return to menu]";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// ────────────────────────────────────────────────────────────
//  Section header helper
// ────────────────────────────────────────────────────────────
void header(const std::string& title) {
    std::cout << "\n══════════════════════════════════════\n";
    std::cout << "  " << title << "\n";
    std::cout << "══════════════════════════════════════\n";
}

// ────────────────────────────────────────────────────────────
//  Main menu
// ────────────────────────────────────────────────────────────
void printMenu() {
    std::cout << "\n======================================\n";
    std::cout << "             MAIN MENU                \n";
    std::cout << "======================================\n";
    std::cout << "  1. View network topology\n";
    std::cout << "  2. Run Kruskal's MST\n";
    std::cout << "  3. Run Prim's MST\n";
    std::cout << "  4. Dijkstra shortest paths\n";
    std::cout << "  5. Simulate packet transmission\n";
    std::cout << "  6. Compare MST vs Shortest Path\n";
    std::cout << "  7. Performance comparison report\n";
    std::cout << "  0. Exit\n";
    std::cout << "--------------------------------------\n";
    std::cout << "Choice: ";
}

// ────────────────────────────────────────────────────────────
//  Menu handlers
// ────────────────────────────────────────────────────────────

void showTopology(const Graph& g) {
    header("NETWORK TOPOLOGY");
    g.print();
    pause();
}

void runKruskal(const Graph& g) {
    header("KRUSKAL'S ALGORITHM  —  Minimum Spanning Tree");
    std::cout << "\n  What it does:\n";
    std::cout << "    Sorts all connections by cost (cheapest first).\n";
    std::cout << "    Adds a connection only if it does NOT create a loop.\n";
    std::cout << "    Stops when all routers are connected.\n";

    Kruskal k(g);
    k.compute();
    k.print();
    pause();
}

void runPrim(const Graph& g) {
    header("PRIM'S ALGORITHM  —  Minimum Spanning Tree");
    std::cout << "\n  What it does:\n";
    std::cout << "    Starts from Router0. Grows the MST one router at a time\n";
    std::cout << "    by always picking the cheapest available connection.\n";

    Prim p(g);
    p.compute();
    p.print();

    std::cout << "\n  Note: Kruskal and Prim always give the SAME total cost.\n";
    std::cout << "        The edge order may differ, the MST cost never does.\n";
    pause();
}

void runDijkstra(const Graph& g) {
    header("DIJKSTRA'S ALGORITHM  —  Shortest Path");
    std::cout << "\n  What it does:\n";
    std::cout << "    Finds the FASTEST route for a data packet from one\n";
    std::cout << "    router to every other router in the network.\n";

    int src = 0;
    std::cout << "\n  Enter source router number (0 to " << g.getV()-1 << "): ";
    std::cin >> src;

    Dijkstra d(g);
    d.compute(src);
    d.print(src);
    pause();
}

void simulatePacket(const Graph& g) {
    header("PACKET TRANSMISSION SIMULATION");

    int src, dest;
    std::cout << "\n  Enter source router      : "; std::cin >> src;
    std::cout << "  Enter destination router : "; std::cin >> dest;

    Dijkstra d(g);
    d.compute(src);

    std::cout << "\n  Sending packet: Router" << src << "  -->  Router" << dest << "\n\n";

    if (d.getDistance(dest) == INT_MAX) {
        std::cout << "  Status  : DROPPED — no path exists between these routers.\n";
    } else {
        std::cout << "  Status      : DELIVERED\n";
        std::cout << "  Latency     : " << d.getDistance(dest) << " ms\n";
        std::cout << "  Route taken : " << d.getPath(src, dest) << "\n";
    }
    pause();
}

void compareAlgos(const Graph& g) {
    header("MST BACKBONE  vs  SHORTEST PACKET PATH");

    int src, dest;
    std::cout << "\n  Enter source router      : "; std::cin >> src;
    std::cout << "  Enter destination router : "; std::cin >> dest;

    Kruskal k(g); k.compute();
    Dijkstra d(g); d.compute(src);

    std::cout << "\n  Router" << src << "  -->  Router" << dest << "\n\n";
    std::cout << "  MST total backbone cost    :  " << k.getCost() << " ms\n";
    std::cout << "    (cost to connect ALL " << g.getV() << " routers with minimum wiring)\n\n";

    int sp = d.getDistance(dest);
    if (sp == INT_MAX)
        std::cout << "  Shortest path for packet   :  UNREACHABLE\n";
    else {
        std::cout << "  Shortest path for packet   :  " << sp << " ms\n";
        std::cout << "    (" << d.getPath(src, dest) << ")\n";
    }

    std::cout << "\n  Key insight:\n";
    std::cout << "    MST answers  : What is the cheapest way to wire the network?\n";
    std::cout << "    Dijkstra asks: What is the fastest way to send ONE packet?\n";
    std::cout << "    These are two different questions on the same graph.\n";
    pause();
}

void performanceReport(const Graph& g) {
    header("PERFORMANCE COMPARISON REPORT");

    auto time_it = [](auto fn) {
        auto t = std::chrono::high_resolution_clock::now();
        fn();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - t).count();
    };

    Kruskal k(g); long kt = time_it([&]{ k.compute(); });
    Prim    p(g); long pt = time_it([&]{ p.compute(); });
    Dijkstra dij(g); long dt = time_it([&]{ dij.compute(0); });

    std::cout << "\n  " << std::left
              << std::setw(18) << "Algorithm"
              << std::setw(22) << "Time (nanoseconds)"
              << "MST Cost\n";
    std::cout << "  " << std::string(52, '-') << "\n";
    std::cout << "  " << std::setw(18) << "Kruskal (MST)"
              << std::setw(22) << kt << k.getCost() << " ms\n";
    std::cout << "  " << std::setw(18) << "Prim (MST)"
              << std::setw(22) << pt << p.getCost() << " ms\n";
    std::cout << "  " << std::setw(18) << "Dijkstra (SSSP)"
              << std::setw(22) << dt << "see path output\n";

    std::cout << "\n  Time complexity summary:\n";
    std::cout << "    Kruskal  : O(E log E)       best for sparse graphs\n";
    std::cout << "    Prim     : O(E log V)        best for dense graphs\n";
    std::cout << "    Dijkstra : O((V+E) log V)    single-source shortest path\n";
    std::cout << "\n  Both MST algorithms produce the same minimum cost.\n";
    pause();
}

// ────────────────────────────────────────────────────────────
//  Entry point
// ────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║   Network Packet Routing Simulator   ║\n";
    std::cout << "║   DSA Project  |  C++                ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";

    std::cout << "\n  Loading network from: data.txt\n";
    Graph graph = loadFromFile("data.txt");
    std::cout << "  Routers loaded : " << graph.getV() << "\n";
    std::cout << "  Connections    : " << graph.getEdges().size() << "\n";

    int choice = -1;
    while (choice != 0) {
        printMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: showTopology(graph);    break;
            case 2: runKruskal(graph);      break;
            case 3: runPrim(graph);         break;
            case 4: runDijkstra(graph);     break;
            case 5: simulatePacket(graph);  break;
            case 6: compareAlgos(graph);    break;
            case 7: performanceReport(graph); break;
            case 0: std::cout << "\n  Exiting simulator. Goodbye!\n\n"; break;
            default: std::cout << "  Invalid option. Try again.\n";
        }
    }
    return 0;
}
