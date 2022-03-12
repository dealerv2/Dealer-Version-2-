# No shebang since this file brought in by require.
## Author      Version  Date           Comment
#  JGM         1.0      2020/10/22     Low Level Util Subs. Main Subs file getting too long....
#  JGM         10.0      2020/11/02     All working.
#  JGM         11        2020/12/18     Tweaked usage.
#  JGM         13.0     2021/01/22     Changed Version # to be in sync with main branch.
#  JGM         13.2     2021/05/07     Added Hint sub for me to document how I did things.
#  JGM         21.0     2021/08/28     Mods for -q (terse mode) for dealer
#
#------------------------------------------------------------------------------------------------------------
# Utility routines for using the suit hashes and the arrays there-in. SEE COMMENTS IN get_sptrs SUB re SUIT_hash_refs
sub get_suit_href_W {         # West Hand. called with C, D, H, S or 0..3. No Hand Ref.
 $s_ref = get_suit_href(\%Hand_W, $_[0]) ;
 local $req_dbg = $req_dbg + 1 ;
   if (!defined ($s_ref) ) {
      print STDERR "get_suit_href_W FAILED for suit $_[0] \n";
      die "FATAL at " .  __FILE__ . "Line No. " . __LINE__ ;
   }
   else {
      printf STDERR "get_suit_href_W :: SUCCESS.  suit# $_[0] ref type = %s\n", ref($s_ref) if $Debug > $req_dbg;
   }
   return ($s_ref);
}
sub get_suit_href_E {         # East Hand. called with C, D, H, S or 0..3. No Hand Ref.
 $s_ref = get_suit_href(\%Hand_E, $_[0]) ;
 local $req_dbg = $req_dbg + 1 ;
   if (!defined ($s_ref) ) {
      print STDERR "get_suit_href_E FAILED for suit $_[0] \n";
      die "FATAL at " .  __FILE__ . "Line No. " . __LINE__ ;
   }
   else {
      printf STDERR "get_suit_href_E :: SUCCESS.  suit# $_[0] ref type = %s\n", ref($s_ref) if $Debug > $req_dbg;
   }
   return $s_ref;
}

sub get_sptrs {               # Called with $hand_ref, returns array of suit hashrefs [0] = Clubs, [3]=Spades
   my $hnd_ref = shift ;      # Note suit_hashrefs point to hashes that only have scalars in them. so $s_href->{KEY}
   my @ptrs = ();             # but never $s_ref->[$i] or $s_ref->{KEY}->[$i] etc.
   foreach $p (0..3) { push @ptrs, $hnd_ref->{SUITS}->[$p] ; }
   return @ptrs ;
} #end get_sptrs

sub show_suit_type {    #called with (suit_ref) returns suit_type. Suit Hashes are blessed.
   my $s_ref = shift;
   my $s_type = ref $s_ref;
   print STDERR ("Suit has type: %s \n", ref($s_ref) );
   return $s_type;
}  #end sub show_suit_type
sub get_suit_key   {    #called with ($suit_ref, Key_String).  Returns value for that key.
   my $suit_ref = shift;
   my $key_name = shift;
   if (defined ($suit_ref->{$key_name}) ) {
         return $suit_ref->{$key_name};
   }
   return undef;
}  # end sub get suit key
sub set_suit_key   {    #called with ($suit_ref, Key_String). Sets value for that key.
   ($suit_ref, $key_name, $key_value) = @_ ;
   if (exists ($suit_ref->{$key_name}) ) {
      $suit_ref->{$key_name} = $key_value;
      return 1;
   }
   return 0 ;  # Failed to set a non existent key...
}  #end sub set suit key

#------------------------------------------------------------------------------------------------------------
# Some low level utility subs.
sub chk_ref_type {               # called with a reference
   return ref($_[0]);
}
sub max {               # two scalars, returns the max of the two. (signed compare. max (2,-5) returns 2
   return(($_[0] > $_[1])?$_[0]:$_[1]);
}
sub min {               # two scalars, returns the max of the two. (signed compare. max (2,-5) returns 2
   return(($_[0] < $_[1])?$_[0]:$_[1]);
}
# could I do this with a grep or a map?
sub array_max_idx {
   my $arr_ref = shift ;
   my ($idx, $val_max, $idx_max) ;
   $val_max = $$arr_ref[0];
   $idx_max = 0 ;
   for ($idx = 0; $idx < $#$arr_ref ; $idx++ ) {
      if ($$arr_ref[$idx] > $val_max) { $val_max = $$arr_ref[$idx] ; $idx_max = $idx ; }
   }
   if (wantarray() ) {  return ($idx_max, $val_max) ;  }
   return $idx_max ;
}
sub array_max {
   my $arr_ref = shift ;
   my $arr_max = $$arr_ref[0];
   my $arr_item ;
   foreach $arr_item (@{$arr_ref}) {
      # print STDERR "Comparing $arr_item to $arr_max \n";
      $arr_max = $arr_item if $arr_item > $arr_max ;
   }
   return $arr_max ;
}
sub array_min {
   my $arr_ref = shift ;
   my $arr_min = $$arr_ref[0];
   my $arr_item ;
   foreach $arr_item (@{$arr_ref}) {
      # print STDERR "Comparing $arr_item to $arr_max \n";
      $arr_min = $arr_item if $arr_item < $arr_min ;
   }
   return $arr_min ;
}
sub array_sum  {         # arg=array_ref; returns sum of items in array
   my $arr_ref = shift ;
   my $sum_v = 0;
   foreach $i (0 .. $#{$arr_ref} ) { $sum_v += $arr_ref->[$i];  }
   return $sum_v;
}
sub sum_suit  {         # arg=array_ref; returns sum of items in array; any array not just suits.
   my $arr_ref = shift ;
   my $sum_v = 0;
   foreach $i (0 .. $#{$arr_ref} ) { $sum_v += $arr_ref->[$i];  }
   return $sum_v;
}
sub find_long_strain {  # arg= $Deal_ref, return suit number 0 .. 3 with the longest fit
   my $dl_ref = shift;
   my ( $max_fit, $long_strain ) = (-1 , 4 );
   for ($i=3 ; $i>= 0; $i-- ) {  # start with spades.
      print STDERR "find_long_Strain::Testing suit[$i] vs $long_strain and $max_fit aka $$dl_ref{FIT_LEN}[$i] [$Debug gt $req_dbg +2} \n" if $Debug > $req_dbg +2  ;
      if ($dl_ref->{FIT_LEN}->[$i] > $max_fit ) {
         $long_strain = $i ;
      }
   }
   return $long_strain ;
}

sub set_long_strain {
   my $dl_ref = shift;
   local $req_dbg = $req_dbg + 1;
   @sorted_fits = &sort_arr_idxes($$dl_ref{FIT_LEN});
   $idx_top = $sorted_fits[3];
   $idx_2nd = $sorted_fits[2];
   print STDERR "set_long_strain:: two longest fits are suit#[$idx_top] and suit#[$idx_2nd] [$Debug gt $req_dbg +2} \n" if $Debug > $req_dbg +2  ;
   if ($$dl_ref{FIT_LEN}->[$idx_top] > $$dl_ref{FIT_LEN}->[$idx_2nd] ) { return $idx_top ; }
   # two fit lengths must be equal. Check their DOP_DFIT values and choose the fit with the most DOP_DFIT pts.
   if ($$dl_ref{DOP_DFIT}->[$idx_top] >= $$dl_ref{DOP_DFIT}->[$idx_2nd]  ) { return $idx_top; }
   else { return $idx_2nd; }
} #end sub set_long_strain

sub sort_arr_idxes {       # sort the indexes such that they return array items in increasing order.
   sub byidx { $aref->[$a] <=> $aref->[$b]; }
   $aref = shift;
   my $topidx = $#{$aref};
   local $req_dbg = $req_dbg + 1;
   my @sortedidx = sort byidx ( 0 .. $#{$aref} );
   print STDERR "sort_arr_idx:: input=[@{$aref} ], ouput=[@sortedidx] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   return @sortedidx ;
}

#
# some length subs
sub suit_strlen {       # called with a string like strlen but handles void --- suits
   $_ = $_[0];
   s/-//g;  # turn a void into a zero length string
   tr///c;  # return count of non null chars
}

#------------------------------------------------------------------------------------------------------------
# debugging subs. Poor man's Data Dumper
sub show_Hand_arrays {        # arg=Hand_ref; shows the suit related DOP for this hand.
   my $hand_ref = shift;
   my $hand_type = ref($hand_ref);
   foreach $kn ( DOP_MISFIT, DOP_WASTE, DOP_DFIT, DOP_HF ) {
   $arr_ref = $hand_ref->{$kn};  show_arr("$hand_type Array $kn ", $arr_ref);
   }
} # end sub show_Hand_Arrays

sub show_DOP_hash { # passed in a hash descr/name, hashref, prints out sorted keys and values
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
                  show_arr("[$h_key] sub-Array ", $h_ref->{$h_key} ) ;
                  print  STDERR "..................................................\n";
               }
               elsif ($ref_t eq "HASH") {
                  show_hash("[$h_key] sub-Hash ", $h_ref->{$h_key} ) ;
                  print STDERR "..................................................\n";
               }
            }
            else        { printf STDERR ("%30s  => %30s\n", $h_key, $h_ref->{$h_key});    }
        } # end foreach key
} #end sub show_DOP_hash

sub show_hash_defs {    # prints out only those keys that are defined. ignores existing that are undef.
   my $msg = shift;
   my $h_ref = shift;
   my ($h_key, $ref_t);
   $ref_t = ref $h_ref;
   printf STDERR "<--------Hash %12s of type %10s has %6s buckets  --------->\n", $msg, $ref_t, scalar(%{$h_ref});
   foreach $h_key (sort keys %{$h_ref} ) {
       if ( defined ($h_ref->{$h_key})) {printf STDERR ("%30s  => %30s\n", $h_key, $h_ref->{$h_key}) } ;
   } # end foreach key
} #end sub show_hash_defs
sub show_hash {         # passed in a hash descr/name, hashref, prints out sorted keys and values
        my $h_name = shift @_;
        my $h_ref  = shift @_;
        my ($h_key, $ref_t);
        $ref_t = ref $h_ref;
        printf STDERR "<--------Hash %12s of type %10s has %6s buckets  --------->\n", $h_name, $ref_t, scalar(%{$h_ref});

        foreach $h_key (sort keys %{$h_ref} ) {
                printf STDERR "%30s  => %30s\n", $h_key, $h_ref->{$h_key};
        } # end foreach key
} #end sub show_hash
sub show_arr  {         # passed an array descr as a scalar string and an array_ref
        my $a_name = shift @_;
        my $idx = 0 ;
        my $a_ref = shift @_ ;
        my ($item, $s_item) ;
        # printf STDERR "a_ref points to: %s \n", ref($a_ref);
        # printf "STDERR Array[0] = %s \n", $a_ref->[0];
        printf STDERR "<------------Array %12s has %6d items --------------->\n", $a_name, scalar(@{$a_ref});
        foreach $item ( @{$a_ref} ) {
                $s_item = sprintf "%10.30s", $item;
                printf STDERR "\t %d = %-20.30s \n", $idx, $s_item ;
                $idx++;
        }  # end foreach item
}  #end sub show_arr
sub var_type {
   my $var_ref = shift;
   my $var_t;
   if (ref($var_ref) ) { $var_t = ref($var_ref) . "_ref" ; return $var_t ;}
   else {
      $v_ref = \$var_ref ;
      if (ref($v_ref) ) { $var_t = ref($v_ref) . "_var"; }
      else { $var_t = "UNKOWN" ; }
   }
   return $var_t;
   print STDERR "TYPE of var is $var_t\n" if $Debug >= $req_dbg;
} #end sub Var_type

sub trix_max {
   my $tmax = 0 ;  # trix will never be less than this.
   foreach my $t (@_) {
      $t = ($t eq 'A' ) ? 10 : ($t eq 'B' ) ? 11 : ($t eq 'C' ) ? 12 : ($t eq 'D' ) ? 13 : $t ;
      $tmax = $t if $t > $tmax;
   }
   return $tmax ;
}
sub trix_min {       # called with $trix_val = trix_max ( @strain_tricks) ;
   my $tmin = 13 ;  # trix will never be more than this.
   foreach my $t (@_) {
      $t = ($t eq 'A' ) ? 10 : ($t eq 'B' ) ? 11 : ($t eq 'C' ) ? 12 : ($t eq 'D' ) ? 13 : $t ;
      $tmin = $t if $t < $tmin;
   }
   return $tmin ;
}

sub usage {             #arg: 0 .. 4 level of verbosity in the help message.
my $verbose = shift;
print "Usage:: Version $MainVersion -- Verbosity = $verbose { $Debug ge 1 } \n" if $Debug >= 1;
if ($verbose == 0 ) {
   print <<"BRIEF";
Hint:  Type $0 -Hn where n is a number from 0 to 9 for more verbose help.
            $0 -Zn for an explanation of how values are calculated (n=0)
Usage:
 $0 [-h] [-Hn] [-OW|E] [-R<spec>] [-i infile_name ] [-o reportfile_name] -d<deal_num> -D<dbg_level> -R<DS>\\
    [-W<spades>/<hearts>/<diam>/<clubs>] [-E<spades>/<hearts>/<diam>/<clubs>] [-w<list>] [-e<list>] \\
    [-gY|N] [-G<gibset>] [-q] [-AN|S|D] [-lC|D|H|S] [-rC|D|H|S]
    The last three (-A, -l, -r) apply only to (adjusted) DOP when they open the bidding.

If neither W nor E hand is specified on the cmd line, the program attempts to read from stdin.
Type -H3 to get fmt of data coming from infile or stdin.
Type -H4 to get the fmt of the GIB (PBN) Deal file.
Type -H5 for how to Adjust DOP in Competition
....................................................................................................
BRIEF
} # end verbose 0
# end verbose 0
if ($verbose >= 1 ) {
   print <<"INTRO"
Hint:  Type $0 -Hn where n is a number from 0 to 9 for more verbose help.
            $0 -Zn for explanation of how values calculated (n=0)
Usage:
 $0 [-h] [-Hn] [-OW|E] [-R<spec>] [-i infile_name ] [-o reportfile_name] -d<deal_num> -D<dbg_level> \\
    [-W<spades>/<hearts>/<diam>/<clubs>] [-E<spades>/<hearts>/<diam>/<clubs>] [-w<list>] [-e<list>] \\
    [-gY|N] [-G<gibset>] [-q] [-AN|S|D] [-lC|D|H|S] [-rC|D|H|S]
    The last three (-A, -l, -r) apply only to (adjusted) DOP when they open the bidding.
If neither W nor E hand is specified on the cmd line, the program attempts to read from stdin.
Type -H3 to get fmt of data coming from infile or stdin.
Type -H4 to get the fmt of the GIB (PBN) Deal file.
Type -H5 for how to Adjust DOP in Competition

In the above cmd line invocation:
-O defines who opens the bidding, E or W. The default is W.
    This affects which hand can count shape points before a trump fit is found.
-R defines the type of report to produce. The Default is SN. Deal=Summary, Hand=None Other Combos SS, SD, DS, DD.
-W<string> and -E<string> define the cards in the West and East hands.
    Enter both hands or one hand only. If none is entered the program attempts to read from stdin.
-w<list> and -e<list> define the strains and declarer for which to produce an evaluation report.
   The default is nothing. <list> is a series of comma separated letters from the set [C D H S N L]
   The Strain evaluation gives more detail especially re the DFIT pts, and the number of tricks "expected".
-q specifies terse report. Returns 8 numbers, the DOP when playing in C, D, H, S, NT , L the longest fit. 
   and the Quick Losers when playing in a suit or NoTrump
----------------------------------------------------------------------------------------------------
INTRO
} # end verbose >=1
# end verbose >=1

if ($verbose >= 2 ) {
   print <<"HELPFUL"
In the above cmd line invocation: ...
-W<spades>/<hearts>/<diamonds>/<clubs> refers to the definition of the corresponding hand:
   This example : -WAJT23,567,KQ109,7  -EKxxx/a5Q6k10/--/qjXXXXX
   Demonstrates that:
   a) the order of the cards in a suit does not matter. It will however look funny in your reports.
   b) You can use either 10 or T to represent a Ten. 10 is converted to T which is used thereafter.
   c) You can enter in upper or lower case. Upper X's are converted to x, other lowers are converted to upper.
   d) you can enter voids with dashes
   e) you can use either slashes (/) or commas(,) to separate the suits.
   f) you can enter invalid hands. The program will error check and reject them.
Aside:
   g) When giving compass directions, you can enter S(south) as a synonym for W, and N(north) as a synonym for E.
      But W and E will show in reports.
      ditto for "s" as a synonym for "w" and "n" as a synonym for "e".

   <list> (in the terms -w<list> or -e<list>) is comma seperated letters chosen from N,S,H,D,C,L
   These letters represent the strain(s) that you want the program to report details about.
   Example -wN,L -eN,S,D
   The above says to evaluate the hands and produce reports for the following:
   a) East is Declaring NT, b) East is Declaring in Spades, c) East is Declaring in Diamonds
   d) West is Declaring NT, e) West is Declaring in the E/W Longest fit.
   If no Strain option is specified then the program defaults to no strain level detail.
   These Strain details are not crucial:
   both the Deal Summary, and the Hand Summary provide more detail than you really need,
   Not to mention the Deal Detail and Hand Detail report.
R=<spec>
   <spec> is the level of detail to be reported. It is one of: S|D|SS|SD|DS|DD
   Giving the level of detail reported for the deal (both hands) and/or Hand(s) individually as:
   i) S=Deal Summary,  ii) D=Deal Summary and Deal Detail
   iii) SS=Deal Summary and Hand Summary, iv) SD=Deal Summary and Hand Detail
   v) DS=Deal Summary and Detail plus Hand Summary  vi) DD=Summary and Detail for both Deal and Hand(s)
-q specifies terse report. Returns 8 numbers, the DOP when playing in C, D, H, S, NT , L the longest fit. 
   and the Quick Losers when playing in a suit or NoTrump
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
HELPFUL
} #end verbose >=2
#end verbose >=2
if ($verbose >= 3 ) {
   print <<"DETAIL"
Entering one or both hands from the cmd line is a quick and easy way to evaluate one deal or hand.
However if you want to evaluate a series of hands you might want to prepare a file of deals and
have it redirected to the program's STDIN.

   Example: DOP_eval -eS,H -wL,N -RDS <dealfile.dat >dealfile.rpt
or you can use the -i and -o options:
            DOP_eval -eS,H -wL,N -RDS -i dealfile.dat  -o dealfile.rpt

This tells the program to read the hands from: dealfile.dat write the reports to: dealfile.rpt
Note that if using STDIN you cannot specify ANY deal on the cmd line.
Options you enter on the cmd line become the default options unless overridden in the detail file.
The above example sets the default opener to be East (rather than West)
and the default East strains to be the majors, (rather than nothing), the default Report to be DS
These defaults can still be overriden on a deal by deal basis from the line in the data file.

********************************************************************************************
IMPORTANT NOTE: ********* Dealfile must have EOF as the last line in the file.  ************
if you do not do this the last deal line in the file will not be processed. (Mild Bug?)
********************************************************************************************

Here is a sample line from a deal data file:
d=6 W=AJ62,KT,AK963,A5 E=74/QJ984/Q/QJ632 O=W w=C,N e=H, R=DD T=Challenge problem 4.

(Note there are NO minus signs (-) before the 'switches'
instead the switches are followed by equal signs (=). i.e. d=6 NOT -d6 or -d=6

====================================================================================================
DETAIL
} # end $verbose >= 3
# end $verbose >= 3
if ($verbose >= 4 ) {
   print <<"GIBDEAL"
Analyzing Deals solved by GIB (or output in pseudo PBN fmt)
As a verification tool for the DOP method, we can refer to a Database of over 100K deals solved by the GIB DD solver.
We can then see how often DOP's estimated tricks compare to those that are actually possible on the given deal.
The -G switch gives a text string to identify the source of the GIB deal database.
      Will default to (gib)input_filename cleaned up so there are only 0-9A-Za-z also _ . + -  are allowed in filenames.
The -g switch says the infile is GIB fmt, and we want GIB_CSV output report.
The -o switch gives the output GIB_CSV report file name.
The -i switch gives GIB infile name.

This gibdeal file is NOT a full blown PBN file; rather it contains one line per deal, this line has a text string
giving the cards in each hand (Starting with WEST on the left), and 20 chars at the end of the line that specify
how many tricks the hand on lead can take in each of five strains. Example:

#GIB line (87 Chars) = T5.K4.652.A98542 K6.QJT976.QT7.Q6 432.A.AKJ93.JT73 AQJ987.8532.84.K:65658888888843433232
#                      S   H  D    C     S   H     D   C   S K  D     C     S      H   D C[.NT..Sp..He..Di..Cl.]
#                      <--- West------>  <--- North ---> <---- East-----> <--- South ---->[leader = s,e,n,w for each]
# suits separated by DOTs, Compass separated by SPACE , COLON marks end of cards, start of tricks.
# tricks are given for the hand on lead,  5 sets of 4 chars [0-9A-D]; 1st set = NT, then S,H,D,C; each set dealer=s,e,n,w

DOP does not care about the leader so this pgm just compares the number of tricks estimated by DOP
with the highest number in each strain. This program analyzes both sides, NS and EW since the data is available.
GIBDEAL
} # end $verbose >= 4


if ($verbose >= 5 ) {
   print <<"COMPETITION"
DOP in Competition (They Open): aka Adjusted Optimal Count.

   To evaluate a hand or pair of hands in competitive auctions you first evaluate the hands normally to come up with the DOP.
   Then you adjust the DOP for length or shortness and Lone Kings, Lone Jacks, and KQ combos  in their suits,
   The adjustments for length depend on whether Intervenor intends to intervene with NT OCall, T/O DBL, or suit OCall.
   You also adjust for Lone Kings and Lone Jacks in 3 or 4 card side suits; i.e. NOT their suits.
   A lone King or Lone Jack means no card higher than a 9 in that suit; i.e. KT(x) or JT(x) no Adjustment.

   Accordingly we have the extra cmd line arguments:
   -A[NSD] which specifies No AOP report, Summary AOP report, and Detail AOP report.
   -r[CDHSN] (RHO) and -l[CDHSN] (LHO) with the strain that they bid.
   i.e. -rz or -lz which states whether the Opener is on our right (we are behind him)  or left (he is over us).
   "z" is the strain he opened in: C,D,H,S,N (or c,d,h,s,n). So -rs RHO opened in Spades. -lN LHO opened in NT.
   It is possible that both RHO and LHO will be bidding different suits as in (1D) - P - (1S) - 2H
   Our RHO has bid spades and our LHO has bid Diamonds so we enter -rS, -lD on the cmd line.

   In competitive auctions "Opener" for our side is in fact, "Intervenor", but it is not worth going thru the whole program
   to change this. So in Competitive auctions "Opener" means the first hand to bid for our side.
   In order to keep the option of allowing any compass direction to be our Opener we don't specify the compass direction
   for their side; we just specify RHO or LHO relative to our side's first bidder.

COMPETITION
} # end $verbose >= 5

} # end usage
# end usage
#------------------------------------------------------------------------------------------------------------
## --                   # New subs Typically ones not debugged yet. --

sub useful_hint {
   my $verbose = shift ;
   if ($verbose == 0 ) {
   print <<"HINT0";
Hint:  Type $0 -Zn for coding documentation
Here is how the various 'Points' are grouped in the various reports.
------- Individual Components --------
H = Honor Pts on the DOP scale
L = Length pts on the DOP scale. Note that Responder can only claim a max of 2L pts, and no D pts
D = Opener's D pts on the DOP scale; stiff=2, void=4. two Doubletons = 1
    Responder cannot claim any D pts unless he becomes the long trump hand.
    If both hands have 5+ trumps, neither can count Dfit but both can count D.
Sy = Synergy points.
ADJ = Adjustments: -1A, -1K, -1Q, -1Sq, or +1Ax4, +1Qx4, +1Kx3, +2Kx4; At most 2 pts can be deducted per hand.
Hf = +1 for Jx(x), Qx(x), QJ(x), Kx(x) in any of partner's long suits.
FN = Fit length pts; +1 for each 8 fit, +2 for each 9 fit, +3 for each 10+ fit
Dfit = The points for shortness when holding 2-4 trumps. Can be counted by either hand; but only for 1 short suit.
         If you count Dfit you can't count D. Dfit = #trumps - short_suit_length
Dm = Misfit points for shortness (xx, or stiff, or void) in pard's 5+ suit.
Mirror = -2 for two mirror hands, -1 for hands with two mirror suits.
Waste = -3 for honors vs a void, -2 for honors vs a stiff, but not more than the honor is worth. AJx vs stiff = -1
NoWaste = +1 for Axx(x) vs a stiff, +2 for xxx(x) vs a stiff, +3 for xxx(x) vs void
NT Shortness Dedcution: Declarer = -3 points for a void, -2 points for a stiff. Dummy counts -2 and -1.
                        If there is more than one short suit, then deductions for both suits are counted.
---------------------------------------------------------------------------------------
********************* Groupings *******************************************************
HL = H + L + Sy + Adj on a per hand or per deal basis. (even if Adj is shown separately HL includes it.)
HLD = HL + Opener's D pts. (or Responder's D pts if he is long trump hand, or Opener bids NT)
         Note that Dpts often disappear to be replaced by Dfit pts. HLD used mostly for single hand evaluations.
HLF_NT = HL + Hf + FN - Dm - Mirror - Waste
HLF_Suit = HL + Hf + FN - Dm - Mirror - Waste + NoWaste
HLDF column when the strain is NT: HLF_NT - NT_Declarer_Shorteness_Deduction - NT_Dummy_Shortness_Deduction
HLDF column when the strain is a suit: HLF_Suit + Dfit + D(if any still kept)
*****************************************************************************************
HINT0
   } # end verbose 0
   # end verbose 0
   if ($verbose >= 1 ) {
   print <<"HINT1"
Nothing here, Try -Z0
----------------------------------------------------------------------------------------------------
HINT1
   } # end verbose 1
   # end verbose >=1
} # end useful_hint

1;                      # so require succeeds ...
