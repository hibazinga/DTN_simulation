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
#include <cmath>

using namespace std;


/*********************************************************************/
//                          init()
/*********************************************************************/

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
    /*
    cout << "init matrix: ----------------------------\n";
    for(int i=0; i < k; i++){
        for (int j=0; j < k; j++) {
            if (lambda_matrix[i][j] == INT32_MAX) {
                cout << "X.XXXX";
            }else if(lambda_matrix[i][j] == 0)
                cout<<"-.----";
            else
                cout<<lambda_matrix[i][j];
            cout<< "\t";
        }
        cout<<endl;
    }
    cout << " ----------------------------\n";
    */
    
}


/*********************************************************************/
//                          Modeling
/*********************************************************************/


class Comparator
{
public:
    bool operator()(pair<int,double> n1,pair<int,double> n2) {
        return n1.second > n2.second;
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
        if (lambda_matrix[node][i] > 0){
            double lower_level_exp = Multihop::getSubOptimalSet(i, level - 1);
            if(lower_level_exp < INT32_MAX){
                pq.push(make_pair(i, lower_level_exp));
            }
        }
    }
    if(level == 1){
        while (pq.size() > 0) {
            pair<int, double> temp = pq.top();
            pq.pop();
            if(min(1/lambda_matrix[temp.first][k-1], 1/lambda_matrix[node][k-1]) < exp){
                //cout<< "current exp: " << exp << endl;
                //cout<< "self_lower_level_exp" << self_lower_level_exp << endl;
                //cout<< "temp.second" << temp.second << endl;
                bool find = false;
                for (int kkk = 0; kkk < optimal_set[node][level].size(); kkk++) {
                    if(optimal_set[node][level][kkk].first == temp.first)
                    {
                        find = true;
                        break;
                    }
                }
                if(find) continue;
                optimal_set[node][level].push_back(temp);
                //cout <<"node:" << node << " level:" << level << " next_hop: "<< temp.first << " exp:" << temp.second << endl;
                exp = calculateExp0(node, level);
                //cout << "after calc: " << exp << endl << endl;
                
            }else break;
        }
    }else{
        while (pq.size() > 0) {
            pair<int, double> temp = pq.top();
            pq.pop();
            if(min(self_lower_level_exp, temp.second) < exp){
                //cout<< "current exp: " << exp << endl;
                //cout<< "self_lower_level_exp" << self_lower_level_exp << endl;
                //cout<< "temp.second" << temp.second << endl;
                bool find = false;
                for (int kkk = 0; kkk < optimal_set[node][level].size(); kkk++) {
                    if(optimal_set[node][level][kkk].first == temp.first)
                    {
                        find = true;
                        break;
                    }
                }
                if(find) continue;
                optimal_set[node][level].push_back(temp);
                //cout <<"node:" << node << " level:" << level << " next_hop: "<< temp.first << " exp:" << temp.second << endl;
                exp = calculateExp(node, level);
                //cout << "after calc: " << exp << endl << endl;
            }else break;
        }
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
    //cout << "node: " << node << " level: " << level << " exp: "<< exp << endl;
    return exp;
}

// helper function: calculate exp0, for level 1
// source: node, dest: k-1, intermediate node: temp_node
double Multihop::calculateExp0(int node, int level){
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
    if(Sn == 0) cout<<"\n\nbad!!!!!\n\n";
    double exp = 1.0/Sn;
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == k - 1) {
            continue;
        }
        if(min(node_level_exp[node][level-1],node_level_exp[temp_node][level-1]) == 0) cout<< "***********\n";
        exp += lambda_matrix[node][temp_node] / Sn * min(1/lambda_matrix[node][k-1], 1/lambda_matrix[temp_node][k-1]);
    }
    //cout << "node: " << node << " level: " << level << " exp: "<< exp << endl;
    return exp;
}





/*********************************************************************/
//                              simulation
/*********************************************************************/


double Multihop::simulate(int source_node){
    double simulation_min_time = simulate_helper(source_node, level - 1);
    cout << "Simulation result: " << simulation_min_time << endl;
    return simulation_min_time;
}

double getMin(vector<double> v){
    if(v.size() == 0) return INT32_MAX;
    double result = v[0];
    for (int i = 1;  i < v.size(); i++) {
        result = min(result, v[i]);
    }
    return result;
}

double Multihop::simulate_helper(int node, int level) {
    double random = ((double) rand() / (RAND_MAX));
    double t0;
    if(lambda_matrix[node][k-1] == INT32_MAX) t0 = INT32_MAX;
    else t0 = -log(1-random) / lambda_matrix[node][k-1];
    cout <<"#####t0:" <<t0 << endl;
    if(level == 0) return t0;
    priority_queue<pair<int, double>, vector<pair<int,double>>, Comparator> pq; // pair<node, exp>
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int next = optimal_set[node][level][i].first;
        double temp = -log(1-random) / lambda_matrix[node][next];
        pq.push(make_pair(next, temp));
    }
    vector<double> v;
    v.push_back(t0);
    for (int i = 0; i < level; i++) {
        double top = pq.top().second;
        if(top > t0) break;
        else{
            v.push_back(top + simulate_helper(pq.top().first, level-1));
        }
        pq.pop();
    }
    return getMin(v);
}

double Multihop::wrapper_simalate(int source_node, int times){
    double sum = 0;
    int count = times;
    for (int i = 0; i < times; i++) {
        double result = simulate(source_node);
        if (result < INT32_MAX) {
            sum += result;
        }else count--;
    }
    return sum/count;
}


/*********************************************************************/
//                              debug
/*********************************************************************/

void Multihop::printEXP(){
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < level; j++) {
            if (i == 0)
                cout << "node: " << i << " level: " << j << " EXP: " << node_level_exp[i][j] <<endl;
        }
    }
}
void Multihop::printSET(){
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < level; j++) {
            cout << "node: " << i << " level: " << j << " set:"<<endl;
            for (int m = 0; m < optimal_set[i][j].size(); m++) {
                cout << "      node: " << optimal_set[i][j][m].first << " Exp: " << optimal_set[i][j][m].second <<endl;
                
            }
        }
    }
}

