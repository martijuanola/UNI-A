/***************************************************************************
    local_search.cpp
    (C) 2021 by C.Blum & M.Blesa
    
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

#include "Timer.h"
#include "Random.h"
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

// string for keeping the name of the input file
string inputFile;

// number of applications of local search
int n_apps = 1;

// dummy parameters as examples for creating command line parameters -> 
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
        
        // reading the number of applications of local search 
        // from the command line (if provided)
        else if (strcmp(argv[iarg],"-n_apps")==0) n_apps = atoi(argv[++iarg]); 
        
        // example for creating a command line parameter param1 -> 
        // integer value is stored in dummy_integer_parameter
        else if (strcmp(argv[iarg],"-param1")==0) {
            dummy_integer_parameter = atoi(argv[++iarg]); 
        }
        // example for creating a command line parameter param2 -> 
        // double value is stored in dummy_double_parameter
        else if (strcmp(argv[iarg],"-param2")==0) {
            dummy_double_parameter = atof(argv[++iarg]);  
        }
        iarg++;
    }
}


/**********
Main function
**********/

int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    
    // setting the output format for doubles to 2 decimals after the comma
    std::cout << std::setprecision(2) << std::fixed;

    // initializing the random number generator. 
    // A random number in (0,1) is obtained with: double rnum = rnd->next();
    rnd = new Random((unsigned) time(&t));
    rnd->next();

    // vectors for storing the result and the computation time 
    // obtained by the <n_apps> applications of local search
    vector<double> results(n_apps, std::numeric_limits<int>::max());
    vector<double> times(n_apps, 0.0);

    // opening the corresponding input file and reading the problem data
    ifstream indata;
    indata.open(inputFile.c_str());
    if(not indata) { // file couldn't be opened
        cout << "Error: file could not be opened" << endl;
    }

    indata >> n_of_nodes;
    indata >> n_of_arcs;
    neighbors = vector< set<int> >(n_of_nodes);
    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();

    // main loop over all applications of local search
    for (int na = 0; na < n_apps; ++na) {

        // the computation time starts now
        Timer timer;

        // Example for requesting the elapsed computation time at any moment: 
        // double ct = timer.elapsed_time(Timer::VIRTUAL);

        cout << "start application " << na + 1 << endl;

        // HERE GOES YOUR LOCAL SEARCH METHOD

        // The starting solution for local search may be randomly generated, 
        // or you may incorporate your greedy heuristic in order to produce 
        // the starting solution.
        
        // Whenever you move to a new solution, first take the computation 
        // time as explained above. Say you store it in variable ct.
        // Then, write the following to the screen: 
        // cout << "value " << <value of the current solution>;
        // cout << "\ttime " << ct << endl;

        // When a local minimum is reached, store the value of the 
        // corresponding solution in vector results: 
        // results[na] = <value of the local minimum>;
        
        // Finally store the needed computation time (that is, the time 
        // measured once the local minimum is reached) in vector times: 
        // times[na] = ct;

        cout << "end application " << na + 1 << endl;
    }

    // calculating the average of the results and computation times, and 
    // their standard deviations, and write them to the screen
    double r_mean = 0.0;
    int r_best = std::numeric_limits<int>::max();
    double t_mean = 0.0;
    for (int i = 0; i < int(results.size()); i++) {
        r_mean = r_mean + results[i];
        if (int(results[i]) < r_best) r_best = int(results[i]);
        t_mean = t_mean + times[i];
    }
    r_mean = r_mean/double(results.size());
    t_mean = t_mean/double(times.size());
    double rsd = 0.0;
    double tsd = 0.0;
    for (int i = 0; i < int(results.size()); i++) {
        rsd = rsd + pow(results[i]-r_mean,2.0);
        tsd = tsd + pow(times[i]-t_mean,2.0);
    }
    rsd = rsd/double(results.size());
    if (rsd > 0.0) {
        rsd = sqrt(rsd);
    }
    tsd = tsd/double(results.size());
    if (tsd > 0.0) {
        tsd = sqrt(tsd);
    }
    cout << r_best << "\t" << r_mean << "\t" << rsd << "\t";
    cout << t_mean << "\t" << tsd << endl;
}

