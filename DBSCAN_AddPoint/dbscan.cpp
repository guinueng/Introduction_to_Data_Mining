#include "dbscan.h"

int DBSCAN::run()
{
    int clusterID = 1;
    vector<Point>::iterator iter;
    for(iter = m_points.begin(); iter != m_points.end(); ++iter)
    {
        if ( iter->clusterID == UNCLASSIFIED )
        {
            if ( expandCluster(*iter, clusterID) != FAILURE )
            {
                clusterID += 1;
            }
        }
    }

    m_clusterQuantity = clusterID - 1;
    return 0;
}

int DBSCAN::expandCluster(Point point, int clusterID)
{    
    vector<int> clusterSeeds = calculateCluster(point);

    if ( clusterSeeds.size() < m_minPoints )
    {
        point.clusterID = NOISE;
        return FAILURE;
    }
    else
    {
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        for( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
        {
            m_points.at(*iterSeeds).clusterID = clusterID;
            if (m_points.at(*iterSeeds).x == point.x && m_points.at(*iterSeeds).y == point.y && m_points.at(*iterSeeds).z == point.z )
            {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

        for( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            vector<int> clusterNeighbors = calculateCluster(m_points.at(clusterSeeds[i]));

            if ( clusterNeighbors.size() >= m_minPoints )
            {
                vector<int>::iterator iterNeighbors;
                for ( iterNeighbors = clusterNeighbors.begin(); iterNeighbors != clusterNeighbors.end(); ++iterNeighbors )
                {
                    if ( m_points.at(*iterNeighbors).clusterID == UNCLASSIFIED || m_points.at(*iterNeighbors).clusterID == NOISE )
                    {
                        if ( m_points.at(*iterNeighbors).clusterID == UNCLASSIFIED )
                        {
                            clusterSeeds.push_back(*iterNeighbors);
                            n = clusterSeeds.size();
                        }
                        m_points.at(*iterNeighbors).clusterID = clusterID;
                    }
                }
            }
        }

        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(Point point)
{
    int index = 0;
    vector<Point>::iterator iter;
    vector<int> clusterIndex;
    for( iter = m_points.begin(); iter != m_points.end(); ++iter)
    {
        if ( calculateDistance(point, *iter) <= m_epsilon )
        {
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

inline double DBSCAN::calculateDistance(const Point& pointCore, const Point& pointTarget )
{
    long double x_dist = (pointCore.x - pointTarget.x);
    long double y_dist = (pointCore.y - pointTarget.y);
    long double z_dist = (pointCore.z - pointTarget.z);

    return x_dist * x_dist + y_dist * y_dist + z_dist * z_dist;
}


int DBSCAN::addPoint(Point& point) {
    m_points.push_back(point);
    m_pointSize++;

    vector<int> clusterSeeds = calculateCluster(point);
    if (clusterSeeds.size() < m_minPoints) {
        m_points.back().clusterID = NOISE;
        return FAILURE;
    } else {
        set<int> candidates;
        for (auto idx : clusterSeeds) {
            int cid = m_points.at(idx).clusterID;
            if (cid != UNCLASSIFIED && cid != NOISE) {
                candidates.insert(cid);
            }
        }
        if (candidates.empty()) {
            // New cluster
            m_clusterQuantity++;
            for (auto idx : clusterSeeds) {
                m_points.at(idx).clusterID = m_clusterQuantity;
            }
        } else {
            int min_idx = *candidates.begin();
            for (auto idx : clusterSeeds) {
                m_points.at(idx).clusterID = min_idx;
            }
            candidates.erase(min_idx);
            if (!candidates.empty()) {
                // Filter out cluster IDs <= 0
                std::vector<int> merge_ids;
                for (auto cid : candidates) {
                    if (cid > 0) {
                        merge_ids.push_back(cid);
                    }
                }
                if (!merge_ids.empty()) {
                    updateClusterID(merge_ids, min_idx);
                }
            }
        }
        return SUCCESS;
    }
}

int DBSCAN::updateClusterID(const vector<int>& mod_idx, int target_idx) {
    map<int, int> cluster_map;
    for (auto cid : mod_idx) {
        if (cid > 0 && cid <= (int)m_clusterQuantity) {
            cluster_map[cid] = target_idx;
        }
    }
    for (auto& pt : m_points) {
        auto it = cluster_map.find(pt.clusterID);
        if (it != cluster_map.end()) {
            pt.clusterID = it->second;
        }
    }
    // Optionally, update m_clusterQuantity if clusters are merged
    return SUCCESS;
}
