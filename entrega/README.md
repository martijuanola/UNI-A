## Fitxers que hi han de ser:

//4 fitxers .cpp

//carpeta CPLEX

//time + random

//makefile(mirar que funcioni bé canviant directoris!!)

//.sh guais(mirar que funcioni bé canviant directoris!!)

//documentació en pdf

//aclariments per executar cada codi



## Aclariments local_search 

Paràmetres:

/* ------- Execució -------*/

- -i	-> Indicar el path(benchmarks a /data)
- -n	-> Indica el nombre d'execucions
- -p	-> Només es treurant els ouputs escencials per l'anàlisis de l'algorisme

/* ------- Paràmetres Generals Busqueda Local -------*/

- -a	-> Indica l'algorisme que s'utilitzarà
  - 1 - Hill Climbing
  - 2 - Simulate Annealing 
- -h	-> Indica l'heurístic que s'utilitzarà
  - 1 - minimitzar la quantitat de vèrtex del conjunt PIDS actual
  - 2 - maximitzar la suma de les cardinalitats dels vèrtex del conjunt PIDS actual
  - 3 - minimitzar la suma de les diferències per cada vèrtex del valor mínim de veïns que han de ser del conjunt PIDS i el nombre actual de veïns de PIDS
- -si	-> Indica l'algorisme per obtenir la solució/estat inicial de cada execució
  - 0 - D ple
  - 1 - Solució aleatoria
  - 2 - Golafre Estocàstic

/* ------- Paràmetres Simulated Annealing -------*/

- -T	-> Indica la temperatura màxim a la que arribarà el SA
- -iT	-> Indica el nombre d'iteracions per grau de temperatura
- -k	-> Especifica la constant k pel càlcul de la probabilitat acceptadora quan el fill generat té un heurístic inferior a l'actual.
- -l	-> Especifica la constant lambda pel càlcul de la probabilitat acceptadora quan el fill generat té un heurístic inferior a l'actual. 





## Aclariments ILP/CPLEX
Abans de poder compilar el fitxer cplex.cpp (mitjancant la comanda 'make'), 
s'ha d'instal.lar el software CPLEX. Per a fer-ho, s'ha de copiar el fitxer 

    CPLEX_Studio201.tgz 

al lloc desitgat de la computadora. Per exemple, a 

    /home/r5d8/fib/A/CPLEX_program/

Despres, descomprimir amb la comanda

    tar xvzf CPLEX_Studio201.tgz

Una vegada descomprimit, s'ha d'ajustar les rutes per tal que el compilador 
trobi el software del CPLEX. Això es fa al Makefile, afegint les seguents 
dues linies:

    CPLEXDIR      = /home/r5d8/fib/A/CPLEX_program/CPLEX_Studio201/cplex
    CONCERTDIR    = /home/r5d8/fib/A/CPLEX_program/CPLEX_Studio201/concert

El Makefile que es proporciona ja inclou aquestes linies, pero s'hauran 
d'adaptar convenientment al cami on s'hagi instal.lat el CPLEX.


## Aclariments Metaheurística 

Per la metaheurística els paràmetres utilitzats son els donats per la plantilla.

## Aclariments Golafre 

Conté aquests parametres:
	-i indicar el path (recomenat fitxers del folder data ../../data/<fitxer>.txt)
	-a indicar l'algorisme que s'utilitza:
		0 Greedy lent pero bo (Agafa els vertexs amb més nodes no satisfets)
		1 Greedy lent i dolent (Comença ple i treu aleatoriament)
		2 Greedy adaptatiu segons parametre -p (Semblant al 0 pero amb un parametre)
		3 Greedy aleatori segons parametre -p (Igual que el 2 però amb certa aleatorietat)
		
	-p només útil pels algorismes 2 i 3. Un parametre que indica la profunditat de la cerca al actualitzar un node.
	Com major, és més lent serà l'algorisme i millors seràn els resultats (limitat al resultat del algorisme 0)
	Elegir el parametre depenent del resultat desitjat:
		· 1 per un resultat molt rapid pero amb un 15% - 25% més de vertexs
		· 2 per un resultat rapid i bastant bo
		· >10*N per un resultat semblant al algorisme 0
