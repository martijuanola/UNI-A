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
#include <unordered_set>
#include <limits>
#include <iomanip>

// global variables concerning the random number generator (in case needed)
time_t t;
Random* rnd;

// Data structures for the problem data
int N;
int M;
vector< unordered_set<int> > neighbors;
//global variables for greedy
vector<int> NND; //Nombre de nodes
vector<bool> D; //Vector de nodes dominants
vector<bool> s; //altGreedy true si es un vertex amb influencia positiva
vector<unordered_set<int>> nodes;//altGreedy posicio del vector es valor del node = suma dels s dels veins, dins del set es els nodes que compleixen
int maxPos; //altGreedy indica quina es la posicio més alta del vector on hi ha vertexs
vector<int> pos; //altGreedy indica quina es la posicio al vector de nodes de cada vertex (no ordenat)
vector<bool> calculat; //altGreedy per indicar quins vertexs ja s'han calculat


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

//GLOBAL VARIABLES AND TYPES FOR THE METAHEURISTIC
typedef vector<bool> Gene;
typedef vector<Gene> Population;
typedef vector<int>  Pop_Fitness;

int POP_SIZE = 10;
double CROSS_PROB = 0.8;
double MUTATE_PROB = 0.15;
int MAX_GEN = 500;
int profunditat = 6;

Gene best_greedy;

//funcions plantilla
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

//GREEDY FUNCTIONS FOR GENERATING INITIAL SOLUTIONS. (Jaume made them)
int minNND(int n) {
    return ceil(float(n)/2);
}

bool dominador() {
    for(int i = 0; i < N; i++) {
        if(NND[i] < float(neighbors[i].size())/float(2)) return false;
    }
    return true;
}

int minimal3(vector<bool> DV, vector<int> NND) {
    for(int i = 0; i < DV.size(); i++) {
        if(DV[i]) {
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

void g(int v) {
    int res = 0; //passa de bool a int

    auto itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {
        res += 1 - s[*itr];
        ++itr;
    }
    if (pos[v] != -1 and res != pos[v]) {
        if (res > maxPos) maxPos = res;
        //El canviem de llista
        nodes[pos[v]].erase(v);
        nodes[res].insert(v);
        pos[v] = res;
    }
}

void recalculG(int v, int prof) {
    if (prof == 0) return;
    unordered_set<int>::iterator itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {
        if (not calculat[*itr]) {
            g(*itr); //recalcular g pels veins
            calculat[*itr] = true;
            recalculG(*itr, prof-1);
        }
        ++itr;
    }
}

void actualitzaDades(int v) { //v abans no formava part de D i ara si

    s[v] = (NND[v] >= minNND(neighbors[v].size()));

    unordered_set<int>::iterator itr = neighbors[v].begin();
    while (itr != neighbors[v].end()) {
        ++NND[*itr];
        s[*itr] = (NND[*itr] >= minNND(neighbors[*itr].size())); //Si arriba a ser true mai tornara a ser false
        ++itr;
    }

    calculat = vector<bool> (N, false); //Comprova quins vertexs ja han siguit calculats o no s'han de calculat g()
    calculat[v] = true;
    recalculG(v, profunditat);

    if (nodes[maxPos].empty()) {
        bool found = false;
        int i = maxPos;
        while (not found) {
            if (not nodes[i].empty()) {
                maxPos = i;
                found = true;
            }
            --i;
        }
    }
}

void greedyRandom() { //Igual que altGreedy pero amb aleatorietat
    srand(time(NULL));
    int maxSize = 0;
    for (int i = 0; i < N; ++i) {
        if (neighbors[i].size() > maxSize) maxSize = neighbors[i].size();
    }
    maxSize += 2; //+1 per que el node també conta 1 i +1 pel 0
    nodes = vector<unordered_set<int>>(maxSize);

    D = vector<bool>(N, false); //Comença amb cap node a la solucio
    s = vector<bool>(N, false); //Tots començen a false (0)
    pos = vector<int>(N, 0);
    NND = vector<int>(N, 0);

    maxPos = 0;
    for (int i = 0; i < N; ++i) {
        int p = neighbors[i].size();
        if (p > maxPos) maxPos = p;
        nodes[p].insert(i);

        pos[i] = p;
    }


    while (not dominador()) {
        auto it = nodes[maxPos].begin();
        int rando = rand()%nodes[maxPos].size();

        while (rando > 0) {
            ++it;
            --rando;
        }

        int v = *it;
        nodes[maxPos].erase(it);

        pos[v] = -1;
        D[v] = true; //l'afegim al set dominant
        actualitzaDades(v);
    }
}

//METAHEURISTIC FUNCTIONS
int fitness(const Gene & gen) {
    int fitness = 0;

    for (int vertex = 0; vertex < N; ++vertex) {
        unordered_set<int> adjacents = neighbors[vertex];

        int grau_half = adjacents.size();
        if(grau_half%2 == 1) ++grau_half;
        grau_half = grau_half/2;

        int adj_marked = 0;
        unordered_set<int>::iterator it = adjacents.begin();
        while (it != adjacents.end()) {
            if(gen[(*it)] == 1) ++adj_marked;
            ++it;
        }

        int cost_vertex = adj_marked - grau_half;
        //doesnt reach 50% and we must penalize it
        if (cost_vertex < 0) fitness += cost_vertex*cost_vertex+500;
        else fitness += cost_vertex;
    }

    return fitness;
}

int random(int min, int max) {
   static bool first = true;
   if (first)
   {
      srand( time(NULL) );
      first = false;
   }
   return min + rand() % (( max + 1 ) - min);
}

void generate_pop_ini(Population& pop, Pop_Fitness& pop_fitness, Gene& best, int& best_fitness, Timer& timer) {
    for (int i = 0; i < POP_SIZE and timer.elapsed_time(Timer::VIRTUAL) < time_limit-10; ++i) {
        greedyRandom();
        /*
        D = vector<bool>(N, false);
        for (int j = 0; j < N; ++j) {
          D[j] = random(0,1);
        }*/

        /*
        int  idx_remove = minimal3(D, NND);
        while(idx_remove != -1 and timer.elapsed_time(Timer::VIRTUAL) < time_limit) {
          D[idx_remove] = 0;

          NND = vector<int>(N, 0);
          for (int i = 0; i < N; ++i) {
              auto it = neighbors[i].begin();
              while (it != neighbors[i].end()) {
                  if (D[*it]) ++NND[i];
                  ++it;
              }
          }
          //cout << "Removed vertex "<< idx_remove << endl;
          idx_remove = minimal3(D, NND);
        }
        */
        pop[i] = D;

        int fitness_current = fitness(D);
        pop_fitness[i] = fitness_current;

        if (fitness_current < best_fitness) {best = D; best_fitness = fitness_current;}
        cout << "Generated initial solution: " << i+1 << "/" << POP_SIZE;
        cout << ". Fitness: " << fitness_current;
        cout << ". Time: " << (int) timer.elapsed_time(Timer::VIRTUAL) << "/" << time_limit << endl;
    }

    best_greedy = best;
    cout << "Generated initial solutions"  << endl;

    int MDPI_size = 0;
    for (int i = 0; i < N; ++i) MDPI_size += best[i];
    cout << "Best Solution value: " << MDPI_size << endl;
}

Gene cross(const Gene & x, const Gene & y) {
    Gene aux(N);
    int threshold = random(0, N-1);

    for (int i = 0; i < N; ++i) {
        if (i < threshold) aux[i] = x[i];
        else aux[i] = y[i];
    }

    return aux;
}

void mutate(Gene & child) {
    int mutation = random(0, N-1);
    child[mutation] = not child[mutation];
}

int selection(const Pop_Fitness & pop_fitness) {
    int red = random(0, POP_SIZE-1);
    int blue = random(0, POP_SIZE-1);

    if(pop_fitness[red] < pop_fitness[blue]) return red;
    else return blue;
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

    indata >> N;
    indata >> M;
    neighbors = vector< unordered_set<int> >(N);
    int u, v;
    while (indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();

    // main loop over all applications of the metaheuristic
    cout << "METAHEURISTIC" << endl;

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
        int FITNESS_TOTAL = std::numeric_limits<int>::max()-1;
        int FITNESS_ANT = std::numeric_limits<int>::max();

        Population pop(POP_SIZE, Gene(N));
        Pop_Fitness pop_fitness(POP_SIZE, -1);
        Gene best(N);
        int best_fitness = std::numeric_limits<int>::max();

        generate_pop_ini(pop, pop_fitness, best, best_fitness, timer);

        for (int generation = 0;
          //we iterate up to the maximum generation or...
          generation < MAX_GEN and
          //until our total fitness does not improve or...
          FITNESS_TOTAL != FITNESS_ANT and
          //until we reach the time limit.
          timer.elapsed_time(Timer::VIRTUAL) < time_limit-10;
          ++generation) {

              FITNESS_ANT = FITNESS_TOTAL;
              FITNESS_TOTAL = 0;

              Population pop_new(POP_SIZE, Gene(N));
              Pop_Fitness pop_fitness_new(POP_SIZE, -1);
              for (int child_idx = 0; child_idx < POP_SIZE and timer.elapsed_time(Timer::VIRTUAL) < time_limit-10;
              ++child_idx) {

                Gene x = pop[selection(pop_fitness)];
                Gene y = pop[selection(pop_fitness)];

                Gene child = x;
                if (rnd->next() <= CROSS_PROB) child = cross(x,y);
                if (rnd->next() <= MUTATE_PROB) mutate(child);

                pop_new[child_idx] = child;

                int child_fitness = fitness(child);
                pop_fitness_new[child_idx] = child_fitness;
                FITNESS_TOTAL += child_fitness;

                if(best_fitness >= child_fitness) {
                  best_fitness = child_fitness;
                  best = child;
                  }

                }

                cout << "Generation: " << generation+1 << "/" << MAX_GEN <<". Fitness Best: " << best_fitness;
                cout << ". Fitness Total: " << FITNESS_TOTAL << ". Time: " << (int) timer.elapsed_time(Timer::VIRTUAL) <<"/"<<time_limit<<endl;

                pop = pop_new;
                pop_fitness = pop_fitness_new;
            }

            D = best;
            NND = vector<int>(N, 0);
            for (int i = 0; i < N; ++i) {
                auto it = neighbors[i].begin();
                while (it != neighbors[i].end()) {
                    if (D[*it]) ++NND[i];
                    ++it;
                }
            }
            if (dominador()) {
                cout << "The best solution is a Dominating Set" << endl;

                int  idx_remove = minimal3(best, NND);
                while(idx_remove != -1 and timer.elapsed_time(Timer::VIRTUAL) < time_limit) {
                  best[idx_remove] = 0;

                  D = best;
                  NND = vector<int>(N, 0);
                  for (int i = 0; i < N; ++i) {
                      auto it = neighbors[i].begin();
                      while (it != neighbors[i].end()) {
                          if (D[*it]) ++NND[i];
                          ++it;
                      }
                  }
                  cout << "Removed vertex "<< idx_remove << endl;
                  idx_remove = minimal3(best, NND);
                }
                cout << "The best solution is a Minimal Dominating Set" << endl;

            }
            else {cout << "The best solution is NOT a Dominating Set. Welp. Recovering greedy best" << endl; best = best_greedy;}

        double ct = timer.elapsed_time(Timer::VIRTUAL);

        int MDPI_size = 0;
        for (int i = 0; i < N; ++i) MDPI_size += best[i];

        cout << "value " << MDPI_size;
        cout << "\ttime " << ct << endl;

        results[na] = MDPI_size;
        times[na] = ct;

        cout << "end application " << na + 1 << endl;
        cout << endl;
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
