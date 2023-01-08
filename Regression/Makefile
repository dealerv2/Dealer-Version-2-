#
# Makefile to process all Descr.* files with dealer and write the output
# to Output.*.  make test executes the test, make clean cleans up.
# Depending on the CPU power that you have, these test may take up to
# 10 minutes to run.
# Edit the line DEALER to call upon the test version or the production version.
#

DEALER = ../dealerv2
#DEALER = dealerv2
SHELL=/bin/sh

OUTPUT = $(wildcard Output.*)

test: examples distribution

distribution:
	${DEALER} -s 1 <Test.distribution | ./convert.pl >Output.distribution
	diff Output.distribution Refer.distribution

examples:
	./test_dealer.pl

refer:
	-for f in ${OUTPUT}; do \
		echo $$f; \
		f1=`echo $$f | sed s/Output/Refer/` ; \
		echo $$f1; \
		/bin/cp $$f $$f1; \
	done

clean:
	-rm -fv ${OUTPUT}
