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
    // node_level_exp[node][level] = exp;
    vector<vector<double>> node_level_exp;
public:
    Multihop(int, int);
    void init_matrix();
    double getSubOptimalSet(int node, int level);
    double calculateExp(int node, int level);
    double calculateExp0(int node, int level);
    double simulate(int source_node);
    double simulate_helper(int node, int level);
    double wrapper_simalate(int source_node, int times);
    void printEXP();
    void printSET();
    //void optimal_simulate();
};

#endif /* defined(__DTN_simulation__multihop__) */
