/***************************************************************************
    cplex.cpp 
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

#include "Timer.h"
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

// the following "include" is necessary for the correct working of CPLEX.
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

// Data structures for the problem data
int n_of_nodes;
int n_of_arcs;
vector< set<int> > neighbors;

// string for keeping the name of the input file
string inputFile;

// time limit for CPLEX 
// (can be supplied to the algorithm via the -t comand line parameter)
double time_limit = 3200.0;


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
        else if (strcmp(argv[iarg],"-t")==0) time_limit = atof(argv[++iarg]);
        iarg++;
    }
}

ILOSOLVECALLBACK4(loggingCallback,
                    Timer&, timer,
                    double&, time_stamp,
                    double&, result,
                    double&, gap) {
                    
    if (hasIncumbent()) {
        IloNum nv = getIncumbentObjValue();
        double newTime = timer.elapsed_time(Timer::VIRTUAL);
        double newGap = 100.0*getMIPRelativeGap();
        if (result > double(nv)) {
            cout << "value " << nv;
            cout << "\ttime " << newTime;
            cout << "\tgap " << newGap << endl;
            result = double(nv);
            time_stamp = newTime;
            gap = newGap;
        }
    }
}


void run_cplex(Timer& timer) {

    // variables for storing the result, the computation time, and 
    // the optimality gap obtained by CPLEX for the provided input file 
    // (within the allowed computation time)
    double result = std::numeric_limits<int>::max();
    double time_stamp = 0.0;
    double gap = 0.0;

    IloEnv env;
    // this instruction redirects the standard output of CPLEX 
    // to the null stream in order to avoid printing it to the screen
    env.setOut(env.getNullStream());
    try{

        IloModel model(env);

        // HERE GOES YOUR ILP MODEL FOR THE MPIDS PROBLEM
    
        IloCplex cpl(model);

        cpl.setParam(IloCplex::TiLim, time_limit);
        cpl.setParam(IloCplex::EpGap, 0.0);
        cpl.setParam(IloCplex::EpAGap, 0.0);
        cpl.setParam(IloCplex::Threads, 1);
        cpl.setWarning(env.getNullStream());
        cpl.use(loggingCallback(env, timer, time_stamp, result, gap));

        cpl.solve();
    
        if (cpl.getStatus() == IloAlgorithm::Optimal or 
            cpl.getStatus() == IloAlgorithm::Feasible) 
        {
            double newTime = timer.elapsed_time(Timer::VIRTUAL);
            double lastVal = double(cpl.getObjValue());
            double lastGap = 100.0*cpl.getMIPRelativeGap();
            if (lastGap < 0.0) lastGap *= -1.0;
            cout << "value " << lastVal;
            cout << "\ttime " << newTime;
            cout << "\tgap " << lastGap << endl;
            if (cpl.getStatus() == IloAlgorithm::Optimal) {
                cout << "optimality proven" << endl;
            }
        }
        env.end();
    }
    catch(IloException& e) {
        cerr  << " ERROR: " << e << endl;
    }
    env.end();
}


// Main function

int main( int argc, char **argv ) {

    read_parameters(argc,argv);
    
    // setting the output format for doubles to 2 decimals after the comma
    std::cout << std::setprecision(2) << std::fixed;

    // opening the corresponding input file and reading the problem data
    ifstream indata;
    indata.open(inputFile.c_str());
    if (not indata) { // file couldn't be opened
        cout << "Error: file could not be opened" << endl;
    }

    indata >> n_of_nodes;
    indata >> n_of_arcs;
    neighbors = vector< set<int> >(n_of_nodes);
    int u, v;
    while (indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();

    // the computation time starts now
    Timer timer;

    // Example for requesting the elapsed computation time at any moment: 
    // double ct = timer.elapsed_time(Timer::VIRTUAL);

    run_cplex(timer);

}

