GCC		= gcc
GCC_FLAGS	= -g -flto -mtune=corei7 -fopenmp -std=gnu17 -pthread -O3
PROGRAM = TimeOneBoard

# If your C++ compiler name is not given here, change it.
CC	 	= g++
CXX		= g++
CXX_FLAGS	= -O3 -flto -mtune=corei7 -fopenmp -std=gnu++17 -pthread
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

$(PROGRAM) : $(PROGRAM).o DealingSubs.o allheaders.h
	$(CXX)  -o $@ $(CC_FULL_FLAGS)  $^ $(LIB4_FLAGS)


$(PROGRAM).o : $(PROGRAM).c ../include/deal_dds.h ../include/dds_solve.h allheaders.h
	$(GCC) $(GCC_FLAGS) $(LD_FLAGS) $(LIB4_FLAGS) -c $<


DealingSubs.o : DealingSubs.c  ../include/deal_dds.h ../include/dds_solve.h allheaders.h
	$(GCC) $(GCC_FLAGS) $(LD_FLAGS) $(LIB4_FLAGS) -c $<

allheaders :
	touch allheaders.h

clean:
	rm -f *.o *.gch

showme :
	@echo PROGRAM = $(PROGRAM)
	@echo CXX=$(CXX)   CXXFLAGS=$(CXX_FLAGS)
	@echo GCC=$(GCC) GCCFLAGS=$(GCC_FLAGS)
	@echo LDFLAGS=$(LD_FLAGS)   LIBFLAGS=$(LIB_FLAGS) $(LIB4_FLAGS)
	@echo REMEMBER YOU CAN COMPILE YOUR APP WITH GCC BUT YOU CANNOT LINK WITH GCC. MUST USE g++ for that.
