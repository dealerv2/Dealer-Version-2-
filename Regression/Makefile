# 2023/03/19 Slight Mods by JGM to use dealerv2, bash, seeds in the Descr.* files 
# 2023/03/20 Renamed all the Dealer input files from Descr.xxxxx to xxxxx.dli 
#            the Output files become xxxxx.dlo and the Refer xxxxx.ref
#            and the ones with scripting from HOW2.xxxxx to xxxxx.dls
#
# Makefile to process all *.dli files with dealer and write the output
# to *.out  'make test' executes the test, 'make clean' cleans up 'make refer' copies the current .dlo files to .ref 
# Edit the line DEALER to call upon the test version or the production version.
#

DEALER = ../dealerv2
#DEALER = dealerv2
SHELL=/usr/bin/bash

OUTPUT = $(wildcard ./Output/*.out)

test: examples deal_stats
# next dealer line cannot have -v (suppress eoj stats) since fmt_deal_stats needs to know how many were generated.
deal_stats:
	${DEALER}  -m chk_deal_stats.dli | ./fmt_deal_stats.pl >Output/chk_deal_stats_fmt.out
	if [[ -e Refer/chk_deal_stats_fmt.ref ]];then diff Output/chk_deal_stats_fmt.out Refer/chk_deal_stats_fmt.ref ; fi

examples:
	./regress_test.pl

copy_refer:
	./copy_out_to_ref.bash Refer
cmp_refer:
	./cmp_out_to_ref.bash Refer
clean:
	-rm -fv ${OUTPUT}
showme:
	@echo OUTPUT = $(OUTPUT)
