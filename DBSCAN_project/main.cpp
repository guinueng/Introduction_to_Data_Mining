#include <stdio.h>
#include <iostream>
#include "dbscan.h"

#define MINIMUM_POINTS 4     // minimum number of cluster
#define EPSILON (0.75*0.75)  // distance for clustering, metre^2

void readBenchmarkData(vector<Point>& points, const char* file_name)
{
    // load point cloud
    FILE *stream;
    stream = fopen (file_name,"ra");

    unsigned int minpts, num_points, cluster, i = 0;
    double epsilon;
    fscanf(stream, "%u\n", &num_points);

    Point *p = (Point *)calloc(num_points, sizeof(Point));

    while (i < num_points)
    {
          fscanf(stream, "%f,%f,%f,%d\n", &(p[i].x), &(p[i].y), &(p[i].z), &cluster);
          p[i].clusterID = UNCLASSIFIED;
          points.push_back(p[i]);
          ++i;
    }

    free(p);
    fclose(stream);
}

void printResults(vector<Point>& points, int num_points)
{
    int i = 0;
    printf("Number of points: %u\n"
        " x     y     z     cluster_id\n"
        "-----------------------------\n"
        , num_points);
    while (i < num_points)
    {
          printf("%5.2lf %5.2lf %5.2lf: %d\n",
                 points[i].x,
                 points[i].y, points[i].z,
                 points[i].clusterID);
          ++i;
    }
}

int main()
{    
    vector<Point> points;

    // read point data
    readBenchmarkData(points, "benchmark_hepta.dat");

    // constructor
    DBSCAN ds(MINIMUM_POINTS, EPSILON, points);

    // main loop
    ds.run();

    // result of DBSCAN algorithm
    printf("1\ncluster qty: %d\n", ds.getClusterQuantity());
    printResults(ds.m_points, ds.getTotalPointSize());    
    printf("\n\n\n");

    vector<Point> u_points;
    readBenchmarkData(u_points, "benchmark_hepta_uh.dat");
    DBSCAN u_ds(MINIMUM_POINTS, EPSILON, u_points);
    u_ds.run();
    printf("2\ncluster qty: %d\n", u_ds.getClusterQuantity());
    printResults(u_ds.m_points, u_ds.getTotalPointSize());  
    printf("\n\n\n");

    vector<Point> l_points;
    readBenchmarkData(l_points, "benchmark_hepta_lh.dat");
    for(vector<Point>::iterator iter = l_points.begin(); iter != l_points.end(); iter++){
        u_ds.addPoint(*iter);
    }

    // result of DBSCAN algorithm
    printf("3\ncluster qty: %d\n", ds.getClusterQuantity());
    printResults(u_ds.m_points, u_ds.getTotalPointSize());  

    return 0;
}
