#!/usr/bin/perl
# Run thru all the dealer specification files that start with Descr, and generate corresponding files that start with Output
# Compare the Output.* files to files that start with Refer
# This allows checking that a modified version of dealer does not introduce bugs in previously working code.
#

$exe_test = "../dealerv2";                # my test dealer under development; completely different random number generator. May not have same results as Debian
$exe_refer = "/usr/local/bin/DealerV2/dealerv2";         # Previous version of dealerv2 known to be working. Use this to generate refer files.

foreach $input (`ls Descr.*`) {
  # Loop over all files that start with Descr.
  chop $input;
  print "------------------------------------------------------------------\n";
  print "Now processing $input\n";

  $output = $input;
  $output =~ s/Descr/Output/;
  $refer  = $input;
  $refer  =~ s/Descr/Refer/;

  system ("$exe_test  $input >$output");
#  system("$exe_refer $input >$refer" );

  print "Done, output in $output [and in $refer]?\n";

  if (-e $refer) {         #if the refer file exists then run the diff cmd
     print STDERR "Comparing output in $output (<) against reference output from $refer (>) \n";
     print STDERR `diff $output $refer`;
  }

} # end for each ls Descr.*

