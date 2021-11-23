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


//FUNCIONS:

//COMPROVAR SI ÉS DOMINADOR
bool dominador(vector<set<int>> llista, vector<bool> ds, int M) {

    bool b = true;

    int i = 0;
    while(b and i < llista.size()) {
        //ITERAR SOBRE TOTS ELS NODES

        int n = llista[i].size();
        int count = 0;
        set<int>::iterator itr = llista[i].begin();
        while(count < float(n)/2 and itr != llista[i].end() and b) { //mirar que funcioni bé la divisio per float
            if(ds[*itr]) count++;
            itr++;
        }
        if(count < float(n)/2) {
            b = false; //mirar que funcioni bé la divisio per float
        }
        i++;
    }

    return b;
}

int dominador(vector<set<int>> llista, vector<bool> ds, vector<int> nnds, int M) {

    bool b = true;
    int falta = -1;
    int i = 0;
    while(i < llista.size() and b) {
        if(nnds[i] < float(llista[i].size())/2) {
            b = false;
            falta = i;
        }
        i++;
    }

    if (falta != -1) return falta+1;
    return falta;
}

//COMPROVA SI ÉS MINIMAL(HA DE SER DOMINADOR)
int minimal2(vector<set<int>> llista, vector<bool> ds, vector<int> nnds, int M) {
    int inutil = -1;
    bool minimal = true;
    int i = 0;
    while (i < llista.size() && minimal) {
        if (ds[i]) {
            set<int>::iterator itr = llista[i].begin(); //Tots els veïns han de ser minimals
            bool esUtil = false;
            while (itr != llista[i].end() && not esUtil) {
                esUtil = nnds[*itr]-1 < float(llista[*itr].size())/float(2); //Amb un menys no te infuencia positiva, per tant és necessari
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


    //VECTOR<SET<INT>> + VECTOR<BOOL>

    Timer timer;
    int b1 = dominador(neighborsS,DV,NND,M); //-1 no ha trobat cap vertex que no tingui influencia positiva
    int b2 = -1;
    if(b1 == -1) b2 = minimal2(neighborsS,DV,NND,M); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant

    if(b1 == -1) {
        cout << "It's a Positive Influence Dominator Set" << endl;
        if(b2 == -1) cout << "It's also MINIMAL" << endl;
        else cout << "It's NOT minimal! Redundant vertex: " << b2 << endl;
    }
    else cout << "It's NOT a Positive Influence Dominator Set!!!! " << b1 << " does not fulfill the requirements." << endl;
    double ct = timer.elapsed_time(Timer::VIRTUAL);
    cout << "time " << ct << endl;

    //VECTOR<SET<INT>> + VECTOR<UNORDERED_SET<INT>>


    //VECTOR<UNORDERED_SET<INT>> + VECTOR<BOOL>


    //VECTOR<UNORDERED_SET<INT>> + VECTOR<UNORDERED_SET<INT>>


    return 0;
}

