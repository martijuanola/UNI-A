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

// Data structures for the problem data
int N;
int M;
vector<unordered_set<int>> neighbors;

// string for keeping the name of the input file
string inputFile;

//Global parameters
//execution
int n_apps = 1;
int print = 1;

//Heuristic, ops and initial solution
int H = 0;
int O = 0; // 0-REMOVE | 1-REMOVE+ADD | 2-REMOVE+SWAP | 3-REMOVE+ADD+SWAP
int SI = 0;

//SA parameters
int T = 20000;
int iT = 15;
int k = 6;
float l = 0.04;

inline int stoi(string &s) {

  return atoi(s.c_str());
}

inline double stof(string &s) {

  return atof(s.c_str());
}

void read_parameters(int argc, char **argv) {

    int iarg = 1;

    while (iarg < argc) {
        if (strcmp(argv[iarg],"-i")==0) inputFile = argv[++iarg];               //input file
        else if (strcmp(argv[iarg],"-n")==0) n_apps = atoi(argv[++iarg]);       //iterations of algorithm
        else if (strcmp(argv[iarg],"-p")==0) print = 0;                             //elimina prints

        else if (strcmp(argv[iarg],"-h")==0) H = atoi(argv[++iarg]);            //heuristic function
        else if (strcmp(argv[iarg],"-o")==0) O = atoi(argv[++iarg]);            //operators
        else if (strcmp(argv[iarg],"-si")==0) SI = atoi(argv[++iarg]);          //initial solution/state

        else if (strcmp(argv[iarg],"-T")==0) T = atoi(argv[++iarg]);            //Temperatura
        else if (strcmp(argv[iarg],"-iT")==0) iT = atoi(argv[++iarg]);          //Iteracions per temperatura
        else if (strcmp(argv[iarg],"-k")==0) k = atoi(argv[++iarg]);            //K
        else if (strcmp(argv[iarg],"-l")==0) l = atof(argv[++iarg]);            //lambda
        iarg++;
    }
}

void printD(const vector<bool>& v) {
    for(int i = 0; i < v.size(); i++) if(v[i]) {
        if(v[i]) cout << i << "\t";
        else cout << "-\t";
    }
    cout << endl;
}

int minNND(int n) {
    return ceil(float(n)/2);
}

//POST: retorna el nombre d'elements de D
int ND(const vector<bool>& v) {
    int count = 0;
    for(int i = 0; i < v.size(); i++) if(v[i]) count++;
    return count;
}

int dominador(const vector<int>& NND) {
    for(int i = 0; i < neighbors.size(); i++) {
        if(NND[i] < minNND(neighbors[i].size())) return i;
    }
    return -1;
}

int minimal3(const vector<bool>& DV, const vector<int>& NND) { 
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

//FUNCIONS HILL CLIMBING

//SOLUCIO INICIAL

//PRE: SI indica quina solució inicial utilitzar
//POST: tots els nodes pertanyen a D i per tant tots els veins d'un node també
vector<bool> solucio_inicial(vector<int>& NND) {
    vector<bool> s(N,false);
    NND = vector<int> (N,0);
    
    //aleatori totalment
    if(SI == 1) {
        while(dominador(NND) != -1) {
            float n = rnd->next();
            int pos = int(N*n);
            while(s[pos]) pos = (pos+1)%N;

            s[pos] = true;
            for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) NND[*itr]++;
        }
    }

    //
    //elseif

    //tot ple
    else {
        s = vector<bool> (N,true);
        for(int i = 0; i < N; i++) NND[i] = neighbors[i].size();
        return s;
    }
    
    return s;
}

//HEURISTICA

//PRE: H indica l'heurístic que es farà servir
//POST: retorna el valor de l'heurístic H
double heruistic(const vector<bool>& v, const vector<int>& NND) {
    //maximitza nodes a ND
    if(H == 1) { 
        if(dominador(NND) != -1) return 0.0;
        else {
            return double(N - ND(v));
        }
    }

    //maximitzar sumatori nombre arestes dels vertex de D
    else if(H == 2) {
        if(dominador(NND) != -1) return 0.0;
        else { //diferència del minim i l'actual NND
            double sum = 0, count = 0;
            for(int i = 0; i < N; i++){ 
                if(v[i]) {
                    sum += neighbors[i].size();
                    count++;
                }
            }
            return sum/count;
        }
    }
    
    //minimitza diferència entre minNND i NND
    else if(H == 3) {
        if(dominador(NND) != -1) return 0.0;
        else { //diferència del minim i l'actual NND
            double sum = 0;
            for(int i = 0; i < N; i++) sum += neighbors[i].size() - (NND[i] - minNND(neighbors[i].size()));
            return sum;
        }
    }

    //minimitza diferència entre minNND i NND per nodes de ND
    else if(H == 4) {
        if(dominador(NND) != -1) return 0.0;
        else { //diferència del minim i l'actual NND
            double sum = 0;
            for(int i = 0; i < N; i++) {
                if(not v[i]) sum += neighbors[i].size() - (NND[i] - minNND(neighbors[i].size()));
            }
            return sum;
        }
    }

    //minimitza diferència entre minNND i NND però no descarta
    else if(H == 5) {
        double sum = 0;
        for(int i = 0; i < N; i++) {
            double aux = NND[i] - minNND(neighbors[i].size());
            if(aux < 0) aux *= aux;
            sum += - aux;
        }
        return sum + 2*M;
    }

    //que pugui sortir de l'espai i tornar prioritzant adds(ND)
    else return 0.0;
}


//OPERADORS

//PRE: l'element pos no ha d'estar al set D
//POST: s'ha afegit l'element pos a D i s'ha actualitzat NND
void opADD(vector<bool>& v, vector<int>& NND, int pos) {
    if(ND(v) == N) {
        cout << "ADD + D ple!!!" << endl;
        return;
    }

    if(v[pos]) {
        cout << "ERROR(opADD): l'element " << pos+1 << " ja està a D, no es pot tornar a afegir." << endl;
        return;
    }

    v[pos] = true;
    for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) {
        NND[*itr]++;
    }
}

//PRE: l'element pos ha d'estar a D
//POST: s'ha eliminat l'element pos de D i s'ha actualitzat NND
void opREMOVE(vector<bool>& v, vector<int>& NND, int pos) {
    if(ND(v) == 0) {
        cout << "REMOVE + D buit!!!" << endl;
        return;
    }
    if(not v[pos]) {
        cout << "ERROR(opREMOVE): l'element " << pos+1 << " no està a D, no es pot tornar a eliminar." << endl;
        return;
    }
    v[pos] = false;
    for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) {
        NND[*itr]--;
    }
}

//PRE: l'element posND no ha d'estar a D i l'element posD sí que ho ha d'estar
//POST: posND ara està a D i posD no. NND s'ha acutalitzat.
void opSWAP(vector<bool>& v, vector<int>& NND, int posND, int posD) {
    if(ND(v) == N) {
        cout << "SWAP + D ple!!!" << endl;
        return;
    }
    else if(ND(v) == 0) {
        cout << "SWAP + D buit!!!" << endl;
        return;
    }

    if(v[posND] or not v[posD]) {
        if(v[posND]) cout << "ERROR(opSWAP): l'element " << posND+1 << " ja està a D, no es pot tornar a afegir." << endl;
        if(not v[posD]) cout << "ERROR(opSWAP): l'element " << posD+1 << " no està a D, no es pot tornar a eliminar." << endl;
        return;
    }

    v[posND] = true;
    v[posD] = false;
    for(auto itr = neighbors[posND].begin(); itr != neighbors[posND].end(); itr++) {
        NND[*itr]++;
    }
    for(auto itr = neighbors[posD].begin(); itr != neighbors[posD].end(); itr++) {
        NND[*itr]--;
    }
}

vector<bool> simulatedAnnealing() {

    int min = N+1;

    vector<string> ops(0);
    vector<int> NND;
    vector<bool> s = solucio_inicial(NND);
    double h = 0;

    int iter = 0;
    while(iter < T) {
        //cout << "Temperatura " << T << endl;

        for(int si = 0; si < iT; si++) {
            //cout << "SubIteration " << si << endl;
            //nou fill
            float r1 = rnd->next(); // per l'operació
            float r2 = rnd->next(); //element de D
            float r3 = rnd->next(); //element de ND
            int r2i = int(r2*N);
            int r3i = int(r3*N);
            while(not s[r2i] and ND(s) != N) { r2i = (r2i+1)%N;}
            while(s[r3i] and ND(s) != N) { r3i = (r3i+1)%N;}

            vector<bool> auxS = s;
            vector<int> auxNND = NND;
            string str;
            double auxH = 0;

            float nops = 3.0;
            if(O == 1 or O == 2) nops = 2.0;
            else if(O == 0) nops = 1.0;

            if(r1 <= 1.0/nops) {
                opREMOVE(auxS,auxNND,r2i);
                auxH = heruistic(auxS,auxNND);
                str = "H" + to_string(auxH) + "\tREMOVE\t ND=" + to_string(ND(auxS)) + "\tElement " + to_string(r2i);
            }
            else if(r1 <= 2.0/nops and O != 2) {
                opADD(auxS,auxNND,r3i);
                auxH = heruistic(auxS,auxNND);
                str = "H" + to_string(auxH) + "\tADD\t ND=" + to_string(ND(auxS)) + "\tElement " + to_string(r3i);
            }
            else {
                opSWAP(auxS,auxNND,r3i,r2i);
                auxH = heruistic(auxS,auxNND);
                str = "H" + to_string(auxH) + "\tSWAP\t ND=" + to_string(ND(auxS)) + "\tElement of D " + to_string(r2i) + " with element " + to_string(r3i);
            }

            double difH = auxH - h;

            if(difH > 0) {//millora 
                NND = auxNND;
                s = auxS;
                h = auxH;
                ops.push_back(str);
                if(print == 1) cout << iter << ":\t" << str << endl ;
            }
            else {//igual o pitjor
                float p = rnd->next();
                float P = exp(difH/(k*exp(-l*iter)));
                if(p <= P) { // es fa el canvi igualment
                    NND = auxNND;
                    s = auxS;
                    h = auxH;
                    ops.push_back(str);
                    if(print == 1) cout << iter << ":\t" << str << endl ;
                }
            }
            if(ND(s) < min) min = ND(s);
        }
        iter++;
    }


    //error en algun lloc
    if(dominador(NND)) cout << "THE D IS A POSITIVE DOMINATOR SET" << endl;
    else cout << "THE D IS NOT VALID" << endl;

    if(minimal3(s,NND)) cout << "MINIMAL!!" << endl;
    else cout << "NOT MINIMAL!!" << endl;

    //print results
    if(print == 1) {
        cout << "MIN=" << min << endl;
        /*if(ops.size() != 0) {
            cout << "OPERATIONS PERFORMED:" << endl;
            for(int i = 0; i < ops.size(); i++) cout << ops[i] << endl;
        }
        else cout << "NO OPERATIONS WERE PERFORDMED" << endl;*/
    }
    
    return s;
}

//PRE: graf inicialitzat
//POST: retorna una solució(paràmetres H, O i SI)
vector<bool> hillClimbing() {
    //valors per les iteracions
    int iteracions = 0;
    vector<string> ops(0);
    
    //valors de l'estat actual
    vector<int> NND = vector<int>(N,0);
    vector<bool> s = solucio_inicial(NND);
    double h = 0;

    //valors per detectar la fi de l'algorisme(local maximum)
    bool fi = false;
    int max_iteracions = N;
   
    while(not fi and iteracions < max_iteracions) { //possible maxim d'iteracions
        //valors pels fills millors provisionals
        bool millora = false;
        vector<bool> bestS;
        vector<int> bestNND;
        double bestH = h;
        string str;

        //utilitzar els operadors per trobar un fill millor
        //REMOVE (&& ADD)
        for(int i = 0; i < N; i++) {
            vector<bool> auxS = s;
            vector<int> auxNND = NND;
            if(s[i]) {
                opREMOVE(auxS,auxNND,i);
                double auxH = heruistic(auxS,auxNND);
                if(auxH > bestH) {
                    bestH = auxH;
                    bestS = auxS;
                    bestNND = auxNND;
                    str = "H" + to_string(auxH) + "\tREMOVE\t ND=" + to_string(ND(auxS)) + "\tElement " + to_string(i) ;
                    millora = true;
                }
            }
            else if(O == 1 or O == 3) {
                opADD(auxS,auxNND,i);
                double auxH = heruistic(auxS,auxNND);
                if(auxH > bestH) {
                    bestH = auxH;
                    bestS = auxS;
                    bestNND = auxNND;
                    str = "H" + to_string(auxH) + "\tADD\t ND=" + to_string(ND(auxS)) + "\tElement " + to_string(i) ;
                    millora = true;
                }
            }
        }
        //cout << str << endl;

        //SWAP
        for(int i = 0; i < N and (O == 2 or O == 3); i++) { //iterar sobre els nodes que NO son de D
            if(s[i]) continue;
            for(int j = 0; j < N; j++) { //iterar sobre els nodes que son de D
                if(not s[j]) continue;
                vector<bool> auxS = s;
                vector<int> auxNND = NND;
                opSWAP(auxS,auxNND,i,j);
                double auxH = heruistic(auxS,auxNND);
                //if(auxH != 0.0) cout << "H" + to_string(auxH) + "\tSWAP\t ND=" + to_string(ND(auxS)) + "\tElement of D " + to_string(j) + " with element " + to_string(i) << endl;
                if(auxH > bestH) {
                    bestH = auxH;
                    bestS = auxS;
                    bestNND = auxNND;
                    str = "H" + to_string(auxH) + "\tSWAP\t ND=" + to_string(ND(auxS)) + "\tElement of D " + to_string(j) + " with element " + to_string(i) ;
                    millora = true;
                }
            }
        }

        if(millora) {
            cout << iteracions << ":\t" << "ND=" << ND(bestS) << "\t" << str << endl ;//<< endl ;
            s = bestS;
            h = bestH;
            NND = bestNND;
            ops.push_back(str);
        }
        else fi = true;
        iteracions++;
    }

    //TO MUCH ITERATIONS
    if(not fi) cout << "THE ALGORITHM WAS STOPEED. PERFORMED 50 ITERATIONS." << endl;

    //error en algun lloc
    if(dominador(NND)) cout << "THE D IS A POSITIVE DOMINATOR SET" << endl;
    else cout << "THE D IS NOT VALID" << endl;

    //print results
    if(fi and false) {
        if(ops.size() != 0) {
            cout << "OPERATIONS PERFORMED:" << endl;
            for(int i = 0; i < ops.size(); i++) cout << ops[i] << endl;
        }
        else cout << "NO OPERATIONS WERE PERFORDMED" << endl;
    }
    
    return s;
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

    indata >> N;
    indata >> M;
    neighbors = vector< unordered_set<int> >(N);
    int u, v;
    while(indata >> u >> v) {
        neighbors[u - 1].insert(v - 1);
        neighbors[v - 1].insert(u - 1);
    }
    indata.close();

    // main loop over all applications of local search
    for (int na = 0; na < n_apps; ++na) {
        cout << "start application " << na + 1 << endl;
        Timer timer;

        cout << "------------------------------------------" << endl;
        //vector<bool> solution = hillClimbing();
        vector<bool> solution = simulatedAnnealing();
        cout << "------------------------------------------" << endl;

        results[na] = ND(solution);
        double ct = timer.elapsed_time(Timer::VIRTUAL);
        times[na] = ct;
        
        cout << "value " << results[na];
        cout << "\ttime " << times[na] << endl;

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

