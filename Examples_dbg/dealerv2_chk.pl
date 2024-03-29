#!/usr/bin/perl
# Run thru all the dealer specification files that start with Descr, and generate corresponding files that start with Output
# Compare the Output.* files to files that start with Refer 
# This allows checking that a modified version of dealer does not introduce bugs in previously working code.
#
#$exe = "/usr/games/dealer" ;             # Version 1.4 of dealer from Debian known to be working. use this to generate the 'Refer' files.
$exe = "../dealerv2";         # my test dealer under development; completely different random number generator. May not have same results as Debian 
##
# Fixed seed so that the output should be the same from run to run
#
$seed = 1;

foreach $input (`ls Descr.*`) {
  # Loop over all files that start with Descr.
  chop $input;
  print "------------------------------------------------------------------\n";
  print "Now processing $input\n";

  $output = $input;
  $output =~ s/Descr/Output/;
  $refer  = $input;
  $refer  =~ s/Descr/Refer/;

  system ("$exe -s $seed $input >$output");

  print "Done, output in $output\n";

  if (-e $refer) {
     print STDERR "Comparing output in $output (<) against reference output from $refer (>) \n";
     print STDERR `diff $output $refer`;
  }
 
  $seed++;

}

