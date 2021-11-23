/***************************************************************************
    greedy.cpp 
    (C) 2021 by C. Blum & M. Blesa
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../Timer.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <unordered_set>
#include <cstring>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
#include <iomanip>

// global variables concerning the random number generator (in case needed)
time_t t;

// Data structures for the problem data
int N;
int M;
vector< unordered_set<int> > neighbors;
vector<int> NND;
vector<bool> D;

// string for keeping the name of the input file
string inputFile;

// dummy parameters as examples for creating command line parameters 
// see function read_parameters(...)
int dummy_integer_parameter = 0;
int dummy_double_parameter = 0.0;


inline int stoi(string &s) {

  return atoi(s.c_str());
}

inline double stof(string &s) {

  return atof(s.c_str());
}

void read_parameters(int argc, char **argv) {

    int iarg = 1;

    while (iarg < argc) {
        if (strcmp(argv[iarg],"-i")==0) inputFile = argv[++iarg];
        
        // example for creating a command line parameter param1 
        //-> integer value is stored in dummy_integer_parameter
        else if (strcmp(argv[iarg],"-param1")==0) 
            dummy_integer_parameter = atoi(argv[++iarg]); 
            
        // example for creating a command line parameter param2 
        //-> double value is stored in dummy_double_parameter
        else if (strcmp(argv[iarg],"-param2")==0) 
            dummy_double_parameter = atof(argv[++iarg]);  
            
        iarg++;
    }
}

int minNND(int n) {
    return ceil(float(n)/2);
}

int dominador2(vector<int> NND) {
    for(int i = 0; i < neighbors.size(); i++) {
        if(NND[i] < minNND(neighbors[i].size())) return i;
    }
    return -1;
}

int minimal3(vector<bool> DV, vector<int> NND) { 
    for(int i = 0; i < DV.size(); i++) {
        if(not DV[i]) continue;
        int min = minNND(neighbors[i].size());
        int count = 0;
        unordered_set<int>::iterator itr = neighbors[i].begin();
        while(itr != neighbors[i].end() and count < min) {
            if(NND[*itr]-1 < min) count++;
            itr++;
        }
        if(count < min) return i;
    }
    return -1;
}

bool dominador() {
    for(int i = 0; i < N; i++) {
        if(NND[i] < float(neighbors[i].size())/float(2)) return false;
    }
    return true;
}

int millorV() {
    int max = -1, maxPos = 0;
    for (int i = 0; i < N; ++i) {
        if (!D[i]) {
            unordered_set<int>::iterator itr = neighbors[i].begin();
            int count = 0; //Quants veins del vertex no tenen 50% o més
            while (itr != neighbors[i].end()) {
                if (NND[*itr] < float(neighbors[*itr].size())/float(2)) ++count;
                ++itr;
            }
            if (count > max) {
                max = count;
                maxPos = i;
            }
            else if (count == max) {
                if (neighbors[count].size() > neighbors[maxPos].size()) {
                    max = count;
                    maxPos = i;
                }
            }
        }
    }
    return maxPos;
}

void afegeixVeins(int v) { //v abans no formava part de D i ara si
    unordered_set<int>::iterator itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {
        ++NND[*itr];
        ++itr;
    }
}

void eliminaVeins(int v) { //v abans formava part de de D i ara no
    unordered_set<int>::iterator itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {
        --NND[*itr];
        ++itr;
    }
}

void greedy() {

    while (not dominador()) {
        int v = millorV();
        D[v] = true;
        afegeixVeins(v);
    }
    if (dominador()) cout << "yayyy"<<endl;

    int b2 = 0;
    while (b2 != -1) {
        b2 = minimal3(D,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant
        if (b2 != -1) {
            D[b2] = false;
            eliminaVeins(b2);
        }
    }
}


/************
Main function
*************/

int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    
    // setting the output format for doubles to 2 decimals after the comma
    std::cout << std::setprecision(10) << std::fixed;

    // variables for storing the result and the computation time 
    // obtained by the greedy heuristic
    double results = std::numeric_limits<int>::max();
    double time = 0.0;

    // opening the corresponding input file and reading the problem data
    ifstream indata;
    indata.open(inputFile.c_str());
    if(not indata) { // file couldn't be opened
        cout << "Error: file could not be opened" << endl;
    }

    indata >> N;
    indata >> M;
    NND = vector<int> (N, 0); //Nombre de veins que pertanyen a D
    D = vector<bool>(N); //Nodes que pertanyen a D
    neighbors = vector< unordered_set<int> >(N);
    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();


    //Timer timer;

    greedy();

    int count = 0;
    for (int i = 0; i < N; ++i) if (D[i]) {
        ++count;
    }
    cout << "Number of vertices in solution: " << count << endl;

    int b2 = minimal3(D,NND);
    int b1 = dominador2(NND);
    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;

    //double ct = timer.elapsed_time(Timer::VIRTUAL);
    //cout << "value " << results << "\ttime " << ct << endl;

}

