TARGET = greedy cplex local_search metaheuristic hybrid_metaheuristic
CXXFLAGS = -ansi -O3 -fpermissive -std=c++17 
OBJS = Random.o Timer.o
CPLOBJS = Random.o Timer.o

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /home/mjblesa/ILOG/CPLEX_Studio201/cplex
CONCERTDIR    = /home/mjblesa/ILOG/CPLEX_Studio201/concert
GCC = gcc
CCC = g++
CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++17 -fpermissive -w
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -lpthread -ldl
CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -lm -pthread -lpthread -ldl
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

all: ${TARGET}

cplex: cplex.o $(CPLOBJS)
	$(CCC) $(CCFLAGS) cplex.o $(CPLOBJS) -o cplex $(CCLNFLAGS)

cplex.o: cplex.cpp
	$(CCC) -c $(CCFLAGS) cplex.cpp -o cplex.o 

hybrid_metaheuristic: hybrid_metaheuristic.o $(CPLOBJS)
	$(CCC) $(CCFLAGS) hybrid_metaheuristic.o $(CPLOBJS) -o hybrid_metaheuristic $(CCLNFLAGS)

hybrid_metaheuristic.o: hybrid_metaheuristic.cpp
	$(CCC) -c $(CCFLAGS) hybrid_metaheuristic.cpp -o hybrid_metaheuristic.o 
 
greedy: greedy.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o $@ $^

local_search: local_search.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o $@ $^

metaheuristic: metaheuristic.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o $@ $^

clean:
	@rm -f *~ *.o ${TARGET} core

