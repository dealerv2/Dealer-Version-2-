#!/usr/bin/perl
# This is K&R for perl. From Jeff Goldsmith.
# Some changes, comments, cosmetic etc. by JGM.
# Version: 2015-05-01 -- added stdhcp calc and printout. Cleaned up comments.
# Version: 2022-10-10 :
#                         the site: http://www.jeff-goldsmith.org/cgi-bin/knr.cgi
#                         which hosted the code for many years is no longer available.
# Greatly expanded the comments; fixed a bug in the SuitQuality code for 7 card suits missing QJ.
# Fixed bug in DK_CCCC code for richness; 0.325 becomes 0.375
# Added quotes from the orginal Bridge World Article. including how to calculate support points.

# see the =BridgeWorld pod at the end of the code.
#
# Usage:
#        Reads from STDIN.
#        Suits should be in order Spades Hearts Diamonds Clubs separated by SPACES
#        Suits may contain either uppor or lower case AKQJT, 10, digits 2-9 , upper or lower case x and minus signs for voids
#        The cards in the suits should be in descending order ie. QJ9876 NOT Q9876J or 2345KA
#         (usually it will not matter but there are a couple of corner cases where it does.)
# If you want to pass the values on the cmd line you would write the code to ARGV.
#
$dbg = 5;
print "Enter Hand in suit order spades hearts diamonds clubs; space separated  (^D to exit)\n";
while (<STDIN>)    {
   print "You entered: $_ \n" if $dbg > 1;
    &lineparse;                  # clean up input; toupper; 10 -> T etc.
    # deal with silly input
    $cards  = &strlen($_[0]);
    $cards += &strlen($_[1]);
    $cards += &strlen($_[2]);
    $cards += &strlen($_[3]);
    if ($cards != 13)   {
      print "Sorry, this 'hand' has $cards cards.  Please try again.\n";
   }
    else {
       $val     = &cccc(@_);
       $dkval   = &dkcccc(@_);
       $hcp4321 = &stdhcp(@_);
        # deal with printing voids
        if ($_[0] eq "") {$_[0] = "---";}
        if ($_[1] eq "") {$_[1] = "---";}
        if ($_[2] eq "") {$_[2] = "---";}
        if ($_[3] eq "") {$_[3] = "---";}
        printf "($_[0] $_[1] $_[2] $_[3])\n";
        printf "KnR cccc = %.2f\n",$val;
        printf "DK  cccc = %.2f\n",$dkval;
        printf "HCP      = %.2f\n",$hcp4321;
   }
   print "Another hand or ^D to quit:";
} #end while <>

#stdhcp added by JGM.
sub stdhcp  {
       local(@len);  # lengths
       local($pakqj);   # 4321 points + 0.5 for each Ten
    # 4321 count for AKQJ and .5 for T
    $pakqj = 0;
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$pakqj += 4;}
      if ($_[$i] =~ /K/) {$pakqj += 3;}
      if ($_[$i] =~ /Q/) {$pakqj += 2;}
      if ($_[$i] =~ /J/) {$pakqj += 1;}
      if ($_[$i] =~ /T/) {$pakqj +=.5;}
   }
   $pakqj;  #return value
} #end stdhcp

# cccc takes four arguments, spades, hearts,# diamonds, then clubs,
# all using the only the chars "AKQJT98765432".
sub cccc    {
    local(@len);  # lengths
    local($pakq,  # 321 points
          $plen,  # Suit Quality points
          $pdist, # shortness points
         $psk,    # short honor deductions
         $plq,    # long queen deduction
         $plh,    # lower honor points
         $p4333); # 4333 deduction
    local($i,     # counter
     $dblton);    # flag for first dblton.

    # 321 count for AKQ
    $pakq = 0;
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$pakq += 3;}
      if ($_[$i] =~ /K/) {$pakq += 2;}
      if ($_[$i] =~ /Q/) {$pakq += 1;}
   }
   printf( "pakq=%.2f [dbg=%d] \n", $pakq, $dbg ) ;
    # Suit Quality points
    # find suit lengths
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
      $len[$i] = tr///c;   #count non blanks.
   }

    $plen = 0; #Suit Quality points
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$plen += $len[$i] * 4};  #if suit has an Ace, then 4*suit length.
      if ($_[$i] =~ /K/) {$plen += $len[$i] * 3};
      if ($_[$i] =~ /Q/) {$plen += $len[$i] * 2};
      if ($_[$i] =~ /J/) {$plen += $len[$i] * 1};
   }
   printf( "AKQJ plen=%.2f [dbg=%d] \n", $plen, $dbg ) if $dbg >= 1;
    # adjust length factor for long suits missing 0, 1, or 2 of Queen and Jack
    # Note: the article doesn't really specify this very well.  Mark Nau suggested that
    # this is likely to be what they really meant and I (Jeff) concur.
    # JGM: The Bridge World text says(Emphasis by JGM):<quote>
    # A 7 card suit would count 1 point extra (before multiplication by .7 for length) if EITHER minor honor were missing.
    # an 8 card suit, UP TO 2 extra points for missing honors and an even longer suit, UP TO 3 extra.
    # </quote>
    for ($i=0;$i<4;$i++)   {
      # This is Jeff's original code. but =~ /[QJ]/ will match EITHER honor so to get the bonus BOTH must be missing.
      # JGM thinks the code should say =~ /QJ/ (assuming that the cards are given in order in the hand )
      if    (($len[$i] == 7) && !($_[$i] =~ /QJ/))  { $plen +=  7;} # +1 for 7 card suit missing Either Q or J .
      if    (($len[$i] == 8) && !($_[$i] =~ /Q/))   { $plen += 16;} #8 card suit missing Q gets Q*8
      elsif (($len[$i] == 8) && !($_[$i] =~ /J/))   { $plen +=  8;} #8 card suit missing J gets J*8 only if Q missing.
      if    (($len[$i] >  8) && !($_[$i] =~ /Q/))   { $plen +=  2 * $len[$i];} # longer suits can get extra for
      if    (($len[$i] >  8) && !($_[$i] =~ /J/))   { $plen +=  $len[$i];}   # either or both missing 0,1,2 or 3.
      printf( "suit[%d] Missing QJ plen=%.2f [dbg=%d] \n", $i, $plen, $dbg ) if $dbg >= 3;
   } #end Suit Quality AKQJ for loop

    # lower honor Suit Quality mods
    for ($i=0;$i<4;$i++)   {
      # tens
      if ($_[$i] =~ /T/)       {  #if the suit contains a T
         $_ = $_[$i];
         if (($len[$i] <= 6) && (tr/AKQ// >= 2 || /J/)) #Credit T=1 if with a J or two or more of AKQ and length <=6
                              {$plen += $len[$i];}
         else                 {$plen += .5 * $len[$i];} # else credit T = 0.5 (assuming text means still in <=6 suit)
      }
      # nines
      if ($_[$i] =~ /9/ && $len[$i] <= 6)     { #if suit contains a 9, and is less than 7 long,
         $_ = $_[$i];
         if (tr/AKQJ// >= 2 || /T/ || /8/)  #credit 9 with 0.5 if with a T, 8, or 2 of AKQJ
                              {$plen += .5 * $len[$i];}
       }
     printf( "suit[%d] T & 9 plen=%.2f [dbg=%d] \n", $i, $plen, $dbg ) if $dbg >= 3 ;
   } #end lower honors Suit Quality for loop

    # distribution (shortness) points
    $pdist = 0;
    $dblton = 0;
    for ($i=0;$i<4;$i++)   {
      if ($len[$i] == 0) {$pdist += 3};   # 3pts for a void
      if ($len[$i] == 1) {$pdist += 2};   # 2pts for a stiff
      if ($len[$i] == 2) {$dblton++ > 0 ? $pdist += 1 : $pdist }; # dont add for first dblton
      printf( "suit[%d] pdist=%.2f [dbg=%d] \n", $i, $pdist, $dbg ) if $dbg >= 3 ;
   }

   # JGM Commented out the next line, after making mod to previous line.
    #if ($pdist) {$pdist -= 1;}  #discount 1st doubleton --
    # Above looks like an error. What if the dist pts were just 3 for void? and there was no doubleton

    # Test for stiff kings and doubleton queens. (Comments below refer to value of Honor; the shortness value is extra)
    $psk = 0;
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
      if (/K/ && $len[$i] == 1) {$psk += -1.5;}  # stiff K gets 1.5 subtracted from 2 becoming 0.5
      if (/Q/ && $len[$i] < 3)  {   # Q in AQ, or KQ is worth .5 instead of 1; other dblton Q are worth 0.25, stiff Q worth 0
         $psk += -1;    # subtract 1 for stiff Q. Then if Q is not stiff, add some back.
         if (/A/ || /K/)     {$psk += .5;}    # AQ or KQ tight in which case only 0.5 subtracted. (Q worth 0.5)
         elsif ($len[$i] == 2) {$psk += .25;} # Qx subtract only 0.75 (Dblton Q without A or K worth 0.25)
       }
       printf( "suit[%d] Short KQ adj=%.2f [dbg=%d] \n", $i, $psk, $dbg ) if $dbg >= 3 ;
   } #end for stiff K and stiff or Dblton Q for

    # long queens (Qxx or longer) originally worth 1 pt are demoted to 0.75 if no A or K in suit
    $plq = 0;
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
      if (/Q/ && !/A/ && !/K/ && $len[$i] >= 3) {$plq += -.25;}
      printf( "suit[%d] Qxx(x) adj=%.2f [dbg=%d] \n", $i, $plq, $dbg ) if $dbg >= 3 ;
      }

    # lower honors
    $plh = 0;
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
        # Jacks are worth .5 if with precisely 2 (not 3) higher honors, 0.25 if with one higher honor.
   if (/J/)  {
       if (tr/AKQ// == 2) {$plh += .5};
       if (tr/AKQ// == 1) {$plh += .25};
       }
   # Tens are worth .25 with precisely 2 higher or with the 9 and precisely 1 higher.
   if (/T/)   {
       if (tr/AKQJ// == 2) {$plh += .25;}
       if (tr/AKQJ// == 1 && /9/) {$plh += .25;}
       }
       printf( "suit[%d] J & T hcp=%.2f [dbg=%d] \n", $i, $plh, $dbg ) if $dbg >= 3 ;
   }

    # 0.5 deduction for 4333
    $p4333 = 0;
    if ((($len[0] == 3) || ($len[0] == 4)) &&
   (($len[1] == 3) || ($len[1] == 4)) &&
   (($len[2] == 3) || ($len[2] == 4)) &&
   (($len[3] == 3) || ($len[3] == 4)))
       {$p4333 = -.5;}
   printf( "Square Ded=%.2f [dbg=%d] \n", $p4333, $dbg ) if $dbg >= 2 ;

    printf("Recap   : pakq=%.2f, plen=%.2f, pdist=%.2f, psk=%.2f, plq=%.2f, plh=%.2f, p4333=%.2f, Total=%.2f \n",
                   $pakq ,    $plen/10., $pdist ,    $psk ,    $plq ,    $plh ,    $p4333,
      ($pakq + $plen/10. + $pdist + $psk + $plq + $plh + $p4333) ) if $dbg >= 1 ;


    # return value
    $pakq + $plen/10. + $pdist + $psk + $plq + $plh + $p4333;
  } # end cccc (line  69 )

# this converts $_ from fairly flexible input format
# into @_ in cccc's form.
sub lineparse    {  #add parm list here?
   print "Calling lineparse with: $_ \n" if $dbg > 2;
    s/10/T/g;        # in the string $_ convert all occurrences of 10 to T.
    tr/akqjt/AKQJT/; # in the string $_ convert all letters to upper case.
    tr/,/ /s;        # convert commas to spaces.
    print " cleaned up input line: $_\n" if $dbg > 2;
    # following line fixed by JGM. Was simply "split' which does not work.
    @_=split /\s+/;  #create @_ with individual elements $_[0], etc. from the string $_ split on whitespace.
    print "Split creates: $_[0], $_[1], $_[2], $_[3] \n" if $dbg > 2;
    $_[0] =~ tr/AKQJT98765432-/x/c;    #convert any non valid spade card to 'x' (/c is complement the search list)
    $_[1] =~ tr/AKQJT98765432-/x/c;    # ditto for hearts.
    $_[2] =~ tr/AKQJT98765432-/x/c;
    $_[3] =~ tr/AKQJT98765432-/x/c;
    $_[0] =~ s/-//g;             # convert a void (-, --, etc.) to an empty string, so length is zero.
    $_[1] =~ s/-//g;
    $_[2] =~ s/-//g;
    $_[3] =~ s/-//g;
} # End line parse; returns @_ by default I guess..

sub strlen    {
    $_ = $_[0];
   tr///c;        # return count of non null chars in the string.
}

# dkcccc takes four arguments, spades, hearts, diamonds, then clubs,
# all using the chars only "AKQJT98765432".

# This is Danny Kleinman's variation of the CCCC count. NOT DK's adaptation of LJP by Bennion.
# Much of the code is the same as above.

sub dkcccc    {
    local(@len);  # lengths
    local($pakq,  # 321 points
          $plen,  # length points
          $prich,   # richness points
          $pdist, # shortness points
          $psk,     # short honor deductions
          $plq,    # long queen deduction
          $plh,      # lower honor points
          $pbh,    # bare honor suits
          $p4333); # 4333 deduction
    local($i,     # counter
        $dblton );# flag for first dblton which is not counted. JGM

    # 321 count for AKQ  (Same as K&R)
    $pakq = 0;
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$pakq += 3;}
      if ($_[$i] =~ /K/) {$pakq += 2;}
      if ($_[$i] =~ /Q/) {$pakq += 1;}
   }

    # richness
    $prich = 0;
    $temp = 0;

    # count number of face cards
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$prich++; $temp += 4;}
      if ($_[$i] =~ /K/) {$prich++; $temp += 3;}
      if ($_[$i] =~ /Q/) {$prich++; $temp += 2;}
      if ($_[$i] =~ /J/) {$prich++; $temp += 1;}
   }
    $prich = ($temp-2.5*$prich) * .375; # was 0.325 but 3/8 makes much more sense.

    # length points
    # find suit lengths
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
      $len[$i] = tr///c;
   }

    # length is per high card, not per value as in KnR
    $plen = 0;
    for ($i=0;$i<4;$i++)   {
      if ($_[$i] =~ /A/) {$plen += $len[$i] * 2.5}; # KnR say 4*len
      if ($_[$i] =~ /K/) {$plen += $len[$i] * 2.5}; # KnR say 3*len
      if ($_[$i] =~ /Q/) {$plen += $len[$i] * 2.5}; # KnR say 2*len
      if ($_[$i] =~ /J/) {$plen += $len[$i] * 2.5}; # knR say 1*len
   }

    # adjust length factor for long suits missing
    # useless lower honors

    # Note: the article doesn't really specify
    # this very well.  Mark Nau suggested that
    # this is likely to be what they really meant,
    # and I concur.
    for ($i=0;$i<4;$i++)       {
        if    (($len[$i] == 7) && !($_[$i] =~ /QJ/))   { $plen += 7;} #s/b missing either honor so /QJ/. was /[QJ]/
        if    (($len[$i] == 8) && !($_[$i] =~ /Q/))    { $plen += 16;}
        elsif (($len[$i] == 8) && !($_[$i] =~ /J/))    { $plen +=  8;}
        if    (($len[$i] >  8) && !($_[$i] =~ /Q/))    { $plen += 2 * $len[$i];} # 9 card suits can have an extra 3hcp.
        if    (($len[$i] >  8) && !($_[$i] =~ /J/))    { $plen += $len[$i];}
        }

    # lower honor suit length mods
    for ($i=0;$i<4;$i++) {
      # tens
      if ($_[$i] =~ /T/)       {
         $_ = $_[$i];
         if (($len[$i] <= 6) && (tr/AKQ// >= 2 || /J/))  { $plen += $len[$i]; }
         else                                            { $plen += .5 * $len[$i]; }
       }
      # nines
      if ($_[$i] =~ /9/ && $len[$i] <= 6) {
         $_ = $_[$i];
         if (tr/AKQJ// >= 2 || /T/ || /8/)               { $plen += .5 * $len[$i]; }
       }
   } #end honor suit lenght mods for

    # distribution points
    $pdist = 0;
    $dblton = 0;
    for ($i=0;$i<4;$i++)   {
      if ($len[$i] == 0) {$pdist += 3};
      if ($len[$i] == 1) {$pdist += 2};
      if ($len[$i] == 2) {$dblton++ > 0 ? $pdist += 1 : $pdist }; # dont add for first dblton.
   }
    # Fixed: JGM. if ($pdist) {$pdist -= 1;}  #discount 1st doubleton  #Prob an error. What if there was no DBLlton?

    # Test for stiff kings and stiff or doubleton queens.
    $psk = 0;
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
      if (/K/ && $len[$i] == 1) {$psk += -1.5;}
      if (/Q/ && $len[$i] < 3)  {
         $psk += -1;                #assume Q is stiff and subtract 1. Then if not stiff add some back.
         if (/A/ || /K/)      {$psk += .5;}
         elsif ($len[$i] == 2)  {$psk += .25;}
       }
   } # end for stiff Kings, stiff or dblton Queens

    # long queens are demoted if no A or K in suit
    $plq = 0;
    for ($i=0;$i<4;$i++) {
      $_ = $_[$i];
      if (/Q/ && !/A/ && !/K/ && $len[$i] >= 3) {$plq += -.25;}
      }  # end for long queens.

    # lower honors ( J & T )
    $plh = 0;
    for ($i=0;$i<4;$i++)   {
      $_ = $_[$i];
        # Jacks are worth .5 if with 2 higher honors, 0.25 if with only one.
      if (/J/)    {
         if (tr/AKQ// == 2) {$plh += .5};
         if (tr/AKQ// == 1) {$plh += .25};
       }
      # Tens are worth .25 with 2 higher or 1 plus the 9
      if (/T/)    {
         if (tr/AKQJ// == 2) {$plh += .25;}
         if (tr/AKQJ// == 1 && /9/) {$plh += .25;}
       }
   } # end for lower honors.

    # deduction for 4333
    $p4333 = 0;
    if ((($len[0] == 3) || ($len[0] == 4)) &&
   (($len[1] == 3) || ($len[1] == 4)) &&
   (($len[2] == 3) || ($len[2] == 4)) &&
   (($len[3] == 3) || ($len[3] == 4)))
       {$p4333 = -.5;}

    # deduction for bare  honor suits -- this differs from KnR --
    #Also seems an error. len should be >=2 since stiff already accounted for.
    $pbh = 0;
    for ($i=0;$i<4;$i++)   {
        $_ = $_[$i];
        # if (tr/0123456789TXx//c == $len[$i]) {$pbh -= .5;} # This was the original code.
        # This looks to be in addition to Ded for stiff K and Dblton Q; using this stiff K = 0, and Qx = -0.25
        # perhaps we should insist that len >= 2 so AQJ or KQ or KJ etc would qualify?
        if (tr/0123456789TXx//c == $len[$i] && $len[$i] >= 2 ) {$pbh -= .5;}

   }

# debug stuff
printf("Recap_DK: pakq=%.2f, plen=%.2f, pdist=%.2f, psk=%.2f, plq=%.2f, plh=%.2f, p4333=%.2f, pbh=%.2f prich=%.2f  Total=%.2f \n",
                  $pakq ,    $plen/10., $pdist ,    $psk ,    $plq ,    $plh ,    $p4333,     $pbh,    $prich,
      ($pakq + $plen/10. + $pdist + $psk + $plq + $plh + + $p4333 + $pbh +  $prich  ) )  if $dbg >= 1 ;

    # return DKcccc value
    ($pakq + $plen/10. + $pdist + $psk + $plq + $plh + $pbh +  $prich + $p4333  );
} # end dkcccc
=BridgeWorld
The text which follows that is between <quote> ... </quote> is taken from the orginal BW article.
1. It identifies a bug in Jeff Goldsmith's original code re SuitQuality calculation for 7 card suits when
   missing Q, or J, or both.
2. It documents for the first time anywhere on the net, how the CCCC count values shortness once a fit has been found.
1. <quote>
     A 7 card suit would count 1 point extra (before multiplication by .7 for length) if _EITHER_ minor honor were missing.
     an 8 card suit, _UP TO_ 2 extra points for missing honors and an even longer suit, _UP TO_ 3 extra.
</quote>
2. When discussing this example hand,
         AJTxxx KT9x xx x,
originally valued as "just under 13" (actually 12.95 -- JGM)
<quote>
For later in the auction, in responding and rebidding, distributional hands like the example above
may greatly increase or decrease in value. The 3 points in distribution for that singleton and doubleton, 3 non-losers,
will be subtracted by the computer on a totally misfitting auction; and the distribution count may be as much as
doubled when the auction reveals a good fit. When supporting partner's suit with three or more trumps,
the computer adds an extra 50% to the short suit count if sure of an eight-card fit, and adds an extra 100%
when sure of a better fit. When it is partner who has supported, the extra credit is 25% for an assured eight-card fit,
50% for nine, 100% for huge fits.
Responder to that minimum one-spade opening gives a limit raise to three spades. The computer now
values opener's hand as close to 16, and bids on to game. Quite right too!
</quote>

JGM Comments:
1. Assuming the LR shows 4 card support, there is a 10 fit, so the 2 points for the stiff club become 4 and the hand
is now worth 14.95. I don't see 16 unless the doubleton now counts for something? say 1 point.
Perhaps if it takes two doubletons to count 1 distribution point when openining, then
when a fit is found count one doubleton as 0.5? and apply the above 25%, 50%, 100% rule?
Then how would we count two doubletons once a fit is found? 0.5 for the first and 1 for the second? total = 1.5
So an 8 fit would give an extra .375, a 9 fit an extra 0.75 and a 10+ fit an extra 1.5 for two doubletons?
The above comment is regarding the 'non-supporting' hand.

2. Regarding supporting hands:
It sounds like when there is an 8 fit, (50% increase), the computer would give a hand with two doubletons, an extra half point,
(2nd doubleton now worth 1.5) and a hand with a stiff would get an extra point (stiff now worth 3 points).
When there is a 9+ fit (100% increase) a stiff would be worth 4 points and a second doubleton would be worth two.
If there were more than one stiff (a 6511 or 7411 hand say) then BOTH stiffs would be upgraded to 4 points.

The text refers to '8 fit' and '9 fit', not 3 trumps and 4 trumps. It is unclear how the KnR algorithm would treat
a supporting hand in a 4=4 fit. Would the stiff only increase in value from 2 to 3 or would it go from 2 to 4?
Presumably the non-supporting hand would still add 25% to its shortness points for the 8 fit.

=cut



