# No shebang since this file brought in by require.
# File Util_Subs -- Subs for housekeeping ; zeroing out hashes and arrays, getting cmd line options, and so on.
## Author      Version  Date           Comment
#  JGM         3.0a      2023/02/07
sub get_opts {
   use Getopt::Std;
   my $opt_href = shift ;
   local $req_dbg += 1 ;
   getopts("hG:D:T:");           # parse cmd line switches
   if (defined($Getopt::Std::opt_h) ) { &usage() ; exit 7 ; }

   if (defined($Getopt::Std::opt_D) ) {
      $opt_href->{D} = $Getopt::Std::opt_D ;
       print STDERR "Opt_D= $Getopt::Std::opt_D \n" if $jgmDebug >= $req_dbg;
   }
   else { $opt_href->{D} = 0 ; $Getopt::Std::opt_D = 0 ;
      print STDERR "Getopt::Std::opt_D is Undefined  Setting to zero \n" if $jgmDebug >= $req_dbg;
   }

   if (defined($Getopt::Std::opt_T) ) { $opt_href->{T} = $Getopt::Std::opt_T ; }
   else { $opt_href->{T} = localtime ;  $Getopt::Std::opt_T = $opt_href->{T} ;
       print STDERR "Getopt::Std::opt_T is Undefined  Setting to localtime \n" if $jgmDebug >= $req_dbg;
      }
   if (defined($Getopt::Std::opt_G) ) { $opt_href->{G} = $Getopt::Std::opt_G ; }
   else { $opt_href->{G} = 50.00 ;  $Getopt::Std::opt_G = $opt_href->{G} ;
       print STDERR "Getopt::Std::opt_G is Undefined  Setting to 50.00 \n" if $jgmDebug >= $req_dbg;
   }
   if    ($jgmDebug >= $req_dbg )        {show_hash("GETOPTS Returns Hash ", $opt_href ) ; }
} # end sub getopts
sub usage() {
      print "Usage: This template takes hD:T: options and a list of file names to process. Output to STDOUT \n";
  return 1 ;
}

# Debugging show subs -- I like better than Data::Dumper which could do the job.
sub show_argv {
   if (defined $_[0] ) {  print STDERR  "$_[0]:: "; }
   foreach $arg (@ARGV) { print STDERR  "[$arg] " ; }
   print STDERR "\n";
   return 1 ;
}
sub show_arr  {         # passed an array descr as a scalar string and an array_ref. Vers 3 shows sub arrays and sub hashes
        my $a_msg = shift @_;
        my $idx = 0 ;
        my $a_ref = shift @_ ;
        my ($item, $s_item) ;
        printf STDERR "<------------Array %12s has %6d items --------------->\n", $a_name, scalar(@{$a_ref});
        foreach $item ( @{$a_ref} ) {
           $ref_t = ref($item); #print "Array item [ $idx ] has ref_type $ref_t\n";
           if ($ref_t eq "HASH") {
                  &show_hash("$a_msg:$idx sub-Hash ", $item ) ;
                  print STDERR "..................................................\n";
           }
           elsif ($ref_t eq "ARRAY") {
                  &show_arr("$a_msg:$idx sub-array ", $item ) ;
                  print STDERR "..................................................\n";
           }
           else {
                $s_item = sprintf "%10.30s", $item;
                printf STDERR "\t %d = %-20.30s \n", $idx, $s_item ;
          }
          $idx++;
        }  # end foreach item
}  #end sub show_arr
sub show_hash { # passed in a hash descr/name, hashref, prints out sorted keys and values
        my $h_msg = shift @_;
        my $h_ref = shift @_;
        my ($h_key, $ref_t);
        $ref_t = ref $h_ref;
        printf STDERR "<--------Hash:[ %12s] of type [%10s] has %6s buckets  --------->\n", $h_msg, $ref_t, scalar(%{$h_ref});

        foreach $h_key (sort keys %{$h_ref} ) {
            $ref_t = ref($h_ref->{$h_key});
            if ($ref_t) {
               printf STDERR ("%30s  => %30s\n", $h_key, ("has type " . $ref_t ) );
               if    ($ref_t eq "ARRAY") {
                  &show_arr("[$h_key] sub-Array ", $h_ref->{$h_key} ) ;
                  print  STDERR "..................................................\n";
               }
               elsif ($ref_t eq "HASH") {
                  &show_hash("[$h_key] sub-Hash ", $h_ref->{$h_key} ) ;
                  print STDERR "..................................................\n";
               }
               else { print STDERR  "with ref value: ", $h_ref->{$h_key} , "\n"; }
            }
            else        { printf STDERR ("%30s  => %30s\n", $h_key, $h_ref->{$h_key});    }
        } # end foreach key
        return
} #end sub show_hash
sub show_keys {
   my $Descr = shift ;
   my $href = shift ;
   print STDERR "+-- $Descr --+ \n";
   foreach $hkey (sort keys %{ $href } ) { print STDERR "$hkey, " ; }
   print STDERR "\n";
} # end show_keys

  1 ; # so require succeeds
