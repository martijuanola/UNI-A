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
#include "../Random.h"
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
#include <cstring>
#include <sstream>
#include <vector>
#include <set>
#include <limits>
#include <iomanip>

// global variables concerning the random number generator (in case needed)
time_t t;
Random* rnd;

// Data structures for the problem data
int n_of_nodes;
int n_of_arcs;
vector< set<int> > neighbors;

// Data structures added
int n_of_d;
set<int> d;
vector<int> nn;

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


bool dominador(vector<set<int>> llista, set<int> ds, int M) {

    bool b = true;

    int i = 0;
    while(b and i < llista.size()) {
        //ITERAR SOBRE TOTS ELS NODES

        int n = llista[i].size();
        int count = 0;
        set<int>::iterator itr = llista[i].begin();
        while(count < float(n)/2 and itr != llista[i].end() and b) { //mirar que funcioni bé la divisio per float
            if(ds.find(*itr) != ds.end()) count++;
            itr++;
        }
        if(count < float(n)/2) {
            b = false; //mirar que funcioni bé la divisio per float
        }
        i++;
    }

    return b;
}

bool dominador(vector<set<int>> llista, set<int> ds, vector<int> nnds, int M) {

    bool b = true;

    int i = 0;
    while(i < llista.size() and b) {
        if(nnds[i] < float(llista[i].size())/2) b = false;
        i++;
    }

    return b;
}

bool minimal(vector<set<int>> llista, set<int> ds, vector<int> nnds, int M) {

    bool b = true;

    int i = 0;
    while(b and i < llista.size()) {
        //ITERAR SOBRE TOTS ELS NODES

        //SI TOTS ELS VEINS DE I TENEN UN NNDS AMB UN NODE DE MARGE, NO ÉS MINIMAL
        //(I PODRIA NO SER DE D I ES CONTINUARIA COMPLINT LA CONDICIO DE DOMINANT)

        set<int>::iterator itr = llista[i].begin();
        bool found = false;
        while(b and (not found) and itr != llista[i].end()) {
            if(nnds[*itr]-1 < float(llista[*itr].size())/2) found = true;
        }
        if(not found) b = false;
    }

    return b;
}

/************
Main function
*************/

int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    
    // setting the output format for doubles to 2 decimals after the comma
    std::cout << std::setprecision(2) << std::fixed;

    // initializing the random number generator. 
    // A random number in (0,1) is obtained with: double rnum = rnd->next();
    rnd = new Random((unsigned) time(&t));
    rnd->next();

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

    indata >> n_of_nodes;
    indata >> n_of_arcs;
    neighbors = vector< set<int> >(n_of_nodes);

    nn = vector<int>(n_of_nodes,0);
    indata >> n_of_d;

    for(int i = 0; i < n_of_d; i++) {
        int aux;
        indata >> aux;
        d.insert(aux-1);
    }

    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
        if(d.find(v) != d.end()) nn[u]++;
        if(d.find(u) != d.end()) nn[v]++;
    }
    indata.close();


    for(int i = 0; i < n_of_nodes; i++) {
        if(d.find(i) != d.end()) cout << "(D)";
        cout << "\tNode " << i << "\t";
        set<int>::iterator itr = neighbors[i].begin();
        while(itr != neighbors[i].end()){
            cout << *itr << " ";
            itr++;
        }
        cout << endl;
    }


    // the computation time starts now
    Timer timer;
    double ct = timer.elapsed_time(Timer::VIRTUAL);

    bool b1 = dominador(neighbors,d,n_of_arcs);
    bool b2 = false;
    if(b1) b2 = minimal(neighbors,d,nn,n_of_arcs);


    if(b1) {
        cout << "IS dominator" << endl;
        if(b2) cout << "IS minimal" << endl;
        else cout << "IS NOT minimal" << endl;
    }
    else cout << "cagaste" << endl;

    cout << "time " << ct << endl;
}

