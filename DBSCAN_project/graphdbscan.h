#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <map>
#include <set>
#include <queue>

#define UNCLASSIFIED -1
#define CORE_POINT 1
#define BORDER_POINT 2
#define NOISE -2
#define SUCCESS 0
#define FAILURE -3

struct Node {
    int id;
    int clusterID;
    Node() : id(-1), clusterID(UNCLASSIFIED) {} // Default constructor
    Node(int i) : id(i), clusterID(UNCLASSIFIED) {}
};

class GraphDBSCAN {
public:
    GraphDBSCAN(int minPts, int eps, const std::map<int, std::vector<int>>& adj)
        : m_minPts(minPts), m_eps(eps), m_adj(adj), m_clusterCount(0) {
        // Initialize nodes from adjacency list
        for (const auto& kv : adj) {
            nodes.emplace(kv.first, Node(kv.first));
        }
    }

    void run();
    std::vector<int> getNeighborhood(int nodeId);
    void expandCluster(int nodeId, int clusterID);

    int addPoint(int nodeId);
    int updateClusterID(const std::vector<int>& mod_idx, int target_idx);
    void update_adj(std::map<int, std::vector<int>>& adj){
        m_adj = adj;
    }
    
    int getClusterCount() {return m_clusterCount;}

    std::map<int, Node> nodes;
    std::map<int, std::vector<int>> m_adj;

private:
    int m_minPts;
    int m_eps;
    int m_clusterCount;
};


#endif // DBSCAN_H
