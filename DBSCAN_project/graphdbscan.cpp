#include "graphdbscan.h"

std::vector<int> GraphDBSCAN::getNeighborhood(int nodeId) {
    std::set<int> visited;
    std::queue<std::pair<int, int>> q; // (node, depth)
    q.push({nodeId, 0});
    visited.insert(nodeId);

    while (!q.empty()) {
        auto [current, depth] = q.front(); q.pop();
        if (depth >= m_eps) continue;
        for (int neighbor : m_adj[current]) {
            if (visited.insert(neighbor).second) {
                q.push({neighbor, depth + 1});
            }
        }
    }
    visited.erase(nodeId); // remove self
    return std::vector<int>(visited.begin(), visited.end());
}

void GraphDBSCAN::run() {
    int clusterID = 1;
    for (auto& [id, node] : nodes) {
        if (node.clusterID != -1) continue; // already processed
        auto neighbors = getNeighborhood(id);
        if (neighbors.size() < m_minPts) {
            node.clusterID = -2; // NOISE
        } else {
            expandCluster(id, clusterID);
            clusterID++;
        }
    }
    m_clusterCount = clusterID - 1;
}

void GraphDBSCAN::expandCluster(int nodeId, int clusterID) {
    std::queue<int> q;
    nodes[nodeId].clusterID = clusterID;
    q.push(nodeId);

    while (!q.empty()) {
        int current = q.front(); q.pop();
        auto neighbors = getNeighborhood(current);
        if (neighbors.size() >= m_minPts) {
            for (int neighbor : neighbors) {
                if (nodes[neighbor].clusterID == -1 || nodes[neighbor].clusterID == -2) {
                    nodes[neighbor].clusterID = clusterID;
                    q.push(neighbor);
                }
            }
        }
    }
}

int GraphDBSCAN::addPoint(int nodeId) {
    // Add node if it doesn't exist
    if (nodes.find(nodeId) == nodes.end()) {
        nodes.emplace(nodeId, Node(nodeId));
    }
    Node& node = nodes[nodeId];

    // Find neighbors within m_eps hops
    std::vector<int> clusterSeeds = getNeighborhood(nodeId);

    if (clusterSeeds.size() < m_minPts) {
        node.clusterID = NOISE;
        return FAILURE;
    } else {
        std::set<int> candidates;
        for (int neighborId : clusterSeeds) {
            int cid = nodes[neighborId].clusterID;
            if (cid != UNCLASSIFIED && cid != NOISE) {
                candidates.insert(cid);
            }
        }
        if (candidates.empty()) {
            // New cluster
            m_clusterCount++;
            node.clusterID = m_clusterCount;
            for (int nid : clusterSeeds)
                nodes[nid].clusterID = m_clusterCount;
        } else {
            int min_idx = *candidates.begin();
            node.clusterID = min_idx;
            for (int nid : clusterSeeds)
                nodes[nid].clusterID = min_idx;
            candidates.erase(min_idx);
            // Only positive cluster IDs
            std::vector<int> merge_ids;
            for (int cid : candidates)
                if (cid > 0) merge_ids.push_back(cid);
            if (!merge_ids.empty())
                updateClusterID(merge_ids, min_idx);
        }
        return SUCCESS;
    }
}

int GraphDBSCAN::updateClusterID(const std::vector<int>& mod_idx, int target_idx) {
    std::set<int> valid_ids;
    for (int cid : mod_idx) {
        if (cid > 0 && cid <= m_clusterCount)
            valid_ids.insert(cid);
    }
    for (auto& kv : nodes) {
        if (valid_ids.count(kv.second.clusterID))
            kv.second.clusterID = target_idx;
    }
    // Recompute the number of unique clusters (excluding noise/unclassified)
    std::set<int> unique_clusters;
    for (const auto& kv : nodes) {
        if (kv.second.clusterID > 0)
            unique_clusters.insert(kv.second.clusterID);
    }
    m_clusterCount = unique_clusters.size();
    return SUCCESS;
}
