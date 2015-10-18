//
//  multihop.h
//  DTN_simulation
//
//  Created by Yanbin Ren on 10/12/15.
//  Copyright (c) 2015 Yanbin Ren. All rights reserved.
//

#ifndef __DTN_simulation__multihop__
#define __DTN_simulation__multihop__

#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

class Multihop{
private:
    int k;               // number of nodes
    double **lambda_matrix;      // describes the lambda of each two nodes
    int level;
    //  optimal_set[node][level] = vector<pair<node, exp>>;
    vector<vector<vector<pair<int,double>>>> optimal_set;
    vector<vector<double>> node_level_exp;
    double simulation_min_time;
    //double simulation_mean_time;
public:
    Multihop(int, int);
    void init_matrix();
    double getSubOptimalSet(int node, int level);
    double calculateExp(int node, int level);
    double simulate();
    double wrapper_simalate(int times);
    //void optimal_simulate();
};

#endif /* defined(__DTN_simulation__multihop__) */