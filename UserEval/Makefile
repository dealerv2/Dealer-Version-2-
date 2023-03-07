# JGM Makefile for deal_v5  UserEval Server in UserEval Directory.
# This Makefile in the {ROOT} dir, i.e. {Dealer}/UserEval
#
# 2022/12/13 --  First Draft
# 2023/01/26 JGM Added KnR.c and .o

JGMDBG := 1

CC      = gcc
CFLAGS = -std=gnu17 -mtune=corei7 -flto -fopenmp -pthread -Wall -pedantic  -I../include
#Note CPP and CPPFLAGS are for the C PreProcessor. Use CXX and CXXFLAGS for C++ aka g++
CXX = g++
CXXFLAGS = -std=gnu++17 -mtune=corei7 -flto -fopenmp -pthread  -I../include

#can add --verbose to this next one to debug linker problems also -t (trace)
# LDFLAGS =  -lm --verbose -t
LDFLAGS = -lm

DESTDIR = .
# This next program name is hardwired into the dealer code. But can be over-ridden with the -U switch on dealer cmd line.
PROGRAM  = DealerServer

ifdef JGMDBG
CFLAGS   += -DJGMDBG  -g -Og
CXXFLAGS += -DJGMDBG  -g -Og
else
CFLAGS += -O2
CXXFLAGS += -O2
endif

SRCS  = ./src/factors_subs.c  ./src/metrics_calcs.c  ./src/metrics_util_subs.c ./src/UserServer.c ./src/KnR.c

OBJS = factors_subs.o metrics_calcs.o metrics_util_subs.o UserServer.o KnR.o
OBJS += UserEval_globals.o

HDR  = ./include/allheaders.h ./include/dbgprt_macros.h ./include/UserEval_protos.h  ./include/UserEval_externs.h ./include/UserEval_types.h
HDR += ./include/Server_protos.h

${PROGRAM}: ${OBJS}
	 $(CC) $(CFLAGS) -o $@ ${OBJS} $(LDFLAGS)

showme:
	@echo DestDir = [ $(DESTDIR) ] JGMDBG = [ $(JGMDBG) ] PROGRAM = [ $(PROGRAM) ]
	@echo CC=$(CC) CFLAGS=$(CFLAGS) LDFLAGS=$(LDFLAGS)
	@echo
	@echo HDR=$(HDR)
	@echo SRCS=$(SRCS)
	@echo
	@echo OBJS=$(OBJS)
	@echo
	@echo BUILD=$(CC) $(CFLAGS) -o $(PROGRAM) ${OBJS}  $(LDFLAGS)

allheaders :
	touch ./include/allheaders.h

clean:
	rm -f ${OBJS} ${LOBJ} ${YOBJ} *.gch ./include/*.gch ./include/*.o ./src/*.gch ./src/*.o a.out ./src/a.out

#
# File dependencies
#

UserServer.o :  ./src/UserServer.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

factors_subs.o :  ./src/factors_subs.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

metrics_calcs.o :  ./src/metrics_calcs.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

metrics_util_subs.o :  ./src/metrics_util_subs.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

UserEval_globals.o : ./include/UserEval_globals.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<
KnR.o :  ./src/KnR.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

