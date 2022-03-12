# No shebang since this file brought in by require.
## Author      Version  Date           Comment
#  JGM         1.0      2020/09/22     Subroutines for DOP evaluation hand stuff.
# <SNIP>
#  JGM        13.4      2021/04/03     Fixed Reports for better Waste/No Waste Reporting esp for NT.
#  JGM        20.0      2021/06/30     Added option -G to getopts for GIBDEALS file name.
#  JGM        21.0      2021/08/28     Mods for the -q cmd line option for terse mode for dealer (None so far)
#
package main ;
our $MyVersion = 21.0;
our @suit_symb = ('C', 'D', 'H', 'S' );

# General Environment subs
sub tell_myname {       # show which Subs file is loaded.
               # Used to check which file version is loaded.
   my $msg = shift ;
   print STDERR "Subs File" .  __FILE__ . "  Version: $MyVersion \n RxD MSG[: $msg :]\n";
   return $MyVersion;
}
sub get_dbg_lvl {       # pass in a ref to the %ENV hash which should have the key DEBUG defined.
   my $env_href = shift ;
   my $dbg = 0 ;        # 0 should not cause any dbg output and some code might not undestand undef
   $dbg = $env_href->{DEBUG} if defined $env_href->{DEBUG};
   ### Get DBG LVL is : $dbg
   return $dbg;
}

#------------------------------------------------------------------------------------------------------------
# Routines to setup critical Vars before all else
sub init_4suit_hashes {    # args: list of hand_hashrefs. calls make_suit_keys. Blesses suit Hashes
   local $req_dbg = $req_dbg + 1;
   my $hand_ref_t;
   while (@_) {                  # still some hand_hashref to do
      my $hand_ref = shift;
      $hand_ref_t = ref $hand_ref;
      print "init_4suit_hashes:: hand_type=$hand_ref_t {$Debug gt $req_dbg} " if ($Debug > $req_dbg );;
      for ($m=0; $m < 4; $m++ ){
            $suit_ref = get_suit_href($hand_ref, $m);
            print "init_4suit_hashes:: Hand[$hand_ref_t] SUIT[$Suit_Names[$m]] \n" if ($Debug > $req_dbg );
            make_suit_keys ($suit_ref);    # creates keys, sets defaults from the %default_suit hash.
            $suit_ref->{LET} = $suit_lets[$m];
            bless $suit_ref,$Suit_Names[$m];          # can always know which suit now.
            $junk = ref($suit_ref);
            print "init_suit_hashes:: Hand[$hand_ref_t] = $suit_ref->{LET} Hash blessed into $junk from $Suit_Names[$m] " .
                  "{$Debug ge $req_dbg +1}\n"   if ($Debug > $req_dbg + 1);
            # don't need to zero the keys because the default suit does that where needed.
      } # end for each suit.
   } #end while handrefs left
   return 1;
} #end init_4suit_hashes
sub make_suit_keys {       # called with suit href. inits the hash from @suit_keys and @suit_defaults
   my $s_ref = shift;
   %{$s_ref} = %default_suit;
} # end make_suit_keys
sub zero_suit_keys {       # called with suit href. This hash has no arrays in it. All scalars.
   my $s_ref = shift;
   my $ref_t = ref $s_ref;
   foreach $s_key ( keys %{$s_ref} ) { $s_ref->{$s_key} = 0; }
   $s_ref->{LET} = substr($ref_t, 0, 1);    #suit hash was blessed into class "Clubs", or "Diamonds" etc.
   $s_ref->{STR} = "";
   #### Zero_SUIT_Keys Done...
} #end zero suit sub

sub reset_hand_hash {      # arg=hand_hashref Uses Global Vars. Resets both Hand_W and Hand_E hashes to init state.
   local $req_dbg = $req_dbg + 1;
   my ($reset_ref_t,$reset_ref) ;
   while (@_) {                  # still some hand_hashref to do
      $reset_ref = shift;
      $reset_ref_t = ref $reset_ref;
      print "reset_hand_hash:: Calling zero_hand_hash for hand_type=$reset_ref_t { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
      &zero_hash($reset_ref);          # Everything to zero (except SUITS key)

      #Now setup the alphas to more descriptive defaults
      my $h_letter ="W";
      if ( ($reset_ref_t =~ m/E/) )    {
         $reset_ref->{NAME} = "East";
         $reset_ref->{LET} = "E";
         # should not need this next one.
#  @{$reset_ref->{SUITS}} = (\%Clubs_E, \%Diams_E, \%Hearts_E, \%Spades_E ) ; # re-use same arrays.
         $h_letter = "E"; }
      else {               # no match E assume West.
         $reset_ref->{NAME} = "West";
         $reset_ref->{LET} = "W";
         # should not need this next one.
#  @{$reset_ref->{SUITS}} = (\%Clubs_E, \%Diams_E, \%Hearts_E, \%Spades_E ) ; # re-use same arrays.
      }  # end if -- else reset_ref_t
      if ( ($h_letter eq $df_opener) ) {$reset_ref->{OPEN} = "Y"; }
      else                             {$reset_ref->{OPEN} = "N"; }
      if ( $Debug >= $req_dbg ) {
         $dbg_suit_t = ref ($reset_ref->{SUITS}->[2] );
         print "reset_hand_hash:: For Hand Type $reset_ref_t, has NAME = $reset_ref->{NAME} and " .
           " suit #2 type = $dbg_suit_t {$Debug ge $req_dbg }\n"  ;
      }  # end if Debug
      #reset the string scalars
      $reset_ref->{NT_Misfit} = "N";
      $reset_ref->{STR} = "";

   } # end while hand refs
   return 1
} # end reset_hand_hash

sub zero_4suit_hashes  {   # Called with a list of hand_hashrefs. Sets to zero. Does not bless or create keys.
   local $req_dbg = $req_dbg + 1;
   my $hand_ref_t;
   while (@_) {                  # still some hand_hashref to do
      my $hand_ref = shift;
      $hand_ref_t = ref $hand_ref;
      for ($m=0; $m < 4; $m++ ){
            $suit_ref = get_suit_href($hand_ref, $m);
            print "zero_4suit_hashes:: Hand[$hand_ref_t] SUIT[$Suit_Names[$m]] {$Debug ge $req_dbg +2}\n" if ($Debug > $req_dbg + 2);
            &zero_suit_keys($suit_ref);
      } # end for each suit.
   } #end while handrefs left
   return 1;
} #end init_4suit_hashes

sub zero_deal_hash {       # reset the deal hash to zeros and default strings. calls zero_hash
   my $dl_ref = shift;
   local $req_dbg = $req_dbg + 1;
   my $prev_deal_num = $$dl_ref{DEAL_NUM};
   print STDERR "\nzero_deal_hash:: Zeroing Deal Hash for prev deal = $prev_deal_num {$Debug >= $req_dbg} \n" if $Debug >= $req_dbg;
   # put zeroes into all the keys.
      &zero_hash ($dl_ref);
   # now set the alpha keys to some more descriptive defaults.
      $$dl_ref{DESCR} = "" ;
      $$dl_ref{DS_E} = "" ;
      $$dl_ref{DS_W} = "" ;
      $$dl_ref{OPENER} = "" ;
      $$dl_ref{STR} = "" ;

      $$dl_ref{DEAL_NUM} = $prev_deal_num + 1 if $prev_deal_num > 0 ;


   print STDERR "zero_deal_hash:: Deal Number[$dl_ref->{DEAL_NUM}]  {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg ;
   print STDERR "zero_deal_hash:: DOP_FN_TOT=$$dl_ref{DOP_FN_TOT}, DOP_FN ARRAY= @{ $dl_ref->{DOP_FN} } {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg ;
   &show_DOP_hash("zero_deal_hash:: Deal Hash Reset",    $dl_ref)             if $Debug >= ($req_dbg + 3);
} #end sub zero_deal_hash

sub zero_hash {            # Generic routine to put zeros to scalars and array elements in a hash.
   local $req_dbg = $req_dbg + 2;  # straight fwd routine not a lot of debug output needed.
   while (@_) {
   my $hash_ref = shift;
   my ($hkey, $hvalue, $hvalue_t );
   while (($hkey, $hvalue ) = each %{$hash_ref}) {
      $hvalue_t = ref $hvalue ;
      next if $hkey =~ m/SUIT/ ;  #dont zero out pointers.
      if ($hvalue_t eq "ARRAY" ) {
         printf  STDERR "zero_hash:: Reset $hkey of type ARRAY and current value= %s to zeroes {$Debug ge $req_dbg } \n",
                     join ":", @{$hash_ref->{$hkey}} if $Debug >= $req_dbg +2 ;
         @{$hash_ref->{$hkey}} = (0) x @{$hash_ref->{$hkey}};
      }
      else {
         printf  STDERR "zero_hash:: Reset $hkey of type SCALAR and current value=[$$hash_ref{$hkey}] to 0 {$Debug ge $req_dbg } \n"
                  if $Debug >= $req_dbg +2;
         $$hash_ref{$hkey} = 0 ; # must be numeric scalar
      }
   } #end while each key,value
 } #end while hashrefs to do
} # end sub zero_hash

sub zero_AOC_hash {       # process a list of AOC hash ptrs. Set values of scalars and arrays to 0. Preserve the 'Descriptive' keys.
   local $req_dbg = $req_dbg + 1;
   while (@_) { # process a list of pointers.
      $aoc_href = shift;
      $aoc_htype = ref $aoc_href ;
      $aoc_letter =  $aoc_href->{AOC_LET} ;
      print  STDERR "\nzero_AOC_hash:: Zeroing AOC Hash of type $aoc_htype {$Debug >= $req_dbg} \n" if $Debug >= $req_dbg;
      # put zeroes into all the keys.
      while (($hkey, $hvalue ) = each %{$aoc_href}) {
         $hvalue_t = ref $hvalue ;
         if ($hvalue_t eq "ARRAY" ) {
             printf  STDERR "zero_hash:: Reset $hkey of type ARRAY and current value= %s to zeroes {$Debug ge $req_dbg } \n",
                     join ":", @{$aoc_href->{$hkey}} if $Debug >= $req_dbg +2 ;
            @{$aoc_href->{$hkey}} = (0) x @{$aoc_href->{$hkey}};
         }
         else {
            # next if $hkey =~ m/LET/ ;
            printf  STDERR "zero_hash:: Reset $hkey of type SCALAR and current value=[$$aoc_href{$hkey}] to 0 {$Debug ge $req_dbg } \n"
                  if $Debug >= $req_dbg +2;
            $$aoc_href{$hkey} = 0 ; # Will fix the letter later.
         }
      } #end while each key,value

      # now set the alpha keys to some more descriptive defaults.
      $aoc_href->{AOC_LET} = $aoc_letter ;

      &show_DOP_hash("zero_aoc_hash:: AOC Hash Reset",    $aoc_href)             if $Debug >= ($req_dbg + 3);
   } #end while pointer list
} #end sub zero_aoc_hash


#------------------------------------------------------------------------------------------------------------
# Some parsing routines for hand entry and cleanup ready to populate suit_hash{STR}
sub setup_ds {   # called with E|W , <ds_string>. returns comma separated list of valid ds letters.
                 # when called with a null string it should return the default value for that compass direction.
my $ew = shift;                                 #compass direction in question
my $ds = shift;                                 # string to fixup.
local $req_dbg = $req_dbg + 1;

if     ($ew eq "W" ) { $ds_rslt = $get_opts{OPT_w}; }    # set result for west to the west default.
elsif  ($ew eq "E" ) { $ds_rslt = $get_opts{OPT_e}; }    # set result for east to the east default.
print  STDERR "setup_ds:: Hand_let=[$ew], Input_ds=[$ds], ds_rslt=[$ds_rslt] {$Debug ge $req_dbg + 3 }\n" if $Debug >= $req_dbg + 3;
if ($ds ne "" ) {  # don't waste time on null entries
   $ds = uc $ds;
   $ds =~ tr/LNCDHS//cd ;                          # delete any invalid chars. delete commas also. will put back later.
   print  STDERR "setup_ds:: Compass=[$ew] -- cleaned ds =[$ds] \n" if $Debug >= $req_dbg + 3;
} #end ds ne null
if  ( $ds  ne "" ) {                               # an invalid input might be null at this point.
   $ds_rslt=join q(:), split //,$ds;               # return usable ds string if it got squashed to say NLS
}
print  STDERR "setup_ds:: Compass=[$ew] returning[$ds_rslt] {$Debug ge $req_dbg + 3 }\n" if $Debug >= $req_dbg + 3;
return ($ds_rslt) ;
} # end setup_ds

sub setup_opener { # called with opener string; Cleans and validates. Returns single letter either E or W. Caller to save in %Deal.
                   # Note this opener is only valid for this deal. Does not affect defaults set by cmdline or init subs.
   my $O_op = shift;
   local $req_dbg = $req_dbg + 1;
   print  STDERR "setup_opener:: IN=[$O_op] " if $Debug >= ($req_dbg + 1);
   $O_op =~ tr/WEweNSns/WEWEEWEW/;                    # translate W->W, E->E, S-> W, N-> E. Others kept not txlated.
   $O_op =~ tr/WE//cd;                                # delete any that are not W or E. May wind up with more than one W and/or E.
   if ( $O_op ne "" ) {$O_op = substr $O_op,0,1 }     # keep only first char of whatever is left.
   else               {$O_op = $df_opener; }          # make sure we return something valid.
   print  STDERR " Cleaned O_op=[$O_op] { $Debug ge $req_dbg + 1 }\n"  if $Debug >= ($req_dbg + 1);
   return ($O_op);
} #end sub setup_opener

sub do_squash { #removes redundant spaces and commas in any combination.
   $_ = shift;
   s/^\s*//g;     # remove leading spaces
   s/\s*$//g;     # remove trailing spaces
   s/\s+/ /g;     # squash consecutive spaces to one space
   s/,\s*,/,/g;   # squash consecutive commas with or without intervening space to one comma
   s/\s+,/,/g;    # remove leading spaces before a comma
   s/,\s+/,/g;    # remove trailing spaces after a comma
   s/^\s*,//g;    # delete leading commas
   s/\s*,$//g;    # delete trailing commas
   return ($_);
} # end sub do_squash

sub cleanup_hand {   # first stage of cleanup. upper case (except x) 10->T, strip spaces, separate with comma, keep only 4 suits etc.
   my $hnd_str = shift;
   local $req_dbg = $req_dbg + 1;
   print  STDERR "cleanup_hand::  Doing Cleanup for hand = $hnd_str { $Debug >= $req_dbg }\n" if $Debug >= $req_dbg;
   if ($hnd_str eq "") { return ("") ; }  # dont try to process null hand.
    $hnd_str =~ s/10/T/g;                 # convert all occurrences of 10 to T.
    $_ = uc($hnd_str);                    # convert all letters to upper case.
    $_ =~ tr/AKQJT98765432xX\-/,/cs;;     # convert any non valid hand char to one comma; This is why periods work as suit separators
                                          # (/cs is complement the search list, and squash repeats to one)
    $_ = do_squash($_);                   # remove redundant spaces and commas, if any still left.  Dashes are tricky
    tr/X/x/;                              # Now put the x's back to lower case
    s/-+/---/g;                           # convert one or more dashes to exactly 3 dashes.
    # There might still be some redundant x's and -'s in the hand string. e.g. xxx,xxx,xxx,xxxx ---- xxx
    m/([^,]+),([^,]+),([^,]+),([^,]+)(.*$)/; # collect text before first 3 commas and then to 4th comma or EOL if no 4th
    $hnd_str = "$1,$2,$3,$4";    #keep only the first four chunks separated by commas. dont switch order yet.
    print  STDERR "cleanup_hand:: Returns[$hnd_str] { $Debug >= $req_dbg }\n" if $Debug >= $req_dbg;
    return ($hnd_str) ;    # caller should put cleaned string into the hand hash.
}
sub split_hand    {  # Split into suits. Expected input: spades,hearts,diamonds,clubs; should be clean input by now.
                     # COMMAs between suits are required at this point. Some suits might be 3 dashes
    my $hnd_str = shift ;
    my (@s_arr, $j_str) ;
    local $req_dbg = $req_dbg + 1;
    $_ = $hnd_str;

    print  STDERR "split_hand::  Splitting hand = [$hnd_str ] into suits {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
    #@_=split /,/;  #create @_ with individual elements $_[0], etc.
    #print  STDERR "split_hand:: creates: $_[0], $_[1], $_[2], $_[3] \n" if $Debug > $req_dbg ;
                                                # We expect Comma separated suits at this point.
    m/([^,]+),([^,]+),([^,]+),([^,]+)(.*$)/;    # manual split. we only want the first 4 terms.
    @_ = ($1, $2, $3, $4) ;
    print  STDERR "split_hand:: Manually, creates: $_[0], $_[1], $_[2], $_[3] {$Debug gt $req_dbg} \n" if $Debug > $req_dbg ;
    # create very clean hand string for later printout and analysis
    $hnd_str = join '/',@_ ;
    $hnd_str =~ tr/-/-/s;     # squash adjacent dashes to 1 dash.
    $hnd_str =~ s/-/---/g;    # replace one dash with 3 for pretty printout.
    print  STDERR "split_hand:: Rebuilt Hand_STR=$hnd_str {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;

    # create suit strings that are easy to use.
    $_[0] =~ s/-//g;             # convert a void (-, --, etc.) to an empty string, so length is zero.
    $_[1] =~ s/-//g;             # suits stored in suit hashes can be zero len. Hand hash{STR} can have ---.
    $_[2] =~ s/-//g;
    $_[3] =~ s/-//g;

    @s_arr = reverse @_;            #convert from user enter order, to internal pgm order (Clubs=suit[0])
    printf  STDERR "split_hand:: result = [%s] {$Debug ge $req_dbg}\n", join ':',@s_arr if $Debug >= $req_dbg;
    return (@s_arr, $hnd_str) ;           # return suit strings. Caller to put into Suit hashes.
}
sub chk_hand_valid { # called with a hand_ref. To be valid, a Hand must be exactly 13 cards. And also valid suits.
   my $hnd_ref = shift;
   my ($num_cards, $tst_str, $hand_t);
   local $req_dbg = $req_dbg + 1;
   $hand_t = ref($hnd_ref);
   $num_cards = 0;

   for ($m=0; $m < 4; $m++ ) { # check each suit in turn.
      $suit_ref = get_suit_href($hnd_ref, $m) ;
      $tst_str = $suit_ref->{STR};
      print  STDERR "chk_hand_valid :: calling valid_suit for Suit# $m, [$tst_str] {$Debug gt $req_dbg +1}\n" if $Debug > ($req_dbg + 1 )   ;
      if ( !(valid_suit($suit_ref)) ) {
         printf STDERR "chk_hand_valid:: for %s. Sorry. This suit: %s has a duplicate card. Fix and Retry\n", $hand_t, $suit_ref->{STR} ;
         return 0;
      }
      $num_cards += &suit_strlen( $tst_str );
      print  STDERR "chk_hand_valid :: $hand_t Suit # $m [$suit_ref->{STR}] cards running total = $num_cards {$Debug gt $req_dbg +1}\n" if $Debug > ($req_dbg+1);
    } # end for each suit.

     if ($num_cards != 13) {
      print  STDERR "chk_hand_valid:: for $hand_t. Sorry, this hand has $num_cards cards.  Fix and Retry\n";
      return 0 ;
   }  #end cards != 13
    else { return 1 };
}   #end chk_hand_valid
sub valid_suit {     # called with suit ref from chk_hand_valid.
   my ($s_ref, $s_cpy, $s_str);
   $s_ref = shift;
   local $req_dbg = $req_dbg + 1;
   $s_str = $s_ref->{STR};
   $s_cpy = $s_str;
   $s_cpy = ($s_str eq "" ) ? "---" : $s_str;
   print  STDERR "valid_suit:: Validating suit string[ $s_cpy ]\n" if $Debug > $req_dbg ;
   if (&suit_strlen($s_str) == 0 ) { print  STDERR "Suit is a VOID; Always valid\n" if $Debug > $req_dbg ; return 1;  }  # void suits always valid

   foreach $s_card ( @card_names ) {  #card names does not include x or - so these can be repeated OK.
      $s_cpy = $s_str;
      my $cnt = $s_cpy =~ s/$s_card/$s_card/g || 0;
#     my $kards = eval "qw($s_cpy) =~ tr/$s_card//; " ;   # alternate to previous line.
      return (0) if $cnt > 1 ;  # invalid suit if more than 1 card of same value in it.
   } #end foreach card
   return 1;
} #end valid_valid suit;

#------------------------------------------------------------------------------------------------------------
# User Input routines.
#        A) cmdline input one deal only.

sub get_opts {    #sets some vars, and returns ( cleaned up $h_W, $h_E,  and defaults for: ds_W, ds_E,&  Opener )
                  # user can enter a west : east pair on the cmd line, OR via stdin.
   my $opt_href = shift;
   local $req_dbg = $req_dbg + 1;
   my ($h_W, $h_E, $ds_w, $ds_e, $O_op, $R_op, $H_op, $q_op)  = ("","","","", "", 0, "");
   my ($in_fname, $rpt_fname) = ("", "");
   my ($lho_suit, $rho_suit, $aoc_rpt) = ("","","");  #initialize the AOC stuff to nothing.

   print  STDERR "get_opts::  hE:W:N:S:e:w:n:s:O:R:i:o:H:D:d:l:r:A:Z:g:G:q with Debug=$Debug and req_dbg =$req_dbg \n" if $Debug >= $req_dbg;
   $O_op = "W";                        # assume Opener is West.
   $R_op = "SN";                       # assume Deal Summary and no Hand report as the default.

   getopts('hE:W:e:w:N:n:S:s:O:R:i:o:H:D:d:l:r:A:Z:g:G:q');           # parse cmd line switches
   # I think that a call to getopts like: getopts("......",\%get_opts) would fill in get_opts hash automatically.

   if (defined($Getopt::Std::opt_D) ) { $Debug = $Getopt::Std::opt_D; }
   if (defined($Getopt::Std::opt_H) ) { $H_op = $Getopt::Std::opt_H; }  # show usage in various levels of detail.
   if (defined($Getopt::Std::opt_h) ) { $h_op = $Getopt::Std::opt_h; }  # terse usage msg.
   if (defined($Getopt::Std::opt_E) ) { $h_E =  $Getopt::Std::opt_E; }
   if (defined($Getopt::Std::opt_W) ) { $h_W  = $Getopt::Std::opt_W; }
   if (defined($Getopt::Std::opt_e) ) { $ds_e = $Getopt::Std::opt_e; }
   if (defined($Getopt::Std::opt_w) ) { $ds_w = $Getopt::Std::opt_w; }
   if (defined($Getopt::Std::opt_N) ) { $h_E =  $Getopt::Std::opt_N; }     # allow flex in compass dirs
   if (defined($Getopt::Std::opt_S) ) { $h_W  = $Getopt::Std::opt_S; }
   if (defined($Getopt::Std::opt_n) ) { $ds_e = $Getopt::Std::opt_n; }
   if (defined($Getopt::Std::opt_s) ) { $ds_w = $Getopt::Std::opt_s; }
   if (defined($Getopt::Std::opt_O) ) { $O_op = $Getopt::Std::opt_O; }     # Default Opener. Usually West.
   if (defined($Getopt::Std::opt_R) ) { $R_op = $Getopt::Std::opt_R; }     # Reporting level of detail.
   if (defined($Getopt::Std::opt_i) ) { $in_fname  = $Getopt::Std::opt_i;  }
   if (defined($Getopt::Std::opt_o) ) { $rpt_fname = $Getopt::Std::opt_o;  }
   if (defined($Getopt::Std::opt_l) ) { $lho_suit = $Getopt::Std::opt_l;  }
   if (defined($Getopt::Std::opt_r) ) { $rho_suit = $Getopt::Std::opt_r;  }
   if (defined($Getopt::Std::opt_A) ) { $aoc_rpt  = $Getopt::Std::opt_A;  }
   if (defined($Getopt::Std::opt_d) ) { $deal_num = $Getopt::Std::opt_d; }
   if (defined($Getopt::Std::opt_g) ) { $gib_sw   = $Getopt::Std::opt_g; }
   if (defined($Getopt::Std::opt_G) ) { $df_gib_pfx = $Getopt::Std::opt_G; }
   if (defined($Getopt::Std::opt_q) ) { $q_op = 'Y' ;   }
   print  STDERR "get_opts:: Getopts::Std Result E=[$h_E], W=[$h_W  ], e=[$ds_e] w=[$ds_w], O=[$O_op], D=$Debug { $Debug >= $req_dbg }\n"
         if $Debug >= $req_dbg;

   if ( defined($h_op) )               { &usage(0)      ;                    exit -5 ; }  # 1 is tersest level.
   if ( defined($Getopt::Std::opt_H) ) { &usage($Getopt::Std::opt_H) ;       exit -6 ; }  # various verbosity levels.
   if ( defined($Getopt::Std::opt_Z) ) { &useful_hint($Getopt::Std::opt_Z) ; exit -7 ; }  # various verbosity levels.
   ($df_rpt_deal, $df_rpt_hand) = &set_rpt_spec($R_op);  # returns two letters, one for Deal rpts, one for Hand rpts
   $rpt_deal=$df_rpt_deal;
   $rpt_hand=$df_rpt_hand;
   $$opt_href{OPT_RD} = $rpt_deal;
   $$opt_href{OPT_RH} = $rpt_hand;
   print  STDERR "get_opts:: Getopts::Std Result2 R_op[$R_op], Deal_Rpt[$rpt_deal], Hand_Rpt[$rpt_hand] { $Debug ge $req_dbg }\n"
         if $Debug >= $req_dbg;
   $O_op = &setup_opener($O_op);                      # returns either W or E.
   if ( ($h_W eq "") && ($h_E eq "") || (defined $Getopt::Std::opt_i)  ) {             # user must be planning on stdin ...
      print STDERR "get_opts:: cmdline switches done. Will input hands from $Getopt::Std::opt_i \n" if $Debug > 1;
   } #end both W & E are null.
   elsif ( ($h_W   eq "" ) && ($h_E ne "" ) ) {       # SWAP E and W if W is null and E is not.
      $h_W   = $h_E;
      $h_E = "";
      if ( $ds_e ne "" ) { $ds_w = $ds_e ; $ds_e="";} # since we switched hands must also switch the contracts
      if ( $O_op eq "E") { $O_op = "W" ; }            #   and also swap the opener
      print  STDERR "get_opts:: East and West hands SWAPPED { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
   } # null checking W & E

   # check the  -l and -r options will return a suit the opp opened in or 'N' or Null string.
   $$opt_href{OPT_l} = &aoc_chk_suit($lho_suit); $$opt_href{OPT_r} = &aoc_chk_suit($rho_suit);
   if ($$opt_href{OPT_l} eq 'N' && $$opt_href{OPT_r} eq 'N' ) { $$opt_href{OPT_A} = 'N' ; }
   else  { $$opt_href{OPT_A} = &aoc_chk_rpt($aoc_rpt); }  #only report AOC if there is a valid opposing opener.

   # set the hash to hold the cmd line switches. Keep the return list for backwards compatibility.
   $$opt_href{OPT_W} = $h_W;                    # either "" or something from -W switch
   $$opt_href{OPT_E} = $h_E;                    # either "" or something from -E switch
   $$opt_href{OPT_w} = &setup_ds('W', $ds_w);  # make sure the run defaults make sense.
   $$opt_href{OPT_e} = &setup_ds('E', $ds_e);
   $$opt_href{OPT_O} = $O_op;
   $$opt_href{OPT_RD}= $df_rpt_deal;
   $$opt_href{OPT_RH}= $df_rpt_hand;
   $$opt_href{OPT_i} = $in_fname;
   $$opt_href{OPT_o} = $rpt_fname;
   $$opt_href{OPT_l} = $lho_suit;
   $$opt_href{OPT_r} = $rho_suit;
   $$opt_href{OPT_A} = $aoc_rpt;
   $$opt_href{OPT_d} = $deal_num;   #may be undef?
   $$opt_href{OPT_g} = ( uc($gib_sw) || 'N');
   $$opt_href{OPT_G} = $df_gib_pfx;
   $$opt_href{OPT_q} = $q_op;
   # if its a gib report run, then over-ride the otherwise default report options.
   if ($$opt_href{OPT_g} ne 'N' ) { $$opt_href{OPT_RD} = "G"; $$opt_href{OPT_RH}= ""; }
   &show_DOP_hash("In Getopts:: Final GetOpt Result", $opt_href) if $Debug >= $req_dbg+1 ;

   return ($h_W, $h_E, $ds_w, $ds_e, $O_op) ;  # keep for compatibility. all info should be in the hash.
} # end sub get_opts

sub set_rpt_spec {  #arg: two letter string SS, SD etc. Returns two letters.
   $R_op = shift;
   local $req_dbg = $req_dbg + 1;
   # Setup Reporting options
   my ($deal_spec, $hand_spec);
   print  STDERR "set_rpt_spec:: R-op=[$R_op], Getopt::Std::opt_R = $Getopt::Std::opt_R ($Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   $R_op = uc($R_op);
   if    ( $R_op eq "S" ) { $deal_spec = "S" ; $hand_spec = "N" ; }     # No hand report
   elsif ( $R_op eq "D" ) { $deal_spec = "D" ; $hand_spec = "N" ; }
   elsif ( $R_op eq "SS") { $deal_spec = "S" ; $hand_spec = "S" ; }
   elsif ( $R_op eq "SD") { $deal_spec = "S" ; $hand_spec = "D" ; }
   elsif ( $R_op eq "DS") { $deal_spec = "D" ; $hand_spec = "S" ; }
   elsif ( $R_op eq "DD") { $deal_spec = "D" ; $hand_spec = "D" ; }
   elsif ( $R_op eq "DX") { $deal_spec = "D" ; $hand_spec = "X" ; }     # eXtensive hand rpt. each suit reported...
   else                   { $deal_spec = "S" ; $hand_spec = "N" ; }     # if nothing specified use Deal Summary, no Hand.
   return ($deal_spec, $hand_spec);
} #end sub set_rpt_spec

# Used to pre-process the raw hand strings into a more standard form. UC, 10->T, no redundant spaces etc.
# Updates the hand hash with the cleaned up string. Returns a code depending on which hands are not NULL.
# Does not count cards or do other validation at this stage.
sub Check_raw_hand_strings {     # returns 0=Both Null, 1=W not Null, 2=E not Null, 3=Both not Null.
      my $hnd_hrefW = shift ;
      my $hnd_hrefE = shift ;
      local $req_dbg = $req_dbg + 1;
      my $h_W = $hnd_hrefW->{STR};     # the
      my $h_E = $hnd_hrefE->{STR};
      print  STDERR "Check_raw_hand_strings:: Clean up West/South=[$h_W  ] and East/North=[$h_E] { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
      do {$h_W = "" ; $hnd_hrefW->{STR} = "" ; } unless $h_W =~ m/[^- ]/ ; #hand with all dashes/spaces is like a null string
      do {$h_E = "" ; $hnd_hrefE->{STR} = "" ; } unless $h_E =~ m/[^- ]/ ;
      if ( ($h_W eq "") && ($h_E eq "") ) { return 0 ; }       # common if user is redirecting  STDIN

      $h_W  = cleanup_hand($h_W  );  # we still need to check for valid suits, and 13 cards total.
      $$hnd_hrefW{STR} = $h_W;
      $h_E = cleanup_hand($h_E);
      $$hnd_hrefE{STR} = $h_E;
      print  STDERR "chk_hand_strings:: Clean West/South=[$h_W ] and East/North=[$h_E]{$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      if    (($h_W eq "") && ($h_E eq "") ) { return 0 ; } # no hand strings on cmd line. use stdin.
      elsif  ($h_E eq "")                   { return 1 ; } # West only.
      elsif  ($h_W eq "")                   { return 2 ; } # East only
      else                                  { return 3 ; } # Both East and West
 } # end chk_hand_strings. Valid strings in %Hand_W and %Hand_E This sub returns a status code 0, 1, 2, or 3.

sub get_stdin {            # get deal lines from stdin. return line or "EOF(stdin)"
            #W=<s>,<h>,<d>,<c> E=<s>,<h>,<d>,<c> [ w=<west strains>] [e=<east strains>] [d=<dealNum>] [R=S|D] [T:<descr text>]
            # strains are one or more of: N,S,H,D,C,L(longest fit). T= should be last one on the line.
   my $filehandle = shift;   # caller calling with stdin or with an opened filehandle.
   my $line;
   local $req_dbg = $req_dbg + 1;
   print  STDERR "get_line:: entered with lineno already at $. { $Debug ge $req_dbg +4 } \n" if $Debug >= ($req_dbg + 4);

LINE: while ( <$filehandle> ) { #while auto puts input into $_; we skip blank lines and comments.
      local $. ;
      chomp;
      $line = $_;
      next LINE if /^\s*#/;      # skip comment lines
      next LINE if /^$/;         # skip null lines
      next LINE if /^\s+$/;      # skip blank lines
      print  STDERR "getline::  Lineno:[$.] is[$line] { $Debug ge $req_dbg } \n"  if $Debug >= $req_dbg;
      last LINE ;                # break out of loop if a data line found.
   }  # end while LINE
   return ( ( eof($filehandle) ) ? "EOF(INFILE)" : $line );
}  #end get_stdin

#
# Next Sub is Version 10 sub. Previous one used substrings rather than match operator and Regexes
#
sub parse_stdin {    # called with raw input line  parsed Fields returned as a list to caller.
                     # @line_keys =  ( "W=", "E=", "w=", "e=", "d=", "O=", "R=", "T=") defined in VarDefns file.
                     # order on line does not matter (except that T= should be the last one).
                     # results are returned in the order given by @line_keys, again T= should be last one.
                     # Version 13: added code to return results in hash %Deal_line. Also to parse lho_suit, rho_suit, aoc_rpt
  # this routine does not seem to do any error checking of the various (non hand ) options ????
   my $in_str = shift ;
   my $dl_ptr = shift ;  # ptr to %Deal_Line
   my (@deal_fields);
   local $req_dbg = $req_dbg + 1;

   # @line_keys =  ( "W=", "E=", "w=", "e=", "d=", "O=", "R=", "T=");
  @line_keys = qw( W E w e d O R l r A T );      #vers 13/up includes Adjusted Opt Count flags for AOC rpts.
   $in_str =~ tr!/! !s;  #translate slashes to space and squash. Don't txlate : or = as the user may be using them.
   $in_str =~s/n=/e=/; $in_str =~s/s=/w=/; $in_str =~s/N=/E=/; $in_str =~s/S=/W=/;  #convert S to W and N to E. allows user flex.
   #$in_str = $in_str  . "%";
   #print  STDERR "parse_stdin:: Source:[$in_str] \n";
   foreach $key_let ( @line_keys ) {
       $key = $key_let . "=";                   #append an = sign since that is on input
       if ( $key eq "T=" ) {                    # T= needs maximal match to EOL
          if ($in_str =~ m/T=(.*)(%|$)/) { $tok = $1; }
          else { $tok = "" ; }
          #print  STDERR "Key $key [[ $tok]]\n";
       }
       else {                                   #minimal match to the next key
          if ($in_str =~ m/$key(.*?)([WEwedORTlrA]=)|$/ ) { $tok = $1; }
          else { $tok = "" ; }
      }
      $tok =~ s/\s*$//;                      # trim trailing spaces
      push @deal_fields,$tok;
      $$dl_ptr{$key_let} = $tok ;            #dont use the = sign in the key since $D{W=} gives an error
      #
   }  # end foreach key
   &show_DOP_hash("Deal Line Hash Result",    $dl_ptr) if $Debug >= ($req_dbg + 3);
   # print  STDERR ".......... returning last tok = $tok \n";
   return @deal_fields;
} # end sub parse_stdin

# Deal Reporting Subs. Show  totals for the Deal, for each Hand and for Each suit. depending on level.
#------------------------------------------------------------------------------------------------------------

sub Rpt_Deal {
   my ($dl_ref, $specs_ref ) = (@_); # hash with deal, hash with rpt_file, rpt_level, rpt_hdr etc.
   $rpt_type = $specs_ref->{RPT_DEAL_LVL};
   $hdr_yn   = $specs_ref->{RPT_DEAL_HDR};
   my $rptfile = $specs_ref->{RPTFILE};
   print STDERR "Rpt_Deal:: rpt_type[$rpt_type] with rptfile handle { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;

   $Rpt_Deal_Smry_Hdr0 = "\n{******* DOP Deal %7s Report Deal: %-4d****** QK_Losers}\n";
   $Rpt_Deal_Smry_Hdr1=" Deal# O  HLF_NT NT-D NT-Tx %  LS HLDFl LS-Tx %  [S,NT]<------------------- Hand ------------------>";
   $Rpt_Deal_Smry_Fmt1="[%04d]%2s  %5.2f %2d,%2d %2d %3d   %2s %5.2f %3d %3d   %d,%d   [%-43s]\n";
   $Rpt_Deal_Smry_Fmt2="[%04d] HL[%5.2f] ADJ[%d] HF[%d] FN[%d] Misfit[%2d] Mirror[%2d] Waste[%3.1f] noWASTE[%3d]\n   DFit_in: Sp[%d], He[%d], Di[%d], Cl[%d]  ";
   local $req_dbg = $req_dbg + 1;
   my $Title = "Summary";
   if ($rpt_type =~ m/D|d/ ) { $Title = "Detail "; }
   my $dl_str = "W= " . $$dl_ref{STR};
   $dl_str =~ s/::/ E=/;
   print STDERR "Rpt_Deal:: $Title, $rpt_type, $dl_str {$Debug >= $req_dbg} \n"  if $Debug >= $req_dbg;

   if ($hdr_yn !~ m/0|n|N/ ) {
      printf $rptfile  $Rpt_Deal_Smry_Hdr0,$Title, $$dl_ref{DEAL_NUM};
      print  $rptfile "$Rpt_Deal_Smry_Hdr1\n";
   }
   # The Deal Summary report includes the expected tricks and pct for NT and Long Strain
   # need to calculate who is declaring in NT. For Deal Analysis assume it is Opener.
   $nt_ded_total = &Calc_nt_ded_tot($dl_ref, $$dl_ref{OPENER} ) ;
   @nt_lvl   = &DOP_2_Trix($$dl_ref{DOP_HLF_NT}+$nt_ded_total, "N", $$dl_ref{QUICK_LOSERS_NT}); # N means NT not North
   @suit_lvl = &DOP_2_Trix($$dl_ref{DOP_HLDFl}, "S", $$dl_ref{QUICK_LOSERS_SUIT});    # S means Suit not South
   $ls_let = $$dl_ref{LONG_STRAIN};
    printf $rptfile $Rpt_Deal_Smry_Fmt1, $$dl_ref{DEAL_NUM}, $$dl_ref{OPENER}, $$dl_ref{DOP_HLF_NT},
          $$dl_ref{DOP_NT_DED}->[0], $$dl_ref{DOP_NT_DED}->[1], $nt_lvl[0],$nt_lvl[1],
          $ls_let, $$dl_ref{DOP_HLDFl}, $suit_lvl[0],$suit_lvl[1], $$dl_ref{QUICK_LOSERS_SUIT},$$dl_ref{QUICK_LOSERS_NT},$$dl_ref{STR} ;
   if ($rpt_type =~ m/D|d/ ) {
      printf $rptfile $Rpt_Deal_Smry_Fmt2, $$dl_ref{DEAL_NUM},$$dl_ref{DOP_HL},$$dl_ref{ADJ_TOT},$$dl_ref{DOP_HF},$$dl_ref{DOP_FN_TOT},
          $$dl_ref{DOP_MISFIT}, $$dl_ref{DOP_MIRROR}, $$dl_ref{DOP_WASTE}, $$dl_ref{DOP_noWASTE}, #LF here...
          $$dl_ref{DOP_DFIT}->[3], $$dl_ref{DOP_DFIT}->[2],$$dl_ref{DOP_DFIT}->[1],$$dl_ref{DOP_DFIT}->[0]; # No LF here
      if ($$dl_ref{DOP_FN_TOT} > 0 ) {
         printf $rptfile "FNs=> S[%2d] H[%2d] D[%2d] C[%2d]",
                $$dl_ref{DOP_FN}->[3],$$dl_ref{DOP_FN}->[2],$$dl_ref{DOP_FN}->[1],$$dl_ref{DOP_FN}->[0];
      }
     printf $rptfile "\n";
      if ($$dl_ref{DESCR} ne "" ) {
         printf $rptfile "    Descr:      %-50s\n\n", $$dl_ref{DESCR};
      }
      else  {printf $rptfile "\n" ; }
   } # end rpt_type m/D|d/

} #end sub Rpt_Deal
#end sub Rpt_Deal

# Hand Reporting. Summary and Detail (Suit Breakdown)
sub Rpt_Hand {          # $hand_ref, S/D detail, show_hdr=y/n, show_nt y/n
   my ($hnd_ref, $rpt_lvl, $hdr_yn, $nt_yn, $deal_num) = (@_);
   local $req_dbg = $req_dbg + 1;
   if ( $file_specs{RPT_FILE_OK} != 0 ) {
      print STDERR "Rpt_Hand:: Selecting RPTFILE OK[$file_specs{RPT_FILE_OK}] {$Debug ge $req_dbg} \n" if  $Debug >= $req_dbg ;
      $rptfile = $file_specs{RPTFILE};
   }
   print STDERR "Rpt_Hand:: Check Caller: $$hnd_ref{NAME}, Lvl=$rpt_lvl, hdr=$hdr_yn, NT=$nt_yn {$Debug gt $req_dbg} \n" if $Debug >= $req_dbg;
   &show_DOP_hash( "Rpt_Hand:: Hash", $hnd_ref ) if $Debug > $req_dbg + 2;

   # DOP_HL includes H, +L, +Sy, +K34 +Q4  minus deductions for (Sq, noQ, noK, noA)

   my($h_pts, $l_pts, $d_pts, $sy_pts, @adj, $adj_tot, $ded_tot, $tot_pts,
      $nt_flag, $nt_decl, $nt_dummy, $bidder, $bid_let);
   my $Rpt_Header="Dir  O/R DOP HLD   <------- Hand ------->   H   L   D[*]  Sy   <------- ADJ ------- Tot>  HF  Dm  W\n";
   my $Rpt_Fmt   ="%-4s %-2s   %-6.2f   %-22s %-6.2f%-3d %-3s %5.2f   Sq=%2d A=%2d K=%2d Q=%2d %3d %3d %3d%3d\n";
      $deal_num = (defined $deal_num) ? $deal_num : -1 ;
      $rpt_title = ($rpt_lvl =~ m/D|d/ ) ? "Detailed" : "Summary";
      $h_pts   = $hnd_ref->{DOP_H};
      $l_pts   = $hnd_ref->{DOP_L};
      $sy_pts  = $hnd_ref->{DOP_Sy};
      @adj=($hnd_ref->{Sq}, $hnd_ref->{No_A}, $hnd_ref->{No_K}, $hnd_ref->{No_Q}, $hnd_ref->{K_3_4}, $hnd_ref->{Q_4} );
      print STDERR "Rpt_Hand:: Sq=[$adj[0]], -A$adj[1], -K$adj[2], -Q$adj[3], +K$adj[4] +Q$adj[5] {$Debug ge $req_dbg +3}\n" if $Debug > ($req_dbg +1);
      $adj_tot = $hnd_ref->{ADJ_TOT};              # Kings bonus not part of deductions. so Sq, -Q, -A <= 2 always.
      $nt_flag = $hnd_ref->{NT_Misfit} ;

   if ($hnd_ref->{OPEN} eq "Y") {
      $bidder  = "Opener" ; $bid_let ="O";
      $d_pts   = $hnd_ref->{DOP_D};
      $tot_pts = $hnd_ref->{DOP_HL} + $d_pts;
      $d_star_pts  = "$hnd_ref->{DOP_D}" . " "; #String with NO asterisk
   }
   else {
      $bidder = "Responder"; $bid_let ="R";
      $d_pts = 0;                # Resp never counts D unless opener opens in NT!!
      $tot_pts = $hnd_ref->{DOP_HL};
      $d_star_pts = "$hnd_ref->{DOP_D}" . "*"; #String with asterisk
   }
   $nt_decl = $tot_pts - $d_pts + ( ($nt_flag eq "V") ? $HAND_DOP{NT_VOID} : ( ($nt_flag eq "S" ) ? $HAND_DOP{NT_STIFF} : 0 ) ) ;
   $nt_dummy = $nt_decl + (($nt_flag eq "N") ? 0 : $HAND_DOP{NT_DUMMY} );
   # summary only
   if ( $hdr_yn =~ m/y|Y|1/ ) {
      printf $rptfile "\n[----- DOP $rpt_title HAND EVALUATION REPORT for Deal# %-4d -----] \n", $deal_num;
      printf $rptfile "$Rpt_Header";
   }
   $hnd_hf = &sum_suit($$hnd_ref{DOP_HF});
   $hnd_Dm = &sum_suit($$hnd_ref{DOP_MISFIT});
   $hnd_W =  &sum_suit($$hnd_ref{DOP_WASTE});

   print STDERR "Rpt_Hand:: $$hnd_ref{NAME}, Totals  HF=$hnd_hf, Dm=$hnd_Dm, W=$hnd_W  { $Debug ge $req_dbg} \n" if $Debug >= $req_dbg ;
   printf $rptfile "$Rpt_Fmt", $$hnd_ref{NAME}, $bid_let,  $tot_pts, $$hnd_ref{STR}, $h_pts, $l_pts, $d_star_pts, $sy_pts,
         $adj[0], $adj[1], ($adj[2] + $adj[4]), ($adj[3]+$adj[5]), $adj_tot, $hnd_hf, $hnd_Dm, $hnd_W;

   if ( $nt_yn =~ m/y|Y|1/ ) {  # Show the 'NT effect'
      print $rptfile "          DOP in NT for $hnd_ref->{NAME}: Declaring[$nt_decl]  Dummy[$nt_dummy]\n" if ( $nt_flag ne "N" || $d_pts > 0 );
   } # Show the NT effect
   if ($rpt_lvl =~ m/D|d/ ) { &Rpt_Suits($hnd_ref, $deal_num, "Y"); } # show suit breakdown for Detailed hand report.
} # end sub Rpt_Hand
# end Rpt_Hand

sub Rpt_Suits {   # called by Rpt_Hand if hand detail wanted. The FN numbers per suit are in the DEAL hash, since not hand specific
   my ($hnd_ref, $deal_num, $hdr_yn ) = (@_);
   $Rpt_suit_fmt="[%04d] %2s %2s  %6.2f%3d %3d %4.2f %3d%3d%3d%3d [%-13s]\n";
   $Rpt_suit_hdr=" Deal   Suit     H    L   D  Sy   HF Dm  W LEN STR \n";
   print STDERR "Rpt_Suits Called with deal_num[$deal_num] { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;

   print $rptfile "$Rpt_suit_hdr" if ($hdr_yn =~ m/Y|y/);

   foreach $nsuit ( 0 .. 3 ) {
      $sptr = $$hnd_ref{SUITS}->[$nsuit];
      $suit_let = $$sptr{LET};
      $suit_len = $$sptr{LEN};
      $suit_str = $$sptr{STR};

      printf $rptfile $Rpt_suit_fmt, $deal_num,$$hnd_ref{LET}, $suit_let, $$sptr{DOP_H}, $$sptr{DOP_L},$$sptr{DOP_D},$$sptr{DOP_Sy},
                     $$hnd_ref{DOP_HF}->[$nsuit],
                     $$hnd_ref{DOP_MISFIT}->[$nsuit], $$hnd_ref{DOP_WASTE}->[$nsuit],
                     $suit_len, $suit_str ;
   } # end foreach nsuit
} # end Rpt_Suit
# end Rpt_Suit

sub Rpt_Strain {     #args: deal_ref, strains_str, W|E declarer
   my ($dl_ref, $strains, $declarer, $rpt_strain_hdr ) = (@_) ;
   local $req_dbg = $req_dbg + 1;

   my $Rpt_strain_hdr = " Num Decl HLDF  Strain Tricks %  [ H+L+Sy  HF+FN  Dfit  Misfit, Mirror Waste noWaste NT-Ded]\n";
   my $Rpt_strain_fmt = "[%04d]%2s [%6.2f]%3s  [%3d][%3d] [ %6.2f   %3d   %3d     %3d    %3d   %4.1f    %3d   %3d ]\n";

   $strains = uc ($strains);
   $strains =~ tr/CDHSNL://cd ;  # Should be a well fmted string. single letters, colon separated.
   print STDERR "Rpt_Strain:: Strains[$strains], Decl[$declarer], Header[$rpt_strain_hdr] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   &show_DOP_hash ("RPT_STRAIN::Deal Hash - { $Debug >= $req_dbg}", $dl_ref )  if $Debug >= $req_dbg + 1 ;
   @strains = split /:/, $strains ; #split into individual letters.
   $nt_ded_tot = &Calc_nt_ded_tot($dl_ref, $declarer);
   $rpt_fn = $$dl_ref{DOP_FN_TOT} + $$dl_ref{DOP_HF} ;

   print $rptfile $Rpt_strain_hdr if ( $rpt_strain_hdr =~ m/Y|y/ ) ;
   foreach $st_let (@strains) {
      $st_nsuit = $st_let;
      $st_nsuit =~ tr/CDHSLN/012345/;  # "suits" 4 and 5 don't exist but use for Long and NT.
      $rpt_st_let = $st_let ;
      if ( $st_let eq "N" ) {
         $rpt_hldf = $$dl_ref{DOP_HLF_NT} + $nt_ded_tot ;
         @trix = &DOP_2_Trix($rpt_hldf, "N", $$dl_ref{QUICK_LOSERS_NT});
         $rpt_dfit = 0;
          printf $rptfile $Rpt_strain_fmt, $$dl_ref{DEAL_NUM},$declarer,$rpt_hldf,$rpt_st_let,$trix[0],$trix[1],
             $$dl_ref{DOP_HL}, $rpt_fn, $rpt_dfit, $$dl_ref{DOP_MISFIT}, $$dl_ref{DOP_MIRROR},
             $$dl_ref{DOP_WASTE}, 0 ,$nt_ded_tot ; # noWASTE never counted in NT; neither is Dfit
         next ; # done with this 'suit'
      }
      # Strain is a SUIT - either 'Long' or specific C,D,H,S
      if ( $st_let eq "L" ) {
            $st_nsuit = &set_long_strain($dl_ref);
            $rpt_st_let = "L-" . $suit_lets[$st_nsuit] ;
         }
     $rpt_dfit = $$dl_ref{DOP_DFIT}->[$st_nsuit];
     $rpt_hldf = $$dl_ref{DOP_HLDF_SUIT}->[$st_nsuit];
     print STDERR "RPT_STRAIN:: suit=$st_nsuit, dfit=$rpt_dfit, HLF_NT=$$dl_ref{DOP_HLF_NT}, HDLF_result= $rpt_hldf \n" if $Debug >= $req_dbg;;
     @trix = &DOP_2_Trix($rpt_hldf, "S", $$dl_ref{QUICK_LOSERS_SUIT});
     printf $rptfile $Rpt_strain_fmt, $$dl_ref{DEAL_NUM},$declarer,$rpt_hldf,$rpt_st_let,$trix[0],$trix[1],
             $$dl_ref{DOP_HL}, $rpt_fn, $rpt_dfit, $$dl_ref{DOP_MISFIT}, $$dl_ref{DOP_MIRROR},
             $$dl_ref{DOP_WASTE}, $$dl_ref{DOP_noWASTE}, 0 ; # no NT deduction if playing in a suit.
   } # end foreach strain letter
   print STDERR "Rpt_Strains Done for $declarer with rptfile output { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
#  my $Rpt_strain_hdr = " Num Decl Strain  HLDF   Tricks   %   [ H+L+Sy  HF+FN  Dfit Waste Mirror Misfit, NT-Ded ] \n";
} # end sub Rpt_Strain
# end sub Rpt_Strain

sub Terse_Rpt {  #\%Deal, \%Hand_W, \%Hand_E, $get_opts{OPT_A}, $valid_hw ,  $valid_he
  my $deal_href = shift ;
  my $dop_n ;
  local $req_dbg = 1 + $req_dbg ;
  print STDERR "TERSE REPORT CALLED: $$deal_href{NAME}, $$deal_href{STR}, O=$$deal_href{OPENER}, HLF=$$deal_href{DOP_HLF} {$Debug ge $req_dbg}\n"
         if $Debug >= $req_dbg;
  $dop_n = $$deal_href{DOP_HLF_NT} + &Calc_nt_ded_tot($deal_href, $$deal_href{OPENER} ) ;
  # printf  ("The DOP pts for dealer: ");  - future after testing fscanf
  # This next line is the real output. Might be a problem if the DOP are not integers. Can Dealer print Decimals?
  printf ("%5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n", @{$$deal_href{DOP_HLDF_SUIT}}, $dop_n,
                $$deal_href{DOP_HLDFl} ,  $$deal_href{QUICK_LOSERS_SUIT},$$deal_href{QUICK_LOSERS_NT} );
  # Some extra printout here for debugging only.

  printf STDERR ("TERSE_RPT:: FRL=%5.2f,NT_FRL=%5.2f,SRL=%5.2f,NT_SRL=%5.2f,QuickLosers=[SUIT=%5.2f,NT=%5.2f]{$Debug ge $req_dbg}\n",
            $$deal_href{FRL}, $$deal_href{NT_FRL},$$deal_href{SRL},$$deal_href{NT_SRL},
            $$deal_href{QUICK_LOSERS_SUIT}, $$deal_href{QUICK_LOSERS_NT} )
          if $Debug >= $req_dbg;
  return 1 ;

} # end Terse_Rpt
1; # so require succeeds
