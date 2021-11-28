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
#include <unordered_set>
#include <limits>
#include <iomanip>

// global variables concerning the random number generator (in case needed)
time_t t;
Random* rnd;

// string for keeping the name of the input file
string inputFile;

// dummy parameters as examples for creating command line parameters 
// see function read_parameters(...)
int dummy_integer_parameter = 0;
double dummy_double_parameter = 0.0;

// GLOBAL VARIABLES
int N;
int M;
vector<unordered_set<int> > neighbors;


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

//FUNCIONS:

//COMPROVAR SI ÉS DOMINADOR
int dominador2(vector<int> NND) {
    for(int i = 0; i < neighbors.size(); i++) {
        if(NND[i] < minNND(neighbors[i].size())) return i;
    }
    return -1;
}


//COMPROVA SI ÉS MINIMAL(HA DE SER DOMINADOR)
int minimal3(vector<bool> D, vector<int> NND) { 
    for(int i = 0; i < D.size(); i++) {
        if(D[i]) {
            unordered_set<int>::iterator itr = neighbors[i].begin();
            bool b = true;
            while(itr != neighbors[i].end() and b) {
                if(NND[*itr]-1 < minNND(neighbors[*itr].size())) b = false;
                itr++;
            }
            if(b) return i;
        }
    }
    return -1;
}

/************
Main function
*************/
int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    
    // setting the output format for doubles to 2 decimals after the comma
    std::cout << std::setprecision(10) << std::fixed;

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

    indata >> N;
    indata >> M;
    neighbors = vector< unordered_set<int> >(N);
    int ND; //nombre de nodes de D
    vector<int> NND = vector<int>(N,0); //nombre de veins d'un node a D
    vector<bool> D = vector<bool>(N, false); //nodes de D
    
    indata >> ND;

    for(int i = 0; i < ND; i++) {
        int aux;
        indata >> aux;
        D[aux-1] = true;
    }

    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
        if(D[v-1]) NND[u-1]++;
        if(D[u-1]) NND[v-1]++;
    }
    indata.close();

    Timer timer;
    int b1 = dominador2(NND); //-1 no ha trobat cap vertex que no tingui influencia positiva
    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        int b2 = minimal3(D,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2+1 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1+1 << " does not fulfill the requirements." << endl;

    double ct = timer.elapsed_time(Timer::REAL);
    cout << "time " << ct<< endl;

    return 0;
}

