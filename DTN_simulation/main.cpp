//
//  main.cpp
//  DTN_simulation
//
//  Created by Yanbin Ren on 10/12/15.
//  Copyright (c) 2015 Yanbin Ren. All rights reserved.
//

#include <iostream>
#include "multihop.h"
#include <vector>
#include <queue>
#include <utility>

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    Multihop test(100, 3);
    test.init_matrix();
    cout<< test.getSubOptimalSet(0, 3);

    /*
    class Comparator
    {
    public:
        bool operator()(pair<int,double> n1,pair<int,double> n2) {
            return n1.second<n2.second;
        }
    };
    priority_queue<pair<int, double>, vector<pair<int,double>>, Comparator> pq;
    pq.push(make_pair(1, 8));
    pq.push(make_pair(1, 9));
    pair<int, double> s = pq.top();
    cout<<s.second;
    */
    return 0;
}
