# No shebang since this file brought in by require.
# Author      Version  Date           Comment
#  JGM         13.0      2021/01/21   DOP Competition Subs. Vers 13 of the whole DOP pgm. is really vers 1 of this file.
#  JGM         21.0      2021/08/30   Mods for Terse report. All debug prints to STDERR. No other changes.
#
# First calculate the DOP without competition
# Then adjust for Competition. Depends on which suits they open and/or bid and
# whether we are planning to O'Call in NT, T/O Dbl, O'Call in a suit, or Jump O'Call in a suit.
# Honor Adjustments in plain suits, affect only lone K and lone J and apply only to suits of 3 or 4 cards; Not to shorter or longer suits.
# Honor Adjustments in the opps suit(s), affect only KQ, lone K and lone J. (Suit lengths??????)
# Length Adjustments apply only to the opps suits.
#        If contemplating a suit contract, for shortness in opps suit(s):  +1 for a void (=5), +1 for stiff (=3),
#                                          for length in opps suit, -1,-2,-3 for 3,4,5 card length.
#        If contemplating a NT Overcall, you will not have xx or less in an opps suit;
#                                         For length in the opps suit(s)  0,-1,-2 for 3,4,5 card length. i.e. lesser deduction.

# See -H5 which will print a usage message that includes options for competition evaluations.

#

sub lone_King_adj { #args: suit_ptr, plain_yn (-1 for lone K in 3 or 4 card suit. KTx or better gets NO adj. )
   my $sptr = shift;
   my $plain_yn = shift; # If in opps suit, always adjust for lone Kings; If in Plain suit only if 3 or 4 cards.
   local $req_dbg = $req_dbg +1 ;
   print STDERR "lone_King_adj:: Plain[$plain_yn], Suit[$$sptr{STR}], LEN[$$sptr{LEN}] {$Debug ge $req_dbg+1}\n" if $Debug >= $req_dbg+1;
   if ($plain_yn eq "Y" ) { # Plain suit; adjust only if the length is 3 or 4.
      if ($$sptr{LEN} ne 3 && $$sptr{LEN} ne 4 ) { return 0 ; }  # No adjustment if not a 3 or 4 card suit.
      print STDERR "lone_King_adj::Plain $$sptr{STR},LEN[$$sptr{LEN}],has_K,Ten[$$sptr{Has_K},$$sptr{Has_T}], Sy#=$$sptr{Sy_Num}\n"
            if $Debug >= $req_dbg;
      if ($$sptr{Has_K} ne 1  ||  $$sptr{Sy_Num} ne 1 || $$sptr{Has_T} ne 0 ) { return 0 ; } # No King or King with H or T
      else { return $AOC_DOP{Lone_K} ; } # must be has_K == 1  && Sy_num == 1  && has_T == 0 i.e a lone King.
  } # end plain_yn = Y
  # Testing in opponents suit. Assuming we deduct regardless of length, but maybe not for 2 or less.
  print STDERR "lone_King_adj::OPPS $$sptr{STR},LEN[$$sptr{LEN}],has_K,Ten[$$sptr{Has_K},$$sptr{Has_T}], Sy#=$$sptr{Sy_Num} {$Debug ge $req_dbg}\n"
            if $Debug >= $req_dbg;
  if ($$sptr{Has_K} ne 1  ||  $$sptr{Sy_Num} ne 1 || $$sptr{Has_T} ne 0 ) { return 0 ; } # No King or King with H or T
  else { print STDERR "lone_King_adj:: Adjustment made in Opps suit {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
         return $AOC_DOP{Lone_K} ;
       }
} # end sub lone_King_adj

sub lone_Jack_adj { #args: suit_ptr
   my $sptr = shift ;
   local $req_dbg = $req_dbg +1 ;
   # J, Jx are already worth 0, but Lone Jacks in any suit (Except our suit) with len >= 3 are devalued,
   print STDERR "lone_Jack_adj:: $$sptr{STR},LEN[$$sptr{LEN}],has_J,Ten[$$sptr{Has_J},$$sptr{Has_T}], Sy#=$$sptr{Sy_Num} {$Debug ge $req_dbg}\n"
      if $Debug >= $req_dbg;
   if ($$sptr{Has_J} ne 1  || $$sptr{Sy_Num} ne 1 || $$sptr{Has_T} ne 0 || $$sptr{LEN} lt 3) {
          return 0 ; # No Jack or Jack with H or T, or Jack already worth zero so no adjustment.
   }
   else { return $AOC_DOP{Lone_J} ; } # must be has_J ==1  && Sy_num == 1  && has_T == 0 i.e a lone Jack
}  # end sub lone_Jack_adj

sub KQ_adj { #args: suit_ptr, O|U for KQ_Over or KQ_Under their suit.
   my $sptr = shift ;
   my $pos_OU = shift ;  # we are Over or Under the suit in question.
   local $req_dbg = $req_dbg +1 ;
   print STDERR "KQ_adj:: $$sptr{STR},LEN[$$sptr{LEN}],has_KQ[$$sptr{Has_K},$$sptr{Has_Q}], Sy#=$$sptr{Sy_Num} if {$Debug ge $req_dbg}\n"
      if $Debug >= $req_dbg;
# This adjustment applies regardless of length, since it is only in the opps suit.
   if ($$sptr{Has_K} ne 1  || $$sptr{Has_Q} ne 1 ||  $$sptr{Sy_Num} ne 2 ) { return 0 ; } # No K or No Q or extra honor
   else { return ($pos_OU eq "O" ) ? $AOC_DOP{KQ_over_opp} : $AOC_DOP{KQ_under_opp} ; }
}  # end sub KQ_adj

#:: adjustments for honors in competitive auctions. Same whether we plan NT OC or Suit Intervention.
# Called for each suit so only one test will succeed if any.
sub AOC_h_adj { #args : suit_ptr, LHO_suit_let, RHO_suit_let, $lho_over, $rho_over
   my ($sptr, $lho_suit, $rho_suit, $lho_over, $rho_over ) = @_ ;
   local $req_dbg = $req_dbg +1 ;
   my ($aoc_h, $aoc_k, $aoc_j, $aoc_kq) = (0,0,0,0);       #aoc adjustments to DOP_H
   print STDERR "AOC_h_adj::Suit[$$sptr{LET}]=$$sptr{STR}, LHO=$lho_suit, RHO=$rho_suit,LEN=$$sptr{LEN} {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
   if ($$sptr{LET} ne $lho_suit && $$sptr{LET} ne $rho_suit ) { # plain suit
      if ($$sptr{LEN} eq 3 || $$sptr{LEN} eq 4 ) { # adjust DOP_H only for 3 or 4 card plain suits
         $aoc_k = &lone_King_adj ($sptr, "Y") ; # at most one of these should be non-zero as KJx gets not adj.
      }
      $aoc_j = &lone_Jack_adj ($sptr) ;  # Jacks in all suits get adjusted  regardless of length.
      $aoc_h = $aoc_k + $aoc_j; # only one should be non_zero.
      print STDERR "AOC_h_adj:: returns $aoc_h; for plain suit $$sptr{STR}  {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      return $aoc_h ;  # for plain suit
   } # end plain suit.
   else  { # suit must be LHO_suit or RHO_suit i.e. NOT a plain suit. Adjust regardless of LEN (PD 2021/02/16)
         $aoc_k = &lone_King_adj ($sptr, "N") ; # at most one of these should be non-zero as KJx gets not adj.
         $aoc_j = &lone_Jack_adj ($sptr) ;      # Adjust for lone Jacks in any suit.
         if    ($$sptr{LET} eq $lho_suit && $lho_over eq "Y" ) { $aoc_kq = &KQ_adj ($sptr, "U" ) ; }
         elsif ($$sptr{LET} eq $lho_suit && $lho_over ne "Y" ) { $aoc_kq = &KQ_adj ($sptr, "O" ) ; }
         elsif ($$sptr{LET} eq $rho_suit && $rho_over eq "Y" ) { $aoc_kq = &KQ_adj ($sptr, "U" ) ; }
         elsif ($$sptr{LET} eq $rho_suit && $rho_over ne "Y" ) { $aoc_kq = &KQ_adj ($sptr, "O" ) ; }
#      }
      $aoc_h = $aoc_k + $aoc_j + $aoc_kq ; #only one should be non-zero
   print STDERR "AOC_h_adj:: returns $aoc_h; for Opps suit $$sptr{STR} {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
   return $aoc_h ;  # for (one of ) their suit(s)
   }
return 0 ;    # no honor adjustment needed.
}  # end sub AOC_H_adj

#:: adjustments for length in competitive auctions. Values depend on whether NT OCall or Suit Intervention.
# Assuming having Length > 5 in their suit gets NO adj. Other assumption is >=5 get 5 adjustment.
sub AOC_l_adj { #args : suit_ptr, LHO_suit_let, RHO_suit_let, NT_OC_YN
   my ($sptr, $lho_suit, $rho_suit, $nt_oc ) = @_ ;
   local $req_dbg = $req_dbg +1 ;
   my $aoc_l = 0;
   if ($$sptr{LET} ne $lho_suit && $$sptr{LET} ne $rho_suit ) { return 0 ; } #no len adjustments in plain suits
   if ($nt_oc eq "Y" ) {   # adjust for 4, 5 cards in opps suit(s). Don't NT oc with 0,1, in their suit, rarely 2.
      $aoc_l = ($$sptr{LEN} eq 4 ) ? ($AOC_DOP{Opp_Len_4}+$AOC_DOP{Opp_Len_NT}) :
               ($$sptr{LEN} eq 5 ) ? ($AOC_DOP{Opp_Len_5}+$AOC_DOP{Opp_Len_NT}) :
               ($$sptr{LEN} lt 3 ) ?  $AOC_DOP{Opp_NT_Short} :  # a large fake number to prevent NT overcalls with shortness
               0 ;
      print STDERR "AOC_l_adj::  for NT_OC returns $aoc_l; for Opps suit $$sptr{STR} {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      return $aoc_l ;  # adjustment for 4 or 5 cards in their suit(s).
   }

   # must be planning a suit OC or T/O dbl; current sptr is one of their suits.
   $aoc_l =  ($$sptr{LEN} eq 0 ) ? ($AOC_DOP{Opp_Len_0} ) :
             ($$sptr{LEN} eq 1 ) ? ($AOC_DOP{Opp_Len_1} ) :
             ($$sptr{LEN} eq 3 ) ? ($AOC_DOP{Opp_Len_3} ) :
             ($$sptr{LEN} eq 4 ) ? ($AOC_DOP{Opp_Len_4} ) :
             ($$sptr{LEN} eq 5 ) ? ($AOC_DOP{Opp_Len_5} ) :  0 ;  # is zero for >5 correct?
   print STDERR "AOC_l_adj:: returns $aoc_l; for suit $$sptr{STR} OC suit\n" if $Debug >= $req_dbg;
   return $aoc_l ;

} # end sub AOC_L_adj

sub AOC_DOP_adj { #args: hnd_ref, aoc_hnd_ref, lho_suit, rho_suit
   # calculates the adjustments for Honors and length in each suit for: NT_overcall or (suit ocall or T/O dbl );
   my $hnd_ref = shift ;
   my $aoc_hnd_ref = shift ;
   my $lho_suit = shift ;
   my $rho_suit = shift ;
   local $req_dbg = $req_dbg +1 ;
   my ($lho_over, $rho_over, $aoc_h_tot, $aoc_l_suit_tot, $aoc_l_nt_tot  );
   my (@sptrs, @aoc_h_adj, @aoc_l_suit_adj, @aoc_l_nt_adj, );
   @sptrs = &get_sptrs ( $hnd_ref );
   if ($hnd_ref->{OPEN} eq 'Y' ) { $lho_over = "Y"; $rho_over = "N" ; }
   else                          { $rho_over = "Y"; $lho_over = "N" ; }
   print STDERR "AOC_DOP_adj:: $$hnd_ref{STR},$$hnd_ref{OPEN},$lho_over,$rho_over {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   for $nsuit ( 0 .. 3 ) {
      $aoc_hnd_ref->{AOC_H}->[$nsuit]      = &AOC_h_adj($sptrs[$nsuit], $lho_suit, $rho_suit, $lho_over, $rho_over);
      $aoc_hnd_ref->{AOC_L_SUIT}->[$nsuit] = &AOC_l_adj($sptrs[$nsuit], $lho_suit, $rho_suit, "N"  ) ; # Not a NT O'C
      $aoc_hnd_ref->{AOC_L_NT}->[$nsuit]   = &AOC_l_adj($sptrs[$nsuit], $lho_suit, $rho_suit, "Y"  ) ;
      print STDERR "AOC_DOP_adj:: Suit $nsuit, H_adj=$aoc_hnd_ref->{AOC_H}->[$nsuit], L_adj=$aoc_hnd_ref->{AOC_L_SUIT}->[$nsuit] {$Debug ge $req_dbg+1}\n"
         if $Debug >= $req_dbg +1;
   }
   print STDERR "AOC_DOP_adj:: Totalling suits for hand $$aoc_hnd_ref{LET} {$Debug ge $req_dbg}\n"  if $Debug >= $req_dbg +1;
   &show_DOP_hash("AOC_DOP_adj for Hand before totals",$aoc_hnd_ref) if $Debug >= $req_dbg + 1;
   $aoc_hnd_ref->{AOC_H_TOT}      = &sum_suit($aoc_hnd_ref->{AOC_H});
   $aoc_hnd_ref->{AOC_L_SUIT_TOT} = &sum_suit($aoc_hnd_ref->{AOC_L_SUIT});
   $aoc_hnd_ref->{AOC_L_NT_TOT}   = &sum_suit($aoc_hnd_ref->{AOC_L_NT});
   print STDERR "AOC_DOP_adj:: Total AOC_H=$$aoc_hnd_ref{AOC_H_TOT}, AOC_L=$$aoc_hnd_ref{AOC_L_SUIT_TOT}, AOC_LNT=$$aoc_hnd_ref{AOC_L_NT_TOT} {$Debug ge $req_dbg} \n"
      if $Debug >= $req_dbg;
} # end sub AOC_DOP_adj

sub AOC_DOP_deal_adj { # args: aoc_deal_ref, aoc_whnd_ref, aoc_ehnd_ref, :: updates hash AOC_DOP_DEAL
   my $aoc_dl_ref = shift ;  #output destination
   my $whnd_ref   = shift ;  #input hash -- this is the AOC hash not the DOP hash.
   my $ehnd_ref   = shift ;
   local $req_dbg = $req_dbg +1 ;
   my $nsuit ;
   print STDERR "Entered AOC_DOP_deal_adj Totalling results from AOC_DOP_adj {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
   # now total the two hands into the deal hash which contains the total by suit for both hands, plus overall hand total.
   for $key (qw( AOC_H AOC_L_SUIT AOC_L_NT )) {
       print STDERR "AOC_DOP_deal_adj:: Key=$key {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
      for $nsuit ( 0 .. 3 ) {
         $$aoc_dl_ref{$key}->[$nsuit] = $$whnd_ref{$key}->[$nsuit] + $$ehnd_ref{$key}->[$nsuit] ;
         print STDERR "AOC_DOP_DEAL:: Result = for key[$key] suit[$nsuit] = $$aoc_dl_ref{$key}->[$nsuit] {$Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
      }
      $key_tot = $key . "_TOT" ;
      $$aoc_dl_ref{$key_tot} = &sum_suit($$aoc_dl_ref{$key});
      print STDERR "AOC_DOP_DEAL::Suit sum for $key_tot =  $AOC_DOP_DEAL{$key_tot} {$Debug ge $req_dbg }\n" if $Debug >= $req_dbg;
   } # end for qw
   &show_DOP_hash ("AOC_DOP_DEAL", \%AOC_DOP_DEAL ) if $Debug + 1 ge $req_dbg + 1;
} # end sub AOC_DOP_deal_adj

# ---------------------------------- Error Checking of Competition Option Specs -------------------
sub aoc_chk_suit {
   my $suit = shift ;      # should be a single char, C,D,H,S but let's do some error checking
   $suit =~ tr /a-z/A-Z/ ; #to Upper
   $suit = $suit . 'N';
   $suit =~ tr /CDHSN//cd; #delete invalids
   $suit = substr($suit, 0, 1) ; # keep only the first letter, which is a valid suit (or N) at this point
   return $suit ;
}
sub aoc_chk_rpt {  # check the -A option for the reporting spec. should be N (None), S (Summary), D (Detail)
   my $aoc_rpt = shift ;
   $aoc_rpt = substr($aoc_rpt, 0, 1) ; #keep first char only.
   $aoc_rpt =~ tr/nsd/NSD/; $aoc_rpt =~ tr/NSD//cd;  # make sure we return valid char or nothing.
   if ($aoc_rpt ne "" ) {   return $aoc_rpt; }
   else                 {   return "N" ; }
}
sub AOC_RPT {  # args: aoc_deal_ref, whnd_ref_aoc, ehnd_ref_aoc,  dl_ref, $whnd_ref, $ehnd_ref  ::
my ($aoc_dl_ref, $whnd_ref_aoc, $ehnd_ref_aoc , $lho_suit, $rho_suit, $dl_ref, $whnd_ref, $ehnd_ref  ) = @_;
   local $req_dbg = $req_dbg + 1;
   my ($compass, $aoc_ptr, $hnd_ptr, $bidder, $aoc_hld_suit, $aoc_hl_nt, $lev_suit_oc, $lev_nt_oc, $aoc_fmt, ) ;
   #
   print STDERR "AOC_RPT:: <-$$whnd_ref{STR}::$$ehnd_ref{STR}, LHO_Suit[$lho_suit],RHO_SUIT[$rho_suit] {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg  ;
   my $rptfile = $file_specs{RPTFILE};
   print $rptfile "Adjustments to DOP in Competitive Auctions: RHO Suit=[$rho_suit], LHO_Suit=[$lho_suit] \n";
   print $rptfile "                             Raw    Suit O-Call      NT O-Call                  Suit O-Call NT O-Call\n";
   print $rptfile "Hand O <-----------------> DOP_HLD  AOC_HLD LVL     AOC_HLD LVL  ::  AOC_H_Adj   AOC_L_Adj  AOC_L_Adj\n";
   $aoc_fmt="%2s  %2s  %-18s % 6.2f    % 6.2f  %2d     % 6.2f  %02d   ::    %+3.1f       %+3.1f        %+3.1f\n";
  foreach $compass ( "W", "E" ) {
      if   ($compass eq "W" ) {
         $aoc_ptr = $whnd_ref_aoc;
         $hnd_ptr = $whnd_ref;
         print STDERR "AOC_RPT::West DOP_HLD=$hnd_ptr->{DOP_HLD},DOP_HL=$hnd_ptr->{DOP_HL},DOP_D=$hnd_ptr->{DOP_D} {$Debug ge $req_dbg}\n"
            if $Debug >= $req_dbg;
         print STDERR "AOC_RPT::West NT VALUES: DOP_NT_ADJ=$$hnd_ptr{DOP_NT_DED}, AOC_L_NT_ADJ=$$aoc_ptr{AOC_L_SUIT_TOT} {$Debug ge $req_dbg}\n"
            if $Debug >= $req_dbg;
     }
      elsif ($compass eq "E" ) {
         $aoc_ptr = $ehnd_ref_aoc;
          $hnd_ptr = $ehnd_ref;
         print STDERR "AOC_RPT::East DOP_HLD=$hnd_ptr->{DOP_HLD},DOP_HL=$hnd_ptr->{DOP_HL},DOP_D=$hnd_ptr->{DOP_D} {$Debug ge $req_dbg}\n"
            if $Debug >= $req_dbg;
         print STDERR "AOC_RPT::East NT VALUES: DOP_NT_ADJ=$$hnd_ptr{DOP_NT_DED}, AOC_L_NT_ADJ=$$aoc_ptr{AOC_L_SUIT_TOT} {$Debug ge $req_dbg}\n"
            if $Debug >= $req_dbg;
      }
      if ($hnd_ptr->{OPEN} eq 'Y' ) {$bidder = 'I' ; } # Intervernor. First bidder for our side.
      else                          {$bidder = 'A' ; } # Advancer.
      $aoc_hld_suit = $$hnd_ptr{DOP_HLD} + $$aoc_ptr{AOC_H_TOT} + $$aoc_ptr{AOC_L_SUIT_TOT};
      $aoc_hl_nt    = $$hnd_ptr{DOP_HL}  + $$aoc_ptr{AOC_H_TOT} + $$aoc_ptr{AOC_L_NT_TOT} + $$hnd_ptr{DOP_NT_DED}   ;
      if ($aoc_hl_nt < 0 ) {$aoc_hl_nt = 0 ; }  # because of possible adj for shortness in opps suit.
      $lvl_suit_oc  = ($aoc_hld_suit > 18 ) ? 3 : ($aoc_hld_suit > 15 ) ? 2 : ($aoc_hld_suit >= 13 ) ? 1 : 0;
      $lvl_nt_oc    = ($aoc_hl_nt > 18 ) ? 2 : ($aoc_hl_nt > 15 ) ? 1 :  0;
      printf $rptfile $aoc_fmt, $compass, $bidder, $hnd_ptr->{STR}, $$hnd_ptr{DOP_HLD}, $aoc_hld_suit,  $lvl_suit_oc ,
            $aoc_hl_nt, $lvl_nt_oc ,
            $aoc_ptr->{AOC_H_TOT}, $aoc_ptr->{AOC_L_SUIT_TOT}, $aoc_ptr->{AOC_L_NT_TOT} ;
   } # end foreach compass
   # Decided not to do the totalling of the combined AOC for both hands.
   # since that is unlikely to help overcaller/intervernor decide whether or not to act.

}  # end report sub

  1 ;








