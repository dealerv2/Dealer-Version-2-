# JGM Makefile for deal_v3  2022/01/01 PROD directory
# Note MUST use g++ as the linker stage because DDS library created with g++

CC      = gcc
CFLAGS = -std=gnu17 -mtune=corei7 -flto -fopenmp -pthread -Wall -pedantic -O3  -I../include
#CFLAGS = -std=gnu17 -mtune=corei7 -flto -fopenmp -pthread -Wall -pedantic  -g -Og -I../include
#Note CPP and CPPFLAGS are for the C PreProcessor. Use CXX and CXXFLAGS for C++ aka g++
CXX = g++
CXXFLAGS = -std=gnu++17 -mtune=corei7 -flto -fopenmp -pthread  -O3  -I../include
#CXXFLAGS = -std=gnu++17 -mtune=corei7 -flto -fopenmp -pthread  -g -Og -I../include
#can add --verbose to this next one to debug linker problems
LDFLAGS = -L../lib -lm -ldds
DESTDIR = ../Prod
PROGRAM  = dealerv2

FLEX    = flex
YACC    = bison -d

SRCS  = ../src/dealaction_subs.c ../src/dealdds_subs.c ../src/dealdebug_subs.c ../src/dealeval_subs.c ../src/dealparse_subs.c
SRCS += ../src/Dealer_DDS_IF.c ../src/c4.c ../src/dealerv2.c
OBJS = $(patsubst ../src/%.c,%.o,$(SRCS) ) dealflex.o dealyacc.tab.o

HDR  = ../include/allheaders.h
# Need to link with g++ because libdds library was built with c++
dealerv2: ${OBJS}
	$(CXX) $(CXXFLAGS) -o $@ ${OBJS}  $(LDFLAGS)

allheaders :
	touch ../include/allheaders.h

clean:
	rm -f ${OBJS} ${LOBJ} ${YOBJ} *.gch

showme:
	@echo DestDir = $(DESTDIR) JGMDBG = $(JGMDBG) PROGRAM = $(PROGRAM)
	@echo CC=$(CC) $(CFLAGS) $(LDFLAGS)
	@echo CXX=$(CXX) $(CXXFLAGS) $(LDFLAGS)
	@echo BISON SRC= $(YSRC) OBJ= $(YOBJ)
	@echo FLEX  SRC= $(LSRC) OBJ=$(LOBJ)
	@echo
	@echo SRCS=$(SRCS)
	@echo
	@echo OBJS=$(OBJS)
	@echo
	@echo BUILD=$(CXX) CXXFLAGS -o dealdbg ${OBJS}  $(LDFLAGS)


#
# Flex
#
LSRC = ../src/dealflex.l dealyacc.tab.h
LOBJ = dealflex.c

$(LOBJ) : $(LSRC) $(HDR)
	${FLEX} -o $@ $<

#
# Yacc/Bison -- bison -d creates the file dealyacc.tab.c and dealyacc.tab.h
#

YSRC = ../src/dealyacc.y
YOBJ = dealyacc.tab.c dealyacc.tab.h

$(YOBJ) : $(YSRC) $(HDR)
	${YACC} $<
#
# File dependencies
#

dealerv2.o : ../src/dealerv2.c ../src/mainsubs.c $(HDR) ../src/UsageMsg.h
	$(CC) $(CFLAGS) -o $@ -c $<
dealflex.o : dealflex.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<
dealyacc.tab.o : dealyacc.tab.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<
deal%.o : ../src/deal%.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<
Dealer_DDS_IF.o : ../src/Dealer_DDS_IF.c $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<
c4.o : ../src/c4.c ../include/c4.h $(HDR)
	$(CC) $(CFLAGS) -o $@ -c $<

objects : $(OBJS)
# could I make the last two read $(OBJS) : $(SRCS) $(HDR) ?
