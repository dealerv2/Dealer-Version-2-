#!/usr/bin/perl -w
# Rename every  Dealer Input file Descr.xxxxxx to xxxxxx.dli
#
@files = <Descr.*> ;
foreach $file (@files) {
  $new_fn = $file ;
  $new_fn =~ s/Descr[.]// ;
  $new_fn = $new_fn . ".dli"  ;
  print " Renaming $file to $new_fn \n";
  system "mv $file $new_fn"  ;

}
