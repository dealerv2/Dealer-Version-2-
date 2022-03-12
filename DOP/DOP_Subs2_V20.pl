#File DOP_Subs2_Vxx.pl
## Author      Version  Date           Comment
#  JGM         20.0      2021/07/03    New / extra subs for refactor and GIB analysis
sub Start_Main {  # populate get_opts hash with defaults, other hashes as req'd, bless hashes for debug , ...
   #called from main; needs all the require files to run

   package main ;
   local $req_dbg = $req_dbg + 1 ;
   # Initialize a few data structures
   bless \%Hand_W, "Hand_West";   # Blessing these hashes allows for easier debugging
   bless \%Hand_E, "Hand_East";
   bless \%Hand_N, "Hand_North";
   bless \%Hand_S, "Hand_South";

   bless \%Deal_NS,  "Deal_NS";
   bless \%Deal,     "Deal_EW";
   bless \%GIB_Deal, "Deal_GIB" ;
   bless \%EW_Results, "EW_Results";
   bless \%NS_Results, "NS_Results";

   bless \%AOC_DOP_W, "AOC_West";
   bless \%AOC_DOP_E, "AOC_East";
   bless \%AOC_DOP_DEAL, "AOC_Deal";

   foreach $suit (0..3) {
      print STDERR "Start_Main:: Blessing Suit[$suit] aka $Suit_Names[$suit] x4 { $Debug gt $req_dbg }\n" if $Debug > $req_dbg;
         bless $Suits_N[$suit], "$Suit_Names[$suit]" . "_N";
         bless $Suits_S[$suit], "$Suit_Names[$suit]" . "_S";
         bless $Suits_E[$suit], "$Suit_Names[$suit]" . "_E";
         bless $Suits_W[$suit], "$Suit_Names[$suit]" . "_W";
   }
   if ($Debug >= $req_dbg ) {
      printf STDERR ("Start_Main::  Hand hashes blessed: W=[%s],E=[%s],S=[%s],N=[%s],Deal=[%s],Deal_NS=[%s] { $Debug ge $req_dbg }\n",
         ref(\%Hand_W),ref(\%Hand_E),ref(\%Hand_S), ref(\%Hand_N), ref(\%Deal), ref(\%Deal_NS)) if $Debug >= $req_dbg ;
      printf STDERR "Start_Main:: Suit hashes blessed: C_W= %s, D_E= %s, H_N= %s, S_S= %s { $Debug ge $req_dbg }\n",
                  ref(\%Clubs_W), ref(\%Diams_E), ref(\%Hearts_N), ref(\%Spades_S), if $Debug >= $req_dbg ;

      print STDERR "Start_Main:: non-hand GETOPTS; R=[$get_opts{OPT_R}],O=[$get_opts{OPT_O}],DS_W=[$ds_west],DS_E=[$ds_east],",
         "i=[$get_opts{OPT_i}],o=[$get_opts{OPT_o}],Debug=[$Debug], w=[$get_opts{OPT_w}], e=[$get_opts{OPT_e}] d=[$get_opts{OPT_d}]\n" ;
      print STDERR "Start_Main:: hand GETOPTS W=[$h_west] <==> E=[$h_east]\n" ;
      print STDERR "Start_Main:: Competition GETOPTS::A=[$get_opts{OPT_A}] l=[$get_opts{OPT_l}], r=[$get_opts{OPT_r}] \n" ;
   } # end if $Debug > 0

   printf STDERR "Start_Main::  Calling Init_suit_hashes {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   # Set each of 16 suits to default suit; then fix LET and STR ; then Bless suit into e.g. Hearts_S
   &Init_suit_hashes(\%Hand_N, \%Hand_E, \%Hand_S, \%Hand_W);   #

   #check that the suit hashes have been blessed OK.
   printf STDERR "Start_Main::Clubs_N=%s, Diams_E=%s, Hearts_S=%s, Spades_W=%s {$Debug ge $req_dbg}\n",
      ref(\%Clubs_N), ref( \%Diams_E), ref( \%Hearts_S),  ref( \%Spades_W) if $Debug >= $req_dbg;

   printf STDERR "Start_Main:: Calling Init_hand_hashes {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   &Init_hand_hashes(\%Hand_N, \%Hand_E, \%Hand_S, \%Hand_W ) ;   # calls sub reset_hand_hashes

   if ($Debug >= $req_dbg) {
      foreach $hnd_href (\%Hand_N, \%Hand_E, \%Hand_S, \%Hand_W ) {
         $href_t = ref($hnd_href);
         print STDERR "Start_Main::Checking for hand $href_t LET=[$hnd_href->{LET}], NAME=[$hnd_href->{NAME}] ";
         if ( ($hnd_href->{LET} eq 'W') or ( $hnd_href->{LET} eq 'S') ) {
                print STDERR "Hand should OPEN = $hnd_href->{OPEN} { $Debug ge $req_dbg }\n"  ; }
         else { print STDERR "Hand DO NOT OPEN = $hnd_href->{OPEN}  { $Debug ge $req_dbg }\n" ; }
      } # end foreach hnd_href
   } #end if $debug

   # Note %get_opts always contains valid defaults,
   # because the get_opts routine sets the hash values whether or not there was a switch on the cmd line.

# Setup the Hand strings to treat them same as other important stuff from cmd line. May need considerable cleanup.
   $Hand_W{STR} = $get_opts{OPT_W} ;   #either "" or something from the -W switch.
   $Hand_E{STR} = $get_opts{OPT_E} ;   #either "" or something from the -E switch.
   $GIB_Deal{DEAL_PFX} = $get_opts{OPT_G}; #G_sw, or $df_gib_pfx or $pgm_defaults{DF_GIB_PFX}
   $df_opener = $get_opts{OPT_O}; $default_deal{OPENER} = $df_opener;
   $Deal{OPENER} = $df_opener; $Deal_NS{OPENER} = $df_opener;
   if ($df_opener eq "E" ) {$Hand_E{OPEN}="Y"; $Hand_W{OPEN}="N"; $Hand_N{OPEN}="Y"; $Hand_S{OPEN}="N";}
   else                    {$Hand_W{OPEN}="Y"; $Hand_E{OPEN}="N"; $Hand_S{OPEN}="Y"; $Hand_N{OPEN}="N";}
   $ds_west = setup_ds("W", $get_opts{OPT_w});                   # if OPT_w is null or invalid it returns sane default.
   $ds_east = setup_ds("E", $get_opts{OPT_e});                   # ditto for OPT_e
   $default_deal{DS_W}  = $ds_west ; $default_deal{DS_E} = $ds_east ;
   $get_opts{OPT_w}     = $ds_west;  $get_opts{OPT_e}    = $ds_east;  #save these as run defaults if needed.
   $default_deal{DEAL_NUM}= (defined $get_opts{OPT_d}) ? $get_opts{OPT_d} : 0 ;
   @dd_dop_diff_count = (0,0,0);
   $CSV_rowcount = 0;

   # default deal has all values set to zeros.
   #Use copy sub bec there are pointers to arrays that need array copy not pointer copy.
   printf STDERR "Start_Main:: Calling Init_Deal_hashes { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
   push @STK,$req_dbg; $req_dbg = $req_dbg + 2;

   &copy_DOP_hash(\%Deal,    \%default_deal);         # create keys and initialize to zero
   &copy_DOP_hash(\%Deal_NS, \%default_deal);

   $file_specs{RPT_DEAL_LVL} = $get_opts{OPT_RD}; #level of deal report is OPT_RD
   $file_specs{RPT_HAND_LVL} = $get_opts{OPT_RH}; #level of hand report is OPT_RH

   # The GIB related hashes, %NS_Results, %EW_Results, %GIB_DEAL are setup correctly at compile time.
   #
   $req_dbg = pop @STK;
# all init done; all amounts set to zeros. Default Hashes setup so that copying them to actual hashes inits correctly.
   &check_setup() if $Debug >= $req_dbg+2 ;

   return 1 ;

} # end Start_Main


 # GIB2DOP (N_hptr, S_hptr, W_hptr, E_hptr, Gib_hptr) copies Hand and suit strings from GIB parsed deal to DOP hashes
sub GIB2DOP {
   my $href_n = shift ; my $href_s = shift ;
   my $href_w = shift ; my $href_e = shift ;
   my $gib_ref = shift ;

   local $req_dbg = $req_dbg + 1;
   print STDERR "IN GIB2DOP ... $Debug ge $req_dbg\n" if $Debug >= $req_dbg;
   print STDERR "GIB_W=$gib_ref->{W_GIB_STR}, W=$href_w->{NAME} { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg ;
   print STDERR "GIB_N=$gib_ref->{N_GIB_STR}, N=$href_n->{NAME} { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg ;
   print STDERR "GIB_E=$gib_ref->{E_GIB_STR}, E=$href_e->{NAME} { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg ;
   print STDERR "GIB_S=$gib_ref->{S_GIB_STR}, S=$href_s->{NAME} { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg ;
   # No need to change keys NAME, LET, SUITS, OPEN,
   # mainly need to setup the STR and the suits so the eval can proceed ...
   my @sptrs_n = &get_sptrs($href_n);
   my @sptrs_s = &get_sptrs($href_s) ;
   my @sptrs_w = &get_sptrs($href_w);
   my @sptrs_e = &get_sptrs($href_e) ;

   # copy over the hand string and each suit string; re-ordering as needed.
   $href_n->{STR} = $gib_ref->{N_GIB_STR} ;
   $href_n->{STR} =~ tr!.!/! ;  #convert dot separators to slashes
   #&show_arr("GIB North Suits Array", $$gib_ref{N_suits} );
   $sptrs_n[0]->{STR} = $$gib_ref{N_suits}->[3] ; # clubs in DOP is 0;  clubs  in GIB is 3 ;
   $sptrs_n[1]->{STR} = $$gib_ref{N_suits}->[2] ; # diams in DOP is 1;  diams  in GIB is 2 ;
   $sptrs_n[2]->{STR} = $$gib_ref{N_suits}->[1] ; # hearts in DOP is 2; hearts in GIB is 1 ;
   $sptrs_n[3]->{STR} = $$gib_ref{N_suits}->[0] ; # spades in DOP is 3; spades in GIB is 0 ;

   #&show_arr("GIB South Suits Array", $$gib_ref{S_suits} );
   $$href_s{STR} = $$gib_ref{S_GIB_STR};
   $$href_s{STR} =~ tr!.!/! ;  #convert dot separators to slashes
   $sptrs_s[0]->{STR} = $$gib_ref{S_suits}->[3] ; # clubs in DOP is 0;  clubs  in GIB is 3 ;
   $sptrs_s[1]->{STR} = $$gib_ref{S_suits}->[2] ; # diams in DOP is 1;  diams  in GIB is 2 ;
   $sptrs_s[2]->{STR} = $$gib_ref{S_suits}->[1] ; # hearts in DOP is 2; hearts in GIB is 1 ;
   $sptrs_s[3]->{STR} = $$gib_ref{S_suits}->[0] ; # spades in DOP is 3; spades in GIB is 0 ;

   #&show_arr("GIB West Suits Array", $$gib_ref{W_suits} );
   $$href_w{STR} = $$gib_ref{W_GIB_STR};
   $$href_w{STR} =~ tr!.!/! ;  #convert dot separators to slashes
   $sptrs_w[0]->{STR} = $$gib_ref{W_suits}->[3] ; # clubs in DOP is 0;  clubs  in GIB is 3 ;
   $sptrs_w[1]->{STR} = $$gib_ref{W_suits}->[2] ; # diams in DOP is 1;  diams  in GIB is 2 ;
   $sptrs_w[2]->{STR} = $$gib_ref{W_suits}->[1] ; # hearts in DOP is 2; hearts in GIB is 1 ;
   $sptrs_w[3]->{STR} = $$gib_ref{W_suits}->[0] ; # spades in DOP is 3; spades in GIB is 0 ;

   #&show_arr("GIB East Suits Array", $$gib_ref{E_suits} );
   $$href_e{STR} = $$gib_ref{E_GIB_STR};
   $$href_e{STR} =~ tr!.!/! ;  #convert dot separators to slashes
   $sptrs_e[0]->{STR} = $$gib_ref{E_suits}->[3] ; # clubs in DOP is 0;  clubs  in GIB is 3 ;
   $sptrs_e[1]->{STR} = $$gib_ref{E_suits}->[2] ; # diams in DOP is 1;  diams  in GIB is 2 ;
   $sptrs_e[2]->{STR} = $$gib_ref{E_suits}->[1] ; # hearts in DOP is 2; hearts in GIB is 1 ;
   $sptrs_e[3]->{STR} = $$gib_ref{E_suits}->[0] ; # spades in DOP is 3; spades in GIB is 0 ;
   #&show_arr("GIB East Suits Array", $$gib_ref{E_suits} );

   # Setup the hands as being valid for the momment
   # if we wanted to check them for validity we can do that before now in the gib parse part.
   # GIB2DOP should deliver hands that  are ready for evaluation.
   foreach $href ($href_n, $href_s, $href_w, $href_e ) { $$href{Valid} = 1 ; }
   return 1 ;
} # end sub GIB2DOP

sub split_gib_hand {
   my $hnd_str = shift;
   my ($snum, @suits) ;
   $hnd_str=~m/^(\w*)\.(\w*)\.(\w*)(\.(\w*))?/;
   print STDERR "split_gib_hand:: $hnd_str :: [$1] ; [$2] ; [$3] ; [$4] ; [$5] { $Debug ge $req_dbg } \n" if  $Debug >= $req_dbg;
   @suits = ($1,$2,$3,$5) ;
   for ($snum=0; $snum < 4 ; $snum++ ) {
      if ($suits[$snum] eq "" ) { $suits[$snum] = "---" ; }
   }
   return (@suits);
} #end split_gib_hand

# Parses a line read from gibdeal file and fills the GIB_Deal hash with hands{STR} suits{STR} and tricks
# The deal line consists of the hands string and the trix sting separated by a colon.
# The hands (W,N,E,S) are separated from each other by space.
# The suits (S,H,D,C) are separated from each other by DOTs, most of the time.
# The hands string can be quite irregular if there are void suits. ".H.D" for example if S and C are void.

sub parse_gibline {        # (\%GIB_Deal, $deal_line, $gib_lineno)
   my $gib_href = shift ;
   my $gibline = shift ;
   my $line_no = shift ;
   local $req_dbg = $req_dbg + 1 ;
   $_ = $gibline ;
   chomp;
   $_ = uc $_ ;
   if ( m/^\s*([0-9AKQJTakqjt .:]{13,})/ )    { $gib_str = $1 ; }
   elsif ( m/^\s*#/) { next ; }  # Skip comments
   elsif ( m/^SETID=(\w{3,})/ ) { $pfx=$1 ; $df_gib_pfx = $pfx ; next; } #Save the pfx for future deals
   elsif ( m/^%(\w{3,})%\s*$/ ) { $pfx=$1 ; $df_gib_pfx = $pfx ; next; }
   elsif ( m/^%(\w{3,})%\s*([0-9AKQJTakqjt .:]{13,})/ ) { $pfx=$1 ; $gib_str = $2  }
   else { return -1 ; }  # silently ignore unknown lines

   if ($pfx ne "" ) {
      $df_gib_pfx = $pfx ;
      $gib_href->{DEAL_PFX} = $pfx ; # save this prefix for future deals also.
   }
   $gib_href->{DEAL_ID} = $gib_href->{DEAL_PFX} . sprintf(":%06d", $line_no);
   $gib_href->{DEAL_lineno} = $line_no;
   print STDERR "parse_gibline::$gib_href->{DEAL_ID} gibdeal = [$gib_str] {$Debug ge $req_dbg }\n" if $Debug >= $req_dbg;

   ($deal_str, $trix_str) = split /:/, $gib_str;
   print STDERR "parse_gibline::Deal= [$deal_str]\nTrix=[$trix_str]  {$Debug ge $req_dbg }\n" if $Debug >= $req_dbg;

   @hands = split / /, $deal_str ;
   &show_arr( "parse_gibline:: Array hands: The West,North,East,South Hands", \@hands) if $Debug >= $req_dbg;

   @w_suits = &split_gib_hand($hands[0]) ; $hands[0] = join ',',@w_suits; #commas are needed for further processing..
   @n_suits = &split_gib_hand($hands[1]) ; $hands[1] = join ',',@n_suits;
   @e_suits = &split_gib_hand($hands[2]) ; $hands[2] = join ',',@e_suits;
   @s_suits = &split_gib_hand($hands[3]) ; $hands[3] = join ',',@s_suits;
   foreach $hnd (0..3) {
      print STDERR "parse_gibline:: Hand[$hnd]=$hands[$hnd] { $Debug ge $req_dbg} +1\n" if $Debug >= $req_dbg+1;
   }
   &show_arr( "parse_gibline::West's Suits: ",  \@w_suits) if $Debug >= $req_dbg+1;
   &show_arr( "parse_gibline::North's Suits: ", \@n_suits) if $Debug >= $req_dbg+1;
   &show_arr( "parse_gibline::East's Suits: ",  \@e_suits) if $Debug >= $req_dbg+1;
   &show_arr( "parse_gibline::South's Suits: ", \@s_suits) if $Debug >= $req_dbg+1;

   $gib_href->{RAW_STR} = $gibline ;
   $gib_href->{W_GIB_STR} = $hands[0] ;
   $gib_href->{N_GIB_STR} = $hands[1] ;
   $gib_href->{E_GIB_STR} = $hands[2] ;
   $gib_href->{S_GIB_STR} = $hands[3] ;
   @{$gib_href->{W_suits}} = @w_suits ; #array of text strings
   @{$gib_href->{E_suits}} = @e_suits ;
   @{$gib_href->{N_suits}} = @n_suits ;
   @{$gib_href->{S_suits}} = @s_suits ;
   # translate the trix string into the max # of tricks in each strain for compare with DOP
   $strain = 0 ;
   $trix_len = length($trix_str) ;
   for ($pos=0 ; $pos <= ($trix_len - 4 ) ;  ) {
      $strain_trix = substr($trix_str, $pos, 4) ;
      @trix_tmp = split / */,$strain_trix ;  # split into individual chars
      $ns_trix = &trix_max(@trix_tmp) ;
      $ew_trix = 13 - &trix_min(@trix_tmp) ;
      print STDERR "parse_gibline::Pos=[$pos], Strain=[$strain], " .
            "[$strain_trix] => NSMAX=[$ns_trix] EWMAX=[$ew_trix] { $Debug ge $req_dbg }\n"  if $Debug >= $req_dbg ;
      $gib_href->{NS_TRIX}->[$strain] = $ns_trix ;  #strain 0 is NT, strain 1 is spades.
      $gib_href->{EW_TRIX}->[$strain] = $ew_trix ;
      $pos += 4 ;
      $strain++ ;
   }  # end for pos=0
   print "\n" if $Debug >= $req_dbg;
   &show_DOP_hash("parse_gibline:: GIB Parse Results--{ $Debug ge $req_dbg }", $gib_href) if $Debug >= $req_dbg;
   return $gib_href->{DEAL_ID} ;   # should always be something
} # end parse gibline
# end parse gibline

sub show_suits {
   my $hnd_ref = shift ;
   my @suit_ptrs = &get_sptrs($hnd_ref);
   foreach $s (0 .. 3 ) {
      print STDERR "$$hnd_ref{NAME} Suit[$s] = $suit_ptrs[$s]->{STR} , ";
   }
   print STDERR "\n";
   return 1;
} #end show suits.

sub zero_hash {            # Generic routine to put zeros to scalars and array elements in a hash.
   local $req_dbg = $req_dbg + 2;  # straight fwd routine not a lot of debug output needed.
   while (@_) {
   my $hash_ref = shift;
   my ($hkey, $hvalue, $hvalue_t );
   while (($hkey, $hvalue ) = each %{$hash_ref}) {
      $hvalue_t = ref $hvalue ;
      next if ( ($key =~ m/NAME/) or ($key =~ m/LET/) or ($key =~ m/SUITS/) ) ; #dont touch certain keys
      if ($hvalue_t eq "ARRAY" ) {
         printf "zero_hash:: Reset $hkey of type ARRAY and current value= %s to zeroes {$Debug ge $req_dbg } \n",
                     join ":", @{$hash_ref->{$hkey}} if $Debug >= $req_dbg +2 ;
         @{$hash_ref->{$hkey}} = (0) x @{$hash_ref->{$hkey}};
      }
      else {
         printf "zero_hash:: Reset $hkey of type SCALAR and current value=[$$hash_ref{$hkey}] to 0 {$Debug ge $req_dbg } \n"
                  if $Debug >= $req_dbg +2;
         $$hash_ref{$hkey} = 0 ; # must be numeric scalar
      }
   } #end while each key,value
 } #end while hashrefs to do
 return 1;
} # end sub zero_hash

sub split_path {
   my $pth = shift ;
   my $ch = shift ;
   $pth =~ m!((.*)/)?([^$ch]*)$! ;

   print "[$1] , [$2] , [$3] $Debug ge $req_dbg \n" if $Debug >= $req_dbg;
   my $dirname = $2 ; # trailing slash removed
   my $fname = $ 3 ;
   $fname =~ tr/-A-Za-z0-9_+./~/c ; # remove problem chars
   return ($dirname, $fname) ;
}
sub fname2IDtag {
   my $fname = shift;
   $fname = substr $fname,0,10 ; # this is a tag for the set; don't want longer than 10 chars.
   return $fname ;
}
sub drop_sfx { #p is the path string, ch is the suffix marker (usually DOT} and q is the suffix string.
   my $p = shift ;
   my $ch = shift ;
   my $r = reverse $p ;
   my $c = index $r,$ch ; $q = substr($p,0,length($p)-$c-1);
   return $q ;
}
sub get_hand_keys {        #uses  @Hand_href setup at compile time with refs to the various %Hand_X hashes.
   my $compass = shift ;
   local $req_dbg = $req_dbg + 1;
   my $key ;
   my @values = () ;
   my $hnum = ($compass eq 'N') ? 0 : ($compass eq 'E') ? 1 : ($compass eq 'S') ? 2 : 3 ;
   $ref_t = ref($Hand_href[$hnum]) ;
   print STDERR "Hand_ref [ $hnum ] has type [$ref_t] {$Debug ge $req_dbg} \n";
   print STDERR "$compass , $hnum, $Hand_href[$hnum]->{NAME} {$Debug ge $req_dbg} \n";
   foreach $key ( @_ ) {
      print STDERR "$key => $Hand_href[$hnum]->{$key} {$Debug ge $req_dbg} \n";
      push @values, $Hand_href[$hnum]->{$key} ;
   }
   return @values ;
}
sub Init_hand_hashes {     # define the hand keys and give them defaults. All except SUIT ptrs set at compile time.
   local $req_dbg = $req_dbg + 1 ;
   my $key_t ;
   foreach $hnd_href (@_) {
      print "Init_hand_hashes:: Starting copy default to $hnd_href->{NAME} {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      copy_DOP_hash ($hnd_href , \%default_hand) ; # wont touch NAME, LET, or SUITS; other keys copied.
      if ( ($hnd_href->{LET} eq 'W') or ( $hnd_href->{LET} eq 'S') ) { $hnd_href->{OPEN} = 'Y' ; }
      else  { $hnd_href->{OPEN} = 'N' ; }
   } # end foreach hnd_ref
   return 1;
} #end Init_hand_hashes

sub Init_suit_hashes {    # args: list of hand_hashrefs. calls make_suit_keys. Blesses suit Hashes
   local $req_dbg = $req_dbg + 2;
   my $hand_ref_t;
   foreach $hand_ref (@_) {
      $hand_ref_t = ref $hand_ref;
      print STDERR "Init_suit_hashes:: hand_type=$hand_ref_t {$Debug gt $req_dbg}\n " if ($Debug > $req_dbg );;
      for ($m=0; $m < 4; $m++ ){
            $suit_ref = get_suit_href($hand_ref, $m);
            print STDERR "Init_suit_hashes:: Hand[$hand_ref_t] SUIT[$Suit_Names[$m]] \n" if ($Debug > $req_dbg );
            &copy_DOP_hash ($suit_ref , \%default_suit) ; # safer than assignment in case of arrays etc.
            $suit_ref->{LET} = $suit_lets[$m];
            # don't need to zero the keys because the default suit does that where needed.
      } # end for each $m suit.
   } #end foreach hand_ref
   return 1;
} #end Init_suit_hashes

sub zero_suit_keys {       # called with suit href. This hash has no arrays in it. All scalars.
   my $s_ref = shift;
   my $ref_t = ref $s_ref;
   foreach $s_key ( keys %{$s_ref} ) { $s_ref->{$s_key} = 0; } #Alpha strings fixed next.
   $s_ref->{LET} = substr($ref_t, 0, 1);    #suit hash was blessed into class "Clubs", or "Diamonds" etc.
   $s_ref->{STR} = "";
   #### Zero_SUIT_Keys Done...
   return 1;
} #end zero suit sub


sub get_suit_href {           # called with ($hand_ref, $suit). $suit=  C, D, H, S or 0..3
   my ($hand_ref, $suit  ) = (@_);
   $suit = ($suit eq "C" ) ? 0 : ($suit eq "D" ) ? 1 : ($suit eq "H" ) ? 2 : ($suit eq "S" ) ? 3 : $suit;
   if ((0 le $suit) && ($suit le 3) ) {
      return $hand_ref->{SUITS}->[$suit] ;
   }
   else { return undef ; }
}  # end get suit ref

# Copy one hash to another; the source hash may have scalars, arrays, or other hashes in it.
# but only down one level , i.e. no arrays of hashes, or arrays of arrays etc. inside the main hash
sub copy_DOP_hash {
   my $to_href = shift ;
   my $from_href = shift;
   my $hash_t = ref($to_href) ;
   local $req_dbg = $req_dbg + 1;
   foreach $key ( keys %{$from_href} ) {
       $key_t = ref($$from_href{$key}) ;
       print STDERR "copy_DOP_hash::To $hash_t Processing $key of type [$key_t] { $Debug ge $req_dbg +1 } \n" if $Debug >= ($req_dbg+1);
       next if ( ($key =~ m/NAME/) or ($key =~ m/LET/) or ($key =~ m/SUITS/) ) ;
       if ($key_t) {  # key is some sort of pointer
            # print "$key has type $key_t\n";
         if    ($key_t =~ m/ARRAY/ ) { @{$to_href->{$key}} = @{$from_href->{$key}} ; }
         elsif ($key_t =~ m/HASH/  ) { %{$to_href->{$key}} = %{$from_href->{$key}} ; }
       } #pointer key type
       else {
            # print "Scalar Assign $key \n";
            $$to_href{$key} = $$from_href{$key} ;
       }
    } # end foreach key
    print STDERR " ------Copy to Hash of type $hash_t Init Done --------- { $Debug ge $req_dbg }\n" if  $Debug >= $req_dbg;
   return 1;
} #end sub copy DOP hash

sub show_hash_key_types {
   my $href = shift ;
   my $hash_t = ref $href ;
   print "In hash of type $hash_t \n";
   foreach $key (sort keys %{$href} ) {
      $key_t = ref($href->{$key} );
      if ( $key_t ) { printf "Key[$key] has type [$key_t] \n" ;}
      else          { printf "Key[$key] has type [SCALAR] \n" ;}
   }
   return 1;
} # end sub show_hash_key_types

sub txlate2GIB {
   my $dop_snum = shift ;
   my $gib_snum  = $dop_snum ;
   $gib_snum =~ tr/01235CDHSN/4321043210/;  #in dop NT is number 5; in Gib number 0;
   return ($gib_snum ) ;
}
sub check_setup {
      local $req_dbg = $req_dbg;  #would not be called if we did not want output
      #check the setup to see if all zeros
      printf STDERR "Start_Main::CheckSetup Deal, Hand, and Suit hashes all initialized {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      &show_DOP_hash ("Init Deal",    \%Deal );       print "================ o-o ==============\n";
      &show_DOP_hash ("Init Deal_NS", \%Deal_NS );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Hand_W " , \%Hand_W );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Hand_E " , \%Hand_E );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Hand_S " , \%Hand_S );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Hand_N " , \%Hand_N );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Suit N_Spades " , \%Spades_N );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Suit S_Hearts " , \%Hearts_S );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Suit W_Diams " ,  \%Diams_W  );    print "================ o-o ==============\n";
      &show_DOP_hash ("Init Suit E_Clubs " ,  \%Clubs_E  );    print "================ o-o ==============\n";
      #CHecking some possible duplication of pointers?
=cut
   # this next bit is to see if we have re-used suit pointers in more than one hand hash or deal. Coding error if so.
      print "********** Checking Cross Corruption *****************\n";
      $hnd_href = \%Hand_N ;
      foreach $k (keys (%default_hand ) ) {
         next if $k eq "SUITS" ;
         $k_t = ref($default_hand{$k}) ; print "$k has type [$k_t] \n";
         if ($k_t =~ m/ARRAY/ ) {
            $$hnd_href{$k}->[1] = -99 ;
            print STDERR "for $k : N=$Hand_N{$k}->[1], S=$Hand_S{$k}->[1], W=$Hand_W{$k}->[1], E=$Hand_E{$k}->[1] \n";
         }
         else {
            $$hnd_href{$k} = -88 ;
            print "for $k : N=$Hand_N{$k}, S=$Hand_S{$k}, W=$Hand_W{$k}, E=$Hand_E{$k} \n";
         }
      } # end for each key.
=cut
   return 1;
} # end sub check_setup
sub setup_FILEIO {         # checks the  %get_opt hash; puts descriptive info in %file_specs
                           # modifies the global vars rptfile and infile to contain new file handles if req'd
   my $fh_href = shift ;
   local $req_dbg = $req_dbg + 1;

   # setup input and report files if required.
   $$fh_href{RPTFILE} = $rptfile ;
   $$fh_href{RPT_FILE_OK} = 0;
   if ( $get_opts{OPT_o} ne "" ) {
      $rptfile = "RPTFILE" ;  # or should it be *RPTFILE. It is set to STDOUT on entry to this routine, not *STDOUT
      $rpt_file_name = $get_opts{OPT_o} ;  #get opts hash was filled by get_opts subroutine.
      open $rptfile, ">$rpt_file_name" or die "Can't open $rpt_file_name for output. Error $!\n";
      $$fh_href{RPT_FILE_OK} = 1;
      $$fh_href{RPTFILE} = $rptfile;     # RPTFILE ? Pointer or text ?
      $$fh_href{RPT_FILE_NAME} = $rpt_file_name;
      printf STDERR "$rpt_file_name Opened for OUTPUT, rptfile =%s {$Debug ge $req_dbg }\n", $$fh_href{RPTFILE} if $Debug >= $req_dbg;
   }

   $$fh_href{INFILE} = $infile;
   $$fh_href{IN_FILE_OK} = 0;
   if ($get_opts{OPT_i} ne "" ) {
      $in_file_name = $get_opts{OPT_i} ;
      $infile = DEALFILE ; #define a file handle for the open etc.
      open $infile, "<$in_file_name" or die "Can't open $in_file_name for input. Error $!\n";
      $$fh_href{IN_FILE_OK} = 1;
      $$fh_href{INFILE} = "$infile";       #DEALFILE
      $$fh_href{IN_FILE_NAME} = $in_file_name;
      printf STDERR "$in_file_name Opened for INPUT, infile =%s { $Debug ge $req_dbg }\n", $$fh_href{INFILE} if $Debug >= $req_dbg  ;
   }
   return 1;
 } # end sub setup_FILEIO
# $deal_href, $hnd_hrefW, $hnd_hrefE, $side
 sub parse_side  {
# Takes refs to hashes: Deal, Hand_W, Hand_E; Updates these hashes and also the 8 suit hashes.
# Checks hands for validity: 13 cards, no duplicates in same suit, ... (but not two of same card in diff hands.)

# Note that the "West" hand will always contain something to get here, but the "East" hand may be empty.
# we use W and E for convenience since that is usual case. But could be called with NS hands also if doing GIB run.
      my $deal_href = shift ;
      my $hnd_hrefW = shift ;
      my $hnd_hrefE = shift;
      my $side      = shift ;

      $h_west = $$hnd_hrefW{STR};                # use the cleaned up version from cleanup_hand called by chk_hand_strings
      $h_east = $$hnd_hrefE{STR};
      $$deal_href{STR} = $deal_str = "W|S=$h_west : E|N=$h_east";  #first cut. Will be cleaned up more later.
      $$deal_href{OPENER} = &set_opener($df_opener, $side);
      print STDERR "Parse_Side:: Deal{STR}=[$$deal_href{STR}], Deal{OPENER}=$$deal_href{OPENER} {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      @w_sptr = &get_sptrs($hnd_hrefW);
      @e_sptr = &get_sptrs($hnd_hrefE);

      if ( ($h_west ne "") ) {
         @this_hand = ( &split_hand ($h_west) ) ;   # split into suits; return C,D,H,S (reverse of user entry)+Clean hand str.
         $w_sptr[0]->{STR}  = $this_hand[0];  $w_sptr[1]->{STR}  = $this_hand[1]; #put the suit strings into the suit hashes
         $w_sptr[2]->{STR} =  $this_hand[2];  $w_sptr[3]->{STR} =  $this_hand[3];
         $h_west = $$hnd_hrefW{STR} = $this_hand[4];
         $valid_hw = &chk_hand_valid($hnd_hrefW);     #Check valid suits and valid hand.
      } # end if h_west ne ""
      print STDERR "Parse_Side:: - Done validating Hand WEST|South = : $valid_hw {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      # show_DOP_hash("Splitup  West|South hand Hash",$hnd_hrefW) if $Debug >= ($req_dbg+3);

      if ( ($h_east ne "") ) {
         @this_hand = ( &split_hand ($h_east) ) ; # split into suits; return C,D,H,S (reverse of user entry)+Clean hand str.
         $e_sptr[0]->{STR}  = $this_hand[0];  $e_sptr[1]->{STR}  = $this_hand[1]; #put the suit strings into the suit hashes
         $e_sptr[2]->{STR} =  $this_hand[2];  $e_sptr[3]->{STR} =  $this_hand[3];
         $h_east = $$hnd_hrefE{STR} = $this_hand[4];
         $valid_he = &chk_hand_valid($hnd_hrefE);  #check valid suits and valid hand
      } # end if h_east ne ""
      else { $valid_he = 0 ; }   # no valid East hand means we don't need to process it in later steps.

      print STDERR "Parse_Side:: - Done validating Hand EAST|North = : $valid_he {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      # show_DOP_hash("Splitup  East|North hand Hash",$hnd_hrefE) if $Debug >= ($req_dbg+3);
      $deal_str = $$deal_href{STR}="W|S=$$hnd_hrefW{STR} : E|N=$$hnd_hrefE{STR}";  #just for descr so having a trailing E= with no cards is OK?

      if ( !($valid_hw )) {
         print STDERR "Parse_Side:: West|South hand: [ $h_west ] is INVALID.   Please Correct and  RETRY. \n";
         # exit (-250); dont do this anymore as this routine is used not just on cmd line but for deal lines also.
      }
      if (  ($h_east ne "") &&  !($valid_he)  ) {
         print STDERR "Parse_Side:: East|North hand: [$h_east] is INVALID. Continuing using West|South hand only. \n";
      }

      # some debug output
       if ($Debug >= $req_dbg - 1) {
         $ptr = $hnd_hrefW->{SUITS} ;
         @suits = ($ptr->[0]->{STR},$ptr->[1]->{STR},$ptr->[2]->{STR},$ptr->[3]->{STR});
         print STDERR "ParseSide:: WEST|South Hand C=[$suits[0]],D=[$suits[1]],H=[$suits[2]],S=[$suits[3]] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
         $ptr = $hnd_hrefE->{SUITS} ;
         @suits = ($ptr->[0]->{STR},$ptr->[1]->{STR},$ptr->[2]->{STR},$ptr->[3]->{STR});
         print STDERR "ParseSide:: EAST|North Hand C=[$suits[0]],D=[$suits[1]],H=[$suits[2]],S=[$suits[3]] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      }  # end if Debug
      return ($valid_hw, $valid_he) ;
} # end sub parse_side

sub DOP_Reports {
   my ($deal_href, $hnd_hrefW, $hnd_hrefE, $aoc_rpt, $valid_hw, $valid_he ) = (@_) ;
   if ($valid_he && $valid_hw )  {
         $ds_west = $$deal_href{DS_W};   # the strains to report on.
         $ds_east = $$deal_href{DS_E};   # these are setup from cmd line or code.
         $dl_num  = $$deal_href{DEAL_NUM}; # the current deal number (if any)
         print STDERR "Main::CMDLINE:: Report Options: Deal[$file_specs{RPT_DEAL_LVL}], Hand[$file_specs{RPT_HAND_LVL}], ",
         "Strain_W[$ds_west] Strain_E[$ds_east] {$Debug ge $req_dbg} \n"      if ($Debug ge $req_dbg) ;

         # -------- Do the DEAL REPORT --------
         # rptfile should be setup as either STDOUT or the result of the open statement back in the GET_OPTS section.

         &show_DOP_hash ("Calling RPT DEAL with file specs", \%file_specs) if ($Debug ge $req_dbg) ;;

         &Rpt_Deal  ($deal_href, \%file_specs   ) if $file_specs{RPT_DEAL_LVL} ne "N"; # Level of detail and header line.

         if ($file_specs{RPT_DEAL_LVL} eq "S" and $file_specs{RPT_HAND_LVL} eq "N" and $ds_west eq "" and $ds_east eq "") {
            $file_specs{RPT_DEAL_HDR} = "N" ;   # if only 1 line summary, turn off the header after the first one written
         }
         else {
            $file_specs{RPT_DEAL_HDR} = "Y" ;  # might as well make sure
         }

         # Strains are independent of Deal and Hand
         $strain_rpt_hdr = "Y";
         if ($ds_west ne "" ) {
            print STDERR "DOP_Reports:: Calling Rpt_Strain WEST { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
            &Rpt_Strain ($deal_href, $ds_west, "W", $strain_rpt_hdr ) ;
            $strain_rpt_hdr = "N" ;
         }
         if ($ds_east ne "" ) {
            print STDERR "DOP_Reports:: Calling Rpt_Strain EAST { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
            &Rpt_Strain ($deal_href, $ds_east, "E", $strain_rpt_hdr )  ;
         }
         print STDERR "DOP_Reports:: Deciding on Hand Report: Level[ $file_specs{RPT_HAND_LVL} ] { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
         #           hand     S | D     hdr nt-ded #-- will also report SUITS
         &Rpt_Hand($hnd_hrefW, $file_specs{RPT_HAND_LVL}, "Y", "Y", $dl_num) if $file_specs{RPT_HAND_LVL} eq "D" ;
         &Rpt_Hand($hnd_hrefE, $file_specs{RPT_HAND_LVL}, "Y", "Y", $dl_num) if $file_specs{RPT_HAND_LVL} eq "D" ;
         # Summary only. NT verbosity = Y. No Suit reports
         &Rpt_Hand($hnd_hrefW, $file_specs{RPT_HAND_LVL}, "Y", "N", $dl_num) if $file_specs{RPT_HAND_LVL} eq "S";
         &Rpt_Hand($hnd_hrefE, $file_specs{RPT_HAND_LVL}, "N", "N", $dl_num) if $file_specs{RPT_HAND_LVL} eq "S";
      }  #end both hands present. Must be a West hand only run ...

      elsif    ( $file_specs{RPT_HAND_LVL} eq "S"  or $file_specs{RPT_HAND_LVL} eq "D"  ) {
            &Rpt_Hand(\%Hand_W, $file_specs{RPT_HAND_LVL}, "Y", "Y", -1 );
      }
      elsif    ( $file_specs{RPT_DEAL_LVL}  eq "S"  or $file_specs{RPT_DEAL_LVL} eq "D"  ) {
            &Rpt_Hand(\%Hand_W, $file_specs{RPT_DEAL_LVL}, "Y", "Y", -1 );
      }  # end hand report decision...

      # Report the Adjusted AOC if user asked for it: Opt_A will be N/null if both OPT_l and OPT_r are N or null.
      if ($aoc_rpt eq 'S' || $aoc_rpt eq 'D') {
         &AOC_RPT(\%AOC_DOP_DEAL, \%AOC_DOP_W, \%AOC_DOP_E, $Deal_Line{l}, $Deal_Line{r}, \%Deal, \%Hand_W, \%Hand_E );
      } #end printing AOC adjustments.
      #Reporting all done for this side.
      return 1 ;
} # end DOP_Reports

sub reInit_DOP_Deal  {  #can be called with either set of Deal and Hand ptrs. using E/W names for convenience.
   my $dl_href = shift;
   my $hnd_hrefW = shift;
   my $hnd_hrefE = shift ;
   my $save_dealno = $$dl_ref{DEAL_NUM};
   local $req_dbg = $req_dbg + 1;

   &copy_DOP_hash ($dl_href , \%default_deal) ; # default_deal setup to all zeros or null strings
   # now reset some deal hash keys to the run defaults ...
   $$dl_ref{DEAL_NUM} = 1 + $save_dealno;
   $$dl_ref{OPENER} = $get_opts{$OPT_O} ;       # default Opener for the run
   $$dl_ref{DS_W} = $get_opts{OPT_w};              #reset the strains reqd to the defaults or cmd line switches.
   $$dl_ref{DS_E} = $get_opts{OPT_e};              # these can be overridden on a per deal basis.

   # default_hand hash has zeros/nulls everywhere safe. Only need to reset the OPEN key to Y for the default Opener
   &Init_hand_hashes ($hnd_hrefW, $hnd_hrefE) ; # copy in the default hand. setup OPEN key which might need changing here.
   if ($get_opts{OPT_O} eq 'E') { $$hnd_hrefE{OPEN} = 'Y' ; }  # leave as null string otherwise
   else                         { $$hnd_hrefW{OPEN} = 'Y' ; }  # leave as null string otherwise

   &Init_suit_hashes ($hnd_hrefW, $hnd_hrefE ) ; #copy default suit then fix suit letter.

   return 1;
} # end reInit_DOP_Deal

sub reInit_GIB_Results {
   my ($gib_href, $ns_result_href, $ew_result_href ) = (@_) ;
   my $save_gib_pfx  = $gib_href->{GIB_PFX};
   local $req_dbg = $req_dbg + 1;
   #Fields that need preserving from one deal to the next:  None so far.
      &copy_DOP_hash ($gib_href, \%Zero_GIB_Deal);  # will safely copy scalars and sub-arrays in the hash.
      $gib_href->{GIB_PFX} = $save_gib_pfx ;
      &show_DOP_hash ("reInit_GIB::ZEROed GIB_Deal Hash { $Debug ge $req_dbg }", $gib_href) if $Debug >= $req_dbg ;
      %{$ns_result_href} = %ZERO_Results ; # nothing but scalars in these ones. Straight Hash to Hash assignment is ok.
      %{$ew_result_href} = %ZERO_Results ;
      return 1;
} #end reInit_GIB_Results

# \%NS_Results, "NS", \%Deal_NS, \%Hand_S, \%Hand_N, \%GIB_Deal  (S->W and N->E)
sub set_trick_results {  #called twice from mainline once for NS once for EW since we have two sets of results for each GIB deal.
   my ($result_href, $side, $deal_href, $hnd_hrefW, $hnd_hrefE, $gib_href ) = (@_) ;
   local $req_dbg = $req_dbg + 1;
# Do 1 thru 7 first for the side we were called with.
   #1. (a)Check the raw strings; (b)parse the deal into suits etc. Maybe not necessary as GIB parse done already.
   #2. eval two hands , which evals suits,  and fills hand and suit hashes.
   #3. Once hands and suits are both done, eval the deal and fill the deal hash.
   #4. from the deal_hash find the longest fit strain, and the strain with the most DOP.
   #5. put the NT, LongStrain, and BestStrain DOP into the results hash.
   #6. for each of the above DOP get the 'expected' number of trix and put it into the result_hash
   #7. for each of the above strains, get the GIB trix and put that into the result hash.
# We now have everything we need to write the CSV file all in one hash.

#1.
my ($h_west, $h_east) ;
$h_west = $hnd_hrefW->{STR} ; $h_east = $hnd_hrefE->{STR} ;
print STDERR "set_trick_results:: $side , West|South=[$h_west] :: East|North=[$h_east] { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
$ew_sw = &Check_raw_hand_strings ($hnd_hrefW, $hnd_hrefE)  ;

#                                 \%Deal       \%Hand_W    \%Hand_E or the NS equivalents
($valid_w, $valid_e) = &parse_side($deal_href, $hnd_hrefW, $hnd_hrefE, $side ) ; # use W{STR} and E{STR} to setup suits{STR} and Deal{STR}
#2.
$rc = &eval_hand ( $hnd_hrefW );   # calculate overall hand DOP, Deductions for 4333, no_A etc. also total HLD pts.
$rc = &eval_hand ( $hnd_hrefE );
#3. & 4
   &eval_deal($deal_href, $hnd_hrefW, $hnd_hrefE,);  #fill in everything in all the hashes. Does NOT do Trix.

#5. & 6.
   &DOP2Results($result_href, $deal_href, $side ) ;
#7.
   &GIB2Results($result_href, $gib_href, $side) ;

# Debug show
   &show_DOP_hash ("Set_tricks::Returning Set Tricks for $side { $Debug ge $req_dbg }", $result_href) if $Debug >= $req_dbg ;
   return 1;
} # end set_trick_results

sub DOP2Results {  #needs only the two hashes ; GIB2Results could fill in the Side.
   my ($result_href, $deal_href, $side, ) = (@_) ;
   my ($nt_ded_total, $deal_t);
   $deal_t = ref($deal_href);
   local $req_dbg = $req_dbg + 1;
   print "DOP2Results:: From $deal_t, $$deal_href{NAME}, $$deal_href{OPENER}  { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg;

   $$result_href{Side} = $side ;

   $$result_href{Long_Fit}    = $$deal_href{LONG_STRAIN};  #alpha letter
   $$result_href{Best_Fit}    = $$deal_href{BEST_STRAIN};
   $$result_href{NT_HLF}      = $$deal_href{DOP_HLF_NT};
   $nt_ded_total = &Calc_nt_ded_tot($deal_href, $$deal_href{OPENER} ) ;
   @trix_result   = DOP_2_Trix( $$deal_href{DOP_HLF_NT}+$nt_ded_total, 'N', $$deal_href{QUICK_LOSERS_NT}) ;
   $$result_href{NT_Trix}     = $trix_result[0] ;
   $$result_href{NT_Pct_lo}   = $trix_result[1] ;
   $$result_href{NT_Pct_hi}   = $trix_result[2] ;
   $$result_href{NT_HLF_ADJ}  = $$deal_href{DOP_HLF_NT}+$nt_ded_total;

   $$result_href{Long_HLDF}   = $$deal_href{DOP_HLDFl};
   @trix_result   = DOP_2_Trix($$deal_href{DOP_HLDFl}, 'S', $$deal_href{QUICK_LOSERS_SUIT}) ;
   $$result_href{Long_Trix}   = $trix_result[0] ;
   $$result_href{Long_Pct_lo} = $trix_result[1] ;
   $$result_href{Long_Pct_hi} = $trix_result[2] ;
   $$result_href{Best_HLDF}   = $$deal_href{DOP_HLDFb};
   @trix_result   = DOP_2_Trix($$deal_href{DOP_HLDFb}, 'S', $$deal_href{QUICK_LOSERS_SUIT}) ;
   $$result_href{Best_Trix}   = $trix_result[0] ;
   $$result_href{Best_Pct_lo} = $trix_result[1] ;
   $$result_href{Best_Pct_hi} = $trix_result[2] ;
   $$result_href{Deal_STR}    = $$deal_href{STR};
   print STDERR "DOP2Results returning for Side[$side] Deal $$result_href{Deal_STR} \n" if $Debug >= $req_dbg;
   return 1 ;
} # end sub DOP2Results

sub GIB2Results {       # Needs to know the Side as gib_href contains results for both sides.
   ($result_href, $gib_href, $side) = (@_) ;
   local $req_dbg = $req_dbg + 1 ;
   $gl = $$result_href{Long_Fit} ; $gl =~ tr/NSHDC/01234/;
   $gb = $$result_href{Best_Fit} ; $gb =~ tr/NSHDC/01234/;
   print STDERR "GIB2Results:: Long_Fit =  $$result_href{Long_Fit}:$gl Best_Fit=$$result_href{Best_Fit}:$gb { $Debug ge $req_dbg} \n"
         if $Debug >= $req_dbg;
   if ($side eq "NS"  ) {
      $$result_href{GIB_NT_Trix}   = $$gib_href{NS_TRIX}->[0] ;
      $$result_href{GIB_Long_Trix} = $$gib_href{NS_TRIX}->[$gl] ;
      $$result_href{GIB_Best_Trix} = $$gib_href{NS_TRIX}->[$gb] ;
   }
   else { #side must be EW
      $$result_href{GIB_NT_Trix}   = $$gib_href{EW_TRIX}->[0] ;
      $$result_href{GIB_Long_Trix} = $$gib_href{EW_TRIX}->[$gl] ;
      $$result_href{GIB_Best_Trix} = $$gib_href{EW_TRIX}->[$gb] ;
   }
   $$result_href{DEAL_ID} = $$gib_href{DEAL_ID} ;
   &show_DOP_hash("GIB2Results:: for $side { $Debug ge $req_dbg }", $result_href) if $Debug >= $req_dbg;
   return 1 ;
} # end sub GIB2Results

# Heading line and fields output are:
#DealID  Side  Strain   DD Trix  DOP Trix DOP Lo%  DOP Hi%  HLDF  DD<DOP DD=DOP DD>DOP  DD<Tot DD=Tot  DD>Tot  RowCount
# needs global vars @diff_cnt and $rowcount intialized at compile time; or start of run.
#---------- MOD to only write the NS results Maybe the EW stuff is bogus.
sub Rpt_GIB_CSV {
   local $req_dbg = $req_dbg + 1 ;
   my ($rpt_file, $href,  $side, $r_count, ) = (@_) ;
   my (@diff_cols, $stat_trix_gib, $stat_trix_dop) ;
   my ($dd_gt, $dd_eq, $dd_lt, $dd_gt_pct, $dd_eq_pct, $dd_lt_pct );
 if ($Debug >= $req_dbg ) {
   print STDERR "Rpt_GIB_CSV:: Side=$side Starting Rowcount=[$r_count] { $Debug ge $req_dbg } \n";
   print STDERR "Rpt_GIB_CSV:: DOP_NT=$$href{NT_HLF}, DOP_Best=$$href{Best_HLDF}\n";
   print STDERR "\t DOP_NT_Trix=$$href{NT_Trix}, DOP_B_Trix=$$href{Best_Trix}\n";
   print STDERR "\t GIB_NT_Trix=$$href{GIB_NT_Trix}, GIB_B_Trix=$$href{GIB_Best_Trix}\n";
  } # end debug if
      $stat_trix_gib = ( $$href{GIB_NT_Trix} < 7  ) ? 1 :  $$href{GIB_NT_Trix} ; # Fair compare if trx<7 both GIB&DOP=1
      $r_count++;
      (@diff_cols) = &calc_diffs($stat_trix_gib, $$href{NT_Trix}, $r_count  ) ;

      printf $rpt_file "$CSV_fmt\n", $$href{DEAL_ID}, $side, 'NT', $stat_trix_gib,
                     $$href{NT_Trix}, $$href{NT_Pct_lo}, $$href{NT_Pct_hi},
                     $$href{NT_HLF_ADJ}, (@diff_cols), $r_count,$$href{Deal_STR} ;
     print STDERR "Rpt_GIB_CSV:: NT $$href{GIB_NT_Trix}, <=> $$href{NT_Trix} :=> @diff_cols[0 .. 2] { $Debug ge $req_dbg }\n"
            if $Debug >= $req_dbg ;

      $stat_trix_gib = ( $$href{GIB_Best_Trix} < 7  ) ? 1 :  $$href{GIB_Best_Trix} ;
      $r_count++;
      (@diff_cols) = &calc_diffs($$href{GIB_Best_Trix}, $$href{Best_Trix}, $r_count ) ;
      printf $rpt_file "$CSV_fmt\n", $$href{DEAL_ID}, $side, $$href{Best_Fit},
                     $$href{GIB_Best_Trix}, $$href{Best_Trix}, $$href{Best_Pct_lo}, $$href{Best_Pct_hi},
                     $$href{Best_HLDF}, (@diff_cols), $r_count, $$href{Deal_STR} ;
      print STDERR "Rpt_GIB_CSV:: Best $$href{GIB_Best_Trix}, <=> $$href{Best_Trix} :=> @diff_cols[0 .. 2] { $Debug ge $req_dbg }\n"
            if $Debug >= $req_dbg ;

   if (wantarray() ) { return ($r_count, @diff_cols) ; }
   return $r_count;

   sub calc_diffs {  #uses global var @diff_cnt which is a running total
      my $dd_trix  = shift;
      my $dop_trix = shift;
      my $r_cnt = shift;
      my ($i, @g_cmp_d, @g_cmp_d_tot);
      @g_cmp_d = (0,0,0); #array will set relevant <, = , > to 1; others remain zero.
      @g_cmp_d_tot =(0,0,0);
      $i = 1+( $dd_trix <=> $dop_trix ) ;
      $g_cmp_d[$i] = 1;
      $dd_dop_diff_count[$i] += 1;  # global var, running total
      return (@g_cmp_d, @dd_dop_diff_count)  ; # give running total of number of times <, =, > occurred.
   }
   return 1 ;
} #end sub Rpt_GIB_CSV

#   [N|S|E|W] [NS | EW ]  ,
sub set_opener {
   my $df_opener = shift;
   my $side      = shift;
   if ($side =~ m/N|S/ ) {
      if ($df_opener =~ m/N|E/ ) {return  'N' ; }
      else {  return 'S' ; }
   }
   else {
      if ($df_opener =~ m/N|E/ ) { return 'E' ; }
      else {  return 'W' ; }
   }
} #end set_opener



1; #So Require succeeds




