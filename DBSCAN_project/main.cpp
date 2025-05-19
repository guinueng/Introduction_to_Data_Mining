#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include "graphdbscan.h"

// Function to read a graph from an edge list file
void readGraphData(std::map<int, std::vector<int>>& adj, std::map<int, Node>& nodes, const char* file_name) {
    std::ifstream infile(file_name);
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v)) continue;
        adj[u].push_back(v);
        adj[v].push_back(u); // undirected
        nodes.emplace(u, Node(u));
        nodes.emplace(v, Node(v));
    }
}

// Function to print clustering results
void printResults(const std::map<int, Node>& nodes) {
    std::cout << "NodeID\tClusterID\n";
    for (const auto& kv : nodes) {
        std::cout << kv.first << "\t" << kv.second.clusterID << "\n";
    }
}

int main() {
    // Parameters for DBSCAN
    int minPts = 12;
    int eps = 1;

    // 1. Cluster first dataset
    std::map<int, std::vector<int>> adj1;
    std::map<int, Node> nodes1;
    readGraphData(adj1, nodes1, "./Dataset/p2p-Gnutella08.txt");
    auto full_start = std::chrono::steady_clock::now();
    GraphDBSCAN ds1(minPts, eps, adj1);
    ds1.nodes = nodes1;
    ds1.run();
    auto full_end = std::chrono::steady_clock::now();
    std::cout << "Full cluster\nqty: " << ds1.getClusterCount() << "\n";
    printResults(ds1.nodes);
    std::cout << "\n\n";

    // 2. Cluster second dataset
    std::map<int, std::vector<int>> adj2;
    std::map<int, Node> nodes2;
    readGraphData(adj2, nodes2, "./Dataset/p2p-Gnutella08_lh.txt");
    auto half_start = std::chrono::steady_clock::now();
    GraphDBSCAN ds2(minPts, eps, adj2);
    ds2.nodes = nodes2;
    ds2.run();
    auto half_end = std::chrono::steady_clock::now();
    std::cout << "Half cluster\nqty: " << ds2.getClusterCount() << "\n";
    printResults(ds2.nodes);
    std::cout << "\n\n";

    // 3. Incrementally add nodes from a third dataset to the first clustering
    std::map<int, std::vector<int>> adj3;
    std::map<int, Node> nodes3;
    readGraphData(adj3, nodes3, "./Dataset/p2p-Gnutella08_uh.txt");
    auto add_start = std::chrono::steady_clock::now();
    for (const auto& kv : nodes3) {
        ds2.addPoint(kv.first); // Add new node by ID
        // std::cout << "After adding node " << kv.first << ":\n";
        // printResults(ds2.nodes);
    }
    auto add_end = std::chrono::steady_clock::now();

    std::cout << "Cluster with additional points\nqty: " << ds1.getClusterCount() << "\n";
    printResults(ds2.nodes);

    auto full_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(full_end - full_start);
    auto half_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(half_end - half_start);
    auto add_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(add_end - add_start);
    std::cout << "Full DBSCAN elapsed time: " << full_elapsed.count() << " ms" << std::endl;
    std::cout << "Half DBSCAN elapsed time: " << half_elapsed.count() << " ms" << std::endl;
    std::cout << "Add DBSCAN elapsed time: " << add_elapsed.count() << " ms" << std::endl;

    return 0;
}