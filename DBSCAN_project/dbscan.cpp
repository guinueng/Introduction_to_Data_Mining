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
    return pow(pointCore.x - pointTarget.x,2)+pow(pointCore.y - pointTarget.y,2)+pow(pointCore.z - pointTarget.z,2);
}


int DBSCAN::addPoint(Point& point){
    m_points.push_back(point);
    printf("1\n");
    vector<int> clusterSeeds = calculateCluster(point);
    printf("size: %d\n", clusterSeeds.size());

    if ( clusterSeeds.size() < m_minPoints ){
        printf("2\n");
        point.clusterID = NOISE;
        return FAILURE;
    }
    else{
        printf("3\n");
        int index = 0, indexCorePoint = 0;
        vector<int>::iterator iterSeeds;
        vector<int> candidates;
        for(iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds){
            if(m_points.at(*iterSeeds).clusterID != UNCLASSIFIED && m_points.at(*iterSeeds).clusterID != NOISE){
                candidates.push_back(m_points.at(*iterSeeds).clusterID);
            }
        }
        printf("candidates: ");
        for(size_t i = 0; i < candidates.size(); i++){
            printf("%d ", candidates[i]);
        }
        printf("\n\n4\n");
        if(candidates.size() == 0){ // New cluster happened.
            printf("5\n");
            m_clusterQuantity++;
            for(iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds){
                m_points.at(*iterSeeds).clusterID = m_clusterQuantity;
            }
        }
        else{
            printf("6\n");
            vector<int>::iterator iter;
            vector<int>::iterator min_iter = candidates.begin();
            for(iter = candidates.begin(); iter != candidates.end(); iter++){
                if(*min_iter > *iter){
                    min_iter = iter;
                }
            }
            printf("8\n");
            int min_idx = *min_iter;
            candidates.erase(min_iter);
            printf("7\n");

            for(iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds){
                m_points.at(*iterSeeds).clusterID = min_idx;
            }
            printf("9\n");

            if(candidates.size() != 0){
                printf("11\n");
                updateClusterID(candidates, min_idx);
            }
            printf("10\n");

        }
    }
    m_pointSize++;

    return SUCCESS;
}

int DBSCAN::updateClusterID(vector<int>& mod_idx, int target_idx){
    vector<Point>::iterator iterPoints;
    int new_idx[m_clusterQuantity];
    printf("m_clusterQuantity: %d\ntarget_idx: %d\nmod idx: ", m_clusterQuantity, target_idx);
    for(size_t i = 0; i < mod_idx.size(); i++){
        printf("%d ", mod_idx[i]);
    }
    printf("\n\n11\n");
    for(vector<int>::iterator iter = mod_idx.begin(); iter != mod_idx.end(); iter++){
        new_idx[*iter - 1] = target_idx;
    }
    printf("12\n");
    int max = target_idx + 1;
    for(int i = 0; i < m_clusterQuantity; i++){
        if(i < target_idx - 1){
            new_idx[i] = i + 1;
        }
        if(new_idx[i] == 0){
            new_idx[i] = max;
            max++;
        }
    }

    for(iterPoints = m_points.begin(); iterPoints != m_points.end(); iterPoints++){
        if(iterPoints -> clusterID >= 1 && iterPoints -> clusterID != new_idx[iterPoints -> clusterID - 1]){
            iterPoints -> clusterID = new_idx[iterPoints -> clusterID - 1];
        }
    }

    return SUCCESS;
}