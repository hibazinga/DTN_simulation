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
#include <string>

using namespace std;


int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    int num_of_nodes = 100;
    int level = 4;
    int source_node = 60;
    Multihop test(num_of_nodes, level);
    test.read_matrix();
    //test.printLambdaMatrix();
    cout<<"EXP: " <<test.getSubOptimalSet(source_node, level) << endl;
    test.printEXP();
    test.printSET();
    test.getMultiCopyTwoHopSet(source_node, 10);
    test.printMultiCopyTwoHopSET(10);
    test.printMultiCopyTwoHopEXP(10);
    //test.printSET();
    //cout<<"multi-hop: " << test.simulate(source_node) << endl;
    //cout<<"2-hop:" << test.two_hop_simulate(source_node, 9) << endl;
    return 0;
}
