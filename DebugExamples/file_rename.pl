#!/usr/bin/perl -w
@files = <*.dli.dli> ;
foreach $file (@files) {
  $new_fn = $file ;
  $new_fn =~ s/[.]dli$// ;
  $new_fn = $new_fn  ;
  print " Renaming $file to $new_fn \n";
  system "mv $file $new_fn"  ;

}
