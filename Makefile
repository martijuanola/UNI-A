TARGET = basic greedy local_search metaheuristic #cplex hybrid_metaheuristic
CXXFLAGS = -ansi -O3 -fpermissive -std=c++17 
OBJS = src/Random.o src/Timer.o
CPLOBJS = src/Random.o src/Timer.o

GCC = gcc
CCC = g++
CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++17 -fpermissive -w

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /home/mjblesa/ILOG/CPLEX_Studio201/cplex
CONCERTDIR    = /home/mjblesa/ILOG/CPLEX_Studio201/concert

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread -lpthread -ldl
CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -lm -pthread -lpthread -ldl
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

all: ${TARGET}

basic: src/basic/basic.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o bin/$@ $^

greedy: src/greedy/greedy.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o bin/$@ $^

local_search: src/local_search/local_search.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o bin/$@ $^

metaheuristic: src/metaheuristic/metaheuristic.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o bin/$@ $^

#hybrid_metaheuristic: hybrid_metaheuristic.o $(CPLOBJS)
#	$(CCC) $(CCFLAGS) hybrid_metaheuristic.o $(CPLOBJS) -o hybrid_metaheuristic $(CCLNFLAGS)

#hybrid_metaheuristic.o: hybrid_metaheuristic.cpp
#	$(CCC) -c $(CCFLAGS) hybrid_metaheuristic.cpp -o hybrid_metaheuristic.o 

#cplex: cplex.o $(CPLOBJS)
#	$(CCC) $(CCFLAGS) cplex.o $(CPLOBJS) -o cplex $(CCLNFLAGS)

#cplex.o: src/cplex/cplex.cpp
#	$(CCC) -c $(CCFLAGS) src/cplex/cplex.cpp -o cplex.o 

clean:
	@rm -f *~ *.o ${TARGET} core
	@rm -Rf bin/*

