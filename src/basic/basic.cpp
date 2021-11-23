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
vector<set<int>> neighborsS;
vector<unordered_set<int>> neighborsUS;


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


//Fa servir neighborsUS i nnd
int dominador2(vector<int> NND) {
    for(int i = 0; i < neighborsUS.size(); i++) {
        if(NND[i] < minNND(neighborsUS[i].size())) return i+1;
    }
    return -1;
}


//COMPROVA SI ÉS MINIMAL(HA DE SER DOMINADOR)
int minimal1(vector<bool> ds, vector<int> nnds) {
    int inutil = -1;
    bool minimal = true;
    int i = 0;
    while (i < N && minimal) {
        if (ds[i]) {
            set<int>::iterator itr = neighborsS[i].begin(); //Tots els veïns han de ser minimals
            bool esUtil = false;
            while (itr != neighborsS[i].end() && not esUtil) {
                esUtil = nnds[*itr]-1 < float(neighborsS[*itr].size())/float(2); //Amb un menys no te infuencia positiva, per tant és necessari
                ++itr;
            }
            if (not esUtil) {
                minimal = false;
                inutil = i;
            }
        }
        ++i;
    }
    if (inutil != -1) return inutil+1;
    else return inutil;
}

int minimal2(unordered_set<int> ds, vector<int> nnds) {
    int inutil = -1;
    bool minimal = true;
    auto itr = ds.begin();
    while (itr != ds.end() && minimal) { //Només recorres els vectors dominants
        set<int>::iterator itr2 = neighborsS[*itr].begin(); //Tots els veïns han de ser minimals
        bool esUtil = false;
        while (itr2 != neighborsS[*itr].end() && not esUtil) {
            esUtil = nnds[*itr2]-1 < float(neighborsS[*itr2].size())/float(2); //Amb un menys no te infuencia positiva, per tant és necessari
            ++itr2;
        }
        if (not esUtil) {
            minimal = false;
            inutil = *itr;
        }
        ++itr;
    }
    if (minimal) return -1;
    else return inutil;
}
int minimal3(vector<bool> DV, vector<int> NND) { 
    for(int i = 0; i < DV.size(); i++) {
        if(not DV[i]) continue;
        int min = minNND(neighborsUS[i].size());
        int count = 0;
        unordered_set<int>::iterator itr = neighborsUS[i].begin();
        while(itr != neighborsUS[i].end() and count < min) {
            if(NND[*itr]-1 < min) count++;
            itr++;
        }
        if(count < min) return i+1;
    }
    return -1;
}

int minimal4(unordered_set<int> DUS, vector<int> NND) {
    unordered_set<int>::iterator itr1 = DUS.begin();
    while(itr1 != DUS.end()) {
        int min = minNND(neighborsUS[*itr1].size());
        int count = 0;
        unordered_set<int>::iterator itr2 = neighborsUS[*itr1].begin();
        while(itr2 != neighborsUS[*itr1].end() and count < min) {
            if(NND[*itr2]-1 < min) count++;
            itr2++;
        }
        if(count < min) return *itr1+1;
        itr1++;
    }
    return -1;
}

void print(vector<bool> DV) {
    for(int i = 0; i < N; i++) {
        if(DV[i]) cout << "(D)";
        cout << "\tNode " << i+1 << "\t";
        set<int>::iterator itr = neighborsS[i].begin();
        while(itr != neighborsS[i].end()){
            cout << *itr+1 << " ";
            itr++;
        }
        cout << endl;
    }
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

    indata >> N;
    indata >> M;
    neighborsS = vector< set<int> >(N);
    neighborsUS = vector< unordered_set<int> >(N);

    // VARIABLES PEL RECOBRIMENT
    int ND; //nombre de nodes de D
    vector<int> NND = vector<int>(N,0); //nombre de veins d'un node a D
    vector<bool> DV = vector<bool>(N, false); //nodes de D
    unordered_set<int> DUS; //nodes de D
    
    indata >> ND;

    for(int i = 0; i < ND; i++) {
        int aux;
        indata >> aux;
        DV[aux-1] = true;
        DUS.insert(aux-1);
    }

    int u, v;
    while(indata >> u >> v) {
        neighborsS[u - 1].insert(v - 1);
        neighborsS[v - 1].insert(u - 1);
        neighborsUS[u - 1].insert(v - 1);
        neighborsUS[v - 1].insert(u - 1);
        if(DV[v-1]) NND[u-1]++;
        if(DV[u-1]) NND[v-1]++;
    }
    indata.close();

    //print();

    int b1, b2;
    //VECTOR<SET<INT>> + VECTOR<BOOL>

    Timer timer;
    b1 = dominador2(NND); //-1 no ha trobat cap vertex que no tingui influencia positiva
    b2 = -1;
    if(b1 == -1) b2 = minimal1(DV,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant

    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;
    double ct1 = timer.elapsed_time(Timer::VIRTUAL);
    cout << "time " << ct1 << endl;

    //VECTOR<SET<INT>> + VECTOR<UNORDERED_SET<INT>>

    b1 = dominador2(NND); //-1 no ha trobat cap vertex que no tingui influencia positiva
    b2 = -1;
    if(b1 == -1) b2 = minimal2(DUS,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant

    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;
    double ct2 = timer.elapsed_time(Timer::VIRTUAL);
    cout << "time " << ct2-ct1 << endl;

    //VECTOR<UNORDERED_SET<INT>> + VECTOR<BOOL>

    b1 = dominador2(NND); //-1 no ha trobat cap vertex que no tingui influencia positiva
    b2 = -1;
    if(b1 == -1) b2 = minimal3(DV,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant

    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;
    double ct3 = timer.elapsed_time(Timer::VIRTUAL);
    cout << "time " << ct3-ct2 << endl;

    //VECTOR<UNORDERED_SET<INT>> + VECTOR<UNORDERED_SET<INT>>

    b1 = dominador2(NND); //-1 no ha trobat cap vertex que no tingui influencia positiva
    b2 = -1;
    if(b1 == -1) b2 = minimal4(DUS,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant

    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;
    double ct4 = timer.elapsed_time(Timer::VIRTUAL);
    cout << "time " << ct4-ct3 << endl;


    return 0;
}

