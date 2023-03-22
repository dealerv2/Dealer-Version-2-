#!/usr/bin/perl
# Run thru all the dealer specification files that start with Descr, and generate corresponding files that start with Output
# Compare the Output.* files to files that start with Refer
# This allows checking that a modified version of dealer does not introduce bugs in previously working code.
#
# This script should be in the same directory as the .dli files it will be testing
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

$exe_test = "../dealerv2";                               # my test dealer under development;
$exe_refer = "/usr/local/bin/DealerV2/dealerv2";         # Previous version of dealerv2 known to be working. Use this to generate refer files.
@files = <*.dli> ;
foreach $input (@files) {
  # Loop over all files with suffix dli (Dealer Input)
  print "------------------------------------------------------------------\n";
  print "Now processing $input\n";

  $output = $input;
  $output =~ s/dli$/out/ ;
  $refer  = $input;
  $refer  =~ s/dli$/ref/;

  system("$exe_test  -v $input >Output/$output");
  system("$exe_refer -v $input >Refer/$refer"  ); # if we are creating both at once.

  print "Done, output in $output [and in $refer]?\n";

  if (-e "Refer/$refer" ) {         #if the refer file exists then run the diff cmd
     print STDERR "Comparing output in Output/$output (<) against reference output from Refer/$refer (>) \n";
     print STDERR `diff Output/$output Refer/$refer`;
  }

} # end foreach input (@files)

