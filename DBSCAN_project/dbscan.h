#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <cmath>
#include <climits>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>
#include <map>

#define UNCLASSIFIED -1
#define CORE_POINT 1
#define BORDER_POINT 2
#define NOISE -2
#define SUCCESS 0
#define FAILURE -3

using namespace std;

typedef struct Point_
{
    float x, y, z;  // X, Y, Z position
    int clusterID;  // clustered ID
}Point;

class DBSCAN {
public:    
    DBSCAN(unsigned int minPts, float eps, vector<Point> points){
        m_minPoints = minPts;
        m_epsilon = eps;
        m_points = points;
        m_pointSize = points.size();
        m_clusterQuantity = 0;
    }
    ~DBSCAN(){}

    int run();
    vector<int> calculateCluster(Point point);
    int expandCluster(Point point, int clusterID);
    inline double calculateDistance(const Point& pointCore, const Point& pointTarget);
    int addPoint(Point& point);
    int updateClusterID(const vector<int>& mod_idx, int target_idx);

    int getTotalPointSize() {return m_pointSize;}
    int getMinimumClusterSize() {return m_minPoints;}
    int getEpsilonSize() {return m_epsilon;}
    int getClusterQuantity() {return m_clusterQuantity;}
    
public:
    vector<Point> m_points;
    
private:    
    unsigned int m_pointSize;
    unsigned int m_minPoints;
    float m_epsilon;
    unsigned int m_clusterQuantity;
};

#endif // DBSCAN_H
