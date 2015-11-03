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

char * solution (const char *s){
    if(*s == '\0') return nullptr;
    vector<int> list;
    while (*s != '\0') {
        char c = *s++;
        if(c>='0' && c<='9'){
            list.push_back(c-'0');
        }
    }
    int size = (int)list.size();
    char *result;
    if(size % 3 == 0){
        result = (char *)malloc(size + size/3);
        int index = 0;
        for(int i = 0; i < size; i+=3){
            result[index++] = list[i]+'0';
            result[index++] = list[i+1]+'0';
            result[index++] = list[i+2]+'0';
            if(i+3 < size)
                result[index++] = '-';
        }
        result[index++] = '\0';
        return result;
    }else if(size % 3 == 1){
        result = (char *)malloc(size + size/3 + 1);
        int i = 0, index = 0;
        for(; i < size-4; i+=3){
            result[index++] = list[i]+'0';
            result[index++] = list[i+1]+'0';
            result[index++] = list[i+2]+'0';
            result[index++] = '-';
        }
        result[index++] = list[i++]+'0';
        result[index++] = list[i++]+'0';
        result[index++] = '-';
        result[index++] = list[i++]+'0';
        result[index++] = list[i++]+'0';
        result[index++] = '\0';
        return result;
    }else{
        if(size == 2){
            result = (char *)malloc(3);
            result[0] = list[0]+'0';
            result[1] = list[1]+'0';
            result[3] = '\0';
            return result;
        }else{
            result = (char *)malloc(size + size/3 + 1);
            int i = 0, index = 0;
            for(; i < size-2; i+=3){
                result[index++] = list[i]+'0';
                result[index++] = list[i+1]+'0';
                result[index++] = list[i+2]+'0';
                result[index++] = '-';
            }
            result[index++] = list[i++]+'0';
            result[index++] = list[i++]+'0';
            result[index++] = '\0';
            return result;
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //std::cout << "Hello, World!\n";
    /*
    int num_of_nodes = 200;
    int level = 3;
    int source_node = 0;
    Multihop test(num_of_nodes, level);
    test.init_matrix();
    cout<<"EXP: " <<test.getSubOptimalSet(source_node, level) << endl;
    test.printEXP();
    //test.printSET();
    //cout<<"Avg sim: " << test.wrapper_simalate(source_node, 10);
    */
    string c="00123456";
    char *s = solution(c.c_str());
    while(*s != '\0'){
        cout << *s++;
    }
    return 0;
}
