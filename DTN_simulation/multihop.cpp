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
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <unordered_set>

using namespace std;


/*********************************************************************/
//                          init()
/*********************************************************************/

Multihop::Multihop(int num, int __level){
    k = num;
    _level = __level+1; //level, level-1, level-2, ... 0
    vector<pair<int,double>> dummy_set;
    vector<vector<pair<int,double>>> dummy_level(_level, dummy_set);
    for (int i = 0; i < k; i++) {
        optimal_set.push_back(dummy_level);
    }
    vector<double> temp(_level, INT32_MAX);
    for (int i = 0; i < k; i++) {
        node_level_exp.push_back(temp);
    }
    start_node = 30;
    end_node = 75;
}

void Multihop::init_matrix(){
    lambda_matrix = new double*[k];
    for(int i = 0; i < k; ++i)
        lambda_matrix[i] = new double[k];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < k; i++){
        for(int j = 0; j < i; j++){
            int random = rand() % 2;  // each node has roughly
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

vector<double> split(string s){
    vector<double> result;
    string tmp = "";
    for(int i = 0; i < s.length(); i++){
        if(s[i] != ','){
            tmp += s[i];
        }else{
            result.push_back(atof(tmp.c_str()));
            tmp = "";
            i++;
        }
    }
    result.push_back(atof(tmp.c_str()));
    return result;
}

void Multihop::read_matrix(){
    lambda_matrix = new double*[k];
    for(int i = 0; i < k; ++i)
        lambda_matrix[i] = new double[k];
    
    ifstream inFile;
    string buffer;
    inFile.open("/Users/ybren/Desktop/lambda_output.txt");
    int index = 0;
    if (inFile.is_open()) {
        while (!inFile.eof()) {
            getline(inFile, buffer); // Save the line in buffer.
            if(buffer.length() > 0){
                buffer = buffer.substr(1,buffer.length() - 3);
                vector<double> result = split(buffer);
                for(int j = 0; j < result.size(); j++){
                    lambda_matrix[index][j] = result[j];
                }
                index++;
            }
        }
    }
    inFile.close();
    for (int i = 0; i < k; i++) {
        lambda_matrix[i][i] = INT32_MAX;    // lambda = infinite
    }
}


/*********************************************************************/
//                          Modeling
/*********************************************************************/

// ascending order:
class Comparator
{
public:
    bool operator()(pair<int,double> n1,pair<int,double> n2) {
        return n1.second > n2.second;
    }
};

class Comparator0
{
public:
    bool operator()(double o1, double o2) {
        return o1 > o2;
    }
};

// descending order:
class Comparator1
{
public:
    bool operator()(pair<int,double> n1,pair<int,double> n2) {
        return n2.second > n1.second;
    }
};

// return expected time
double Multihop::getSubOptimalSet(int node, int level){
    //cout<< "node: " << node << " level: "<< level << endl;
    if(node_level_exp[node][level] != INT32_MAX) return node_level_exp[node][level];
    if(level == 0){
        if(node == end_node) {
            node_level_exp[node][level] = 0;
            return 0;
        }
        if(lambda_matrix[node][end_node] == 0){
            node_level_exp[node][level] = (double)INT32_MAX;
            return (double)INT32_MAX;
        }
        node_level_exp[node][level] = 1.0/lambda_matrix[node][end_node];
        return 1.0/lambda_matrix[node][end_node];
    }
    if(node == end_node){
        node_level_exp[node][level] = 0;
        return 0;
    }
    double exp;
    if(lambda_matrix[node][end_node]!=0) exp = 1.0/lambda_matrix[node][end_node];
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
            if(min(1/lambda_matrix[temp.first][end_node], 1/lambda_matrix[node][end_node]) < exp){
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
            //if( 1.0 / (1.0/self_lower_level_exp + 1.0/temp.second) < exp){
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
// source: node, dest: end_node, intermediate node: temp_node
double Multihop::calculateExp(int node, int level){
    if (node == end_node) {
        return 0;
    }
    double Sn = lambda_matrix[node][end_node];
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == end_node) {
            continue;
        }
        Sn += lambda_matrix[node][temp_node];
    }
    if(Sn == 0) return INT32_MAX;
    double exp = 1.0/Sn;
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == end_node) {
            continue;
        }
        exp += lambda_matrix[node][temp_node] / Sn * min(node_level_exp[node][level-1],node_level_exp[temp_node][level-1]);
        
        //exp += lambda_matrix[node][temp_node] / Sn / ( 1.0 /node_level_exp[node][level-1] + 1.0/node_level_exp[temp_node][level-1]);
    }
    //cout << "node: " << node << " level: " << level << " exp: "<< exp << endl;
    return exp;
}

// helper function: calculate exp0, for level 1
// source: node, dest: end_node, intermediate node: temp_node
double Multihop::calculateExp0(int node, int level){
    if (node == end_node) {
        return 0;
    }
    double Sn = lambda_matrix[node][end_node];
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == end_node) {
            continue;
        }
        Sn += lambda_matrix[node][temp_node];
    }
    double exp = 1.0/Sn;
    for (int i = 0; i < optimal_set[node][level].size(); i++) {
        int temp_node = optimal_set[node][level][i].first;
        if (temp_node == end_node) {
            continue;
        }
        exp += lambda_matrix[node][temp_node] / Sn * min(1.0/lambda_matrix[node][end_node], 1.0/lambda_matrix[temp_node][end_node]);
    }
    //cout << "node: " << node << " level: " << level << " exp: "<< exp << endl;
    return exp;
}

// must run after getOptimalSet();
void Multihop::getMultiCopyTwoHopSet(int node, int L){
    int cur_L = 0;
    double C = 0;
    double Umax = -1;
    for (int i = 0; i < k; i++) {
        if( i != end_node){
            Umax = max(Umax, lambda_matrix[i][end_node]);
        }
    }
    while (cur_L <= L) {
        if(cur_L == 0){
            multiCopyExp.push_back(0);
            vector<int> tmp;
            multiCopyTwoHopSet.push_back(tmp);
        }else if(cur_L == 1){
            vector<int> tmp;
            multiCopyExp.push_back(node_level_exp[node][1]);
            for(pair<int,double> p: optimal_set[node][1]){
                tmp.push_back(p.first);
            }
            multiCopyTwoHopSet.push_back(tmp);
        }else{
            C = pow(2 * multiCopyExp[1] * Umax, 1.0/pow(2,cur_L-1))/2.0;
            double exp = INT32_MAX;
            priority_queue<pair<int, double>, vector<pair<int,double>>, Comparator1> pq; // pair<node, U> in desc order
            for (int i = 0; i < k; i++) {
                if (i == node) {
                    continue;
                }
                if (lambda_matrix[node][i] > 0 && lambda_matrix[i][end_node] > 0){
                    pq.push(make_pair(i, lambda_matrix[i][end_node]));
                }
            }
            vector<int> tmp;
            multiCopyTwoHopSet.push_back(tmp);
            while (pq.size() > 0) {
                pair<int, double> temp = pq.top();
                pq.pop();
                if(temp.second > C / exp){
                    multiCopyTwoHopSet[cur_L].push_back(temp.first);
                    exp = calculateMultiCopyExp(node, cur_L, C);
                }else break;
            }
            multiCopyExp.push_back(exp);
            //multiCopyTwoHopSet.push_back(tmp);
        }
        cur_L++;
    }
}


double Multihop::calculateMultiCopyExp(int node, int L, double C){
    double lambda_max = -1;
    for (int i = 0; i < k; i++) {
        if(i == node) continue;
        lambda_max = max(lambda_max, lambda_matrix[node][i]);
    }
    double tmp1 = 0;
    for(int i: multiCopyTwoHopSet[L]){
        tmp1 += lambda_matrix[node][i] * lambda_matrix[node][i];
    }
    double tmp2 = 0;
    for(int i: multiCopyTwoHopSet[L]){
        tmp2 += lambda_matrix[node][i] * lambda_matrix[node][i] / lambda_matrix[i][end_node];
    }
    return (lambda_max + C * tmp2) / tmp1;
}

/*********************************************************************/
//                              simulation
/*********************************************************************/


double Multihop::simulate(int source_node){
    double simulation_min_time = simulate_helper(source_node, _level - 1);
    //cout << "Simulation result: " << simulation_min_time << endl;
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
    srand((unsigned int)time(NULL));
    double random = ((double) rand() / (RAND_MAX));
    int times = rand() % 100;
    for (int i = 0; i < times; i++) { // random init
        random = ((double) rand() / (RAND_MAX));
    }
    //double random = 0.7;
    //cout << "#########" <<random << endl;
    double t0;
    if(lambda_matrix[node][end_node] == 0) t0 = INT32_MAX;
    else t0 = -log(1-random) / lambda_matrix[node][end_node];
    //cout <<"#####t0:" << t0 << endl;
    if(level == 0) return t0;
    queue<int> next_node;
    queue<double> cur_exp;
    queue<int> cur_level;
    priority_queue<double, vector<double>, Comparator0> pq;
    pq.push(t0);
    next_node.push(node);
    cur_exp.push(0);
    cur_level.push(level);
    
    while (next_node.size() > 0) {
        int tmp_node = next_node.front(); next_node.pop();
        int tmp_level = cur_level.front(); cur_level.pop();
        double tmp_exp = cur_exp.front(); cur_exp.pop();
        if(tmp_level == 0 && tmp_node == end_node){ continue; }
        if(tmp_level == 0) {
            //srand((unsigned int)time(NULL));
            random = ((double) rand() / (RAND_MAX));
            pq.push(tmp_exp + -1 * log(1-random) / lambda_matrix[tmp_node][end_node]);
            //cout << "###:" << tmp_exp + -1 * log(1-random) / lambda_matrix[tmp_node][end_node] <<endl;
            continue;
        }
        for (int i = 0; i < optimal_set[tmp_node][tmp_level].size(); i++) {
            int next = optimal_set[tmp_node][tmp_level][i].first;
            //srand((unsigned int)time(NULL));
            random = ((double) rand() / (RAND_MAX));
            double temp = -log(1-random) / lambda_matrix[tmp_node][next];
            if(tmp_exp + temp < pq.top()){
                next_node.push(next);
                cur_level.push(tmp_level - 1);
                cur_exp.push(tmp_exp + temp);
            }
            if(lambda_matrix[next][end_node] != 0){
                //srand((unsigned int)time(NULL));
                random = ((double) rand() / (RAND_MAX));
                pq.push(tmp_exp + temp + -1 * log(1-random) / lambda_matrix[next][end_node]);
                //cout << "###:" << tmp_exp + temp + -1 * log(1-random) / lambda_matrix[next][end_node] <<endl;
            }
        }
    }
    return pq.top();
}


double Multihop::two_hop_simulate(int node, int L){ // L: num of copies
    srand((unsigned int)time(NULL));
    double random = ((double) rand() / (RAND_MAX));
    int times = rand() % 100;
    for (int i = 0; i < times; i++) { // random init
        random = ((double) rand() / (RAND_MAX));
    }
    double t0;
    if(lambda_matrix[node][end_node] == INT32_MAX) t0 = INT32_MAX;
    else t0 = -log(1-random) / lambda_matrix[node][end_node];
    if(L == 0) return t0;
    priority_queue<double, vector<double>, Comparator0> pq;
    pq.top();
    unordered_set<int> visited;
    while (L > 0) {
        vector<int> set = multiCopyTwoHopSet[L];
        vector<int> candidate;
        for (int i = 0; i < set.size(); i++) {
            if(visited.find(set[i]) == visited.end()){
                candidate.push_back(set[i]);
            }
        }
        priority_queue<pair<int, double>, vector<pair<int,double>>, Comparator> tmp_pq;
        for(int i = 0; i < candidate.size(); i++){
            random = ((double) rand() / (RAND_MAX));
            double tmp = -log(1-random) / lambda_matrix[node][candidate[i]];
            tmp_pq.push(make_pair(candidate[i], tmp));
        }
        if (tmp_pq.size() == 0) {
            break;
        }
        random = ((double) rand() / (RAND_MAX));
        visited.insert(tmp_pq.top().first);
        pq.push(tmp_pq.top().second + -log(1-random) / lambda_matrix[tmp_pq.top().first][end_node]);
        L--;
    }
    return pq.top();
}




double Multihop::wrapper_simulate(int source_node, int times){
    double sum = 0;
    int count = times;
    for (int i = 0; i < times; i++) {
        double result = simulate(source_node);
        if (result < INT32_MAX) {
            sum += result;
        }else count--;
    }
    if(count == 0) return (double)INT32_MAX;
    return sum/count;
}


/*********************************************************************/
//                              debug
/*********************************************************************/



void Multihop::printLambdaMatrix(){
    for(int i = 0; i < k; i ++){
        for(int j = 0; j < k; j++){
            cout << lambda_matrix[i][j] << ", ";
        }
        cout << endl;
    }

}

void Multihop::printEXP(){
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < _level; j++) {
            if (i == start_node)
                cout << "node: " << i << " level: " << j << " EXP: " << node_level_exp[i][j] <<endl;
        }
    }
}

void Multihop::printSET(){
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < _level; j++) {
            cout << "node: " << i << " level: " << j << " set:"<<endl;
            for (int m = 0; m < optimal_set[i][j].size(); m++) {
                cout << "      node: " << optimal_set[i][j][m].first << " Exp: " << optimal_set[i][j][m].second <<endl;
                
            }
        }
    }
}

void Multihop::printMultiCopyTwoHopEXP(int L){
    for (int i = 0; i < L; i++) {
        cout << "L = " << i << " exp = " << multiCopyExp[i] << endl;
    }
}

void Multihop::printMultiCopyTwoHopSET(int L){
    for (int i = 0; i < L; i++) {
        cout << "L = " << i << " set: ";
        for (int j = 0; j < multiCopyTwoHopSet[i].size(); j++)
            cout << multiCopyTwoHopSet[i][j] << ", ";
        cout << endl;
    }
}


