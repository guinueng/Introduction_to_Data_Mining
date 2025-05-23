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

void readDirectedGraphData(std::map<int, std::vector<int>>& adj, std::map<int, Node>& nodes, const char* file_name) {
    std::ifstream infile(file_name);
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v)) continue;
        adj[u].push_back(v);
        nodes.emplace(u, Node(u));
        nodes.emplace(v, Node(v));
    }
}

// Function to print clustering results
void printResults(const std::map<int, Node>& nodes, std::ostream& os) {
    os << "NodeID\tClusterID\n";
    for (const auto& kv : nodes) {
        os << kv.first << "\t" << kv.second.clusterID << "\n";
    }

    // os << "\n\n";
}

int main() {
    // Parameters for DBSCAN
    int minPts = 5;
    int eps = 2;

    // 1. Cluster first dataset
    std::map<int, std::vector<int>> adj1;
    std::map<int, Node> nodes1;
    // readDirectedGraphData(adj1, nodes1, "./Dataset/p2p-Gnutella08.txt");
    readGraphData(adj1, nodes1, "./Dataset/roadNet-PA.txt");
    auto full_start = std::chrono::steady_clock::now();
    GraphDBSCAN ds1(minPts, eps, adj1);
    ds1.nodes = nodes1;
    ds1.run();
    auto full_end = std::chrono::steady_clock::now();
    // std::ofstream outfile("eps=2_minpts=5_1.txt");
    std::cout << "Full cluster\nqty: " << ds1.getClusterCount() << "\n";
    // Open output file
    // Print to console
    printResults(ds1.nodes, std::cout);
    // Print to file
    // printResults(ds1.nodes, outfile);
    // full_outfile.close();
    std::cout << "\n\n";
    // std::cout << "1\n";

    // 2. Cluster second dataset
    std::map<int, std::vector<int>> adj2;
    std::map<int, Node> nodes2;
    // readDirectedGraphData(adj2, nodes2, "./Dataset/p2p-Gnutella08_remove_last_vertex.txt");
    readGraphData(adj2, nodes2, "./Dataset/roadNet-PA_last_vertex.txt");
    auto half_start = std::chrono::steady_clock::now();
    GraphDBSCAN ds2(minPts, eps, adj2);
    ds2.nodes = nodes2;
    ds2.run();
    auto half_end = std::chrono::steady_clock::now();
    std::cout << "Partial cluster\nqty: " << ds2.getClusterCount() << "\n";
    // Open output file
    // std::ofstream half_outfile("half_clustering_results.txt");
    // Print to console
    printResults(ds2.nodes, std::cout);
    // Print to file
    // printResults(ds2.nodes, outfile);
    // half_outfile.close();
    std::cout << "\n\n";
    // std::cout << "2\n";

    // 3. Incrementally add nodes from a third dataset to the first clustering
    std::map<int, std::vector<int>> adj3;
    std::map<int, Node> nodes3;
    // readDirectedGraphData(adj2, nodes3, "./Dataset/p2p-Gnutella08_last_vertex.txt");
    readGraphData(adj2, nodes3, "./Dataset/roadNet-PA_remove_last_vertex.txt");
    ds2.update_adj(adj2);
    auto add_start = std::chrono::steady_clock::now();
    for (const auto& kv : nodes3) {
        ds2.addPoint(kv.first); // Add new node by ID
        // std::cout << "After adding node " << kv.first << ":\n";
        // printResults(ds2.nodes);
        // For every neighborhood update.
        // for (int neighbor : ds2.m_adj[kv.first]) {
        //     if (ds2.nodes[neighbor].clusterID == NOISE || ds2.nodes[neighbor].clusterID == UNCLASSIFIED) {
        //         ds2.addPoint(neighbor);
        //     }
        // }
    }
    auto add_end = std::chrono::steady_clock::now();

    std::cout << "Cluster with additional points\nqty: " << ds1.getClusterCount() << "\n";
    // Open output file
    // std::ofstream outfile("add_points_results.txt");
    // Print to console
    printResults(ds2.nodes, std::cout);
    // Print to file
    // printResults(ds2.nodes, outfile);
    // std::cout << "3\n";

    auto full_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(full_end - full_start);
    auto half_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(half_end - half_start);
    auto add_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(add_end - add_start);
    std::cout << "minPts: " << minPts << " | eps: " << eps << std::endl;
    std::cout << "Full DBSCAN elapsed time: " << full_elapsed.count() << " ms" << std::endl;
    std::cout << "Half DBSCAN elapsed time: " << half_elapsed.count() << " ms" << std::endl;
    std::cout << "Add DBSCAN elapsed time: " << add_elapsed.count() << " ms" << std::endl;
    // outfile.close();

    return 0;
}