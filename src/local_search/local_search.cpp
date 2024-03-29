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

int resultG = 0;
int incrementLS = 0;

// string for keeping the name of the input file
string inputFile;

//Global parameters
//execution
int n_apps = 1;
bool print = true;

//Heuristic, ops and initial solution
int A = 2;
int H = 3;
int O = 3;
int SI = 2;

//SA parameters
int T = 10000;
int iT = 15;
int k = 10;
float l = 0.075;

//Greedy Jaume
int profunditat = 7;

typedef struct{
    vector<bool> D = vector<bool>(N,false); //elements de D
    vector<int> NND = vector<int>(N,0);     //Veins de D de cada node
    int ND = 0;                             //nombre de nodes de D
    double h = 0;                           //valor de l'heurístic
} estat;

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
        else if (strcmp(argv[iarg],"-p")==0) print = false;                             //elimina prints

        else if (strcmp(argv[iarg],"-a")==0) A = atoi(argv[++iarg]);            //heuristic function
        else if (strcmp(argv[iarg],"-h")==0) H = atoi(argv[++iarg]);            //heuristic function
        else if (strcmp(argv[iarg],"-o")==0) O = atoi(argv[++iarg]);            //operators
        else if (strcmp(argv[iarg],"-si")==0) SI = atoi(argv[++iarg]);          //initial solution/state

        else if (strcmp(argv[iarg],"-T")==0) T = atoi(argv[++iarg]);            //Temperatura
        else if (strcmp(argv[iarg],"-iT")==0) iT = atoi(argv[++iarg]);          //Iteracions per temperatura
        else if (strcmp(argv[iarg],"-k")==0) k = atoi(argv[++iarg]);            //K
        else if (strcmp(argv[iarg],"-l")==0) l = atof(argv[++iarg]);            //lambda

        else if (strcmp(argv[iarg],"-d")==0) profunditat = atoi(argv[++iarg]);  //profunditat pel greedy

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

//POST: retorna el nombre d'elements de D
int NDCalc(const vector<bool>& v) {
    int count = 0;
    for(int i = 0; i < v.size(); i++) if(v[i]) count++;
    return count;
}

int minNND(int n) {
    return ceil(float(n)/2);
}

bool dominador(const vector<int>& NND) {
    for(int i = 0; i < neighbors.size(); i++) {
        if(NND[i] < minNND(neighbors[i].size())) return false;
    }
    return true;
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

/* ----------------- COSES GREEDY ----------------- */

//altGreedy
vector<bool> s; // true si es un vertex amb influencia positiva (també utilitzat per Pan's)
vector<unordered_set<int>> nodes;// posicio del vector es valor del node = suma dels s dels veins, dins del set es els nodes que compleixen
int maxPos; // indica quina es la posicio més alta del vector on hi ha vertexs
vector<int> pos; // indica quina es la posicio al vector de nodes de cada vertex (no ordenat)
vector<bool> calculat; // per indicar quins vertexs ja s'han calculat

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

void actualitzaDades(int v, vector<int>& NND) { //v abans no formava part de D i ara si
   
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

void greedyRandom(vector<bool>& D, vector<int>& NND) { //Igual que altGreedy pero amb aleatorietat
    while (not dominador(NND)) {
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
        actualitzaDades(v,NND);
    }
}


//FUNCIONS HILL CLIMBING

//SOLUCIO INICIAL

//PRE: SI indica quina solució inicial utilitzar
//POST: tots els nodes pertanyen a D i per tant tots els veins d'un node també
vector<bool> solucio_inicial(vector<int>& NND, int& ND) {
    vector<bool> sol(N,false);
    NND = vector<int> (N,0);
    
    //aleatori totalment
    if(SI == 1) {
        while(not dominador(NND)) {
            float n = rnd->next();
            int pos = int(N*n);
            while(sol[pos]) {
                pos = (pos+1)%N;
            }

            sol[pos] = true;
            for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) NND[*itr]++;
        }
    }

    //Random Gready d'en Jaume
    else if(SI == 2) {
        srand(time(NULL));
        int maxSize = 0;
        for (int i = 0; i < N; ++i) {
            if (neighbors[i].size() > maxSize) maxSize = neighbors[i].size();
        }
        maxSize += 2; //+1 per que el node també conta 1 i +1 pel 0
        nodes = vector<unordered_set<int>>(maxSize);
        vector<bool> D = vector<bool>(N, false); //Comença amb cap node a la solucio
        s = vector<bool>(N, false); //Tots començen a false (0)
        pos = vector<int>(N, 0);

        maxPos = 0;
        for (int i = 0; i < N; ++i) {
            int p = neighbors[i].size();
            if (p > maxPos) maxPos = p;
            nodes[p].insert(i);

            pos[i] = p;
        }
        greedyRandom(D,NND);
        sol = D;
        if(print) {
            cout << "FI: Greedy fet!!! nombre de nodes inicials " << NDCalc(sol) << endl;
            if(dominador(NND)) cout << "THE D IS A POSITIVE DOMINATOR SET" << endl;
            else cout << "THE D IS NOT VALID" << endl;
        }
    }

    //tot ple
    else {
        sol = vector<bool> (N,true);
        for(int i = 0; i < N; i++) NND[i] = neighbors[i].size();
    }

    ND = NDCalc(sol);    
    resultG = ND;
    return sol;
}

//HEURISTICA

//PRE: H indica l'heurístic que es farà servir
//POST: retorna el valor de l'heurístic H
double heruistic(const vector<bool>& v, const vector<int>& NND, const int& ND) {
    //maximitza nodes a ND
    if(H == 1) { 
        if(not dominador(NND)) return -10000.0;
        else {
            return double(N - ND);
        }
    }

    //maximitzar sumatori nombre arestes dels vertex de D
    else if(H == 2) {
        if(not dominador(NND)) return -10000.0;
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
    
    //minimitza diferència entre minNND i NND per nodes de ND
    else if(H == 3) {
        if(not dominador(NND)) return -10000.0;
        else { //diferència del minim i l'actual NND
            double sum = 0;
            for(int i = 0; i < N; i++) {
                if(not v[i]) sum += neighbors[i].size() - (NND[i] - minNND(neighbors[i].size()));
            }
            return sum;
        }
    }

    //que pugui sortir de l'espai i tornar prioritzant adds(ND)
    else return 0.0;
}


//OPERADORS

//PRE: l'element pos no ha d'estar al set D
//POST: s'ha afegit l'element pos a D i s'ha actualitzat NND
void opADD(vector<bool>& v, vector<int>& NND, int& ND, int pos) {
    if(ND == N) {
        cout << "ADD + D ple!!!" << endl;
        return;
    }

    if(v[pos]) {
        cout << "ERROR(opADD): l'element " << pos+1 << " ja està a D, no es pot tornar a afegir." << endl;
        return;
    }

    v[pos] = true;
    ND++;
    for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) {
        NND[*itr]++;
    }
}

//PRE: l'element pos ha d'estar a D
//POST: s'ha eliminat l'element pos de D i s'ha actualitzat NND
void opREMOVE(vector<bool>& v, vector<int>& NND, int& ND, int pos) {
    if(ND == 0) {
        cout << "REMOVE + D buit!!!" << endl;
        return;
    }
    if(not v[pos]) {
        cout << "ERROR(opREMOVE): l'element " << pos+1 << " no està a D, no es pot tornar a eliminar." << endl;
        return;
    }
    v[pos] = false;
    ND--;
    for(auto itr = neighbors[pos].begin(); itr != neighbors[pos].end(); itr++) {
        NND[*itr]--;
    }
}

//PRE: l'element posND no ha d'estar a D i l'element posD sí que ho ha d'estar
//POST: posND ara està a D i posD no. NND s'ha acutalitzat.
void opSWAP(vector<bool>& v, vector<int>& NND, int& ND, int posND, int posD) {
    if(ND == N) {
        cout << "SWAP + D ple!!!" << endl;
        return;
    }
    else if(ND == 0) {
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
    vector<string> ops = vector<string>(0); //operacions realitzades sobre s

    estat actual;
    actual.D = solucio_inicial(actual.NND,actual.ND);

    if(actual.ND < min and dominador(actual.NND)) min = actual.ND;

    int iter = 0;
    while(iter < T) {
        for(int si = 0; si < iT; si++) {
            //nou fill
            float r1 = rnd->next(); // per l'operació
            float r2 = rnd->next(); //element de D
            float r3 = rnd->next(); //element de ND
            int r2i = int(r2*N);
            int r3i = int(r3*N);
            while(not actual.D[r2i] and actual.ND != 0) { r2i = (r2i+1)%N;}
            while(actual.D[r3i] and actual.ND != N) { r3i = (r3i+1)%N;}

            estat nou = actual;
            nou.h = 0;
            string str;

            float nops = 3.0;
            if(O == 1 or O == 2) nops = 2.0;
            else if(O == 0) nops = 1.0;

            if(r1 <= 1.0/nops and actual.ND != 0) {
                opREMOVE(nou.D,nou.NND,nou.ND,r2i);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                str = "H" + to_string(nou.h) + "\tREMOVE\t ND=" + to_string(nou.ND) + "\tElement " + to_string(r2i);
            }
            else if(r1 <= 2.0/nops and O != 2 and actual.ND != N) {
                opADD(nou.D,nou.NND,nou.ND,r3i);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                str = "H" + to_string(nou.h) + "\tADD\t ND=" + to_string(nou.ND) + "\tElement " + to_string(r3i);
            }
            else if(actual.ND > 0 and actual.ND < N){
                opSWAP(nou.D,nou.NND,nou.ND,r3i,r2i);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                str = "H" + to_string(nou.h) + "\tSWAP\t ND=" + to_string(nou.ND) + "\tElement of D " + to_string(r2i) + " with element " + to_string(r3i);
            }

            double difH = nou.h - actual.h;

            if(difH > 0) {//millora 
                actual = nou;
                ops.push_back(str);
                if(print) cout << iter << ":\t" << str << endl ;
            }
            else {//igual o pitjor
                float p = rnd->next();
                float P = exp(difH/(k*exp(-l*iter)));
                if(p <= P) { // es fa el canvi igualment
                    actual = nou;
                    ops.push_back(str);
                    if(print) cout << iter << ":\t" << str << endl ;
                }
            }
            if(actual.ND < min and dominador(actual.NND)) min = actual.ND;
        }
        iter++;
    }

    //error en algun lloc
    if(print) {
        if(dominador(actual.NND)) cout << "THE D IS A POSITIVE DOMINATOR SET" << endl;
        else cout << "THE D IS NOT VALID" << endl;

        if(minimal3(actual.D,actual.NND)) cout << "MINIMAL!!" << endl;
        else cout << "NOT MINIMAL!!" << endl;
        
        cout << "BEST SOLUTION FOUND = " << min << endl;
    }

    if(not dominador(actual.NND)) cout << "ERROR! La solucio amb valor " << actual.ND << " no és un PDS" << endl;
    return actual.D;
}

//PRE: graf inicialitzat
//POST: retorna una solució(paràmetres H, O i SI)
vector<bool> hillClimbing() {
    //valors per les iteracions
    bool millora = true;
    int iteracions = 0;
    vector<string> ops(0);
    
    //valors de l'estat actual
    estat actual;
    actual.D = solucio_inicial(actual.NND,actual.ND);

    while(millora) { //possible maxim d'iteracions
        millora = false;
        
        estat best = actual; //valors pels fills millors provisionals
        string str;
        

        //utilitzar els operadors per trobar un fill millor
        //REMOVE (&& ADD)
        for(int i = 0; i < N; i++) {
            estat nou = actual;

            if(actual.D[i]) {
                opREMOVE(nou.D,nou.NND,nou.ND,i);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                if(nou.h > best.h) {
                    best = nou;
                    str = "H" + to_string(nou.h) + "\tREMOVE\t ND=" + to_string(nou.ND) + "\tElement " + to_string(i) ;
                    millora = true;
                }
            }
            else if(O == 1 or O == 3) {
                opADD(nou.D,nou.NND,nou.ND,i);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                if(nou.h > best.h) {
                    best = nou;
                    str = "H" + to_string(nou.h) + "\tADD\t ND=" + to_string(nou.ND) + "\tElement " + to_string(i) ;
                    millora = true;
                }
            }
        }

        //SWAP
        for(int i = 0; i < N and (O == 2 or O == 3); i++) { //iterar sobre els nodes que NO son de D
            if(actual.D[i]) continue;
            for(int j = 0; j < N; j++) { //iterar sobre els nodes que son de D
                if(not actual.D[j]) continue;
                
                estat nou = actual;
                opSWAP(nou.D,nou.NND,nou.ND,i,j);
                nou.h = heruistic(nou.D,nou.NND,nou.ND);
                if(nou.h > best.h) {
                    best = nou;
                    str = "H" + to_string(nou.h) + "\tSWAP\t ND=" + to_string(nou.ND) + "\tElement of D " + to_string(j) + " with element " + to_string(i) ;
                    millora = true;
                }
            }
        }

        if(millora) {
            if(print) cout << iteracions << ":\t" << "ND=" << best.ND << "\t" << str << endl ;//<< endl ;
            actual = best;
            ops.push_back(str);
        }
        iteracions++;
    }

    //TO MUCH ITERATIONS
    if(print) {
        //error en algun lloc
        if(dominador(actual.NND)) cout << "THE D IS A POSITIVE DOMINATOR SET" << endl;
        else cout << "THE D IS NOT VALID" << endl;

        //print results
        if(false) {
            if(ops.size() != 0) {
                cout << "OPERATIONS PERFORMED:" << endl;
                for(int i = 0; i < ops.size(); i++) cout << ops[i] << endl;
            }
            else cout << "NO OPERATIONS WERE PERFORDMED" << endl;
        }
    }
    
    if(not dominador(actual.NND)) cout << "ERROR! La solucio amb valor " << actual.ND << " no és un PDS" << endl;
    return actual.D;
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
        if(print) cout << "start application " << na + 1 << endl;
        

        if(print) cout << "------------------------------------------" << endl;
        vector<bool> solution;

        Timer timer;
        if(A == 1) solution = hillClimbing();
        else if(A == 2) solution = simulatedAnnealing();
        double ct = timer.elapsed_time(Timer::VIRTUAL);
        int ND = NDCalc(solution);
        if(print) cout << "Greedy Result = " << resultG << " | Local Search Result = " << ND << " | Millora = " << resultG - ND << endl;  
        if(print) cout << "------------------------------------------" << endl;

        results[na] = ND;
        times[na] = ct;
        
        if(print) cout << "value " << results[na] << "\ttime " << times[na] << endl;

        if(print) cout << "end application " << na + 1 << endl;
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

