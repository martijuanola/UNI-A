TARGET = cplex_mds
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

cplex_mds: cplex_mds.o $(OBJS)
	$(CCC) $(CCFLAGS) cplex_mds.o $(OBJS) -o cplex_mds $(CCLNFLAGS)

cplex_mds.o: cplex_mds.cpp
	$(CCC) -c $(CCFLAGS) cplex_mds.cpp -o cplex_mds.o 

clean:
	@rm -f *~ *.o ${TARGET} core

