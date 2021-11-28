TARGET = basic greedy local_search metaheuristic #cplex
CXXFLAGS = -ansi -O3 -fpermissive -std=c++17 
OBJS = Random.o Timer.o
CPLOBJS = Random.o Timer.o

GCC = gcc
CCC = g++
CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++17 -fpermissive -w

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR      = /home/mjblesa/ILOG/CPLEX_Studio201/cplex #canviar si cal!!!
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

basic: basic.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o EXES/$@ $^

greedy: greedy.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o EXES/$@ $^

local_search: local_search.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o EXES/$@ $^

metaheuristic: metaheuristic.cpp $(OBJS)
	${CCC} ${CXXFLAGS} -o metaheuristic/$@ $^

#cplex: cplex.o $(CPLOBJS)
#	$(CCC) $(CCFLAGS) cplex.o $(CPLOBJS) -o cplex $(CCLNFLAGS)

#cplex.o: cplex/cplex.cpp
#	$(CCC) -c $(CCFLAGS) cplex/cplex.cpp -o cplex.o 

clean:
	@rm -f *~ *.o ${TARGET} core
	@rm -Rf EXES/*
	@rm -f *.o

