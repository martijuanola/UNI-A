/***************************************************************************
    metaheuristic.cpp
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

// string for keeping the name of the input file
string inputFile;

// computing time limit for each application of the metaheuristic
double time_limit = 3200.0;

// number of applications of the metaheuristic
int n_apps = 1;

// dummy parameters as examples for creating command line parameters
// (see function read_parameters(...))
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
        if (strcmp(argv[iarg],"-i") == 0) inputFile = argv[++iarg];
        // reading the computation time limit
        // from the command line (if provided)
        else if (strcmp(argv[iarg],"-t") == 0) time_limit = atoi(argv[++iarg]);
        // reading the number of applications of the metaheuristic
        // from the command line (if provided)
        else if (strcmp(argv[iarg],"-n_apps") == 0) n_apps = atoi(argv[++iarg]);
        // example for creating a command line parameter
        // param1 -> integer value is stored in dummy_integer_parameter
        else if (strcmp(argv[iarg],"-param1") == 0) {
            dummy_integer_parameter = atoi(argv[++iarg]);
        }
        // example for creating a command line parameter
        // param2 -> double value is stored in dummy_double_parameter
        else if (strcmp(argv[iarg],"-param2") == 0) {
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

    // initializing the random number generator. A random number
    // between 0 and 1 is obtained with: double rnum = rnd->next();
    rnd = new Random((unsigned) time(&t));
    rnd->next();

    // vectors for storing the result and the computation time
    // obtained by the <n_apps> applications of the metaheuristic
    vector<double> results(n_apps, std::numeric_limits<int>::max());
    vector<double> times(n_apps, 0.0);

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

    //GLOBAL VARIABLES AND TYPES FOR THE METAHEURISTIC
    typedef vector<bool> Gene;
    typedef vector<Gene> Population;
    typedef vector<int>  Pop_Fitness;

    int POP_SIZE = 100;
    float CROSS_PROB = 0.8;
    float MUTATE_PROB = 0.2;
    float GREEDY_RAND = 0.5;
    int MAX_GEN = 100;

    // main loop over all applications of the metaheuristic
    for (int na = 0; na < n_apps; ++na) {

        // the computation time starts now
        Timer timer;

        // Example for requesting the elapsed computation time at any moment:
        // double ct = timer.elapsed_time(Timer::VIRTUAL);

        cout << "start application " << na + 1 << endl;

        // HERE GOES YOUR METAHEURISTIC

        // For implementing the metaheuristic you probably want to take profit
        // from the greedy heuristic and/or the local search method that you
        // already developed.
        //
        // Whenever the best found solution is improved, first take the
        // computation time as explained above. Say you store it in variable ct.
        // Then, write the following to the screen:
        // cout << "value " << <value of the new best found solution>;
        // cout << "\ttime " << ct << endl;
        //
        // Store the value of the new best found solution in vector results:
        // results[na] = <value of the new best found solution>;
        //
        // And store the current computation time (that is, the time measured
        // at that moment and stored in variable "ct") in vector times:
        // times[na] = ct;
        //
        // Stop the execution of the metaheuristic
        // once the time limit "time_limit" is reached.
        int FITNESS_TOTAL = 0;
        int FITNESS_ANT = 1;

        Population pop(POP_SIZE, Gene(n_of_nodes));
        Pop_Fitness pop_fitness(POP_SIZE, -1);
        Gene best(n_of_nodes);
        int best_fitness = 0;

        generate_pop_ini(pop&, pop_fitness&, best&, best_fitness&);

        for (int generation = 0;
          //we iterate up to the maximum generation or...
          generation < MAX_GEN and
          //until our total fitness does not improve or...
          FITNESS_TOTAL < FITNESS_ANT and
          //until we reach the time limit.
          timer.elapsed_time(Timer::VIRTUAL) < time_limit;
            ++generation) {

              FITNESS_TOTAL = 0;

              Population pop_new(POP_SIZE, Gene(n_of_nodes));
              Pop_Fitness pop_fitness_new(POP_SIZE, -1);
              for (int child_idx = 0; child_idx < POP_SIZE and timer.elapsed_time(Timer::VIRTUAL);
              ++child_idx) {

                Gene x = selection(pop&);
                Gene y = selection(pop&);

                Gene child = cross(x,y);
                //if MUTATE_PROB mutate child;

                pop_new[child_idx] = child;

                int child_fitness = fitness(const child&);
                pop_fitness_new[child_idx] = child_fitness;
                FITNESS_TOTAL += child_fitness;
                }
            }
            double ct = timer.elapsed_time(Timer::VIRTUAL);

            int MDPI_size = 0;
            for (int i = 0; i < n_of_nodes; ++i) MDPI_size += best[i];

            cout << "value " << MDPI_size;
            cout << "\ttime " << ct << endl;

            results[na] = MDPI_size;
            times[na] = ct;

        cout << "end application " << na + 1 << endl;
    }

    // calculating the average of the results and computation times,
    // and their standard deviations, and write them to the screen
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
    // printing statistical information
    cout << r_best << "\t" << r_mean << "\t" << rsd << "\t";
    cout << t_mean << "\t" << tsd << endl;
}
