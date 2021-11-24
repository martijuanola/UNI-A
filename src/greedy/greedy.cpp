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
vector< unordered_set<int> > neighbors; //Dades inicials
vector<int> NND; //Nombre de nodes
vector<bool> D; //Vector de nodes dominants
vector<bool> s; //altGreedy
vector<unordered_set<int>> nodes;//altGreedy posicio del vector es valor del node, dins del set es els nodes que compleixen
int maxPos; //altGreedy indica quina es la posicio més alta del vector on hi ha vertexs
vector<int> pos; //altGreedy indica quina es la posicio al vector de nodes de cada vertex (no ordenat)

// string for keeping the name of the input file
string inputFile;

// dummy parameters as examples for creating command line parameters 
// see function read_parameters(...)
int algorisme = 0;

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
        else if (strcmp(argv[iarg],"-a")==0) 
            algorisme = atoi(argv[++iarg]); 
            
        iarg++;
    }
}

int minNND(int n) {
    return ceil(float(n)/2);
}

 /*------- DOMINADORS -------*/
int dominador2(vector<int> NND) {
    for(int i = 0; i < neighbors.size(); i++) {
        if(NND[i] < minNND(neighbors[i].size())) return i;
    }
    return -1;
}

bool dominador() {
    for(int i = 0; i < N; i++) {
        if(NND[i] < float(neighbors[i].size())/float(2)) return false;
    }
    return true;
}

 /*------- MINIMALS -------*/

int minimal3(vector<bool> DV, vector<int> NND) { 
    bool minimal = true;
    int i = 0;
    while (i < N && minimal) {
        if (DV[i]) {
            unordered_set<int>::iterator itr = neighbors[i].begin(); //Tots els veïns han de ser minimals
            bool esUtil = false;
            while (itr != neighbors[i].end() && not esUtil) {
                esUtil = NND[*itr]-1 < float(neighbors[*itr].size())/float(2); //Amb un menys no te infuencia positiva, per tant és necessari
                ++itr;
            }
            if (not esUtil) {
                minimal = false;
                return i;
            }
        }
        ++i;
    }
    return -1;
}

 /*------- ESCOLLIR VERTEXS -------*/

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

 /*------- ACTUALITZACIO DADES -------*/

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

 /*------- GREEDIES -------*/

void greedyNaive() {

    while (not dominador()) {
        int v = millorV();
        D[v] = true;
        afegeixVeins(v);
    }
}

void greedyMinimal() {
    int b2 = 0;
    while (b2 != -1) {
        b2 = minimal3(D,NND); //-1 no ha trobat cap vertex que li sobri un vertex del set dominant
        if (b2 != -1) {
            D[b2] = false;
            eliminaVeins(b2);
        }
    }
}

 /*------- MAIN -------*/


void g(int v) { //s'ha de recalcular g pel vertex v
    
}

void actualitzaDades(int v) { //v abans no formava part de D i ara si
    unordered_set<int>::iterator itr = neighbors[v].begin();
    s[v] = true;

    while (itr != neighbors[v].end()) {
        ++NND[*itr];
        if (NND[*itr] >= minNND(neighbors[*itr].size())) s[v] = true;
        ++itr;
    }

    vector<bool> calculat(N, false); //Comprova quins vertexs ja han siguit calculats o no s'han de calculat g()
    calculat[v] = true;

    itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {

        if (not D[*itr] and not calculat[*itr]) {

            g(*itr); //recalcular g pels veins
            calculat[*itr] = true;
            unordered_set<int>::iterator itr2 = neighbors[*itr].begin();

            while (itr2 != neighbors[*itr].end()) { //Recorrer el graf en 2 de profunditat
                if (not D[*itr2] && not calculat[*itr2]){
                    g(*itr2);
                    calculat[*itr2] = true;
                }
                ++itr2;
            }

        }
        ++itr;
    }
}

void altGreedy() {
    while (not dominador()) {
        auto it = nodes[maxPos].begin(); //Possible random 
        nodes[maxPos].erase(it);
        D[*it] = true; //l'afegim al set dominant
        actualitzaDades(*it);
    }
}

int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    std::cout << std::setprecision(10) << std::fixed;

    ifstream indata;
    indata.open(inputFile.c_str());
    if(not indata) { // file couldn't be opened
        cout << "Error: file could not be opened" << endl;
    }

    indata >> N;
    indata >> M;
    NND = vector<int> (N, 0); //Nombre de veins que pertanyen a D
    neighbors = vector< unordered_set<int> >(N);
    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();

    //Timer timer;

    if (algorisme == 0) {
        D = vector<bool>(N, false); //Comença amb cap node a la solucio
        greedyNaive();
    }
    else if (algorisme == 1) {
        D = vector<bool>(N, true); //Comença amb tots els nodes a la solucio
        greedyMinimal();
    }
    else if (algorisme == 2) {
        int maxSize = 0;
        for (int i = 0; i < N; ++i) {
            if (neighbors[i].size() > maxSize) maxSize = neighbors[i].size();
        }
        ++maxSize;
        nodes = vector<unordered_set<int>>(maxSize);
        cout << "tamany nodes: " << nodes.size() << endl;
        maxPos = 0;

        D = vector<bool>(N, false); //Comença amb cap node a la solucio
        s = vector<bool>(N, false); //Tots començen a false (0)
        pos = vector<int>(N, 0); //Tots els nodes estan a la posicio 0 del vector de nodes

        for (int i = 0; i < N; ++i) {
            nodes[0].insert(i);
        }
        altGreedy();
    }



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

