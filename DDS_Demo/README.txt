JGM -- 2022-02-17

This folder, DDS_DEMO, shows a couple of 'scaffolding' programs that use the DDS library without involving Dealer
Could be useful as an example for those who want to use DDS in their own projects.
Could also be useful as an example of how to mix and match the Dealer definitions and the DDS definitions.
These are usually different !
Entity          Dealer                              DDS
-------------  -----------------------------     -----------------------------------------------------------------------
Compass:       N=0,E=1,S=2,W=3                   N=0,E=1,S=2,W=3
Strain :       C=0,D=1,H=2,S=3,N=4               C=3,D=2,H=1,S=0,N=4
PBN Deal:      printoneline fmt                  printoneline fmt but with only the starting compass direction specified.
Internal Deal: 52 char array of coded cards      4x4 Integer array with each bit giving presence or absence of card
Vulnerability: 0=none,1=NS,2=EW,3=both           0=none,1=both,2=NS,3=EW
Rank of Cards: Deuce=0, Ace=12                   Deuce=2, Ace=14

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Building the programs
The following folders each have their own Makefile for building the programs from source, as well as the prebuilt
binaries. To rebuild just cd to the folder and type 'make'
The Makefiles assume the presence of the DDS library in the DDS_Demo folder.
Two things to be aware of:
   a) To compile these programs all you need is a gcc.
   b) To link the programs with the library you MUST use g++.
   The reason is that the libraries are built from C++ code with g++ and the linker needs to be g++ in order
   to properly resolve the 'mangled' symbols.
The Makefiles also include a target 'showme' that will display the various compile and link flags.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Folder          Program                Explanation
include                                The various .h files for Dealer and DDS defs needed by the source.
SolveOneBoard   OneBoard.c             see: OneBoard_Demo.rpt for a sample terminal session
                                       This program is like mode 1 in dealer. Returns 1 result at a time.
SolveTables     TablesPBN4.c           see: TablesPBN4_Demo.rpt and par_TablesPBN.rpt
                                       This program is like mode 2 in dealer. Returns 20 results and a Par result at a time
TimeOneBoard    TimeOneBoard.c         This program generates Dealer style deals, converts them to DDS internal deals
                                       and then solves them one result at a time, for user specified compass and strain
                                       combinations.
                                       Useful example on how to use DDS from a Dealer perspective.
                                       see: TimeOneBoard_Demo.rpt
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Further Descriptions:
1. SolveOneBoard
This directory contains code that solves bridge hands one at a time. 
It corresponds to how code is used in Dealer for Mode 1. The binary should run as is; type ./OneBoard -h or peruse the code.

2. SolveTables
This directory contains code that solves Bridge hands using the DDS functions for solving all 20 results at a time. 
It further illustrates that DDS can solve several deals with one call thus taking maximum advantage of multi threading. 
This is the most efficient way to use DDS. Dealer uses a slightly stripped down version of this code when using Mode 2.
This code uses the PBN version of DDS's routines which is slower than using the Binary version.

3. TimeOneBoard
This directory contains code that times how long it takes to solve a deal one result at a time. No output is really produced, other than the timing info. 
The main benefit of this code is that it illustrates how to use DDS starting from a Dealer formatted deal. 
That is to say not a PBN deal, but a 52 byte array with each byte containing a coded card. 
The various header files used in this program allow the two different universes/namespaces of DDS an Dealer to communicate harmoniously.

In addition this directory contains the DDS precompiled library that is statically linked into Dealer, and an include directory and within the above three directories some sample output, and test input.






