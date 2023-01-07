#!/usr/bin/perl
# Run thru all the dealer specification files that start with Descr, and generate corresponding files that start with Output
# Compare the Output.* files to files that start with Refer 
# This allows checking that a modified version of dealer does not introduce bugs in previously working code.
#
#$exe = "/usr/games/dealer" ;             # Version 1.4 of dealer from Debian known to be working. use this to generate the 'Refer' files.
$exe_test = "../dealerv2";                # my test dealer under development; completely different random number generator. May not have same results as Debian 
$exe_refer = "/usr/local/bin/DealerV2/dealerv2";         # Previous version of dealerv2 known to be working. Use this to generate refer files.
#
# Fixed seed so that the output should be the same from run to run -- each Descr file in the Regression folder should have its own seed.
# This is more reliable than relying on the number and order of the files in the directory,
# But version 1.4 of dealer does not have the ability to have a seed in the input file so need the cmd line one if using version 1.4
#
$seed = 1;

foreach $input (`ls Descr.*`) {
  # Loop over all files that start with Descr.
  chop $input;
  print "------------------------------------------------------------------\n";
  print "Now processing $input with seed $seed\n";

  $output = $input;
  $output =~ s/Descr/Output/;
  $refer  = $input;
  $refer  =~ s/Descr/Refer/;

  system ("$exe_test -s $seed $input >$output");
  system("$exe_refer -s $seed $input >$refer" );

  print "Done, output in $output [and in $refer]\n";

  if (-e $refer) {			#if the refer file exists then run the diff cmd
     print STDERR "Comparing output in $output (<) against reference output from $refer (>) \n";
     print STDERR `diff $output $refer`;
  }
 
  $seed++;

}

