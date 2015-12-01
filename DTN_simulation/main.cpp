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
    int source_node = 30;
    Multihop test(num_of_nodes, level);
    //test.init_matrix();
    test.read_matrix();
    //test.printLambdaMatrix();
    for (int j = 0; j < 50; j++) {
        source_node = j;
        test.start_node = j;
    
        test.getSubOptimalSet(source_node, level);
    //test.printEXP();
    //test.printSET();
    test.getMultiCopyTwoHopSet(source_node, 10);
    //test.printMultiCopyTwoHopSET(10);
    //test.printMultiCopyTwoHopEXP(10);
    //test.printSET();
    
    double count = 0;
        /*
    for (int i = 0 ; i < 100; i++) {
        count += test.simulate(source_node);
    }
    cout<<j << " " << count / 100.0 << endl;
         */
     //   count = 0;
    for (int i = 0 ; i < 100; i++) {
        count += test.two_hop_simulate(source_node, 3);
    }
    cout<<j << " " << count / 100.0 << endl;
    }
    return 0;
}
