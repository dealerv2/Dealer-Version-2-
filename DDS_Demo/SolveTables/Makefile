# This is a Makefile for , JGMs program to just solve some pbn deals for DOP project
# Because the libdds.a file was created with C++ we must use C++ for our code even if
# we are not using any C++ features.
# Valid C is supposed to be valid C++ so that should not present a problem.

# these next two for testing not usually used in production
GCC		= gcc
GCC_FLAGS	= -Og -g -flto -mtune=corei7 -fopenmp -std=gnu17

# If your C++ compiler name is not given here, change it.
CXX		= g++
CXX_FLAGS	= -Og -flto -mtune=corei7 -fopenmp -std=gnu++17
#CC_FLAGS	= -O3 -flto -mtune=corei7
# These flags are not turned on by default, but DDS should pass them.
# Turn them on below.
WARN_FLAGS	= 		\
	-Wshadow 		\
	-Wsign-conversion 	\
	-pedantic -Wall -Wextra  \
	-Wcast-align -Wcast-qual \
	-Wdisabled-optimization \
	-Winit-self 		\
	-Wlogical-op		\
	-Wmissing-declarations 	\
	-Wmissing-include-dirs 	\
	-Wredundant-decls 	\
	-Wstrict-overflow=1 	\
	-Wswitch-default -Wundef \
	-Werror 		\
	-Wno-unused 		\
	-Wno-unknown-pragmas 	\
	-Wno-long-long		\
	-Wno-format

# Here you can turn on warnings.
# CC_FULL_FLAGS	= $(CC_FLAGS)
CC_FULL_FLAGS	= $(CXX_FLAGS) $(WARN_FLAGS)

DLLBASE		= dds
STATIC_LIB	= lib$(DLLBASE).a


# the library is in the parent of the source dir.
LIB_FLAGS	= -L.. -l$(DLLBASE)
LIB4_FLAGS	= -L.. -lddsomp

LD_FLAGS	=

# In all these targets make sure you put the LIB_FLAGS at the END or else some symbols wont be found.

TablesPBN4: TablesPBN4.o
	$(CXX)  -o $@ $(CC_FULL_FLAGS)  $< $(LIB4_FLAGS)


TablesPBN4.o: TablesPBN4.c
	$(GCC) $(GCC_FLAGS) $(LD_FLAGS) $(LIB4_FLAGS) -c $<

TablesPBN: TablesPBN.o
	$(CC)  -o $@ $(CC_FULL_FLAGS)  $< $(LIB_FLAGS)

TablesPBN.o: TablesPBN.cpp
	$(CC) $(CC_FULL_FLAGS) $(LD_FLAGS) $(LIB_FLAGS) -c $<

clean:
	rm -f *.o *.gch


showme :
	@echo CC=$(CC)   CCFLAGS=$(CC_FULL_FLAGS)
	@echo GCC=$(GCC) GCCFLAGS=$(GCC_FLAGS)
	@echo LDFLAGS=$(LD_FLAGS)   LIBFLAGS=$(LIB_FLAGS) $(LIB4_FLAGS)
	@echo REMEMBER YOU CAN COMPILE YOUR APP WITH GCC BUT YOU CANNOT LINK WITH GCC. MUST USE g++ for that.
