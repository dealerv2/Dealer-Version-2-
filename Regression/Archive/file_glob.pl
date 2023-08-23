 #!/usr/bin/perl -w
@files = <Descr.*> ;
foreach $file (@files) {
  print $file . "\n";
}
