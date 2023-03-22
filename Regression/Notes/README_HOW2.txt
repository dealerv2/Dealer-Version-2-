This is the dealerv2 Regression directory. This directory contains:
1. Perl scripts to automate the running of the regression tests. See below.
2. A Bash script to compare all the Output.xxxx files to the equivalent Refer.xxxx file using the diff command and report errors.
   The script is not terribly robust as even a mild difference between the two files (such as the time taken) would be reported as an error.
3. A Makefile that will automatically run the correct scripts for a given target.
   Valid targets are: "examples", "distribution", "test", and "refer".
4. Files beginning with "Descr." These are input files to dealerv2 that are meant to test the dealerv2 functionality.
   The scripts will automatically run dealerv2 using each one of these files as input.
5. Files beginning with "Output." These files (if present) are the result of running dealer with the corresponding Descr.
   input file. The script will automatically redirect the dealer output which normally goes to the screen to one of these files.
6. Files beginning with "Refer." These files (if present) are the output of a version of dealerv2 known to work. 
   If dealerv2 is rebuilt or modified, then re-running the regression test should result in output files that differ only 
   slightly (if at all) from the Refer.xxxxx files.
7. Files beginning with "HOW2." These files test the 'scripting' functionality of dealerv2. They cannot be used
   by the scripts as they require the user to enter command line parameters. That is why they do not begin with "Descr."
8. File "Test.distribution". This file is also a dealerv2 input file, that is not run by the script that looks for Descr. files
  but is instead processed by a separate test. Use the command: make distribution.
9. File "eval.dat". To use the GIB functionality of dealerv2, this file MUST be in the CURRENT WORKING directory, in this
   case 'Regression'. You must have a copy of this file in every directory from which you intend to call the GIB functionality 
   of dealerv2. If you do not call the GIB functionality, but use the DDS functionality instead you do not need this file.
--------------------------------------------------------------------------------------------------------------------------
The PERL scripts:
test_dealer.pl : This script will loop over all the files that begin with Descr. and run dealerv2 on them.
      The output is sent to a file named "Output.xxxxxx" where the suffix is the same as the one that was on the Descr.xxxxxx file
      The script will also compare the Output file to the corresponding Refer.xxxxxx file is that file exists.
      This script is what is run by the 'make examples' command.
      The user is expected to change the name of the program that is being run, to call upon either the known to be good version of dealerv2
      or the version under test.

dealerv2_chk.pl: This looks to be the same as test_dealer.pl

convert.pl: Reformat the output of the distribution test into a nice table.

fmt_distribution.pl: same as convert.pl

cmp_output.pl: compare the Output.* files to the Refer.* files without redoing all the tests.

chk_refer.bash: a bash script to compare the Output.* to the Refer.* files. 
		The Refer files can be in a different directory. It also counts the number of files where there was a difference.



