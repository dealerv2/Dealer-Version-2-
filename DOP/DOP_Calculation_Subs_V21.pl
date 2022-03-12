# No shebang since this file brought in by require.
## Author      Version  Date           Comment
#  JGM         1.0      2020/10/22     Split out of original subs file getting too long.
#  JGM         9.0      2020/10/22     Calculation routines, Suit, Hand, Deal.
#  JGM        10.0      2020/11/02     All working.
#  JGM        10.2      2020/11/04     Keeping in sync with numbering
#  JGM        13.0      2021/01/22     Added Sy pts for KQJT, AQJT in 4 card suits. For 5-5 fits added Dpts in both hands.
#                                      Modified Waste pt calcs to So AJx(x) vs a stiff is only -1, not -2.
#  JGM        13.2      2021/02/13     Added +1 for four Queens per Darricades 8Steps PDF.
#                                      Also count First and Second Round Losers (FRL, SRL, NT_FRL, NT_SRL) for report trix.
#  JGM        21.0      2021/08/28     Mods for Terse_Rpt (-q) for dealer (if any)
#
#------------------------------------------------------------------------------------------------------------
# Subs for Evaluation of a single suit. Part 1 of 3 of Complete DOP evaluation
sub eval_suit       {         # called with( hnd_ref, suit_num) uses sref->{STR} to calc {DOP_H}, {Has_A}, etc.
   my $hnd_ref = shift;
   my $suit_num = shift;         ## ## eval suit num : $suit_num
   my ($s_ref, $s_str, $s_cpy);
   local $req_dbg = $req_dbg + 2;
    $s_ref = get_suit_href($hnd_ref, $suit_num )   ;
    $s_str = $s_ref->{STR};
    $s_cpy = $_str ;
    $hnd_t = ref $hnd_ref;
    $suit_t = ref $s_ref;
    print STDERR  "eval_suit:: Got suit ref for suit# $suit_num $hnd_t :: $suit_t [$s_str] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;

   $s_ref->{LET}    = $suit_lets[$suit_num];
   $s_ref->{Has_A}  = $s_str =~ tr/A// ; # should be Zero or One thanks to earlier error checking
   $s_ref->{Has_K}  = $s_str =~ tr/K// ;
   $s_ref->{Has_Q}  = $s_str =~ tr/Q// ;
   $s_ref->{Has_J}  = $s_str =~ tr/J// ;
   $s_ref->{Has_T}  = $s_str =~ tr/T// ;
   $s_ref->{Has_9}  = $s_str =~ tr/9// ;
   $s_ref->{LEN}    = $s_str =~ tr///c ;  # count non blanks
   $s_ref->{Sy_Num} = $s_str =~ tr/AKQJ//; # count honors in suit. T not an honor.
   print STDERR  "eval_suit:: $s_ref->{STR} sets: LET=$s_ref->{LET}, LEN=$s_ref->{LEN}, SyNum=$s_ref->{Sy_Num} {$Debug gt $req_dbg}\n"
         if $Debug > $req_dbg;

   my ($dop_h, $dop_l, $dop_d, $dop_sy) = ( 0,0,0,0,);
   my ($h_adj, $hh_adj, $qj_adj, $q_acc, $j_acc, $t_acc, $k_cnt );
   $s_ref->{DOP_D} =  0;
   $s_ref->{DOP_H} =  0;
   $s_ref->{DOP_L} =  0;
   $s_ref->{DOP_Sy} = 0;

   if ( $s_ref->{LEN} eq 0 ) {
      $s_ref->{DOP_D} = $SUIT_DOP{D_VOID} ; return (1) ;   # void suit has no cards....
   }
   if ( $s_ref->{LEN} == 1 ) {
      $s_ref->{DOP_D} = $SUIT_DOP{D_STIFF};
      &eval_1crd_suit($s_ref)  ;
      print STDERR  "eval_suit:: 1crd H Pts[$s_ref->{DOP_H} ], Stiff D pts [$s_ref->{DOP_D}] {$Debug ge $req_dbg}\n" if $Debug > $req_dbg; ;
      return 1 ;
   }

   if ( $s_ref->{LEN} == 2 ) {
      $s_ref->{DOP_D} = $SUIT_DOP{D_DBL};  #maybe 2 dbltons worth something
      &eval_2crd_suit($s_ref) ; return 1 ;
   }

   $s_ref->{DOP_D} = 0 ; # Never D pts for suits over 2

   if (($s_ref->{LEN} == 3 ) || ($s_ref->{LEN} == 4)  ) { &eval_34crd_suit($s_ref) ; return 1 ; }

   if ( $s_ref->{LEN} == 5 ) { &eval_5crd_suit($s_ref) ; return 1 ; } # 5crd L and Sy are different from ...

   if ( $s_ref->{LEN} >= 6 ) { &eval_long_suit($s_ref) ; return 1 ; } # 6+ card L and Sy
   warn "Cant Happen in eval_suit:: -- impossible return statement\n";
   return 0;
} # end eval suits
sub eval_honors     {         # called with sref. Use for suits >=3 cards. No need to apply short honor adj
      my $s_ref = shift;
      local $req_dbg = $req_dbg + 1;
      $s_str = $s_ref->{STR};
      if ($s_ref->{Has_A} ) {
         $s_ref->{DOP_H} += $SUIT_DOP{H_A};
      }
      if ($s_ref->{Has_K} ) {
         $s_ref->{DOP_H} += $SUIT_DOP{H_K};
      }
      if ($s_ref->{Has_Q} ) {
         $q_acc = $s_ref->{Has_A} || $s_ref->{Has_K} || $s_ref->{Has_J} ; #Accompanied AQ, KQ, QJ worth more than Qx
         $s_ref->{DOP_H} += ($q_acc) ? $SUIT_DOP{H_aQ} : $SUIT_DOP{H_uQ} ; # Qxx = 1.5, Q with H = 2
      }
      if ($s_ref->{Has_J} ) {
         $j_acc = $s_ref->{Has_A} || $s_ref->{Has_K} || $s_ref->{Has_Q} || $s_ref->{Has_T} ;
         $s_ref->{DOP_H} += ($j_acc) ? $SUIT_DOP{H_aJ} : $SUIT_DOP{H_uJ}  ; #Jxx = 0.5, J with H or T = 1
      }
      if ($s_ref->{Has_T} ) { # T worth 1 with a J or Q, else 0.5 with K, else 0
         $dop_T = ( ($s_ref->{Has_J} || $s_ref->{Has_Q}) ?  $SUIT_DOP{H_aT} :
                    ($s_ref->{Has_K} ? $SUIT_DOP{H_kT} : 0 ) ) ;
         $s_ref->{DOP_H} += $dop_T ;
       }  # end Has_T

       # add one more point for 9 with J&T in a suit like AJT9(x), KJT9(x), QJT9(x). Wont work if user entered T9J say.
       if ( ($s_str =~ m/JT9/ ) && ( ($s_str =~ tr/AKQ// ) > 0 ) ) {$s_ref->{DOP_H} += 1 ; }
       return 1 ;  #return success ...
} # end eval honors
sub eval_1crd_suit  {         # set DOP_D = 2 and DOP_H with deduction for short honors. L & Sy = 0
      my $s_ref = shift;
      local $req_dbg = $req_dbg + 1;
      $s_ref->{DOP_D} = $SUIT_DOP{D_STIFF} ; #even stiff H get the D pts.
      print STDERR "eval_1crd_suit:: D Pts $SUIT_DOP{D_STIFF}, $s_ref->{DOP_D} { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;

      $h_adj = $SUIT_DOP{Hs_H};          # -1; add this (ie deduct from ) the value of the honor card.
      print STDERR "Has_A[$s_ref->{Has_A}],Has_K[$s_ref->{Has_K}],Has_Q[$s_ref->{Has_Q}], Has_J[$s_ref->{Has_J}], $h_adj\n" if $Debug >= $req_dbg;

      if ($s_ref->{Has_A} ) {
         $s_ref->{DOP_H} = $SUIT_DOP{H_A} + $h_adj ;
         $s_ref->{DOP_H} = ($s_ref->{DOP_H}  >= 0 ) ? $s_ref->{DOP_H}  : 0; # cant deduct more than the honor is worth..
         return (1) ; # only 1 card in this suit.
      }
      if ($s_ref->{Has_K} ) {
         $s_ref->{DOP_H} = $SUIT_DOP{H_K} + $h_adj ;
         $s_ref->{DOP_H} = ($s_ref->{DOP_H}  >= 0 ) ? $s_ref->{DOP_H}  : 0; # cant deduct more than the honor is worth...
         return (1) ; # only 1 card in this suit.
      }
      if ($s_ref->{Has_Q} ) {
         $s_ref->{DOP_H} = $SUIT_DOP{H_uQ} + $h_adj ; # should be 1.5 - 1 = 0.5
         $s_ref->{DOP_H} = ($s_ref->{DOP_H}  >= 0 ) ? $s_ref->{DOP_H}  : 0; # cant deduct more than the honor is worth...
         print STDERR  "eval_1crd_suit:: Q Pts $SUIT_DOP{H_uQ}, $s_ref->{DOP_H}, $h_adj { $Debug gt $req_dbg } \n" if $Debug > $req_dbg  ;
         return (1) ;
      }
      if ($s_ref->{Has_J} ) {
         $s_ref->{DOP_H} = $SUIT_DOP{H_uJ} + $h_adj ; # should be 0.5 - 1 = -0.5
         $s_ref->{DOP_H} = ($s_ref->{DOP_H}  >= 0 ) ? $s_ref->{DOP_H}  : 0; # cant deduct more than the honor is worth..
         print STDERR  "eval_1crd_suit:: J Pts $SUIT_DOP{H_uJ}, $s_ref->{DOP_H} { $Debug gt $req_dbg } \n" if $Debug > ($req_dbg);

         return (1) ;
      }
      # fall thru to here for [2-9, T, x ]
      $s_ref->{DOP_H} = 0;          # No Honors means no H pts.
      return (1); # Done the D & H pts;
} # end sub 1crd_suit
sub eval_2crd_suit  {         # Check for HH, Hx, HT, xx. Deduct from DOP_H as reqd. set DOP_D = 0.5, L & Sy = 0
                              # No D for Dblton, but we give 0.5 anyway in case hand has two...
# Revised on 2020-10-13 per Darricades acceptance of various Hs_HH combos
# We now have AK < AQ < AJ < Ax = KQ < KJ < KT < Kx < QJ < QT < Qx < JT < Jx = 0.
      my $s_ref = shift;
      local $req_dbg = $req_dbg + 1;

      $s_ref->{DOP_D} = $SUIT_DOP{D_DBL} ; # set to 0.5; when totalling will be truncated by int unless 2+ dbltons
      my $p ;
      my $shh  = $SUIT_DOP{Hs_HH} ;       # deduction for Honors Dblton although there are lots of special cases
      $_ = $s_ref->{STR} ;
      # Doubletons get complicated. Next code avoids a bunch of nested if's
      if    ( m/AK/ || m/KA/ ) { $p = $SUIT_DOP{H_A} + $SUIT_DOP{H_K}  + $shh ; }
      elsif ( m/AQ/ || m/QA/ ) { $p = $SUIT_DOP{H_A} + $SUIT_DOP{H_aQ} + $shh ; }
      elsif ( m/AJ/ || m/JA/ ) { $p = $SUIT_DOP{H_A} + $SUIT_DOP{H_aJ} + $SUIT_DOP{Hs_AJ} ; }   # PD Update 2020-10-23
      elsif ( m/A/  )          { $p = $SUIT_DOP{H_A}                  ; }                       # This means Ax not stiff A.
      elsif ( m/KQ/ || m/QK/ ) { $p = $SUIT_DOP{H_K} + $SUIT_DOP{H_aQ} + $SUIT_DOP{Hs_KQ} ; }   # PD Update 2020-10-23
      elsif ( m/KJ/ || m/JK/ ) { $p = $SUIT_DOP{H_K} + $SUIT_DOP{H_aJ}; }
      elsif ( m/KT/ || m/TK/ ) { $p = $SUIT_DOP{H_K} + $SUIT_DOP{H_kT}; }
      elsif ( m/K/  )          { $p = $SUIT_DOP{H_K}                  ; }                       # This means Kx not stiff K.
      elsif ( m/QJ/ || m/JQ/ ) { $p = $SUIT_DOP{H_aQ} + $SUIT_DOP{H_aJ}+ $SUIT_DOP{Hs_QJ} ; }   # keep the usual HH deduction.
      elsif ( m/QT/ || m/TQ/ ) { $p = $SUIT_DOP{H_uQ} + $SUIT_DOP{H_aT}+ $SUIT_DOP{Hs_QT} ; }   # ditto. Qx , QT < QJ (1,1.5,2)
      elsif ( m/Q/  )          { $p = $SUIT_DOP{H_uQ} + $SUIT_DOP{Hs_Qx} ; }                    # This means Qx not stiff Q.
      elsif ( m/JT/ || m/TJ/ ) { $p = $SUIT_DOP{H_aJ} + $SUIT_DOP{H_aT}+ $SUIT_DOP{Hs_JT} ; }   #
      elsif ( m/J/  )          { $p = $SUIT_DOP{H_uJ} + $SUIT_DOP{Hs_Jx} ; } # This means Jx not stiff J
      else                     { $p = 0 ; } # suits like xx, Tx, dd, get no H and no D pts.
      print STDERR  "eval_2crd_suit:: $_ is worth $p H points { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;
      $s_ref->{DOP_H} = $p;
      return (1); # Done the D pts, and the H pts. Nothing else to do for a Dblton. xx, Tx immaterial
} # end 2crd_suit
sub eval_34crd_suit {         # set DOP_H. No short honor deductions. Therefore DOP_D, DOP_L, = 0.
   my $s_ref = shift;
   local $req_dbg = $req_dbg + 1;

   $s_ref->{DOP_D} = 0 ; #no D pts for suits longer  than 2.
   $s_ref->{DOP_L} = 0 ; #no L pts for suits shorter than 5.
   $s_ref->{DOP_Sy} =0 ; # 2021/01/12. 4 card suit with KQJT, AQJT, gets 1 Sy pt. AKJT and AKQJ already have enough.

   &eval_honors($s_ref); #set the H points

   # Now set the Sy pts for some 4 card suits. This is new in 2021.
   $_ = $s_ref->{STR} ;
   if    ( m/KQJT/ || m/AQJT/ ) { $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_4} ; }
   # Per Darricades email, optional to add 0, 0.5 or 1 pt for AKQJ, AKQT, or AKJT

   print STDERR  "eval_34crd_suit:: $s_ref->{STR} is worth [H:$s_ref->{DOP_H}], [Sy:$s_ref->{DOP_Sy}] points { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;
   return 1 ;
} # end eval_34crd_suit
sub eval_5crd_suit  {         # H, L, Sy pts all need doing ... set DOP_D=0
   my $s_ref = shift;
   local $req_dbg = $req_dbg + 1;

   $s_ref->{DOP_D} = 0 ;      # no D pts for suits longer  than 2.
   &eval_honors($s_ref);      # Set the H pts.
   print STDERR  "eval_5crd_suit:: $s_ref->{STR} is worth $s_ref->{DOP_H} H points { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;

   #   L pts only if H pts >= 3
   $s_ref->{DOP_L} = ($s_ref->{DOP_H} >= 3 ) ? $SUIT_DOP{L_3x5} : 0 ;
   print STDERR  "eval_5crd_suit:: L pts = $s_ref->{DOP_L} { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;

# Sy Points for 5 card suit.
# Original PD in book = 1 Sy pt if 3+ in {AKQJ}.
# New draft proposal: 1 Sy pt for 3 out of 4, + extra pt for 4th honor or a Ten. (HHHH, HHHT, HHHHT all worth 2 Sy)
#For flexibility this extra pt is 2 separate symbolic constants can be anything I like. 0, 0.5 etc.
   if     ($s_ref->{Sy_Num} == 4 ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_5} + $SUIT_DOP{Sy_HHHH} ;
   }
   elsif (($s_ref->{Sy_Num} == 3 ) && $s_ref->{Has_T} ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_5} + $SUIT_DOP{Sy_HHHT} ;
   }
   elsif (($s_ref->{Sy_Num} == 3 ) ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_5};
   }
   else {
         $s_ref->{DOP_Sy} = 0 ;
   } # end Sy elsif chain
   print STDERR  "eval_5crd_suit:: DOP_Sy= $s_ref->{DOP_Sy}, DOP_H =$s_ref->{DOP_H}, DOP_L=$s_ref->{DOP_L} { $Debug ge $req_dbg } \n" if $Debug > $req_dbg ;
} #end 5crd suit
sub eval_long_suit  {         # 6+ suit, H, L, Sy pts all need doing ... set DOP_D=0

   my $s_ref = shift;
   local $req_dbg = $req_dbg + 1;

   $s_ref->{DOP_D} = 0 ;      # no D pts for suits longer  than 2.
   &eval_honors($s_ref);      # Set the H pts.
   print STDERR  "eval_long_suit:: $s_ref->{STR}=> DOP_H = $s_ref->{DOP_H} { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;

   # DOP_L pts = 1L +2*(n-6) if < 3 H pts, 2L +2*(n-6) if >= 3H pts.
   $s_ref->{DOP_L} = ($s_ref->{DOP_H} >= 3 ) ? $SUIT_DOP{L_3x6} : $SUIT_DOP{L_6} ;
   if ($s_ref->{LEN} > 6 ) {
      $s_ref->{DOP_L} += ($s_ref->{LEN} - 6 ) * $SUIT_DOP{L_7plus} ;
   }
   print STDERR  "eval_long_suit:: $s_ref->{STR}=> DOP_L = $s_ref->{DOP_L} { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;

# Sy Points for 6+ card suit. 6,7,8 etc. card suits all get same Sy Pts.
# Original PD in book = 2 Sy pts if 3+ honors in {AKQJ}.
# New draft proposal 2 Sy pts for 3 out of 4, + extra pt for 4th honor or a Ten. (HHHH, HHHT, HHHHT all worth 4 Sy)
#For flexibility this extra pt is 2 separate symbolic constants can be anything I like. 0, 0.5 etc.

   if     ($s_ref->{Sy_Num} == 4 ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_6} + $SUIT_DOP{Sy_HHHH} ;
   }
   elsif (($s_ref->{Sy_Num} == 3 ) && $s_ref->{Has_T} ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_6} + $SUIT_DOP{Sy_HHHT} ;
   }
   elsif (($s_ref->{Sy_Num} == 3 ) ) {
         $s_ref->{DOP_Sy} = $SUIT_DOP{Sy_6};
   }
   else {
         $s_ref->{DOP_Sy} = 0 ;
   } # end long suit Sy elsif chain
   print STDERR  "eval_long_suit:: $s_ref->{STR} => DOP_Sy = $s_ref->{DOP_Sy} { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;
   return 1;
}  #end sub eval_long_suit

#------------------------------------------------------------------------------------------------------------
# Hand Evaluation Routines  Part 2 of 3 for Complete DOP evaluation
sub eval_hand {      # args=hand_href. Eval each suit in turn with eval_suit. Sets hand DOP values in hand hash.
   my $hnd_ref = shift;
   local $req_dbg = $req_dbg + 1;
   my @Hand_Hcards = ();  #A,K,Q,J,T,9
   my ($A, $K, $Q, $J, $T);
   my @sptrs = ();
   @sptrs = &get_sptrs ( $hnd_ref );
   my $hand_t = ref($hnd_ref);

   foreach $j ( 0 .. 3 ) {
      &eval_suit ($hnd_ref, $j) ;
   } # end eval foreach suit.
   print STDERR "eval_hand:: Suit Evals all Done for $hnd_ref->{NAME} --------------->{$Debug gt $req_dbg}\n" if $Debug > $req_dbg;
   print STDERR "\t\t <Continue to get totals and adj for hand as whole> {$Debug ge $req_dbg}\n" if $Debug > $req_dbg;

   $hnd_ref->{Sq} = &set_Sq(@sptrs); # returns 0 or -1
   @Hand_Hcards = &count_hand_Hcards(@sptrs);
   ($A, $K, $Q, $J, $T) = @Hand_Hcards[0 .. 4];
   print STDERR   "eval_hand:: Count Hcards Returns=> Aces= $A, Kings=$K, Queens=$Q, Jacks=$J, Tens=$T {$Debug gt $req_dbg}\n" if $Debug > $req_dbg;
   $hnd_ref->{No_A}  = (($Hand_Hcards[0] eq 0 ) ? $HAND_DOP{No_A} : 0) ;
   $hnd_ref->{No_K}  = (($Hand_Hcards[1] eq 0 ) ? $HAND_DOP{No_K} : 0) ;
   $hnd_ref->{No_Q}  = (($Hand_Hcards[2] eq 0 ) ? $HAND_DOP{No_Q} : 0) ;
   $hnd_ref->{K_3_4} = (($Hand_Hcards[1] eq 4 ) ? $HAND_DOP{K_4}  : (($Hand_Hcards[1] eq 3 ) ? $HAND_DOP{K_3} : 0)) ;
   $hnd_ref->{Q_4}   = (($Hand_Hcards[2] eq 4 ) ? $HAND_DOP{Q_4}  : 0) ;
#               $K3  = (($K              eq 4 ) ?   2             : (($K              eq 3 ) ?     1          : 0)) ;  OK
   $ded_tot =  $hnd_ref->{Sq} + $hnd_ref->{No_K} + $hnd_ref->{No_Q} ;
   # if ded_total is -2 or -3 then we do not need to check the No_A value since -2 is most we can deduct.
   # if not then we will need to check if Opener or Responder to see if deduct for no Aces.
   if    ($ded_tot <= -2 )          { $ded_tot = -2 ; }
   elsif ($hnd_ref->{OPEN} eq 'Y' ) { $ded_tot += $hnd_ref->{No_A} };
   $hnd_ref->{ADJ_TOT} = $ded_tot + $hnd_ref->{K_3_4} + $hnd_ref->{Q_4} ;
   print STDERR  "eval_hand:: ****  ADJ summary  **** {$Debug gt $req_dbg}\n"  if $Debug > $req_dbg;
   printf STDERR ( "\t%4s Hand[ %s ] K_adj=(+[%d],-[%d]) Q_adj=(+[%d],-[%d]) Deductions[-A=%d, Sq=%d ], Adj_Tot=[%d] Opener=[%s] {$Debug gt $req_dbg}\n",
         $hnd_ref->{NAME}, $hnd_ref->{STR}, ($hnd_ref->{K_3_4}, $hnd_ref->{No_K}), ($hnd_ref->{Q_4}, $hnd_ref->{No_Q}),
         $hnd_ref->{No_A}, $hnd_ref->{Sq}, $hnd_ref->{ADJ_TOT}, $hnd_ref->{OPEN} ) if $Debug > $req_dbg;

   # Create array of suit numbers in ascending order of length
   @lengths = ();
   foreach $ptr (@sptrs) {          # sptrs are in order C, D, H, S
      push @lengths, $ptr->{LEN} ;
      #print "Pushing $ptr->{LEN} for suit $ptr->{LET} onto lengths {$Debug gt $req_dbg}\n"  if $Debug > $req_dbg;
   }
   @lengths_sorted = &sort_arr_idxes (\@lengths);
   @{$hnd_ref->{LEN_ORDER}} = @lengths_sorted;  # the suit numbers in increasing order of len. eg 3=5=1=4 gives [2,0,3,1]
   $ss_num = $hnd_ref->{LEN_ORDER}->[0];
   $ss_len = $sptrs[$ss_num]->{LEN};
   printf  STDERR "eval_hand:: $hand_t LEN_ORDER = %s ss_num=[%d],ss_len=[%d]{$Debug >= $req_dbg}\n",
         (join ';',  @{$hnd_ref->{LEN_ORDER}}), $ss_num, $ss_len  if $Debug >= $req_dbg;

   # Set flag in case of NT Misfit (Stiff or void or None), also store the points deducted for Declarer. If dummy reduce later.
   # Note that we only deduct for ONE shortness, the shortest one. This is probably not right, but we should not 
   # be Declaring NT with two shortnesses anyway.
   $hnd_ref->{NT_Misfit} = ( ($ss_len == 0) ? 'V' : ( ($ss_len ==  1) ? 'S' : 'N' ) );
   $hnd_ref->{DOP_NT_DED}= ( ($ss_len == 0) ? $HAND_DOP{NT_VOID} : ( ($ss_len ==  1) ?  $HAND_DOP{NT_STIFF}  :  0 ) );
   print STDERR "eval_hand:: $hand_t Shortest suit=$suit_lets[$ss_num] len=[$lengths[$ss_num]] NT_Misfit= $hnd_ref->{NT_Misfit} {$Debug gt $req_dbg}\n"
         if $Debug > $req_dbg;

   # Total the H, L, D, Sy points for all suits. Return as array
   ($H_tot, $L_tot, $D_tot, $Sy_tot ) = set_hand_HLD (@sptrs) ;

   if ( ($hnd_ref->{OPEN} ne "Y" ) && ( $L_tot > 2 ) ) {$L_tot = 2 ; } # Never count more than 2L unless opener
   print STDERR  "eval_hand:: $hand_t  HLD totals done H=$H_tot, L=$L_tot, D=$D_tot*, Sy=$Sy_tot ADJ_TOT=$hnd_ref->{ADJ_TOT} {$Debug gt $req_dbg}\n"
         if $Debug > $req_dbg;

   # Now create the final hand DOP values including adjustments.
   # Depending on who is declarer and what is trump suit, D_pts may never be used.
   # HL totals will include Adjustments for Sq, No_Q, 3Kings etc. and also Sy pts.
   # HLD total will include HL total and also D total.

   $hnd_ref->{DOP_H}   = $H_tot ;
   $hnd_ref->{DOP_L}   = $L_tot ;
   $hnd_ref->{DOP_D}   = int( $D_tot + 0.01 );  #D_tot = 4.01, 2.01 , 1.01 if two dbltons or 0.51 if only 1dblton.
   $hnd_ref->{DOP_Sy}  = $Sy_tot ;
   $hnd_ref->{DOP_HL}  = $H_tot + $L_tot + $Sy_tot + $hnd_ref->{ADJ_TOT};
   $hnd_ref->{DOP_HLD} = $hnd_ref->{DOP_HL}  + $hnd_ref->{DOP_D} ;  # Only of use if evaluating one hand alone.
   &show_DOP_hash ("Eval Hand Result" , $hnd_ref )  if $Debug > $req_dbg + 1;
   print STDERR "eval_hand:: $hnd_ref->{NAME} Finished  DOP_D=[$$hnd_ref{DOP_D}],DOP_HLD=[$$hnd_ref{DOP_HLD}] {$Debug gt $req_dbg}\n" if $Debug > $req_dbg;
   return 1;
} # end sub eval_hand
sub set_hand_HLD {            # Called with an array of suit pointers
   my ( $ht, $lt, $dt, $syt ) = (0,0,0,0);
   local $req_dbg = $req_dbg + 2;
   foreach $p (@_) {
      print STDERR "set_hand_HLD:: suit = $$p{LET} DOP_H,L,D,Sy = $$p{DOP_H},$$p{DOP_L},$$p{DOP_D},$$p{DOP_Sy} {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      $ht  += $p->{DOP_H};
      $lt  += $p->{DOP_L};
      $dt  += $p->{DOP_D};
      $syt += $p->{DOP_Sy};
   }
   return ($ht, $lt, $dt, $syt );
} # end sub set_hand_HLD
sub set_Sq {                  # Called with array of suit_ptrs
   my ($ptr, $Sq) ;
   $Sq = $HAND_DOP{Sq} ; # Assume hand is square so set Sq to -1
   foreach $ptr (@_) {
      if ( ($ptr->{LEN} ne 3 ) && ($ptr->{LEN} ne 4 ) ) { $Sq = 0; last ; }
   }
   return $Sq ;  # -1 for square hand, 0 otherwise
} # end set_Sq
sub count_hand_Hcards {       # Called with array of suit ptrs for a given hand.
   my ($Aces, $Kings, $Queens, $Jacks, $Tens, $Nines) = (0,0,0,0,0,0);
   foreach $p (@_) {
      $Aces   += $p->{Has_A};
      $Kings  += $p->{Has_K};
      $Queens += $p->{Has_Q};
      $Jacks  += $p->{Has_J};
      $Tens   += $p->{Has_T};
      $Nines  += $p->{Has_9};
   }
   return ($Aces, $Kings, $Queens, $Jacks, $Tens, $Nines) ;
} # end count_hand_Hcards

# show_Hand_HLD used only for DEBUGGING not reporting
sub show_Hand_HLD {           # $hand_ref, verbosity=0..3, show_hdr=0/1
   my $hnd_ref = shift;
   my $vvv = shift;    #0 = summary only, 1,2,3 more and more detail.
   my $hdr_flag = shift;
   local $req_dbg = $req_dbg + 1;
   ### check $Debug > $req_dbg
   &show_DOP_hash( "in show_Hand_HLD", $hnd_ref )if $Debug > $req_dbg + 2;

   # DOP_HL includes H, +L, +Sy, +K34  minus deductions for (Sq, noQ, noK, noA)

   my($h_pts, $l_pts, $d_pts, $sy_pts, @adj, $adj_tot, $ded_tot, $tot_pts,
      $nt_flag, $nt_decl, $nt_dummy, $bidder, $bid_let);
   my $Rpt_Header="Compass O/R  DOP Total <---- DEBUGGING Hand --->   H     L  D[*]    Sy   <------- Ded -------> ADJ Tot";
   my $Rpt_Fmt   ="%-8s %-2s   %-6.2f   %-25s  %-6.2f %-3d %-4s  %-6.2f Sq=%2d A=%2d K=%2d Q=%2d   %4d\n";

      $h_pts   = $hnd_ref->{DOP_H};
      $l_pts   = $hnd_ref->{DOP_L};
      $sy_pts  = $hnd_ref->{DOP_Sy};
      @adj=($hnd_ref->{Sq}, $hnd_ref->{No_A}, $hnd_ref->{No_K}, $hnd_ref->{No_Q}, $hnd_ref->{K_3_4} );
      print STDERR "show_Hand_HLD:: Sq=[$adj[0]], -A$adj[1], -K$adj[2], -Q$adj[3], +K$adj[4] {$Debug ge $req_dbg +3}\n" if $Debug > ($req_dbg +1);
      $adj_tot = $hnd_ref->{ADJ_TOT};                       # Kings bonus not part of deductions.
      $nt_flag = $hnd_ref->{NT_Misfit} ;

   if ($hnd_ref->{OPEN} eq "Y") {
      $bidder  = "Opener" ; $bid_let ="O";
      $d_pts   = $hnd_ref->{DOP_D};
      $tot_pts = $hnd_ref->{DOP_HLD};
      $d_star_pts  = $hnd_ref->{DOP_D} . " "; #String with NO asterisk
   }
   else {
      $bidder = "Responder"; $bid_let ="R";
      $d_pts = 0;                # Resp never counts D unless opener opens in NT!!
      $tot_pts = $hnd_ref->{DOP_HL};
      $d_star_pts = $hnd_ref->{DOP_D} . "*"; #String with asterisk
   }
   $nt_decl = $tot_pts - $d_pts + ( ($nt_flag eq "V") ? $HAND_DOP{NT_VOID} : ( ($nt_flag eq "S" ) ? $HAND_DOP{NT_STIFF} : 0 ) ) ;
   $nt_dummy = $nt_decl + (($nt_flag eq "N") ? 0 : $HAND_DOP{NT_DUMMY} );
   # summary only
   if ($hdr_flag) {
      print STDERR  " --- SHOW HAND DEBUG REPORT --- \n";     #took out \f seems to screw up the order on the screen...
      print "$Rpt_Header\n";
   }
printf "$Rpt_Fmt", $hnd_ref->{NAME}, $bid_let,  $tot_pts, $hnd_ref->{STR}, $h_pts, $l_pts, $d_star_pts, $sy_pts,
         $adj[0], $adj[1], ($adj[2] + $adj[4]), $adj[3], $adj_tot;
   if ($vvv >= 2 ) { # Show the 'NT effect'
      print "          DOP in NT for $hnd_ref->{NAME}: Declaring[$nt_decl]  Dummy[$nt_dummy]\n" if ( $nt_flag ne "N" || $d_pts > 0 );
   } # Show the NT effect
} # end show Hand HLD

#------------------------------------------------------------------------------------------------------------

# Deal Evaluation Routines  Part 3 of 4 for Complete DOP evaluation
# The suit related amts stored in arrays in the Hand_X hashes can be accessed by this syntax:
# $hand_ref->{$keyname}->[$suit_num]  or possibly $$hand_ref{$keyname}->[$suit_num]

sub eval_deal {               # Called with refs to %Deal, %Hand_W, and %Hand_E
                              # primary job is to calculate the DOP for FITs, and Dfits. Also update DOP_D in Hand hash
                              # and put them in either the Deal hash or the correct Hand hash
   local $req_dbg = 1 + $req_dbg;
   my $deal_ref = shift;
   my $href_W = shift;
   my $href_E = shift;
   my (@w_sref, @e_sref, $w_len, $e_len, $w_sslen, $e_sslen, $w_dfit, $e_dfit, $suit_str_we, $fit_len, $mirror_cnt,  );
   my ($dop_mirror, $dop_hf_deal, $dop_hl_deal, $dop_misfit_deal, $dop_waste_deal, $dop_nowaste_deal , $dop_fn_tot, $dop_waste_w, $dop_waste_e, )   ;
   my @quick_losers = (0,0,0,0);
 # setting up temp vars etc.
   my $hW_t = substr(ref($href_W),-1);
   my $hE_t = substr(ref($href_E),-1);
   my $dl_t = substr(ref($deal_ref),-2);
   if ( $Debug >= $req_dbg ) {
      print STDERR "eval_deal:: Deal: #:$deal_ref->{DEAL_NUM}, $deal_ref->{DESCR} = [$deal_ref->{STR}] Opener=[$deal_ref->{OPENER}]" .
               "Strains[ W=$deal_ref->{DS_W} : E=$deal_ref->{DS_E} ] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   }

   #temporary vars not saved in deal hash
   $mirror_cnt = 0;
   # vars for %deal scalars. Totals we can calculate as we go.
   $dop_mirror=0;
   $dop_hf_deal=0;
   $dop_hl_deal=0;
   $dop_misfit_deal=0;
   $dop_waste_deal=0;
   $dop_nowaste_deal=0;
   $dop_fn_tot=0;
   # 4 suit pointers in @w_srefs, so $w_srefs[$i] is an ptr to a hash; $w_srefs[$i]->{$key} is the hash item.
   @w_srefs = &get_sptrs($href_W);
   @e_srefs = &get_sptrs($href_E);
 # end setting up temp vars etc.
   printf STDERR "eval_deal:: Check Setup; ht_W|S=%s, W|S_suit_t[3]=%s, ht_E|N=%s, E|N_suit[1]=%s {$Debug ge $req_dbg} \n",
         ref($href_W),ref($w_srefs[3]),ref($href_E),ref($e_srefs[1]) if $Debug >= $req_dbg;
 # begin looping for all suits Clubs (0) to Spades (3)
   foreach $nsuit (0 .. 3) {  # Clubs to Spades
      $w_Has_A   = $w_srefs[$nsuit]->{Has_A};
      $w_dop_h   = $w_srefs[$nsuit]->{DOP_H};
      $w_len     = $w_srefs[$nsuit]->{LEN};
      $w_suit_str= $w_srefs[$nsuit]->{STR};

      $e_Has_A   = $e_srefs[$nsuit]->{Has_A};
      $e_dop_h   = $e_srefs[$nsuit]->{DOP_H};
      $e_len     = $e_srefs[$nsuit]->{LEN};
      $e_suit_str= $e_srefs[$nsuit]->{STR};

      $suit_str_we = "$w_suit_str || $e_suit_str";
      print STDERR "eval_deal:: For Suit=$suit_lets[$nsuit] West: len[$w_len] DOP_H[$w_dop_h] || East: len[$e_len] DOP_H[$e_dop_h]{$Debug ge $req_dbg}\n"
               if $Debug >= $req_dbg;

 #begin calculating the filling in the deal DOP_FN, FIT_LEN and FIT_TYPE arrays.
      # Fit Length, Fit type (5-3, 6-4 etc) and Fit DOP stored in Deal Hash
      $fit_len = $w_len + $e_len;
      $j = $nsuit + $nsuit;
      $deal_ref->{FIT_LEN}->[$nsuit]   = $fit_len;
      $deal_ref->{FIT_TYPE}->[$j]      = $w_len ;
      $deal_ref->{FIT_TYPE}->[$j + 1]  = $e_len ;
      if ($w_len == $e_len)      { $mirror_cnt += 1; }
      $deal_ref->{DOP_FN}->[$nsuit] = 0;
      if    ( ($fit_len == 8 ) ) { $deal_ref->{DOP_FN}->[$nsuit] = $DEAL_DOP{F8};  }
      elsif ( ($fit_len == 9 ) ) { $deal_ref->{DOP_FN}->[$nsuit] = $DEAL_DOP{F9};  }
      elsif ( ($fit_len >= 10) ) { $deal_ref->{DOP_FN}->[$nsuit] = $DEAL_DOP{F10}; }
      else                       { $deal_ref->{DOP_FN}->[$nsuit] = 0 ;             }
      $dop_fn_tot = $dop_fn_tot + $deal_ref->{DOP_FN}->[$nsuit] ; # total as we go.

      print STDERR "eval_deal:: FITS: len:$deal_ref->{FIT_LEN}->[$nsuit]:, Type:$deal_ref->{FIT_TYPE}->[$j],"
            . "$deal_ref->{FIT_TYPE}->[$j + 1]: DOP_FN:$deal_ref->{DOP_FN}->[$nsuit]: FN_TOT=$dop_fn_tot {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
# end of filling in the FIT arrays.

   # Some Hand DOP that depend on partner's hand. Stored in arrays in HAND hash

# begin calculating the Hf per suit and filling in the DOP_HF array  in each hand
      print STDERR "eval_deal::West Suit $suit_str_we {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg; ; #print a header for debug prints that follow
      # Hf per suit.
      $href_W->{DOP_HF}->[$nsuit] = 0 ;
      $href_E->{DOP_HF}->[$nsuit] = 0;
      if ( ($fit_len >= 8 ) ) {
         if    ( ($w_len >= 3) &&   (0 < $w_dop_h && $w_dop_h < 4) ) {
               $href_W->{DOP_HF}->[$nsuit] = $DEAL_DOP{HF};
         }
         elsif ( ($w_len == 2) && ( (0 < $w_dop_h && $w_dop_h < 4) || $w_srefs[$nsuit]->{Has_J} )  ) { # Jx = 0 dop_h but still gets Hf
               $href_W->{DOP_HF}->[$nsuit] = $DEAL_DOP{HF};
         }

         if    ( ($e_len >= 3) &&   (0 < $e_dop_h && $e_dop_h < 4) ) {
               $href_E->{DOP_HF}->[$nsuit] = $DEAL_DOP{HF};
         }
         elsif ( ($e_len == 2) && ( (0 < $e_dop_h && $e_dop_h < 4) || $e_srefs[$nsuit]->{Has_J} )  ) { # Jx = 0 dop_h but still gets Hf
               $href_E->{DOP_HF}->[$nsuit] = $DEAL_DOP{HF};
         }
         $dop_hf_deal += $href_W->{DOP_HF}->[$nsuit];
         $dop_hf_deal += $href_E->{DOP_HF}->[$nsuit];
         print STDERR "eval_deal::Suit=$nsuit, HF_D[$dop_hf_deal] HF_W:$href_W->{DOP_HF}->[$nsuit]: HF_E:$href_E->{DOP_HF}->[$nsuit] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;

         # either or both hands can get the HF upgrade, e.e. QJxxx vs Kxx +1 in each hand for Hf.
         print STDERR "eval_deal::HF_D:[$dop_hf_deal] HF_W:$href_W->{DOP_HF}->[$nsuit]: HF_E:$href_E->{DOP_HF}->[$nsuit] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      }  # end Fit_Len >= 8 (HF per suit)
# end of calculatin DOP_HF pts (HF only if 8+ Fit. SemiFit for 7 card fit).

# Semi_Fit per suit. Use same array as HF  as can't have both.
      if ( ($fit_len == 7 )  ) {    #Handle 5=2 fits
         if ( ($e_len == 2 ) && ((0 < $e_dop_h && $e_dop_h < 4) || ($e_srefs[$nsuit]->{Has_J})) ) { # Add semiFit for Jx, Qx, Kx, QJ
            $href_E->{DOP_HF}->[$nsuit] = $DEAL_DOP{SF_Hx};
         } #end 5:2 W:E
         if ( ($w_len == 2 ) && ((0 < $w_dop_h && $w_dop_h < 4) || ($w_srefs[$nsuit]->{Has_J})) ) { # Add semiFit for Jx, Qx, Kx, QJ
            $href_W->{DOP_HF}->[$nsuit] = $DEAL_DOP{SF_Hx};
         } #end 2:5 W:E

         $dop_hf_deal += $href_W->{DOP_HF}->[$nsuit];
         $dop_hf_deal += $href_E->{DOP_HF}->[$nsuit];
         print STDERR "eval_deal::Semi_D[$dop_hf_deal] Semi_W:$href_W->{DOP_HF}->[$nsuit]: Semi_E:$href_E->{DOP_HF}->[$nsuit] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;

      } # end Semi fit calculation.
# end Semi Fit code block
# Misfit Calculation  -- shortness vs length; applies only if there is a misfit: 5 vs -,x,xx 6 vs -,x, anything vs void.
# Changed 2021/01/23 so that 6 vs xx and 7 vs x are no longer misfits. vs a void no change not even for 8+
      $href_W->{DOP_MISFIT}->[$nsuit] = 0;
      $href_E->{DOP_MISFIT}->[$nsuit] = 0;
      if ( $w_len >= 5 ) {          #west has length
          $MisfitDescr = "W>=5, Check_East e_len = $e_len $suit_str_we ";
          $Dm_e = &do_misfit_dop ($w_len, $e_len, $e_dop_h, $e_srefs[$nsuit]->{Has_J}, $nsuit  ); #check if E has xx or worse
          $href_E->{DOP_MISFIT}->[$nsuit] = $Dm_e;
      }
      if ( $e_len >= 5 ) {          #east has length
         $MisfitDescr = "E>=5, Check_West w_len = $w_len $suit_str_we";
         $Dm_w = &do_misfit_dop ($e_len, $w_len, $w_dop_h, $w_srefs[$nsuit]->{Has_J}, $nsuit   ); #check if W has xx or worse
         $href_W->{DOP_MISFIT}->[$nsuit] = $Dm_w;
      }
      $dop_misfit_deal += $href_E->{DOP_MISFIT}->[$nsuit];
      $dop_misfit_deal += $href_W->{DOP_MISFIT}->[$nsuit];

      print STDERR "eval_deal::Misfit Suit=$nsuit, w_len[$w_len] e_len[$e_len] Misfit_W:$href_W->{DOP_MISFIT}->[$nsuit]: Misfit_E:$href_E->{DOP_MISFIT}->[$nsuit] {$Debug ge $req_dbg}\n"
         if $Debug >= $req_dbg;
# End of MISFIT calculation

# Wastage Calculation. 2021/01/23 modified so that AJx vs stiff is only -1; other honor combos, except Ax(x) are -2
      $href_W->{DOP_WASTE}->[$nsuit] = 0;
      $href_E->{DOP_WASTE}->[$nsuit] = 0;

      # Checking East vs Shortness in W
      if ( $w_len < 2 ) { # West has shortness.
         $WasteDescr = "W=short";
         $e_waste_dop = &do_waste_dop ($w_len, $e_len, $e_dop_h, $e_Has_A, $nsuit);
         if ($Debug >= $req_dbg) {
            my $action = ($e_waste_dop < 0 ) ? "Deduct " : "Add "  ;
            print STDERR "East: $action $e_waste_dop WH_xxx in suit $nsuit returned by sub do_waste_dop {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg; ;
         }
         $href_E->{DOP_WASTE}->[$nsuit] = $e_waste_dop;
         # keep track of running total for deal Waste / NoWaste separately
         if ( $e_waste_dop < 0 ) {
            $dop_waste_deal += $e_waste_dop ;
         }
         else {
            $dop_nowaste_deal += $e_waste_dop ;
         }

      } #end West has shortness

      # Checking West vs Shortness in E
      if ($e_len < 2 ) { # East has shortness. The West hand gets the deduction.
         $WasteDescr = "E=short";
         $w_waste_dop = &do_waste_dop ($e_len, $w_len, $w_dop_h, $w_Has_A, $nsuit);
         if ($Debug >= $req_dbg) {
            my $action = ($w_waste_dop < 0 ) ? "Deduct " : "Add "  ;
            print STDERR "West: $action $w_waste_dop Dm_W in suit $nsuit returned by sub do_waste_dop {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg; ;
         }
         $href_W->{DOP_WASTE}->[$nsuit] = $w_waste_dop;
         # keep track of running total for deal Waste / NoWaste separately
         if ( $w_waste_dop < 0 ) {
            $dop_waste_deal += $w_waste_dop ;   # total for debugging print....
         }
         else {
            $dop_nowaste_deal += $w_waste_dop ;
         }
      } # end East has shortness

      # Note as of 2021/04/02 we need to keep track of the +ve and -ve Waste amounts separately because
      # NT evaluation gets penalized for -ve Waste, but does not get any  +ve adjustment for xxxx vs stiff

      print STDERR "eval_deal:: suit[$nsuit], Running Tot WASTE_Deal=$dop_waste_deal,Waste_W:$href_W->{DOP_WASTE}->[$nsuit]: Waste_E:$href_E->{DOP_WASTE}->[$nsuit] ",
                     " {$Debug ge $req_dbg}\n-o-o-o-o-\n" if $Debug >= $req_dbg;
# End of Wastage Calculation
   } # end foreach $nsuit (suit number)
 # End looping over all suits code block

  # Now Calculate the DOP_DFIT points. Fill in the DOP_DFIT array in each hand.
  # This array does not give the DFit pts that each suit contributes as the DOP_D value does,
  # Rather it gives the DFit pts for the hand as a whole for each possible trump suit.
  # Can be rather confusing.
   print STDERR "eval_deal:: BEGIN DFIT CALCS for each suit: {$Debug gt $req_dbg} \n" if $Debug > $req_dbg;
  foreach $nsuit (0 .. 3) { # Considering each suit as a possible trump suit,
     $fit_type_w  = $deal_ref->{FIT_TYPE}->[$nsuit + $nsuit] ;
     $fit_type_e  = $deal_ref->{FIT_TYPE}->[$nsuit + $nsuit + 1] ;
     $fit_len     = $deal_ref->{FIT_LEN}->[$nsuit];
     print STDERR "eval_deal_Dfit:: if Suit[$nsuit] is trumps and fitlen is $fit_len [$fit_type_w : $fit_type_e] " .
               "{$Debug gt $req_dbg} \n" if $Debug > $req_dbg ;
     # for 7 card fits, we allow 5:2 only. Not 4-3 not 6-1 or 7-0
     if (  ($fit_len == 7 ) and ( ($fit_type_w == 5) or ($fit_type_e == 5) ) ) {
        &Calc_DFit7($href_W, $href_E, $nsuit) ; # will update DOP_DFIT in both hands.
     }
     else {   # This next routine handles all the case except the 5:2 fits. even if there is no fit.
        &Calc_DFit_x2($href_W, $href_E, $nsuit) ; # will update DOP_DFIT in both hands.
     }
     # now accumulate a total in the deal hash as we go.
     $deal_ref->{DOP_DFIT}->[$nsuit] = $href_W->{DOP_DFIT}->[$nsuit] + $href_E->{DOP_DFIT}->[$nsuit] ;
     print STDERR "eval_deal:: DFIT if trumpsuit=[$nsuit]: $$deal_ref{DOP_DFIT}->[$nsuit] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
   } # end foreach nsuit Dfit Calculations
# end calculating the DFit points (or lack thereof)

# save all the totals in the Deal Hash.
   # put the accumulated totals for all suits and both hands in the Deal hash DFit done already
   # As of 2021/04/02 we have several more 'totals' to be able to separate out NT and Suit contracts
   $deal_ref->{STR}   = $$href_W{STR} . " :: " . $$href_E{STR};    # Hands strings are more cleaned up.

   $deal_ref->{DOP_H} = $href_W->{DOP_H}   + $href_E->{DOP_H} ;
   $deal_ref->{DOP_L} = $href_W->{DOP_L}   + $href_E->{DOP_L} ;
   $deal_ref->{DOP_Sy}= $href_W->{DOP_Sy}  + $href_E->{DOP_Sy} ;
   $deal_ref->{ADJ_TOT}= $href_W->{ADJ_TOT}  + $href_E->{ADJ_TOT} ;
   $deal_ref->{DOP_HL}= $href_W->{DOP_HL}  + $href_E->{DOP_HL} ;  #H + L + Sy + ADJ_TOT

   $deal_ref->{DOP_NT_DED}->[0] = $href_W->{DOP_NT_DED} ;      # Deductions (-3, -2, 0) if Declaring NT. If dummy 1 pt less.
   $deal_ref->{DOP_NT_DED}->[1] = $href_E->{DOP_NT_DED} ;

#  Note these next keys are SCALARS here in the Deal hash, ARRAYS in the hand hashes.
   $deal_ref->{DOP_HF}     = $dop_hf_deal;            #both hands all suits. Running total was calculated above.
   $deal_ref->{DOP_MISFIT} = $dop_misfit_deal;        #both hands all suits. Running total was calculated above

   $deal_ref->{DOP_WASTE}  = $dop_waste_deal;         #both hands all suits. Running total was calculated above
   $deal_ref->{DOP_noWASTE}= $dop_nowaste_deal;

#  Deal only keys. These do not exist in the hand hashes.
   $deal_ref->{DOP_FN_TOT} = $dop_fn_tot;          # running total all suits,

   $deal_ref->{DOP_MIRROR} = ($mirror_cnt == 4) ? $DEAL_DOP{M_HAND} :  ($mirror_cnt > 1) ? $DEAL_DOP{M_SUITS} : 0 ;
   print STDERR "eval_deal:: Mirror: $deal_ref->{DOP_MIRROR} {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
   print STDERR "eval_deal:: Suit, Hand, and Deal hashes updated with DOP for Fits, Misfits, Wastage and Mirror {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;

# Calc some values that are useful for detail printouts as to how we got here.
# DOP_HLF_NT includes everything except +ve Waste and D/Dfit

   $$deal_ref{DOP_HL} = $$deal_ref{DOP_H} + $$deal_ref{ADJ_TOT} + $$deal_ref{DOP_L} + $$deal_ref{DOP_Sy} ;
   $$deal_ref{DOP_HLF} = $$deal_ref{DOP_HL} + $$deal_ref{DOP_HF} + $$deal_ref{DOP_FN_TOT} ;
   $$deal_ref{DOP_HLF_NT} = $$deal_ref{DOP_HLF} + $$deal_ref{DOP_MISFIT} + $$deal_ref{DOP_MIRROR} + $$deal_ref{DOP_WASTE};
   # The relevant NT_DED_TOT is calculated just before calling DOP_2_Trix and is added to DOP_HLF_NT at that time.

  # Store the total HLDF DOP points when playing in a suit: = HLF_NT + noWASTE + DFit[i]
   foreach $nsuit ( 0 .. 3 ) {
      $deal_ref->{DOP_HLDF_SUIT}->[$nsuit] = $$deal_ref{DOP_HLF_NT} + $$deal_ref{DOP_noWASTE} + $deal_ref->{DOP_DFIT}->[$nsuit] ;
   }
   # Find the total HLDF DOP points when playing in the side's longest fit. DFIT[l] includes Dfit and D pts both.
   $long_fit = &set_long_strain ($deal_ref);  # longest or in case of tie choose strain with most DOP_DFIT pts.
   $$deal_ref{DOP_HLDFl} = $deal_ref->{DOP_HLDF_SUIT}->[$long_fit];
   $$deal_ref{LONG_STRAIN} = $suit_lets[$long_fit];
   ($best_fit, $best_DOP) = &array_max_idx($deal_ref->{DOP_HLDF_SUIT} ) ;
   $$deal_ref{DOP_HLDFb} = $best_DOP;
   $$deal_ref{BEST_STRAIN} = $suit_lets[$best_fit];


   # 2021/02/27 --  Calculate the first and second round losers when playing in a suit, and in NT.
   # We use the first round losers to adjust the 'expected trix' returned by DOP_2_Trix if trix > 11;
   # We don't want to report that there is a 70% chance of 12 tricks, when we are short 2 cashing Aces.

   print STDERR "eval_deal:: Counting Quick Losers {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg ;
   ($$deal_ref{FRL},    $$deal_ref{SRL},    @quick_losers  ) = &Count_Quick_Losers($href_W, $href_E, "S") ;
   $ql=&sum_suit(\@quick_losers);
   $$deal_ref{QUICK_LOSERS_SUIT} = &sum_suit(\@quick_losers);
   ($$deal_ref{NT_FRL}, $$deal_ref{NT_SRL}, @quick_losers  ) = &Count_Quick_Losers($href_W, $href_E, "N") ;
   $ql=&sum_suit(\@quick_losers);
   $$deal_ref{QUICK_LOSERS_NT} = &sum_suit(\@quick_losers);
   ## Deal hash filled.

   if ($Debug >= $req_dbg) {
      print STDERR "\neval_deal:: Final Values   Deal#[$$deal_ref{DEAL_NUM}] $$deal_ref{STR}\n   ";
      foreach $key (qw(DOP_H ADJ_TOT DOP_L DOP_Sy DOP_HF DOP_FN_TOT )) {
         print STDERR "[$key=>$$deal_ref{$key}] ";
      }
      print STDERR "\n   ";
      foreach $key (qw(DOP_MISFIT DOP_MIRROR DOP_WASTE DOP_noWASTE DOP_HL DOP_HLF)) {
               print STDERR "[$key=>$$deal_ref{$key}] ";
      }
      print STDERR "\n   ";
      foreach $key (qw( DOP_HLF_NT  LONG_STRAIN DOP_HLDFl BEST_STRAIN DOP_HLDFb QUICK_LOSERS_NT QUICK_LOSERS_SUIT )) {
         print STDERR "[$key=>$$deal_ref{$key}] ";
      }
      print STDERR "\n-o-o-o-o-o- Return from eval_deal -o-o-o-o-o- \n";
   } # end if $Debug print block
      &show_DOP_hash ("Deal Hash All sub arrays filled - { $Debug >= $req_dbg}", $deal_ref )  if $Debug >= $req_dbg ;

}  #end of eval_deal
#end of eval_deal
# as of 2021/01/23 with AHx facing a stiff, we do deduct the lesser of the non_A_dop_H and $DEAL_DOP{WH_x}
# As of 2021/04/02 confirmed that (1) xxx(x) facing a stiff Honor is still counted as +ve Waste pts.
#                                 (2) +ve Waste pts are NOT added to NT totals.
sub do_waste_dop {               # shortness could be stiff or void. waste_dop could be +ve or -ve
   my ($short_len, $suit_length, $suit_dop_h, $Has_A, $n_suit ) = @_;
   my $waste_dop  = 0;
   my $non_Ace_dop_h = 0;

   if ( ($suit_dop_h == 0) && ($suit_length > 2) ) { # we have an xxx(x) suit facing a short suit (void or stiff)
      $waste_dop = ($short_len == 0) ? $DEAL_DOP{Wxxx_void} : $DEAL_DOP{Wxxx_x} ;
      return $waste_dop;         # +ve value
   } #end if no wastage vs shortness

   # from here on, the suit has some H values.
   if ( $Has_A != 1 )   {              # suit is missing the Ace; straight deductions apply.
      $waste_dop = ($short_len == 0) ? $DEAL_DOP{WH_void} : $DEAL_DOP{WH_x} ; # choose Honors vs Void or vs Stiff amt.
      if ( ($waste_dop + $suit_dop_h) < 0 ) { $waste_dop = 0 - $suit_dop_h ; } # cant deduct more than is there.
         return $waste_dop ; # -ve value
   } # end if missing Ace

   # Suit has Ace. Either A vs void => deduction; AH(x) vs stiff => deduction. Ax(x) vs stiff +ve addition
   if ( $short_len == 0 ) { return $DEAL_DOP{WH_void} ; }
   if ( ($suit_dop_h == $SUIT_DOP{H_A}) && ($suit_length > 1) ) { # we have Ax(x). No other honors.
      return $DEAL_DOP{WAx_x} ; # +ve value
   }
   # must be a stiff facing an Ace with honor(s). Deduct the lesser of $DEAL_DOP{WH_x} or the value of the non-Ace honors
   $non_Ace_dop_h = $suit_dop_h - $SUIT_DOP{H_A} ;
   $waste_dop = ( ($DEAL_DOP{WH_x} + $non_Ace_dop_h ) >= 0 ) ? $DEAL_DOP{WH_x} : (0 - $non_Ace_dop_h ) ;
   # print STDERR "DO_WASTE:: SUIT_DOP[$suit_dop_h, $non_Ace_dop_h] , $DEAL_DOP{WH_x}, $waste_dop {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg ;
   return $waste_dop;   # -ve value
} # end sub do_waste_dop

sub do_misfit_dop { # calculate the Potential misfit adjustment. Whether applied or not will depend on length of fit
   my ($long_len, $short_len, $suit_dop_h, $Has_J, $n_suit)= @_;
   print "do_misfit:: [$MisfitDescr] long=$long_len, short=$short_len, H=$suit_dop_h, J=$Has_J, Suit=$n_suit {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   if       ($short_len == 0)                      { return $DEAL_DOP{Dm_void} ; }
   elsif    ($short_len == 1 && $long_len <= 6 )   { return $DEAL_DOP{Dm_x} ;    }
   elsif   (($short_len == 2) && $long_len == 5 && ($suit_dop_h == 0) && !($Has_J) ) { # Jx is worth 0H but it is not worthless misfit.
      return $DEAL_DOP{Dm_xx} ;              # If we treat Jx like Qx and Kx it gets a +1 in the Semi fit calcs  done elsewhere
   }
   return 0 ; # There is no misfit. 7-1 and 6-2 are no longer considered misfits as of 2021/01/23
} # end do_misfit DOP


# --------- DFit Calculations: DFit = (Number of Trump)  - (Length of Shortest Suit)  + any still relevant DOP_D pts ------
# ARGS: whnd_ref, ehnd_ref, trump_suit_num RESULT: updates slot ts_num in both hands DOP_DFIT arrays
# Vers 3: Changed so that if there is a 5-5 fit BOTH hands keep their D pts. (New DOP book)
# Vers 4: Above Implies that a hand with 5+ trumps keeps D-pts even if NOT opener.
#         e.g. E is Opener with 4=1=4=4 shape; W is Resp with 2=3=7=1 shape. E gets 3 Df in H, and W 2D pts in C when D are trump
sub Calc_DFit_x2 {

   print STDERR "Calc_Dfit_x2 Version 4.0 has been called ++++++++ for suit $_[2] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg  ;
   my $whnd_ref = shift;
   my $ehnd_ref = shift;
   my $ts_num = shift;              # trump suit number; the Dfit pts will depend on which suit is trump.
   local $req_dbg = $req_dbg + 1;

   my @Dtemp_w = (0,0,0,0);
   my @Dtemp_e = (0,0,0,0);
   my $opener = "";
   @suit_symb = ('C','D','H','S');
   printf STDERR "Calc_Dfit_x2:: Check Caller. Trumps=$suit_symb[$ts_num], West_ref=%s, %s, East_ref=%s, %s {$Debug ge $req_dbg + 2} \n",
            ref($whnd_ref) , $whnd_ref->{STR}, ref($ehnd_ref), $ehnd_ref->{STR} if $Debug >= $req_dbg + 2;

   my @sptrs_w = &get_sptrs($whnd_ref) ; # returns ptrs to C, D, H, S suits in that order.
   my @sptrs_e = &get_sptrs($ehnd_ref) ;

   my $ts_len_w = $sptrs_w[$ts_num]->{LEN};
   my $ss_num_w = $whnd_ref->{LEN_ORDER}->[0];
   my $ss_len_w = $sptrs_w[$ss_num_w]->{LEN};

   my $ts_len_e = $sptrs_e[$ts_num]->{LEN};
   my $ss_num_e = $ehnd_ref->{LEN_ORDER}->[0];
   my $ss_len_e = $sptrs_e[$ss_num_e]->{LEN};

  print STDERR "Calc_Dfit_x2:: Setup Done. West[ts_len,ss_num,ss_len] = [$ts_len_w, $ss_num_w, $ss_len_w], " .
         "East=[$ts_len_e, $ss_num_e, $ss_len_e] {$Debug ge $req_dbg + 2} \n" if $Debug >= $req_dbg + 1;

# Special Case is if both hands have >= 5 cards in the trump suit.
if ($ts_len_w >= 5 && $ts_len_e >= 5) {
# both hands keep their D pts and there are no Dfit pts allocated.
# Except cannot count shortness in same suit in both hands. So keep highest DOP_D i.e. stiff vs void keep void.
# If both DOP_D the same  (maybe zero) then keep the West one.

      foreach $snum (0 .. 3) { # we don't need to check for snum being the trump suit since 5 card suits have no Dpts anyway.
         $e_DOP_D = $sptrs_e[$snum]->{DOP_D};
         $w_DOP_D = $sptrs_w[$snum]->{DOP_D};
         if    ( $e_DOP_D <= $w_DOP_D ) { $whnd_ref->{DOP_DFIT}->[$ts_num] += $w_DOP_D; } # might be zero
         else                           { $ehnd_ref->{DOP_DFIT}->[$ts_num] += $e_DOP_D; }
      }
      print STDERR "Strain= $ts_num, 5=5 fit; Keeping BOTH D pts// W_Dpts=$whnd_ref->{DOP_DFIT}->[$ts_num], E_Dpts=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
      return 1 ;
}  # end case of 5+ trumps in each hand.

# Begin usual fits, 5-3, 4-4, 6-4 etc.

{ # Collect and save relevant D pts for later use if necessary
  # Vers 4 -- a hand with >= 5 in the trump suit, gets no Df, but keeps D even if not Opener.
   if ($ehnd_ref->{OPEN} eq "Y" ) { #East is Opener
      $opener = "E";
      #Copy over the DOP_D pts from the hand as a starting pt.
      foreach $snum (0 .. 3) {
         $Dtemp_e[$snum] = $sptrs_e[$snum]->{DOP_D};
         if ( $ts_len_w < 5 ) {
            $Dtemp_w[$snum] = 0 ;
         } # No Dpts to start if not Opener and short trump
         else                 {
            $Dtemp_w[$snum] = $sptrs_w[$snum]->{DOP_D};  #keep D pts if trump >=5 regardless
         } # end else
      } # end for
   } # end East is Opener.
   else { # Maybe Nobody is Opener.  But both cannot count D, so choose W as Opener.
      $opener = "W";
      #Copy over the DOP_D pts from the W-hand as a starting pt.
      foreach $snum (0 .. 3) {
         $Dtemp_w[$snum] = $sptrs_w[$snum]->{DOP_D};
         if ( $ts_len_e < 5 ) { $Dtemp_e[$snum] = 0;} # No Dpts to start if not Opener and short trump
         else               {
                  $Dtemp_e[$snum] = $sptrs_e[$snum]->{DOP_D};  #keep D pts if trump >=5 regardless
                  print STDERR "CalcDFitx2 East Non Opener keeps Dtemp[$snum]=$Dtemp_e[$snum] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
         } #end else
      } #end for
   }  # end West (or nobody) is Opener.
   &show_arr ("CalcDfitx2 East DTemp", \@Dtemp_e) if $Debug >= $req_dbg + 2 ;
   &show_arr ("CalcDfitx2 West DTemp", \@Dtemp_w) if $Debug >= $req_dbg + 2 ;
    $dtemp_e = int(&sum_suit(\@Dtemp_e));
    $dtemp_w = int(&sum_suit(\@Dtemp_w));
    print STDERR "CalcDFitx2 East Tot Temp D[$dtemp_e] West Tot Temp D[$dtemp_w] {$Debug ge $req_dbg + 2} \n " if $Debug >= $req_dbg + 2 ;
   # cant have any D pts in the trump suit, even if it is the shortest one.
   $Dtemp_e[$ts_num] = 0 ;
   $Dtemp_w[$ts_num] = 0;
} # end collecting D pts.

   # Does a hand have any Df Pts coming?
   # First check if the fit is long enough. If not we can update the DFit with the DOP and return.
   if ( ($ts_len_w + $ts_len_e) < 8 ) {  # No 8 fit, (and no 5:2 fit) just update hand with collected D pts and return
      $whnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_w) );
      $ehnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_e) );
      print STDERR  "Strain= $ts_num, No-8Fit Returning D pts// W_Dpts=$whnd_ref->{DOP_DFIT}->[$ts_num], E_Dpts=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
      return 1 ;
   }

{  # Handle the 'normal' cases (Special Case is : 4-4 fit with both hands have the same shortest suit.)
   # Does a hand have any Df Pts coming?
   $Df_w_ok = 0;
   if ( ($ts_len_w < 5) and ($ss_len_w < 3) and ($ss_len_w < $ts_len_w ) ) {
      print STDERR  "Calc_Dfit_x2:: West Gets Dfit $ts_len_w > $ss_len_w, setting $Dtemp_w[$ss_num_w] to ZERO {$Debug ge $req_dbg} \n"
                  if $Debug >= $req_dbg;
      $Dtemp_w[$ss_num_w] = 0 ; # If ss_num_w qualifies for DF then it cannot get any D.
      $Df_w_ok = 1;
   }
   if ($Df_w_ok == 1 ) { print STDERR  "Calc_Dfit_x2:: West $Df_w_ok == 1 and West Gets DF points {$Debug ge $req_dbg + 2} \n" if $Debug >= $req_dbg + 2; }

   $Df_e_ok = 0;
   if ( ($ts_len_e < 5) and ($ss_len_e < 3) and ($ss_len_e < $ts_len_e) ) {
      print STDERR "Calc_Dfit_x2:: East Gets Dfit $ts_len_e > $ss_len_e, setting $Dtemp_e[$ss_num_e] to ZERO {$Debug ge $req_dbg} \n"
                  if $Debug >= $req_dbg;
      $Dtemp_e[$ss_num_e] =  0 ; # If ss_num_e qualifies for DF then it cannot get any D.
      $Df_e_ok = 1;
   }
   if ($Df_e_ok == 1 ) { print STDERR  "East $Df_e_ok == 1 and East Gets DF points {$Debug ge $req_dbg + 2} \n" if $Debug >= $req_dbg + 2; }
   # if the short suit is the same for both hands
   if ( ($Df_w_ok == 1 ) and ($Df_e_ok == 1 ) and ($ss_num_w == $ss_num_e) ) { # duplication of short suit, and both hands want DF.
      print STDERR "Calc_Dfit_x2:: West & East both get DF, and SS_nums are the same $ss_num_w. GOTO DUPLICATE_SS {$Debug ge $req_dbg} \n"
                  if $Debug >= $req_dbg;
      goto DUPLICATE_SS;
   }

   # The Dtemp have been setup properly at this point. All we need to do is put the Df in and we are done.
   # Often even if the hand does not get any DF it will keep it's D pts.

   print STDERR "Calc_Dfit_x2:: No 4-4 FIt with Duplicate Short Suit.  Continuing .... {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg  ;

   # Df pts done for both hands unless we have the same short suit in both hands.

   if ( ($Df_w_ok == 1) and ($Df_e_ok == 1) ) { # Both get DF, but not in same suit.
      $Dtemp_w[$ss_num_w] = $ts_len_w - $ss_len_w;  # W gets his Dfit pts
      $Dtemp_e[$ss_num_e] = $ts_len_e - $ss_len_e;  # E gets his Dfit pts
      $Dtemp_w[$ss_num_e] = 0;                      # W loses any D pts in E short suit. (This may be a poor trade)
      $Dtemp_e[$ss_num_w] = 0;                      # E loses any D pts in W short suit.
      $whnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_w) );
      $ehnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_e) );
      print STDERR "Strain= $ts_num, W Dfit=$whnd_ref->{DOP_DFIT}->[$ts_num], E Dfit=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
   }
   elsif ( $Df_w_ok == 1  ) { # only West gets DF
      $Dtemp_w[$ss_num_w] = $ts_len_w - $ss_len_w;  # W gets his Dfit pts
      $Dtemp_e[$ss_num_w] = 0;                      # E loses any D pts in W short suit. This may be a poor trade.
      $whnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_w) );
      $ehnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_e) );
      print STDERR  "!E_ok,Strain= $ts_num, W Dfit=$whnd_ref->{DOP_DFIT}->[$ts_num], No-E Dfit=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
   }
   elsif ( $Df_e_ok == 1  ) { # only East gets DF
      $Dtemp_e[$ss_num_e] = $ts_len_e - $ss_len_e;  # E gets his Dfit pts
      $Dtemp_w[$ss_num_e] = 0;                      # W loses any D pts in E short suit. This may be a poor trade.
      $whnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_w) );
      $ehnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_e) );
      print STDERR  "!W_ok, Strain= $ts_num, W_Dfit=$whnd_ref->{DOP_DFIT}->[$ts_num], E Dfit=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
   }
   else {  # No one gets any Dfit pts, but they still might get D pts. e.g. 5=4=3=1 vs 5=3=3=2; West(opener) keeps 2D pts
      $whnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_w) );
      $ehnd_ref->{DOP_DFIT}->[$ts_num] = int( &sum_suit(\@Dtemp_e) );
      print STDERR  "Strain= $ts_num, No-W Dfit=$whnd_ref->{DOP_DFIT}->[$ts_num], No-E Dfit=$ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg  ;
   }  # end if - elsif chain for W_ok & E_ok

   # Df pts done for both hands unless we have the same short suit in both hands.
   return 1;
} # end of 'normal cases code block.
# end normal case block
DUPLICATE_SS :  # We should only get here if there is a 4-4 fit, and if both hands have <3 cards their shortest suit.
{  # Case 1: Both have a second shortness. Give the DF for the current SS to the hand that has the longest 2nd suit.
   #         Calc new Df for other hand with shortest 2nd suit.
   # Case 2: W has a second short suit, and East does not. Give the DF for current SS to East. Give W his DF for 2nd suit.
   # Case 3: Case 3 with e/w switched.
   # Case 4: Neither has 2nd short suit. W ss_len is less than E ss_len. Give the DF to West and East gets 0 DF
   # Case 5: Neither has 2nd short suit. E ss_len is less than W ss_len. Give the DF to East and West gets 0 DF
   #         ex: W has a stiff-C and a dblton-D. E has Stiff-C and Stiff-D. DF W for the C-stiff, DF E for the D-stiff.

   $ss_num_w2 = $whnd_ref->{LEN_ORDER}->[1];  # get the number of the 2nd shortest suit for each hand.
   $ss_num_e2 = $ehnd_ref->{LEN_ORDER}->[1];
   $ss_len_w2 = $sptrs_w[$ss_num_w2]->{LEN};  # Then get the length therof.
   $ss_len_e2 = $sptrs_e[$ss_num_e2]->{LEN};
   print STDERR  "Calc_DFit_x2:: Duplicate_SS $ss_num_w == $ss_num_e ; 2nd shorts: W=$suit_symb[$ss_num_w2],$ss_len_w2" .
         " :: E=$suit_symb[$ss_num_e2],$ss_len_e2 {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg  ;

   if ( ($ss_len_w2 <= 2) and ($ss_len_e2 <=2) ) { # Case 1
      if ( $ss_len_w2 <= $ss_len_e2 ) { # West's 2nd suit is shorter. He gets DF in that one, East in the common shortsuit.
         $Dtemp_w[$ss_num_w2] = $ts_len_w - $ss_len_w2;
         $Dtemp_w[$ss_num_e]  = 0;        # West cant get D in E's DF suit.
         $Dtemp_e[$ss_num_w2] = 0;        # East cant get D in W's DF suit.
         $Dtemp_e[$ss_num_e]  = $ts_len_e - $ss_len_e ;
         print STDERR  "Calc_DFit_x2:: Case 1a: West's 2nd suit is shorter. He gets DF in that one, East in the common shortsuit. {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg;
      }
      else  { # East 2nd suit is shorter. Reverse the above.
         $Dtemp_e[$ss_num_e2] = $ts_len_e - $ss_len_e2;
         $Dtemp_e[$ss_num_w]  = 0;        # East cant get D in W's DF suit.
         $Dtemp_w[$ss_num_e2] = 0;
         $Dtemp_w[$ss_num_w]  = $ts_len_w - $ss_len_w ;
         print STDERR  "Calc_DFit_x2:: Case 1b: East's 2nd suit is shorter. He gets DF in that one, West in the common shortsuit. {$Debug ge $req_dbg}\n"
                  if $Debug >= $req_dbg;
      }
   }  # End Case 1
   elsif ( $ss_len_w2 <= 2  )  { # Case 2. West has 2nd shortness. East does not. Give W DF for ss#2 and E DF for ss#1
         $Dtemp_w[$ss_num_w2] = $ts_len_w - $ss_len_w2;
         $Dtemp_w[$ss_num_e]  = 0;        # west cant get D in E's DF suit.
         $Dtemp_e[$ss_num_w2] = 0;
         $Dtemp_e[$ss_num_e]  = $ts_len_e - $ss_len_e ;
         print STDERR  "Calc_DFit_x2:: Case 2. West has 2nd shortness in $suit_symb[$ss_num_w2]. East does not. " .
         "Give W DF for ss#2 and E DF for ss#1 {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   } # end Case # 2
   elsif ( $ss_len_e2 <= 2  )  { # Case 3. E has 2nd shortness. W does not. Give E DF for ss#2 and W DF for ss#1
         $Dtemp_e[$ss_num_e2] = $ts_len_e - $ss_len_e2;
         $Dtemp_e[$ss_num_w]  = 0;        # East cant get D in W's DF suit.
         $Dtemp_w[$ss_num_e2] = 0;
         $Dtemp_w[$ss_num_w]  = $ts_len_w - $ss_len_w ;
         print STDERR  "Calc_DFit_x2:: Case 3. E has 2nd shortness in $suit_symb[$ss_num_e2]. W does not. " .
         "Give E DF for ss#2 and W DF for ss#1 {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   } # End Case # 3
   # No 2nd shortness. Give the Df to whoever is shortest in the only short suit.
   elsif ( $ss_len_w < $ss_len_e ) { # Case #4 W has the shortest short suit.
         $Dtemp_w[$ss_num_w]  = $ts_len_w - $ss_len_w ;
         $Dtemp_e[$ss_num_w]  = 0;
         print STDERR  "Calc_DFit_x2:: Case 4. No 2nd shortness. West's shortness is shorter than East's. {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   }
   else {                           # Case #5 E gets the DF
         $Dtemp_e[$ss_num_e]  = $ts_len_e - $ss_len_e ;
         $Dtemp_w[$ss_num_e]  = 0;
         print STDERR  "Calc_DFit_x2:: Case #5 E gets the DF {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   } # end Case 5, and if-elsif chain for Duplicate SS with 4-4 fit.

   # Both Dtemps should now contain any leftover D, and also any DF for one or two short suits.
   # put the total of the D and DF for each hand into the hand hash in the ts_num slot.
   $Dfit_tot = int(&sum_suit(\@Dtemp_w));
   $whnd_ref->{DOP_DFIT}->[$ts_num] = int($Dfit_tot);  # Int to truncate any leftover 0.5 from dbltons no longer counted.
   print STDERR  "West Hand DFIT slot $ts_num updated to $Dfit_tot aka $whnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;

   $Dfit_tot = int(&sum_suit(\@Dtemp_e));
   $ehnd_ref->{DOP_DFIT}->[$ts_num] = int($Dfit_tot);  # Int to truncate any leftover 0.5 from dbltons no longer counted.
   print STDERR  "East Hand DFIT slot $ts_num updated to $Dfit_tot aka $ehnd_ref->{DOP_DFIT}->[$ts_num] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;

   &show_arr ("Calc_DFitx2:: West DOP_DFIT Array Result", $whnd_ref->{DOP_DFIT} ) if $Debug >= $req_dbg + 2;
   &show_arr ("Calc_DFitx2:: East DOP_DFIT Array Result", $ehnd_ref->{DOP_DFIT} ) if $Debug >= $req_dbg + 2;

}  # end of DUPLICATE_SS
# end DUPLICATE_SS block
   return 1;
} # end sub Calc_DFit_x2sub
# end sub Calc_DFit_x2sub

# --------- DFit Calculations: DFit = (Number of Trump)  - (Length of Shortest Suit)  + any still relevant DOP_D pts ------
#  5:2 fits only. ARGS: whnd_ref, ehnd_ref, trump_suit_num RESULT: updates slot ts_num in both hands DOP_DFIT arrays
sub Calc_DFit7 {
   my $whnd_ref = shift;
   my $ehnd_ref = shift;
   my $ts_num = shift;
   local $req_dbg = $req_dbg + 1;
   my @Dtemp_w = (0,0,0,0);
   my @Dtemp_e = (0,0,0,0);
   print STDERR "eval_deal::Calc_Dfit7 has been called {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg ;
   my @sptrs_w = &get_sptrs($whnd_ref) ; # returns ptrs to C, D, H, S suits in that order.
   my @sptrs_e = &get_sptrs($ehnd_ref) ;

   # Set up the starting points, ts_len, ss_len, Opener, Dpts etc.
   my $ts_len_w = $sptrs_w[$ts_num]->{LEN};
   my $ss_num_w = $whnd_ref->{LEN_ORDER}->[0];
   my $ss_len_w = $sptrs_w[$ss_num_w]->{LEN};

   my $ts_len_e = $sptrs_e[$ts_num]->{LEN};
   my $ss_num_e = $ehnd_ref->{LEN_ORDER}->[0];
   my $ss_len_e = $sptrs_e[$ss_num_e]->{LEN};

   if ($whnd_ref->{OPEN} eq "Y" ) {
      #Copy over the DOP_D pts from the hand as a starting pt.
      foreach $snum (0 .. 3) {
         $Dtemp_w[$snum] = $sptrs_w[$snum]->{DOP_D};
         $Dtemp_e[$snum] = 0; # No Dpts to start if not Opener.
      }
   }  #end W is opener
   else {  # East is Opener.
      #Copy over the DOP_D pts from the each suit as a starting point
      foreach $snum (0 .. 3) {
         $Dtemp_e[$snum] = $sptrs_e[$snum]->{DOP_D};
         $Dtemp_w[$snum] = 0; # No Dpts to start if not Opener.
      }
   } # end East is Opener.

#
# Now the 5 trump hand does not get any DF pts, and the 2 trump hand only gets DF pts if it has a stiff or void
   if    ($ts_len_w == 2 and $ss_len_w < 2 ) {
      print STDERR  "Calc_DFit7:: Assigning DF pts to West when $ts_num is trump $ts_len_w > $ss_len_w { $Debug ge $req_dbg}\n" if $Debug > $req_dbg;

      $Dtemp_w[$ss_num_w] = $ts_len_w - $ss_len_w;
      $Dtemp_e[$ss_num_w] = 0;            # long trump gets no D pts in Resp short suit.
   }
   elsif ($ts_len_e == 2 and $ss_len_e < 2 ) {
      print STDERR  "Calc_DFit7:: Assigning DF pts to East when $ts_num is trump $ts_len_e > $ss_len_e { $Debug ge $req_dbg}\n" if $Debug > $req_dbg;
      $Dtemp_e[$ss_num_e] = $ts_len_e - $ss_len_e;
      $Dtemp_w[$ss_num_e] = 0;
   }

   # Now make sure there are no D, or DF pts in the trump suit.
   $Dtemp_w[$ts_num] = 0;
   $Dtemp_e[$ts_num] = 0;

   $Dfit_tot = int(&sum_suit(\@Dtemp_w)); print STDERR  "Calc_Dfit7:: DFit Tot W=$Dfit_tot {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   $whnd_ref->{DOP_DFIT}->[$ts_num] = $Dfit_tot;
   $Dfit_tot = int(&sum_suit(\@Dtemp_e)); print STDERR  "Calc_Dfit7:: DFit Tot E=$Dfit_tot {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   $ehnd_ref->{DOP_DFIT}->[$ts_num] = $Dfit_tot;
   #
   return 1;
} # end sub Calc_DFit7

sub DOP_2_Trix {
# args: $dop, "N" or "notN" for NT or suit QuickLosers.
# Returns ($trix, low%, hi% ,[$trix, low%, hi% ]) only if NT and 33
# 2021/02/27 New Version takes into account First Round Losers if trix > 11
# This is so the reports don't look ridiculous saying we have 70% chance at 13 tricks missing two Aces.
   my ($dop, $strain, $quick_losers) = (@_);
   local $req_dbg = $req_dbg + 1;
   my ($trix_adj, $idx, $raw_tricks, $dbg_strain);
   $trix_adj = min(2,$quick_losers);
   $dbg_strain = ($strain eq "S" ) ? "Suit" : "NT" ;
   print STDERR "DOP_2_Trix::Args := DOP[$dop], Strain[$dbg_strain], QL=[$quick_losers] { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
      $dop = 38 if $dop > 38 ;   #  The table only has 20 entries 20-38 with two for 33.
      $idx = int($dop - 20 );    #  $dop could have been nn.5 or nn.25
      if ($idx < 0 ) { return (1, 90, 99) ; }  # ficticious numbers 1 trick if < 20 pts.
   my @result = ();
   if ( $strain =~ m/N|n/ ) { # Strain is in NT  -- 33 DOP is ambiguous; either 11 trix with 65%+ or 12 trix with 50%.
      $idx += 1 if $dop > 33 ;   # there are two entries for 33.
      @result = (@{$nt_trix[$idx]}) ;
      if ($dop == 33 ) {@result = (@result, @{$nt_trix[$idx+1]}) };
   } # end NT
   else {
      @result = (@{$suit_trix[$idx]}) ;
   }
   print STDERR  "DOP_2_Trix:: Strain=$dbg_strain, DOP=$dop, TrixTriplet[@result], IDX=$idx {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg + 1;
   # If DOP says we have a slam, then IRL we would check for Controls and stop at 5 or 6 level.
   # We would never stop short of game based on quick losers.
   # Adjust the trick count based on the QuickLosers if DOP tricks is at the 6 or 7 level.
   # Also adjust the %ages up by 5% to reflect the lower level with more DOP than necessary.

   $raw_tricks = $result[0];
   if     ($trix_adj == 2 && $raw_tricks > 11  ) {$result[0] = 11 ; $result[1] += 5; $result[2] += 5; }
   elsif  ($trix_adj == 1 && $raw_tricks > 11  ) {$result[0] -= 1 ; $result[1] += 5; $result[2] += 5; }
   print STDERR  "DOP_2_Trix:: Fixed Tricks=$result[0], RawTricks=$raw_tricks, QUICK_LOSERS=$quick_losers {$Debug ge $req_dbg} \n"
      if $Debug >= $req_dbg;
   return @result;
} #end sub DOP_2_Trix
#end sub DOP_2_Trix


sub Count_Quick_Losers { # args whnd_ref, ehnd_ref, Suit|NT ; returns FirstRoundLosers, SecondRoundLosers, @QuickLosers
   # revised 2021-04-13 to check for FRL and SRL in same suit. if so = Two Quick Losers.
   # 2022-01-10. Note: Kx(xx) is NOT a 2nd round loser. 2nd Round losers only count twords Quick Losers if there is a 1st round loser.

   my $whnd_ref = shift;
   my $ehnd_ref = shift;
   my $strain = shift;
   my ($frl , $srl ) = (0,0);
   my $ql = 0;
   my @quick_losers = (0,0,0,0); # first round losers + 2nd rnd losers with first rnd loser in same suit.
   my (@wsptrs, @esptrs, $wlen, $elen) ;
   local $req_dbg = $req_dbg + 1;
   @wsptrs = &get_sptrs ( $whnd_ref );
   @esptrs = &get_sptrs ( $ehnd_ref );
   print STDERR "Counting QUICK_LOSERS in [$strain] for [$whnd_ref->{STR} :: $ehnd_ref->{STR}] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   foreach $suit (0..3) {
      $wlen = $wsptrs[$suit]->{LEN};
      $elen = $esptrs[$suit]->{LEN};
      $ql1 = 0; $ql2 = 0;
      if ($strain ne "N" ) { # if not NT then account for shortness
         if (($wlen == 0) or ($elen == 0) ) {  next ; }  # if void in either hand no quick losers in suit
         if ($wlen == 1 or $elen == 1) {
            if ($wsptrs[$suit]->{Has_A} or $esptrs[$suit]->{Has_A})  { next ; } # a stiff with an Ace = 0 losers
            else { $frl++; $ql1 = 1; next ; }
         } # end len == 1
      }  # end strain ne NT What if strain IS = NT?

      # This next code will count Quick Losers regardless of length;
      #e.g. if a void, then 2 QL unless AK in other hand.
      # So we can apply it if the strain is NT or if it is a suit equally
      # since the suit void and stiff cases already done.
      # Check if we have both Ace and King between the two hands
      if (($wsptrs[$suit]->{Has_A} && $wsptrs[$suit]->{Has_K}) ||
          ($esptrs[$suit]->{Has_A} && $esptrs[$suit]->{Has_K}) ||
          ($wsptrs[$suit]->{Has_A} && $esptrs[$suit]->{Has_K}) ||
          ($esptrs[$suit]->{Has_A} && $wsptrs[$suit]->{Has_K})  )  { next ; } # no losers if we have both A & K in suit.
      if ( $wsptrs[$suit]->{Has_A} || $esptrs[$suit]->{Has_A} ) { # no first round loser;
         # At this point the suit has the Ace, but is missing the King. Count a 2nd round loser unless AQ in one hand.
         if (($wsptrs[$suit]->{Has_A} && $wsptrs[$suit]->{Has_Q}) ||
             ($esptrs[$suit]->{Has_A} && $esptrs[$suit]->{Has_Q}))  {$srl += 0.5; $ql2 = 0 ; next ; }
         else {$srl++ ; $ql2 = 0 ; next ; } # if we have no frl in the suit, then the 2nd round loser is not a quick loser.
      }
      # We are missing the Ace in the 2+ card suit.  we have a first round loser. which is automatically a quick loser also.
      $frl++; $ql1 = 1 ; #
      # Proceed to check on 2nd round losers. No K = 1, Kx = 0.5 ; KQ = 0 2nd round losers;
      if (($wsptrs[$suit]->{Has_K} && $wsptrs[$suit]->{Has_Q}) ||
          ($esptrs[$suit]->{Has_K} && $esptrs[$suit]->{Has_Q}) ||
          ($wsptrs[$suit]->{Has_K} && $esptrs[$suit]->{Has_Q}) ||
          ($esptrs[$suit]->{Has_K} && $wsptrs[$suit]->{Has_Q})  )  { next ; } # no 2nd round losers if KQ in the suit.
      if ( $wsptrs[$suit]->{Has_K} || $esptrs[$suit]->{Has_K} ) { $srl += 0.5 ; $ql2 = 0.5 ;  next ;} # half a 2nd round loser for unsupported K
      # We are missing the King; we have a 2nd round loser in a suit that also has a frl
      $srl++ ; $ql2 = 1 ; next ;
   } # end for each suit.
   continue { $quick_losers[$suit] = int($ql1 + $ql2 ) ;  } # continue for each suit.

   print STDERR "This deal has $frl First Round Losers and $srl Second Round Losers {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;;
   print STDERR "          and suit Quick losers [@quick_losers] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;;
   return ($frl, $srl, @quick_losers);
} # end sub Count_Quick_Losers

sub Calc_nt_ded_tot {
   # Depending on compass calc the sum of the NT_ded as Decl + NT_ded as dummy;
   # One or both could be zero but their sum affects deal as a whole if both have shortness
   my $dl_ref =  shift ;
   my $declarer = shift ;
   my $nt_ded_tot;
   if ( $declarer =~ m/E|e|N|n/ ) {  #is Decl E or N? what if there is no declarer?
      $dcl_idx = 1 ;
      $nt_ded_dcl   = $$dl_ref{DOP_NT_DED}->[1];
      $nt_ded_dummy = ( $$dl_ref{DOP_NT_DED}->[0] == 0 ) ? 0 : $$dl_ref{DOP_NT_DED}->[0] + $HAND_DOP{NT_DUMMY};
   }
   else {  # Declarer must be West or South
      $dcl_idx = 0 ;
      $nt_ded_dcl   = $$dl_ref{DOP_NT_DED}->[0];
      $nt_ded_dummy = ( $$dl_ref{DOP_NT_DED}->[1] == 0 ) ? 0 : $$dl_ref{DOP_NT_DED}->[1] + $HAND_DOP{NT_DUMMY};
   } #end if else
   $nt_ded_tot = $nt_ded_dcl + $nt_ded_dummy;
   return $nt_ded_tot ;
} # end sub calc_nt_ded_tot

sub Quick_Losers_1hand { # args hnd_ref, S|N; returns FirstRoundLosers, SecondRoundLosers, @QuickLosers
   # 2021-08-29 Adapted for one hand from Count_Quick_Losers

   my $hnd_ref = shift;
   my $strain = shift;

   my ($frl, $srl, $ql, $ql1, $ql2 ) = (0,0,0,0,0);
   my @quick_losers = (0,0,0,0); # first round losers + 2nd rnd losers with first rnd loser in same suit.
   my (@sptrs, $slen, $suit) ;
   local $req_dbg = $req_dbg + 1;
   @sptrs = &get_sptrs ( $hnd_ref );
   print STDERR "Counting QUICK_LOSERS in [$strain] for [$hnd_ref->{STR} ] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   foreach $suit (0..3) {           # see also continue statement at end to handle next's
      $slen = $sptrs[$suit]->{LEN};
      $ql1 = 0; $ql2 = 0;           # these get put into an array at the end of the loop
      if ($strain ne "N" ) { # if not NT then account for shortness
         if ( $slen == 0 )  {  next ; }  # if void no quick losers in suit
         if ( $slen == 1 )  {
            if ( $sptrs[$suit]->{Has_A} )  { next ; }          # a stiff with an Ace = 0 losers
            else                           { $frl++; $ql1 = 1; next ; }
         } # end len == 1
      }  # end strain ne NT  if strain IS == NT then the code that follows handles it.

      # This next code will count Quick Losers regardless of length;
      #e.g. if a void, then 2 QL unless AK in other hand.
      # So we can apply it if the strain is NT or if it is a suit equally
      # since the suit void and stiff cases already done.
      # Check if we have both Ace and King between the two hands
      if ( $sptrs[$suit]->{Has_A} and $sptrs[$suit]->{Has_K} ) { next ; } # no losers if we have both A & K in suit.
      if ( $sptrs[$suit]->{Has_A} and $sptrs[$suit]->{Has_Q} ) { # count half 2nd round loser for AQ; but not ql2
          $srl += 0.5; $ql2 = 0 ; next ;
      }
      elsif ( $sptrs[$suit]->{Has_A} ) { # Ax or longer; no K, no Q. so 2nd round loser and ql2.
          $srl++ ; $ql2 = 0 ; next ;     # 2nd rond loser is not a Quick loser if we have first round ctl.
      }

      # We are missing the Ace in the 2+ card suit.  we have a first round loser. which is automatically a quick loser also.
      $frl++; $ql1 = 1 ; 
      # Proceed to check on 2nd round losers. No K = 1, Kx = 0.5 ; KQ = 0 2nd round losers;
      if ($sptrs[$suit]->{Has_K} && $sptrs[$suit]->{Has_Q})    { next ; } # no 2nd round losers if KQ in the suit.
      if ( $sptrs[$suit]->{Has_K}  )                           { $srl += 0.5 ; $ql2 = 0.5 ;  next ;} # half a 2nd round loser for unsupported K
      # We are missing the King; we have a 2nd round loser in a suit that also has a frl
      $srl++ ; $ql2 = 1 ; next ;
   } # end for each suit.
   continue { $quick_losers[$suit] = int($ql1 + $ql2 ) ;  } # continue for each suit.

   print STDERR "This deal has $frl First Round Losers and $srl Second Round Losers {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;;
   print STDERR "          and suit Quick losers [@quick_losers] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;;
   return ($frl, $srl, @quick_losers);
} # end sub Quick_Losers_1hand



1; # So require succeeds ...
