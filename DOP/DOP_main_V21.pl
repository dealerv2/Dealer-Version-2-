#!/usr/bin/perl
## Main program for DOP evaluation -- This file is a duplicate of DOP_main_V21.pl
## Author      Version  Date           Comment
#  JGM        10.3      2020/12/17     minor bug fixes. Reporting routines.
#  JGM        11        2020/12/18     strain specs, help option, other debug.
#  JGM        13        2020/01/22     Modifications based on DOP in Competitive Auctions.
#  JGM        13.1      2020/02/16     AOC almost done.
#  JGM        14.0      2021/04/04     Big Mods for Wastage re NT contracts in particular.
#  JGM        20.0      2021/06/30     Begin code for evaluating GIB database
#                       2021/07/xx     GIB stuff working. Should really be called PBN stuff but too late now
#  JGM        21.0      2021/08/28     Begin coding for adding -q (terse) output for dealer
#  JGM        21.1      2022/01/10     Eliminated lib dir. Moved to BridgeSW/DOPDealer and copy to /usr/local/bin/DOP
#  JGM        21.2      2022/01/12     Created ParseDDSline in DOP_Subs2_V21.pl to handle -q req from Dealer
#
#VERS 21 Note: see the file Doc/Dealer_DOP_IF.txt for descr of how dealer should call the DOP Perl code.

#-------------------------------------------------------------------------------------------------
#
# Run <progname> -h0 for brief usage, -h5 for full blown usage and help.

# On the cmd line we enter: -W<hand> and/or -E<hand> (E is optional).
# on Stdin we enter: W=<hand>  E=<hand> Can also use : colon instead of = equal. No Minus Signs
#
package main;
 #use Smart::Comments  ;            # '###', '####', '#####'  3 is high level, 5 is verbose
 use lib "/home/greg19/Programming/Bridge_SW/DOPDealer";
 use Getopt::Std;
 # ----------------------------------------------------------------------
require "DOP_Requires_V21.pl";
# Set STDERR to autoflush
#my $oldfh = select STDERR ; $| = 1 ; select $oldfh ;
# NOTE if we call this program with the -q switch we really should not be printing anything to STDOUT.
#
print STDERR "Requires Loaded OK. $Require_OK ENV_DEBUG = $ENV{DEBUG}\n" if $ENV{DEBUG} > 0;
 # -----------------------------------------------------------------------

 our $MainVersion = "21.2";
 our $Debug = $ENV{DEBUG} ;                  # 0 none, 2 high level 9 verbose. Use this till getopts changes it.
 our $req_dbg = 2;                           # increment this as we go deeper down the sub calling....
   printf STDERR "Main:: BEGIN Execution of DOP_Main...$MainVersion with DEBUG=$Debug and req_dbg=$req_dbg\n" if $Debug >= $req_dbg;
   tell_myname("DOP Main Program version $MainVersion") if $Debug >= $req_dbg;
   print STDERR "REQUIRE:: Requires version $VERSION loaded.\n" if $Debug >= $req_dbg;
#$Debug = 5;
# First process Cmd Line switches as these may affect defaults for the whole run.
INIT_GETOPTS:

   printf STDERR "Main:: GETOPTS_INIT Calling get_opts {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
#  ------  CALL GET_OPTS --------  [Some opts might affect STDIN processing, even if no W/E hands on cmd line]
# V11/up: use the hash results as defaults for this run. ignore the returned value list.
   ($h_west, $h_east, $ds_w, $ds_e, $opt_opener) = &get_opts(\%get_opts);  # put cmd line switches into hash %get_opts

   #$req_dbg = $req_dbg -1 ;   #get_opts needs more debug output -- lower the threshold.
   print STDERR "Main:: Checking get_opts hash:: OPT_o[$get_opts{OPT_o}], OPT_i[$get_opts{OPT_i}], OPT_RD[$get_opts{OPT_RD}],",
         " OPT_RH[$get_opts{OPT_RH}], OPT_ew[$get_opts{OPT_e}, $get_opts{OPT_w}] OPT_O[$get_opts{OPT_O}],{ $Debug ge $req_dbg} \n"
         if $Debug >= $req_dbg;
   print STDERR "Main:: GETOPTS_INIT Done W=[$get_opts{OPT_W}], E=[$get_opts{OPT_E}] Opener=[$opt_opener],Strains:[$ds_w],<$ds_e>\n"
         if $Debug >= $req_dbg;

   &show_DOP_hash("OPT Specs after GETOPTS INIT",\%get_opts) if $Debug >= $req_dbg+1;
   print STDERR "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;

# Note if only one hand is entered on the cmd line with a -W or -E switch, get_opts will always put that hand
# into the West variable, and also put the -e string into the -w vars, and make sure the -O option is switched if req'd.
   $req_dbg = $req_dbg + 1 ; #Raise threshold again.

   &Start_Main() ; # use the getopts to set the run time defaults. Also initialize a bunch of other stuff.
   print STDERR " Main:: Start Main Done. ********  Calling Setup_FILEIO ********* { $Debug >= $req_dbg }\n" if $Debug >= $req_dbg ;

   &setup_FILEIO(\%file_specs, );
   print STDERR "Main::FileSetupDone IN_FILE_OK[$file_specs{IN_FILE_OK}], RPT_FILE_OK[$file_specs{RPT_FILE_OK}],",
         "rpt_fname=[$file_specs{RPT_FILE_NAME}], in_fname=[$file_specs{IN_FILE_NAME}] { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg;
   &show_DOP_hash("FILE SPECS Hash  {$Debug ge $req_dbg }", \%file_specs) if $Debug >= $req_dbg;
   print $rptfile "FIRST TEST LINE TO RPTFILE HANDLE { $Debug ge $req_dbg +2 }\n" if $Debug >= $req_dbg + 2;
#end Setup for first run phase.


#///--------------------- First Check if Hands from cmd line switches -W and -E -------------------///
#                         if no hands spec'd on cmd line (ew_sw == 0), then GOTO read deals from a file (GIB or DOP style)
# We now convert the entered hands to a standard form. Upper Case, 10 -> T etc.  We don't count cards yet.

   $ew_sw = &Check_raw_hand_strings (\%Hand_W, \%Hand_E) ; # returns 0,1,2,3 depending on which hands not NULL
   # if no hand entered on cmdline, assume user has redirected stdin, or is using some sort of deal file.
   print STDERR "Main::at CMDLINE Check Raw Hand strings returns: [$ew_sw] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
   if ($ew_sw == 0 ) { goto STDIN_LOOP; }  #  goto saves having all the code under nested ifs.
{
CMDLINE:                   # Check_raw_hand_strings found hand(s) on cmd line. Hand is in $Hand_W/E{STR}

      $req_dbg = 2 ;                         # might need some debuggin here.

   # check the hand strings; break into suits. set a few fields in the Deal hash; set the STR key in the suit hashes
   # When parse_side returns there is always a valid West hand if possible. East not always.

      ($valid_w, $valid_e) = &parse_side( \%Deal, \%Hand_W, \%Hand_E) ; # Update the hashes, incl suit hashes. clean DDS hands
      if (!valid_w  ) {
         print STDERR "Shouldn't happen. Parse side returns invalid West hand. Aborting run. \n";
         exit -100 ;
      }
# Beginning Evaluations Section
      print STDERR "\nMain::CMDLINE:: valid_w=$valid_w, valid_e=$valid_e -  <---- Proceeding with Hand Evaluation ----> {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;

     # There will always be a Hand_W at this point.
     # Proceed to evaluate the hand(s) entered on cmd line. Fill in the DOP values that depend only on one hand.

      $rc = &eval_hand ( \%Hand_W ); # calculate overall hand DOP, Deductions for 4333, no_A etc. also total HLD pts.
      # &show_DOP_hash ( "Eval Hand Results", $hnd_ref );

      if ($valid_he ) {             #eval and show East hand
         $hnd_ref = \%Hand_E;
         $rc = &eval_hand ( $hnd_ref ); # calculate overall hand DOP, Deductions for 4333, no_A etc. also total HLD pts.
       } # end if valid_he
   print STDERR "Main::CMDLINE:: HAND EVALS DONE. BEGIN DEAL EVAL {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg ;

   # We have done: 1) individual suits, 2) individual hands all suits.
   # Now we need to calculate the DOP for the two hands together aka Deal. Fits, Wastage, Mirror etc.
      if ($valid_he && valid_hw ) {
         &eval_deal(\%Deal, \%Hand_W, \%Hand_E );  #fill in everything in all the hashes.
         &show_DOP_hash ( "Main:: CMDLINE Eval DEAL Results {$Debug gt $req_dbg + 1}", \%Deal) if $Debug > $req_dbg + 1;
      } # end eval deal for two hands.

      # Version 21 and up we need to create a valid deal hash even if only one hand is valid.
      # This is to simplify the call from dealer when it wants the DOP of either one hand, or the pair of hands.
      elsif ( $valid_hw ) { # only west is valid -- we should never have only East valid here.
         &Hand2Deal(\%Deal, \%Hand_W ) ;
      }  # end setup a deal hash from only West hand.

      # Version 13/up  Add in code to Adjust Optimal Count if the Opps open the bidding
      $aoc_rpt = $get_opts{OPT_A};
      if ($aoc_rpt ne 'N' && $aoc_rpt ne "" ) {
         print STDERR "Main::CMDLINE:: DEAL EVAL DONE Begin AOC EVAL {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg ;
         if ($valid_hw ) {
            &AOC_DOP_adj (\%Hand_W, \%AOC_DOP_W, $get_opts{OPT_l}, $get_opts{OPT_r} ); #Calc AOC adjustments to West's hand
            &show_DOP_hash ( "Main:: CMDLINE Eval AOC WEST {$Debug gt $req_dbg + 1}", \%AOC_DOP_W) if $Debug > $req_dbg + 1;
         }
         if ($valid_he ) {
            &AOC_DOP_adj (\%Hand_E, \%AOC_DOP_E, $get_opts{OPT_l}, $get_opts{OPT_r} ); #Calc AOC adjustments to East's hand
            &show_DOP_hash ( "Main:: CMDLINE Eval AOC EAST {$Debug gt $req_dbg + 1}", \%AOC_DOP_E) if $Debug > $req_dbg + 1;
         }
         &AOC_DOP_deal_adj (\%AOC_DOP_DEAL, \%AOC_DOP_W, \%AOC_DOP_E );  # args: aoc_deal_ref, aoc_whnd_ref, aoc_ehnd_ref
     } # end competition subs.

   # Work is Done. Now report the results.
#end Evaluations
#
# Decide on which reports to output ...
print STDERR "Main::CMDLINE deciding on reports {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg ;
      if ($get_opts{OPT_q} eq 'Y' ) {  #print DOP playing in C, D, H, S, N, L on stdout
         &Terse_Rpt( \%Deal ) ;
         exit 1 ;  # Nothing else to do if -q option chosen
      }
      &DOP_Reports (\%Deal, \%Hand_W, \%Hand_E, $get_opts{OPT_A}, $valid_hw ,  $valid_he ) ;
      if ($rptfile ne "STDOUT" ) { close $rptfile ;}
      if ($infile  ne "STDIN"  ) { close $infile ; }

# end Report Decisions
# finished ew_sw hands from cmd line.
goto ALLDONE ;           # if we do hands on cmd line, we don't do stdin.
} # end CMDLINE code. (This code block may not be a great idea if it affects variable scope... Nice in Geany tho.
 #end CMDLINE code
# ///...................... End of Code for processing cmd line entered hands -----------///


#/// ---------------------- Begin batch processing of hands from a DEALFILE (or STDIN) --------------///
#     cmd line done. ds_west and ds_east may be set. No hands entered on cmd line so read from STDIN
# We come here via a GOTO on line 64 (approx). No hand entered on cmdline, so either a dealfile or stdin redirect.
STDIN_LOOP:
print STDERR "Main::STDIN_LOOP Opt_g=[$get_opts{OPT_g}] {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
if ($get_opts{OPT_g} ne "" && $get_opts{OPT_g} ne "N" ) { goto GIB_DEALS ; }
# There is no GIB processing done here; just DOP processing hence only Deal, Hand_W and Hand_E
# The rptfile and infile Files will have been opened if req'd by the set_FILEIO sub rtn, called earlier.


$req_dbg = $req_dbg + 1;
print STDERR "Main:: STDIN_LOOP starting with Debug=[$Debug] req_dbg=[$req_dbg]\n" if $Debug >= $req_dbg;
$deal_line = "";
$file_specs{RPT_DEAL_HDR} = "Y";                         # print a header line the first time thru.
DEAL_LINE:  while ($deal_line !~ m/EOF/ ) {              # more than one way to get an EOF into deal_line
      # get rid of results of previous input line. Start from init state.
      reInit_DOP_Deal (\%Deal, \%Hand_W, \%Hand_E ) ;

      $file_specs{RPT_DEAL_LVL} = $df_rpt_deal;
      $file_specs{RPT_HAND_LVL} = $df_rpt_hand;
      $w_ds = $get_opts{OPT_w};              #reset the strains reqd to the defaults or cmd line switches.
      $e_ds = $get_opts{OPT_e};              # these can be overridden on a per deal basis.

      $deal_line = get_stdin($infile) ;               # indirect file handle passed for input. STDIN or what was opened.
      chomp $deal_line;                               # $deal_line is either "EOF(STDIN)" or a line of data.
      last DEAL_LINE if $deal_line =~ m/EOF/;         # break out of loop if get_stdin returns EOF

# --------- Parse the DEAL LINE into Fields: West hand, East Hand, Deal Number etc. ---------
# @line_keys = qw( W E w e d O R l r A T )
# Fields returned in order they are defined in @line_keys not order on the input line itself.                                                               # get the W=, E=, w=, e=,  T= n=, O= inputs if any.
#                 ------- CALL PARSE_STDIN --------
# Vers 13: Created hash                      %Deal_Line
      &parse_stdin ($deal_line, \%Deal_Line);  #return results in Deal_Line hash
      &show_DOP_hash("Main Deal Line Keys:", \%Deal_Line) if $Debug >= $req_dbg;
      # the following are to keep compatibility with old code. l=, r=, A= are not done this way. $D{W=} is invalid.
      # but $key = "W="; $D{$key} works fine.
      $h_west =      $Deal_Line{W};
      $h_east =      $Deal_Line{E};
      $w_ds =        $Deal_Line{w};
      $e_ds =        $Deal_Line{e};
      $deal_num =    $Deal_Line{d};
      $deal_opener = $Deal_Line{O};
      $rpt_spec =    $Deal_Line{R};
      $deal_descr =  $Deal_Line{T};
      # Keep the return values for compatibility for the moment.
      # -------------------

      print STDERR "Main:: stdin <West Hand[$h_west] East Hand[$h_east] ds_w=[$w_ds] ds_e=[$e_ds]> \n" if $Debug > $req_dbg;
      print STDERR "Main:: stdin <Opener=[$deal_opener], Num=[$deal_num], [$deal_descr]> { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;
      print STDERR "Main:: stdin A,l,r =<$Deal_Line{A},$Deal_Line{l},$Deal_Line{r}> { $Debug ge $req_dbg } \n" if $Debug > $req_dbg;

#     Process the hands first since we might need to switch them
#     See if we have two hands or one, and if East only then swap to West.
      if     ( ($h_west eq "" ) && ($h_east eq "" ) ) {  #both hands null.
         print STDERR "STDERR ;; Main:: STDIN Line_no $. => Both hands are NULL Nothing To Do skipping to next input_line ... \n";
         next DEAL_LINE ;        # keep reading stdin until eof even if some errors
      } #end both null

      # We need to have a West hand. If only East is defined, copy East to West and null out East.
      elsif  ( ($h_west eq "" ) && ($h_east ne "" ) ) {  # switch E/W. WEST is default. no E w/o W
         $h_west = $h_east;
         $h_east = "";
         if ( $e_ds ne "" )      { $w_ds = $e_ds ; $e_ds = ""; }  #copy east decl strains to west decl strains
         if ( ($deal_opener eq "E") ) { $deal_opener = "W" ;   }  #if east was opener, make West the opener.
         print STDERR "Main:: DEAL_LINE East and West hands switched. Opener is now:[$deal_opener]\n" if $Debug >= $req_dbg;
      } # end switching hands

      # at this point we have a hand for West and maybe for East also.
      # we also have a bunch of fields  defined from the dealline, and the 3 main hashes all initialized.

      # Save deal input in the hash. Hash is reset on every line. Don't touch stuff maybe set by cmdline
      # Deal hash will have some defaults setup. Override these if entered on deal_line.
      if ( ($deal_num + 0) > 0 )  { $Deal{DEAL_NUM} = $deal_num;}
      $Deal{DESCR} = $deal_descr;
      $Deal{DS_W} = setup_ds ("W", $w_ds) ;     # (upper case, valid chars, no commas, or default)
      $Deal{DS_E} = setup_ds ("E", $e_ds) ;     # if input is null string or invalid, return sane defaults.

      ($rpt_deal, $rpt_hand) = &set_rpt_spec($rpt_spec) if $rpt_spec ne "";
      $file_specs{RPT_DEAL_LVL} = $rpt_deal;
      $file_specs{RPT_HAND_LVL} = $rpt_hand;

      $deal_opener           = &setup_opener($deal_opener); # return a letter E or W.
      if ( ($deal_opener eq "W") || ($deal_opener eq "E") ) { $Deal{OPENER}   = $deal_opener ;}
      if ( ($deal_opener eq "E") ) { $Hand_W{OPEN} = "N" ; $Hand_E{OPEN} = "Y" ; } # Affects D vs Dfit pts.
      else                         { $Hand_W{OPEN} = "Y" ; $Hand_E{OPEN} = "N" ; }
      # AOC error Checks.
      # check the  -l and -r options will return a suit the opp opened in or 'N' or Null string.
      $Deal_Line{l} = &aoc_chk_suit($Deal_Line{l});
      $Deal_Line{r} = &aoc_chk_suit($Deal_Line{r});
      print STDERR "Main:: Deal Line AOC error checks l=$Deal_Line{l} r=$Deal_Line{r} {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;
      if ($Deal_Line{l} eq 'N' && $Deal_Line{r} eq 'N' ) { $Deal_Line{A} = 'N' ; }
      else  { $Deal_Line{A} = &aoc_chk_rpt($Deal_Line{A}); }  #only report AOC if there is a valid opposing opener.
      $Hand_W{STR} = $Deal_Line{W};
      $Hand_E{STR} = $Deal_Line{E};
      # continue with at least a $h_west, and maybe a $h_east
      $ew_sw = &Check_raw_hand_strings (\%Hand_W, \%Hand_E) ; #clean STR in W and E. returns 0,1,2,3
      print STDERR "Main::DEAL_LINE ew_sw[$eq_sw] - Clean West Hand[$h_west] Clean East Hand[$h_east] { $Debug gt $req_dbg}\n" if $Debug > $req_dbg;
      next DEAL_LINE if $ew_sw == 0 ;  # both hand strings are null

      ($valid_w, $valid_e) = &parse_side(\%Deal, \%Hand_W, \%Hand_E ) ; # use W{STR} and E{STR} to setup suits{STR} and Deal{STR}
      print STDERR "Main::DEAL_LINE - Done parse_side: valid_W=$valid_hw valid_E=$valid_he {$Debug ge $req_dbg} \n" if $Debug >= $req_dbg;

      if ( !($valid_hw )) {
         print STDERR "Main::STDIN Line_no $. West hand[ $h_west ] is INVALID. Correct and RETRY. Skipping deal... \n";
         next DEAL_LINE;                        # ignore the deal if W is invalid, even if a valid E.
      } #end invalid west hand
      if ( ($h_east ne "") &&  !($valid_he) ) {
         print STDERR "Main::STDIN Line_no $. East hand[ $h_east ] is INVALID. Continuing using West hand only. \n";
         $h_east = "";
         $Hand_E{STR} = "";
      }  #end invalid east hand

      print STDERR "\nMain:: DEAL_LINE<---- Deal, Hand and Suit hashes setup.:: Proceeding with Hand Evaluation ----> {$Debug ge $req_dbg} \n"
               if $Debug >= $req_dbg;
#end parsing and saving arguments.

   # Calculation phase for the Hand(s) that were read from DEALFILE First W, then E (if any)

   #   --------- CALL EVAL_HAND -------- for West and also East if present
      # evaluate West hand; Fill the West hand and suit hashes.
      $hnd_ref = \%Hand_W ;
      &show_suits ($hnd_ref) if $Debug >= $req_dbg;

      $rc = &eval_hand ( $hnd_ref );   # calculate overall hand DOP, Deductions for 4333, no_A etc. also total HLD pts.

      # evaluate East hand; Fill the East hand and suit hashes.
      if ($valid_he) {                 #eval and show east hand if valid_east
         $hnd_ref = \%Hand_E;
         &show_suits ($hnd_ref) if $Debug >= $req_dbg;

         $rc = &eval_hand ( $hnd_ref ); # calculate overall hand DOP, Deductions for 4333, no_A etc. also total HLD pts.
      } #end if valid_he

      # Hand(s) DOP done. Now DOP for the Partnership aka Deal. Fits, Wastage, Mirror etc.

      #------- CALL EVAL_DEAL -------- needs two valid hands
      if ($valid_he && valid_hw ) {
         &eval_deal(\%Deal, \%Hand_W, \%Hand_E);  #fill in everything in all the hashes.
         &show_DOP_hash ( "main:: Deal LINE =>Eval DEAL Prelim Results {$Debug ge $req_dbg +1}", \%Deal) if $Debug >= $req_dbg +1;
      }  # end valid_he && valid_hw

      # Now calc the AOC adjustments for competitive auctions if the user asked for them.
           # Version 13/up  Add in code to Adjust Optimal Count if the Opps open the bidding
      $aoc_rpt = ($Deal_Line{A} ) ? $Deal_Line{A} : 'N';   # $aoc_rpt should be N or S or D (-A[NSD])
      if ($aoc_rpt ne 'N' && $aoc_rpt ne "" ) { #Do AOC adjustment calculations.
         print STDERR "Main:: DEAL EVAL DONE Begin AOC EVAL {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg ;

         if ($valid_hw ) {
            &AOC_DOP_adj (\%Hand_W, \%AOC_DOP_W, $Deal_Line{l}, $Deal_Line{r} ); #Calc AOC adjustments to West's hand
            &show_DOP_hash ( "Main:: DEAL_Line Eval AOC WEST {$Debug gt $req_dbg + 1}", \%AOC_DOP_W) if $Debug > $req_dbg + 1;
         }
         if ($valid_he ) {
            &AOC_DOP_adj (\%Hand_E, \%AOC_DOP_E, $Deal_Line{l}, $Deal_Line{r} ); #Calc AOC adjustments to East's hand
            &show_DOP_hash ( "Main:: CMDLINE Eval AOC EAST {$Debug gt $req_dbg + 1}", \%AOC_DOP_E) if $Debug > $req_dbg + 1;
         }
         &AOC_DOP_deal_adj (\%AOC_DOP_DEAL, \%AOC_DOP_W, \%AOC_DOP_E );  # args: aoc_deal_ref, aoc_whnd_ref, aoc_ehnd_ref
     } # end aoc_adjustment calculations

# END Process the hands for current Deal_Line

 #  Report Results now that Calculations for Deal and (both) hands Done.
    &DOP_Reports (\%Deal, \%Hand_W, \%Hand_E, $aoc_rpt, $valid_hw, $valid_he )

} #end DEAL_LINE while
  #end DEAL_LINE while
  print STDERR " Main:: ~~~~~~~~~~~~~~~~~~~~ Done with $infile ( EOF? )~~~~~~~~~~~~~~~~~\n";
  if ($rptfile ne "STDOUT" ) { close $rptfile ;}
  if ($infile  ne "STDIN"  ) { close $infile ; }
  goto ALLDONE ;  # 0 for std OS success code...

# end of STDIN_LOOP code block ....
# end of program

# we come here if we have a file of DD solved deals (4 hands) and we want to calc the DOP pts for each side
# and see how accurate our DOP to Trix translation is.
GIB_DEALS :

    $deal_line="";
    print STDERR "Main::GIB_DEALS:: Starting GIB_LINE while loop  { $Debug ge $req_dbg }\n" if $Debug >= $req_dbg ;


GIB_LINE:  while ($deal_line !~ m/EOF/ ) {              # more than one way to get an EOF into deal_line
 push @STK, $Debug;
 $Debug = 0;
      # get rid of results of previous input line. Start from init state.
      # RE_INIT THE GIB STUFF; MAY NEED SOME ZERO HASHES?
      reInit_GIB_Results (\%GIB_Deal, \%NS_Results, \%EW_Results, ) ;
      reInit_DOP_Deal (\%Deal,    \%Hand_W, \%Hand_E ) ;
      reInit_DOP_Deal (\%Deal_NS, \%Hand_S, \%Hand_N ) ;
 $Debug = pop @STK;
 #we will be executing this loop thousands of times typically. Better increase req_dbg from rest of mainline
 push @STK, $req_dbg;
 $req_dbg = 5 ;
      #get_stdin skips blanks and comments but does no other parsing other than return EOF at eof(infile)
      $deal_line = get_stdin($infile) ;               # indirect file handle passed for input. STDIN or what was opened.
      chomp $deal_line;                               # $deal_line is either "EOF(STDIN)" or a line of data.
      last GIB_LINE if $deal_line =~ m/EOF/;         # break out of loop if get_stdin returns EOF

# --------- Parse the DEAL LINE into Fields: using gib parsing
      $gib_lineno = sprintf("%06i",$.);
      print STDERR "GIB_LINE:: Gib_lineno[$gib_lineno] {$Debug gt 6 }\n" if $Debug >= 6;
      print STDERR "\tGIBLINE[$deal_line] { $Debug > 7 } \n" if $Debug > 7;

      $gib_deal_id = &parse_gibline(\%GIB_Deal, $deal_line, $gib_lineno) ; #fills in all the fields in %GIB_Deal

      # &show_DOP_hash("MAIN:: Calling GIB2DOP Hand North ", \%Hand_N );
      &GIB2DOP (\%Hand_N, \%Hand_S, \%Hand_W, \%Hand_E, \%GIB_Deal) ; #mostly setup the {STR} keys in the hand hashes.
      # we can now proceed to use the DOP evaluation code first on the N/S hands, then on the E/W hands
      # Once DOP eval done, we can fill in the NS_Results and the EW_Results hashes,
      # and THEN we can write the trix, DOP, suits, etc. to the CSV file spec'd in OPT_o key of %get_opts
# No error checking of hand strings needed since GIB has already solved for these. Go direct to eval_hand() ;
      &set_trick_results (\%NS_Results, "NS", \%Deal_NS, \%Hand_S, \%Hand_N, \%GIB_Deal );
      &set_trick_results (\%EW_Results, "EW", \%Deal,    \%Hand_W, \%Hand_E, \%GIB_Deal );

  # Results in convenient place; Now write to the CSV file for later analysis by spreadsheet etc.
      print STDERR "MAIN::GIBLINE::GIB_CSV_Rpt rowcount=[$CSV_rowcount] { $Debug ge $req_dbg } \n" if $Debug >= $req_dbg ;
      print $rptfile ("$CSV_Col_Hdrs\n") if $CSV_rowcount == 0;
      ($CSV_rowcount, @running_vals) = &Rpt_GIB_CSV($rptfile, \%NS_Results, "NS", $CSV_rowcount) ;
      print STDERR "Main::GIB_LINE::Rpt_CSV_NS: rows=$CSV_rowcount, @running_vals {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;
     # ($CSV_rowcount, @running_vals) = &Rpt_GIB_CSV($rptfile, \%EW_Results, "EW", $CSV_rowcount) ;
     # print STDERR "Rpt_CSV_EW: rows=$CSV_rowcount, @running_vals {$Debug ge $req_dbg}\n" if $Debug >= $req_dbg;

      # done with this deal; get the next one....
   } # end while gib deal_line
   #end of GIB_LINE while
  $req_dbg = pop @STK ;
  print STDERR "GIB deal file: $get_opts{OPT_i} processed; CSV File written to: $get_opts{OPT_o} END of RUN \n" if $Debug > 0;
  if ($rptfile ne "STDOUT" ) { close $rptfile ;}
  if ($infile  ne "STDIN"  ) { close $infile ; }
  goto ALLDONE ;

DEALER_LINE : # come here if the input is from the Dealer program. typically -O -W xx.xxx.xxxx.xxxx but maybe ...xxxxxxxxxxxxx


ALLDONE:
   if ( $Debug > 0 ) {
      if ($file_specs{RPT_FILE_NAME} ne "" ) { print STDERR "Results written to $file_specs{RPT_FILE_NAME} \n"; }
      print STDERR "==================== END OF DOP MAIN : ALLDONE : ========================\n" ;
   }
   exit (0) ; # 0 for std OS success code...

ABORT:
   print STDERR "!!!!!!!!!!!!!!!!!!!!  FATAL ERROR OCCURRED : ABORT : !!!!!!!!!!!!!!!!!!!! \n";
   exit (254) ;

# end MAIN program

