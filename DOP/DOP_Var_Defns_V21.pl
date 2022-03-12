# No shebang since this file brought in by require.
#File DOP_Var_Defns_Vxx.pl
## Author      Version  Date           Comment
#  JGM        10.0      2020/11/02     All working. added ADJ_TOT to Deal hash.
#  JGM        10.1      2020/11/02     Created OPTIONS hash for returning the options, from getopts.
#  JGM        13.0      2021/01/22     Added Sy pts for KQJT, AQJT, AKQJx(x) and HHHTx(x)
#  JGM        13.2      2021/02/23     Added Q_4 pt for four Queens. Added vars to count Quick Losers
#  JGM        20.0      2021/06/30     Added Vars and Code to return a hash of all meaningfull results.
#  JGM        21.0      2021/08/28     Mods for -q (terse) switch for dealer.
#
package main ;          # Make sure our namespaces are aligned
our $Debug = 0;         # maybe should set this from the %ENV via sub get_dbg_lvl
our $DOP_ERR = 0;        # Need an array of error numbers and error messages
our @STK = ();
# -----------------------  Key Global Vars ----------------------------
our   $rptfile = "STDOUT";  # These indirect file handles have to evaluate to a file handle name.
our   $infile  = "STDIN"; # In the following code print $rptfile, "yada yada" and print RPTFILE, "yada yada" are the same

# This file has only the definitions. Many are initialized at compile time.

# some vars I need all the time. These are all temporary not needed outside of a sub or a block.
our($c, $d, $f, $g,  $i, $j, $k, $m, $n, $x, $y, $z, $str, $rc, @suits, @srefs, @this_hand );
our ($ptr, $len, $hnd_ref, $suit_ref, $H_tot, $L_tot, $D_tot, $Sy_tot, $ds_e, $ds_w );

# some vars for this test
our ($suit, $sk, $hand, $hk, $deal, $dk, $east, $west, @hands, @tokens, $cmd_sw);
our ($west_ref, $east_ref, $arr_ref, $hash_ref, $west_ref_t, $east_ref_t, $arr_ref_t, $hash_ref_t);

# vars for added Competition features.
our ($lho_suit, $rho_suit, $aoc_href, $aoc_w_tot, $aoc_e_tot, $aoc_dl_tot, $aoc_rpt );

#vars for analyzing GIB deals, which typically have all four hands specified.
our ($side_str, $side_num, $side_href, $ew_side_href, $ns_side_href, $best_suit, $long_suit, $gib_run, $gib_fname );



# defaults. May be overridden by cmd line opts. Use these to Init and Re-Init the hashes as reqd.

 our $h_east  = "";           #-E opt east hand from cmd line e.g. -E AKxx/QJT2/-/KQ765  or from stdin
 our $h_west  = "";           #-W opt west hand from cmd line

 our $ds_df_w="";         # defaults for reporting not needed. one liner shows NT and Long Strain even if no strain spec.
 our $ds_df_e="";
 our $ds_east="";          #-e opt default east declare strains from cmd line e.g. -e N,C,D,H,S,L
 our $ds_west="";          #-w opt default west declare strains from cmd line
 our $df_opener="W";       #-O opt default opener from cmd line. Either E or W ; if E also N ; if W also S;
 our $df_rpt_deal="S";     #-R opt takes on values S, D, SS, SD, DS, DD (S=Summary, D=Detail)
 our $rpt_deal="S";        # Init current deal report to the default.
 our $df_rpt_hand="N";     # Default to No hand report unless overridden on cmd line (N != Compass)
 our $rpt_hand="N";        # Init current hand report to the default.
 our $df_side ="EW";
 our $deal_num=0;
 our $df_gib_pfx = $pgm_defaults{DF_GIB_PFX}; #updated by either -G cmd sw, or text string in the gib_infile itself.

 our $df_aoc_rpt = "" ;    # Default is no Competition; can be overridden on cmd line.
  $aoc_rpt  = "";          # set current AOC report to default.
  $lho_suit = "";          # No competition analysis is default.
  $rho_suit = "";

 our @quick_losers = (0,0,0,0) ;

 # This hash contains the defaults to be used if they are not overridden on the cmd line. (see next hash)
 our %pgm_defaults = (DF_DS => "", DF_OPENER => "W", DF_RPT_DEAL => "S", DF_RPT_HAND => "N",
                      DF_INFILE => "", DF_RPTFILE => "STDOUT", # no infile (cmdline is def) and STDOUT.
                      DF_LHO => "", DF_RHO => "" , DF_AOC_RPT => "", DF_GIB_RUN => 'N', DF_GIB_PFX => "GIB",
                     );
#Hash to hold the values of the cmd line switches. Set to defaults in case no switch specified.
our %get_opts = ( OPT_W => "", OPT_E => "", OPT_w => $ds_df_w, OPT_e => $ds_df_e, OPT_O => "W", OPT_RD => "S", OPT_RH => "",
                  OPT_i => "STDIN", OPT_o => "STDOUT", OPT_l => "", OPT_r => "", OPT_A => "",  OPT_d => 0 ,
                  OPT_g => "N" , OPT_G => "GIBDEAL:" , OPT_q => "" ,
                );
# Hash to pass to Report Routines and input from file routines.
our %file_specs = (RPT_FILE_OK => 0, RPTFILE => "STDOUT", IN_FILE_OK => 0, INFILE => "STDIN",
                   RPT_DEAL_LVL => "S", RPT_DEAL_HDR => "Y", RPT_HAND_LVL => "N", RPT_HAND_HDR => "N",
                   IN_FILE_NAME => "" , RPT_FILE_NAME => "",
                  );

# These can be overridden on a per deal basis for stdin deals.
# if not set on stdin line input, the defaults will apply. Defaults may be set by cmdline and apply to stdin.
# The ORDER of the keys in the next statement No longer matters as the results are returned in a hash. Except T should be last.
our @line_keys = qw( W E w e d O R l r A T );      #vers 13.  fields that might come from STDIN - NOT cmd line opts
our %Deal_Line = ( # keys will be the line keys above. auto vivified by parse rtn.... )
                  );
our   @Hand_Names = ("North" , "East" , "South" , "West" ); # These are DDS order; Not GIB order
our   @Hand_Lets  = ( 'N' , 'E' , 'S' , 'W' );
our   @Suit_Names = ("Clubs", "Diams", "Hearts", "Spades", "", "NoTrump" ); # leave a gap to prevent confusion re suit #4
our   %Suit_Nums  = ('C' => 0,'D' => 1,'H' => 2, 'S' => 3,     'N' => 5  ); # DOP order not DDS order.
our   @suit_lets  = ("C", "D", "H", "S","","N" );
our   @card_names = ('A','K','Q','J','T','9','8','7','6','5','4','3','2' ); #'x' and '-' also allowed several times.
our   @GIB_Suit_Names = ( 'N', 'S', 'H', 'D', 'C') ;  #NS trix are given in this order
our   %GIB_Suit_Nums  = ( 'C' => 4, 'D' => 3, 'H' => 2, 'S' => 1,     'N' => 0,   ) ;
our   %DDS_Suit_Nums  = ( 'C' => 3, 'D' => 2, 'H' => 1, 'S' => 0,     'N' => 4  ) ; # NT only for tr_Filter

# symbolic constants assigned to various factors in the DOP evaluation.

our   %SUIT_DOP =   ( H_A => 4.5, H_K => 3, H_uQ => 1.5, H_aQ => 2, H_uJ => 0.5,  H_aJ => 1,
                      H_uT => 0, H_kT => 0.5, H_aT => 1,
# Deductions for stiff honors: -1;
# Deductions for 2 Honors dblton depends on honor combination:-1 for AQ&AK; 0 for KJ&KT; Other combos vary
# Hence: AK > AQ > KQ > KJ > KT > QJ > Qx > JT > Jx ;
                      Hs_H => -1, Hs_HH => -1,
# AK = 6.5, AQ = 5.5, AJ = 5, Ax = 4.5, KQ = 4.5, KJ = 4, KT = 3.5 Kx = 3, QJ = 2, QT = 1.5, Qx = 1, JT = 0.5, Jx = 0;
                      Hs_AK => -1, Hs_AQ => -1, Hs_KJ => 0, Hs_AJ => -0.5, Hs_KQ => -0.5,
                      Hs_QT => -1, Hs_QJ => -1.0, Hs_Qx => -0.5, Hs_JT => -1.5, Hs_Jx => -0.5, #<--Accepted by PD
#Shortness aka Distribution points in a hand OPENING in a suit, or any hand with 5 trumps, or a hand responding to NT Opener
                      D_VOID => 4, D_STIFF => 2, D_DBL => 0.5, # 0.5 so that TWO dlbtons adds 1pt and 0.5 is truncated.
# Length pts::with 3+ HCP: +1 for 5 suit, +2 for 6 suit. With less: 0 for 5 suit, +1 for 6 suit. 7 and up = 6 suit amt +2 per card.
                      L_3x5 => 1, L_6 => 1, L_3x6 => 2, L_7plus => 2,
# Synergy Points: 3 honors in 5 card suit = +1; 3 honors in 6+ suit: +2.
# 2021 update: 3 honors with T or 4 honors add an extra +1 in 5 or 6+ suit.
#              4 card suit with KQJT or AQJT gets +1 Sy pt.
                      Sy_4 => 1, Sy_5 => 1, Sy_6 => 2,  Sy_HHHH => 1, Sy_HHHT => 1, #Was 0.5 and 0.25 for debug.
                    );

our   %HAND_DOP =   ( Sq => -1, No_A => -1, No_K => -1, No_Q => -1, K_3 => 1, K_4 => 2, Q_4 => 1, D_2xDBL => 1, D_1xDBL => 0,
                      NT_VOID => -3, NT_STIFF => -2, NT_DUMMY => +1,  # +1 bec  Ded 1pt less than decl if NT_DUMMY.
                    );

our   %DEAL_DOP =   ( F8 => 1, F9 => 2, F10 => 3, HF => 1, SF_Hx => 1, Dm_xx => -1, Dm_x => -2, Dm_void => -3,
                      WH_x => -2, WH_void => -3, Wxxx_x => 2, Wxxx_void => 3, WAx_x => 1,
                      M_HAND => -2, M_SUITS => -1,
                    );

# Adjustments to our hand when in competition
our   %AOC_DOP  =    (  Lone_K => -1, KQ_under_opp => -1, KQ_over_opp => +1, # applies to suits of 3 or 4 cards only.
                        Lone_J => -0.5,      #applies to suits length >= 3 (Jx and J already worth zero)
                        Opp_Len_0 => +1, Opp_Len_1 => +1, Opp_Len_3 => -1, Opp_Len_4 => -2, Opp_Len_5 => -3,
                        Opp_Len_NT => +1, Opp_NT_Short => -99 ,  # fake number to prevent NT overcalls with len in OPPs suit <=2
                     );

# Since these are all Scalars, a statement like: %{$suit_ref} = %default_suit;
#      will work fine. No need for copy_DOP_hash sub
our   %default_suit = ( LET => "" ,  STR => "" , LEN => 0 ,
                        Has_A => 0,  Has_K => 0 ,  Has_Q => 0 , Has_J => 0 , Has_T  => 0, Has_9  => 0,
                        DOP_H => 0 , DOP_D => 0,   DOP_L => 0 ,DOP_Sy => 0 , Sy_Num => 0,
                    );

# use this in Init_hand_hashes sub as input to copy_DOP_hash sub. Will define keys and default values.
# Can also use to zero out a hand hash between deals.
our %default_hand = ( NAME => "" ,  LET => "" ,        # Descriptive stuff. Scalars. don't need to be reset usually.
#
# SUITS Not part of default as we do not want them to overwrite compile time setup.
#                    SUITS  =>  [\%Clubs_W, \%Diams_W, \%Hearts_W, \%Spades_W]
#,
#                       Alpha numeric scalar keys that could vary from deal to deal.
                     OPEN  => "" , STR  => "" , NT_Misfit => "N",
#                       Scalars. zero out between deals.
                     Valid   => 0 ,  # Init to 0 (False) before every deal; Set to 1 if hand OK for DOP eval
                     Sq      => 0 , No_Q   =>  0 , No_K  => 0 ,  No_A   => 0 ,   K_3_4  => 0 , Q_4 => 0,
                     ADJ_TOT => 0 , DOP_D  =>  0 , DOP_H => 0 ,  DOP_L  => 0 ,   DOP_Sy => 0 ,
                     DOP_HL  => 0 , DOP_HLD => 0 ,

#                       Suit numbers in increasing order of length. LEN_ORDER[0] is shortest, LEN_ORDER[3] is longest suit.
                     LEN_ORDER => [0,1,2,3] ,
#                       Arrays that need both hands defined to fill them in.
#                              [0..1,,]                 [-3..0,,,]              [-3..+3 ,,,]
                     DOP_HF  => [0,0,0,0], DOP_MISFIT => [0,0,0,0], DOP_WASTE => [0,0,0,0],
#                       These next ones vary depending on the contract, and on both hands together
                     DOP_DFIT => [0,0,0,0], DOP_NT_DED => 0,
   ); #end default hand
# These are to make it easy to reset the numeric scalars to 0 with a loop.
our @Hand_nvals = qw(Valid ADJ_TOT DOP_D DOP_H DOP_HL DOP_HLD DOP_L DOP_Sy No_A No_K No_Q Sq K_3_4 Q_4 );


# Note that the DEAL hash does not keep arrays of values by suit unless it has to.
# So that DOP_HF for example, which is an array in the HAND hash is just a total value in the DEAL hash

our %default_deal = (DEAL_NUM => 0, STR => "", OPENER => "", DS_E => "", DS_W => "N,L", DESCR => "", NAME => "" ,

#  Store some totals so no need to recalc all the time. Totals for both hands
                     DOP_H  => 0, DOP_L => 0, DOP_Sy => 0, ADJ_TOT => 0, DOP_HL => 0,
                     DOP_MISFIT => 0, DOP_WASTE => 0 , DOP_noWASTE => 0 , DOP_MIRROR => 0, # 0/-1/-2]
                     DOP_HF => 0, DOP_FN_TOT => 0,
#  Arrays for the Fit points in each suit. Need both hands to calc this
#  These entries are non zero only if the fit is 5:2 or longer.
                     DOP_FN => [0,0,0,0,], FIT_LEN => [0,0,0,0] , FIT_TYPE => [0,0, 0,0, 0,0, 0,0],
#                                                                              ^^^  ^^^  ^^^  ^^^
#                                     FIT_TYPE=8 elem array: one pair per suit:5,3  3,5  4,4  5,4  3,6  5,5 etc.
                     DOP_DFIT => [0,0,0,0], # Total of D (if any) and DF (if any) DOP  when strain [i] is trump.

                     DOP_HLF => 0 , #H + ADJ_TOT + L +SY + Hf + FN
                     DOP_HLF_NT => 0, # DOP_HLF + Misfit + Mirror + neg_WASTE   ? Should this include NT adj? No.
                     DOP_HLDF_SUIT => [0,0,0,0] , # HLF_NT + Dfit[i] + noWASTE
                     DOP_HLDFl => 0,   # HLDF_SUIT for the longest strain.
                     LONG_STRAIN => "",  # Suit Letter of Longest strain e.g. C,D,H,S
                     DOP_HLDFb => 0, #HLDF suit with the most HLDF pts.
                     BEST_STRAIN => "", # Suit Letter of Best strain e.g. C,D,H,S
                     DOP_NT_DED => [0,0] , # value to use if W | E is declaring NT. Dummy is 1pt less deduction.
                     FRL => 0, SRL => 0, NT_FRL => 0, NT_SRL => 0, FRL => 0, SRL => 0, NT_FRL => 0, NT_SRL => 0,
                     QUICK_LOSERS_NT => 0, QUICK_LOSERS_SUIT => 0,

#     Note (1) HLF_NT includes everything except D/Dfit and noWASTE which don't apply to NT anyway and NT_DED bec we don't know Declarer
#          (2) HLDF_Suit[i] is  everything:  HLF_NT +noWASTE +Dfit[i]. HDLFl is just a copy of the relevant HLDF_SUIT[i]
   ); # end default_deal

#                    20 DOP      21 DOP      22 DOP      index offset by 20 pts
our @suit_trix = (
                   [8, 40,45], [8, 50,55], [8, 60,"60+"] ,
                   [9, 40,45], [9, 50,55], [9, 60,"60+"] ,  #<----- 23, 24,  25 DOP
                   [10,40,45], [10,50,55], [10,60,"60+"] ,  #<----- 26, 27*, 28 DOP
                   [11,40,45], [11,50,55], [11,60,"60+"] ,  #<----- 29, 30,  31 DOP
                   [12,50,54], [12,55,60], [12,65,"65+"] ,  #<----- 32, 33*, 34 DOP
                   [13,70,70], [13,71,75], [13,76,"76+"] ,  #<----- 35, 36*, 37 DOP
                   [13,76,"76+"], [13,76,"76+"]         );  # 38, 39 extras to make tables same size.
our @nt_trix =   ( [1, 90,99], [7, 45,50],                  # 1 if <7; Assume 1NT with 21HCP
#                    22 DOP      23 DOP      24 DOP      index offset by 20  pts also
                   [8, 40,45], [8, 50,55], [8, 60,"60+"] ,
                   [9, 40,45], [9, 50,55], [9, 60,"60+"] ,  #<----- 25, 26*, 27 DOP
                   [10,40,45], [10,50,55], [10,60,"60+"] ,  #<----- 28, 29,  30 DOP
                   [11,40,45], [11,50,55], [11,60,"60+"] ,  #<----- 31, 32,  33!DOP
                   [12,50,54], [12,55,60], [12,65,"65+"] ,  #<----  33!,34*, 35
                   [13,70,70], [13,71,75], [13,76,"76+"] ); #<----  36, 37*, 38
our @Intervention_Suit_AOC = (13, 15, 18) ;     # HLD_AOC for intervention via a suit bid or t/o dbl at 1,2,3 level
our @Intervention_NT_AOC   = (15, 18, 21) ;     # HLD_AOC for intervention via a NT bid at 1,2,3 level
#
# Vars that hold actual values
#
our   (%Clubs_N, %Diams_N, %Hearts_N, %Spades_N);
our   (%Clubs_S, %Diams_S, %Hearts_S, %Spades_S);
our   (%Clubs_W, %Diams_W, %Hearts_W, %Spades_W);
our   (%Clubs_E, %Diams_E, %Hearts_E, %Spades_E);

our    @Suits_N = (\%Clubs_N, \%Diams_N, \%Hearts_N, \%Spades_N );
our    @Suits_S = (\%Clubs_S, \%Diams_S, \%Hearts_S, \%Spades_S );
our    @Suits_W = (\%Clubs_W, \%Diams_W, \%Hearts_W, \%Spades_W );
our    @Suits_E = (\%Clubs_E, \%Diams_E, \%Hearts_E, \%Spades_E );

# now define the hand hashes with the above keys and some starting values.
# West and East are the two main hashes; N and S are just used for GIB evals.

our %Hand_W   = ( NAME => "West", LET => "W",
                  SUITS => [\%Clubs_W, \%Diams_W, \%Hearts_W, \%Spades_W ],
#                       Alpha numeric scalar keys that could vary from deal to deal.
                     OPEN  => "Y" , STR  => "---" , NT_Misfit => "N",
#                       Scalars. zero out between deals.
                     Valid   => 0 ,  # Init to 0 (False) before every deal; Set to 1 if hand OK for DOP eval
                     Sq      => 0 , No_Q   =>  0,  No_K  => 0 ,  No_A   => 0 ,  K_3_4  => 0 , Q_4 => 0,
                     ADJ_TOT => 0 , DOP_D  =>  0 , DOP_H => 0 ,  DOP_L  => 0,   DOP_Sy => 0 ,
                     DOP_HL  => 0 , DOP_HLD => 0 ,

#                       Suit numbers in increasing order of length. LEN_ORDER[0] is shortest, LEN_ORDER[3] is longest suit.
                     LEN_ORDER => [0,1,2,3] ,
#                       Arrays that need both hands defined to fill them in.
#                              [0..1,,]                 [-3..0,,,]              [-3..+3 ,,,]
                     DOP_HF  => [0,0,0,0], DOP_MISFIT => [0,0,0,0], DOP_WASTE => [0,0,0,0],
#                       These next ones vary depending on the contract, not just on the deal
                     DOP_DFIT => [0,0,0,0], # Total of D (if any) and DF (if any) DOP  when strain [i] is trump.
            );

our %Hand_E   = ( NAME => "East", LET => "E",
                  SUITS => [\%Clubs_E, \%Diams_E, \%Hearts_E, \%Spades_E ],
#                       Alpha numeric scalar keys that could vary from deal to deal.
                     OPEN  => "N" , STR  => "---" , NT_Misfit => "N",
#                       Scalars. zero out between deals.
                     Valid   => 0 ,  # Init to 0 (False) before every deal; Set to 1 if hand OK for DOP eval
                     Sq      => 0 , No_Q   =>  0,  No_K  => 0 ,  No_A  => 0 ,   K_3_4  => 0 , Q_4 => 0,
                     ADJ_TOT => 0 , DOP_D  =>  0 , DOP_H => 0 ,  DOP_L  => 0,   DOP_Sy => 0 ,
                     DOP_HL  => 0 , DOP_HLD => 0 ,

#                       Suit numbers in increasing order of length. LEN_ORDER[0] is shortest, LEN_ORDER[3] is longest suit.
                     LEN_ORDER => [3,2,1,0] ,
#                       Arrays that need both hands defined to fill them in.
#                              [0..1,,]                 [-3..0,,,]              [-3..+3 ,,,]
                     DOP_HF  => [0,0,0,0], DOP_MISFIT => [0,0,0,0], DOP_WASTE => [0,0,0,0],
#                       These next ones vary depending on the contract, not just on the deal
                     DOP_DFIT => [0,0,0,0], # Total of D (if any) and DF (if any) DOP  when strain [i] is trump.
            );
# Min keys spec'd; others will be autovivified by the StartMain copy of %default_hand to these two.
our %Hand_N = (  NAME => "North", LET => "N",
                 Valid   => 0 ,  # Init to 0 (False) before every deal; Set to 1 if hand OK for DOP eval
                 SUITS => [\%Clubs_N, \%Diams_N, \%Hearts_N, \%Spades_N ], ) ;  #used for GIB rpts.
our %Hand_S = (  NAME => "South", LET => "S",
                 Valid   => 0 ,  # Init to 0 (False) before every deal; Set to 1 if hand OK for DOP eval
                 SUITS => [\%Clubs_S, \%Diams_S, \%Hearts_S, \%Spades_S ], ) ; #used for GIB reports.
#
our %Deal   = (DEAL_NUM => 0,  STR => "W= :: E=", OPENER => "W", DS_E => "", DS_W => "", DESCR => "NONE", NAME => "EW",
               DOP_H  => 0, DOP_L => 0, DOP_Sy => 0,  ADJ_TOT => 0, DOP_HL => 0,
               DOP_MISFIT => 0, DOP_WASTE => 0, DOP_noWASTE => 0 ,DOP_MIRROR => 0, # 0/-1/-2]
               DOP_HF => 0, DOP_FN_TOT => 0,
#              Arrays for the Fit points in each suit. Need both hands to calc this
#              These entries are non zero only if the fit is 5:2 or longer.
               DOP_FN => [0,0,0,0,], FIT_LEN => [0,0,0,0] , FIT_TYPE => [0,0, 0,0, 0,0, 0,0],
#                                                                         ^^^^^^^^^^^^^^^^^^
#                           FIT_TYPE=8 elem array: one pair per suit: 5,3  3,5  4,4  5,4  3,6  5,5 etc.
               DOP_DFIT => [0,0,0,0], # Total of D (if any) and DF (if any) DOP  when strain [i] is trump.
               DOP_HLF => 0 , #H + ADJ_TOT + L +SY + Hf + FN
               DOP_HLF_NT => 0, # DOP_HLF + Misfit + Mirror + neg_WASTE   ? Should this include NT adj? No.
               DOP_HLDF_SUIT => [0,0,0,0] , # HLF_NT + Dfit[i] + noWASTE
               DOP_HLDFl => 0,   # HLDF_SUIT for the longest strain.
               LONG_STRAIN => "",  # Suit Letter of Longest strain e.g. C,D,H,S
               DOP_HLDFb => 0, #HLDF suit with the most HLDF pts.
               BEST_STRAIN => "", # Suit Letter of Best strain e.g. C,D,H,S
               DOP_NT_DED => [0,0] , # value to use if W | E is declaring NT. Dummy is 1pt less deduction.
               FRL => 0, SRL => 0, NT_FRL => 0, NT_SRL => 0, QUICK_LOSERS_NT => 0, QUICK_LOSERS_SUIT => 0,
#     Note (1) HLF_NT includes everything except D/Dfit and noWASTE which don't apply to NT anyway and NT_DED bec we don't know Declarer
#          (2) HLDF_Suit[i] is  everything:  HLF_NT +noWASTE +Dfit[i]. HDLFl is just a copy of the relevant HLDF_SUIT[i]
   ); # end live Deal
our %Deal_NS = ( DEAL_NUM => 0,  STR => "N= :: S=", NAME => "NS" ) ;  #initialize if req'd

# Version 13.0 added hashes to hold AOC adjustments
our %AOC_DOP_W = ( AOC_LET => "W", AOC_H => [0,0,0,0], AOC_L_SUIT => [0,0,0,0],  AOC_L_NT => [0,0,0,0],
                                   AOC_H_TOT => 0,     AOC_L_SUIT_TOT => 0,      AOC_L_NT_TOT => 0,
             );
our %AOC_DOP_E = ( AOC_LET => "E", AOC_H => [0,0,0,0], AOC_L_SUIT => [0,0,0,0],  AOC_L_NT => [0,0,0,0],
                                   AOC_H_TOT => 0,     AOC_L_SUIT_TOT => 0,      AOC_L_NT_TOT => 0,
             );
our %AOC_DOP_DEAL=(AOC_LET => "D", AOC_H => [0,0,0,0], AOC_L_SUIT => [0,0,0,0],  AOC_L_NT => [0,0,0,0],
                                   AOC_H_TOT => 0,     AOC_L_SUIT_TOT => 0,      AOC_L_NT_TOT => 0,
              );

# Stuff for analyzing GIB results.
our %NS_Results=( DEAL_ID => "No-ID-NS" ,  Side => "NS" ,
                  Long_Fit => "D" , Best_Fit => "C",  NT_HLF      => 0.0,
                  NT_HLF_ADJ=> 0.0, NT_Trix   => 0,   NT_Pct_lo   => 0.0 , NT_Pct_hi   => 0.0 ,
                  Long_HLDF => 0.0, Long_Trix => 0,   Long_Pct_lo => 0.0 , Long_Pct_hi => 0.0 ,
                  Best_HLDF => 0.0, Best_Trix => 0,   Best_Pct_lo => 0.0 , Best_Pct_hi => 0.0 ,
                  GIB_NT_Trix => 0, GIB_Long_Trix => 0, GIB_Best_Trix => 0,
                  Deal_STR => "" , DEAL_ID  => "" ,
);

our %EW_Results=( DEAL_ID => "No-ID-EW" ,  Side => "EW" ,
                  Long_Fit => "D" , Best_Fit => "C",  NT_HLF      => 0.0,
                  NT_HLF_ADJ=> 0.0, NT_Trix   => 0,   NT_Pct_lo   => 0.0 , NT_Pct_hi   => 0.0 ,
                  Long_HLDF => 0.0, Long_Trix => 0,   Long_Pct_lo => 0.0 , Long_Pct_hi => 0.0 ,
                  Best_HLDF => 0.0, Best_Trix => 0,   Best_Pct_lo => 0.0 , Best_Pct_hi => 0.0 ,
                  GIB_NT_Trix => 0, GIB_Long_Trix => 0, GIB_Best_Trix => 0,
                  Deal_STR => "" , DEAL_ID  => "",
);
our %ZERO_Results=(DEAL_ID => "" ,  Side => "" ,
                  Long_Fit => "D" , Best_Fit => "C",  NT_HLF      => 0.0,
                  NT_HLF_ADJ=> 0.0, NT_Trix   => 0,   NT_Pct_lo   => 0.0 , NT_Pct_hi   => 0.0 ,
                  Long_HLDF => 0.0, Long_Trix => 0,   Long_Pct_lo => 0.0 , Long_Pct_hi => 0.0 ,
                  Best_HLDF => 0.0, Best_Trix => 0,   Best_Pct_lo => 0.0 , Best_Pct_hi => 0.0 ,
                  GIB_NT_Trix => 0, GIB_Long_Trix => 0, GIB_Best_Trix => 0,
                  Deal_STR => "" , DEAL_ID  => "" ,
);

our %GIB_Deal = ( RAW_STR => "", W_GIB_STR => "" , N_GIB_STR => "" , E_GIB_STR => "" , S_GIB_STR => "" ,
                  DEAL_PFX  => "", DEAL_lineno => 0, DEAL_ID => 0 ,      #DEAL_ID should be the OPT_G . line_no or on deal_line
     # next line is max number of tricks that can be taken in each strain; NS=max(GIB), EW=13 - Min(GIB)
     NS_TRIX =>[0,0,0,0,0] , EW_TRIX => [0,0,0,0,0] ,
     GIB_STRAINS => [ "NT","S","H", "D", "C" ] ,            # just for documentation; never used.
      # NT, S, H, D, C  -- the exact opposite order of the rest of the program.

#  The rest of the fields never used. could be omitted.
     W_suits => ["", "", "" , "" ],  E_suits => ["", "", "" , "" ],
     N_suits => ["", "", "" , "" ],  S_suits => ["", "", "" , "" ],
     # Trix taken by NS in  NT, S, H, D, C with s,e,n,w as dealer
     NT_Trix => [0,0,0,0], SP_TRIX => [0,0,0,0], HE_TRIX => [0,0,0,0], DI_TRIX => [0,0,0,0], CL_TRIX => [0,0,0,0],
     # we don't fill these in as we don't care who is on lead.
) ;
our %Zero_GIB_Deal = ( RAW_STR => "", W_GIB_STR => "" , N_GIB_STR => "" , E_GIB_STR => "" , S_GIB_STR => "" ,
                       DEAL_lineno => 0 , DEAL_ID  => "",    #DEAL_ID should be the "OPT_G . line_no" or on deal_line
      # next line is max number of tricks that can be taken in each strain; NS=max(GIB), EW=13 - Min(GIB)
                     NS_TRIX => [0,0,0,0,0] , EW_TRIX => [0,0,0,0,0] ,
                     GIB_STRAINS => [ "NT","S","H", "D", "C" ] ,  # just for documentation; never used.
#  The rest of the fields never used. could be omitted.
     W_suits => ["", "", "" , "" ],  E_suits => ["", "", "" , "" ],
     N_suits => ["", "", "" , "" ],  S_suits => ["", "", "" , "" ],
     # Trix taken by NS in  NT, S, H, D, C with s,e,n,w as dealer
     NT_Trix => [0,0,0,0], SP_TRIX => [0,0,0,0], HE_TRIX => [0,0,0,0], DI_TRIX => [0,0,0,0], CL_TRIX => [0,0,0,0],
     # we don't fill these in as we don't care who is on lead.
);

our @Hand_href = ( \%Hand_N, \%Hand_E, \%Hand_S, \%Hand_W ) ;           # DDS hand order.
our @Suit_href = ([\%Clubs_N, \%Diams_N, \%Hearts_N, \%Spades_N] ,      # DDS hand order; DOP suit order.
                  [\%Clubs_E, \%Diams_E, \%Hearts_E, \%Spades_E] ,
                  [\%Clubs_S, \%Diams_S, \%Hearts_S, \%Spades_S] ,
                  [\%Clubs_W, \%Diams_W, \%Hearts_W, \%Spades_W] ,
                 ) ;

our $CSV_Col_Hdrs = "DealID,Side,Strain,DD Trix,DOP Trix,DOP Lo%,DOP Hi%,HLDF,DD<DOP,DD=DOP,DD>DOP,DD<Tot,DD=Tot,DD>Tot,RowCount,Deal String";
our $CSV_fmt = "%s,%s,%s,%2i,%2i,%5.2f,%5.2f,%5.2f,%2i,%2i,%2i,%6i,%6i,%6i,%07i,%s";
our $CSV_rowcount = 0 ;
our @dd_dop_diff_count = (0,0,0); #DD<DOP, DD=DOP, DD>DOP count of number of times this happened.

$side_str = "EW" ; $side_num = 1 ; $side_href = \%EW_Results;  # GIB works on NS tricks; but we can pretend
$ew_side_href = \%EW_Results;
$ns_side_href = \%NS_Results;
$best_suit = 5 ; $long_suit= 5 ;  #start with invalid numbers.
$gib_run = 0 ; # default to false
#                        .             v                v                v                V
#GIB line (87 Chars) = T5.K4.652.A98542 K6.QJT976.QT7.Q6 432.A.AKJ93.JT73 AQJ987.8532.84.K:65658888888843433232
#                      S   H  D    C     S   H     D   C   S K  D     C     S      H   D C[.NT..Sp..He..Di..Cl.]
#                      <--- West------>  <--- North ---> <---- East-----> <--- South ---->[leader = s,e,n,w for each]
# suits separated by DOTs, Compass separated by SPACE , COLON marks end of cards, start of tricks.
# tricks are given for NS 5 sets of 4 chars [0-9A-D]; 1st set = NT, then S,H,D,C; each set dealer=s,e,n,w





### Declarations Done. Should be some keys in the hand hashes, nothing in suit hashes ...

1; # so require succeeds
