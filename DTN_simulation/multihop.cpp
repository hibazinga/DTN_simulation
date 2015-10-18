//
//  multihop.cpp
//  DTN_simulation
//
//  Created by Yanbin Ren on 10/12/15.
//  Copyright (c) 2015 Yanbin Ren. All rights reserved.
//

#include "multihop.h"
#include <iostream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <queue>
#include <utility>

using namespace std;

Multihop::Multihop(int num, int _level){
    k = num;
    level = _level+1; //level, level-1, level-2, ... 0
    vector<pair<int,double>> dummy_set;
    vector<vector<pair<int,double>>> dummy_level(level, dummy_set);
    for (int i = 0; i < k; i++) {
        optimal_set.push_back(dummy_level);
    }
    vector<double> temp(level, INT32_MAX);
    for (int i = 0; i < k; i++) {
        node_level_exp.push_back(temp);
    }
    simulation_min_time = INT32_MAX;
}

void Multihop::init_matrix(){
    lambda_matrix = new double*[k];
    for(int i = 0; i < k; ++i)
        lambda_matrix[i] = new double[k];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < k; i++){
        for(int j = 0; j < i; j++){
            int random = rand() % 10;  // each node has roughly
                                       // one-tenth of the total nodes as its neightbours
            if(random == 0){
                lambda_matrix[i][j] = 0.005 + rand()%50*0.0001; // range = [0.005, 0.01)
            }else lambda_matrix[i][j] = 0;
        }
    }
    for (int i = 0; i < k; i++) {
        for(int j = i+1; j < k; j++){
            lambda_matrix[i][j] = lambda_matrix[j][i];
        }
    }
    for (int i = 0; i < k; i++) {
        lambda_matrix[i][i] = INT32_MAX;    // lambda = infinite
    }
    cout << "init matrix: ----------------------------\n";
    for(int i=0; i < k; i++){
        for (int j=0; j < k; j++) {
            cout<<lambda_matrix[i][j];
            cout<< " ";
        }
        cout<<endl;
    }
    cout << " ----------------------------\n";
}


class Comparator
{
public:
    bool operator()(pair<int,double> n1,pair<int,double> n2) {
        return n1.second>n2.second;
    }
};
// return expected time
double Multihop::getSubOptimalSet(int node, int level){
    //cout<< "node: " << node << " level: "<< level << endl;
    if(level == 0){
        if(node == k-1) {
            node_level_exp[node][level] = 0;
            return 0;
        }
        if(lambda_matrix[node][k-1] == 0){
            node_level_exp[node][level] = (double)INT32_MAX;
            return (double)INT32_MAX;
        }
        node_level_exp[node][level] = 1.0/lambda_matrix[node][k-1];
        return 1.0/lambda_matrix[node][k-1];
    }
    if(node == k-1){
        node_level_exp[node][level] = 0;
        return 0;
    }
    double exp;
    if(lambda_matrix[node][k-1]!=0) exp = 1.0/lambda_matrix[node][k-1];
    else exp = INT32_MAX;
    priority_queue<pair<int, double>, vector<pair<int,double>>, Comparator> pq; // pair<node, exp>
    double self_lower_level_exp = Multihop::getSubOptimalSet(node, level - 1);
    
    for (int i = 0; i < k; i++) {
        if (i == node) {
            continue;
        }
        if (lambda_matrix[node][i] < INT32_MAX){
            double lower_level_exp = Multihop::getSubOptimalSet(i, level - 1);
            if(lower_level_exp < INT32_MAX){
                pq.push(make_pair(i, lower_level_exp));
            }
        }
    }
    while (pq.size() > 0) {
        pair<int, double> temp = pq.top();
        pq.pop();
        if(min(self_lower_level_exp, temp.second) < exp){
            optimal_set[node][level].push_back(temp);
            //cout <<"node:" << node << " level:" << level << " next_hop: "<< temp.first << " exp:" << temp.second << endl;
            exp = calculateExp(node, level);
        }else break;
    }
    node_level_exp[node][level] = exp;
    return exp;
}

// helper function: calculate exp
// source: node, dest: k-1, intermediate node: temp_node
double Multihop::calculateExp(int node, int level){
    if (node == k-1) {
        return 0;
    }
    double Sn = lambda_matrix[node][k-1];
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == k - 1) {
            continue;
        }
        Sn += lambda_matrix[node][temp_node];
    }
    if(Sn == 0) return INT32_MAX;
    double exp = 1.0/Sn;
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == k - 1) {
            continue;
        }
        if(min(node_level_exp[node][level-1],node_level_exp[temp_node][level-1]) == 0) cout<< "***********\n";
        exp += lambda_matrix[node][temp_node] / Sn * min(node_level_exp[node][level-1],node_level_exp[temp_node][level-1]);
    }
    cout << "node: " << node << " level: " << level << " exp: "<< exp << endl;
    return exp;
}

double Multihop::simulate(){
    simulation_min_time = (double)INT32_MAX;
    
    
    
    
    
    
    return simulation_min_time;
}

double Multihop::wrapper_simalate(int times){
    double sum = 0;
    for (int i = 0; i < times; i++) {
        sum += simulate();
    }
    return sum/times;
}


