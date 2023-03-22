#!/usr/bin/perl

# File: fmt_distribution.pl minor mods by JGM from the original HvS script convert.pl

# This script reformats the output of Descr.distribution_chk into a nice
# table, with the theoretical values next to the hcp and suit distributions
# for the 4 players.
#
# i.e. dealerv2 Descr.distribution_chk  | ./fmt_distribution.pl  >Output.distribution_chk
# or to see the results on the terminal
#      dealerv2 Descr.distribution_chk >Output.distribution_chk ; ./fmt_distribution.pl <Output.distribution_chk

# This is not the best piece of Perl that I've ever written. (from HvS)

#
# Frequency of suit lengths and hcp help by players.  From Frederic H. Frost,
# "Bridge Odds Complete".
#
@suitlen = (1.27, 8.01, 20.59, 28.63, 23.87, 12.47, 4.16, 0.88, 0.12, 0.01, 0,0,0);
@rawhcp = (0.36, 0.79, 1.35, 2.46, 3.85, 5.19, 6.55, 8.03, 8.89, 9.36, 9.41, 8.94, 8.03, 6.91, 5.69, 4.42, 3.31, 2.36, 1.61, 1.04, 0.64, 0.38, 0.21, 0.11, 0.06, 0.03, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

$player_to_int{"N"} = 0;
$player_to_int{"E"} = 1;
$player_to_int{"S"} = 2;
$player_to_int{"W"} = 3;
$suit_to_int{"Spades"}   = 0;
$suit_to_int{"Hearts"}   = 1;
$suit_to_int{"Diamonds"} = 2;
$suit_to_int{"Clubs"}    = 3;
$suit_to_int{"S"} = 0;
$suit_to_int{"H"} = 1;
$suit_to_int{"D"} = 2;
$suit_to_int{"C"} = 3;
$rank_to_int{"T"} = 10;
$rank_to_int{"J"} = 11;
$rank_to_int{"Q"} = 12;
$rank_to_int{"K"} = 13;
$rank_to_int{"A"} = 14;


while (<STDIN>) {
   chop;
   chop;
   if ($_ =~ "HCP") {
      ($dummy, $dummy, $player) = split (/\s+/, $_);
      $player  = $player_to_int{$player};
      for ($i=0; $i<38; $i++) {
         $_ = <STDIN>;
         ($dummy, $dummy, $freq) = (split /\s+/,  $_);
         $hcp[$i][$player] = $freq;
      }
   } elsif ($_ =~ "Frequency Card") {
      ($dummy, $dumy, $player, $card) = (split /\s+/, $_);
      $player = $player_to_int{$player};
      $suit   = $suit_to_int  {substr($card, 0, 1)};
      $rank = substr($card, 1, 1);
      if (defined $rank_to_int{$rank}) {
         $rank = $rank_to_int{$rank};
      }
      $_ = <STDIN>;
      $_ = <STDIN>;
      ($dummy, $dummy, $count) = split (/\s+/, $_);
      $hascard[$player][$suit][$rank] = $count;
   } elsif ($_ =~ "Frequency") {
      ($dummy, $suit, $player) = split (/\s+/, $_);
      $player  = $player_to_int{$player};
      $suit    = $suit_to_int{$suit};
      for ($i=0 ; $i<13; $i++) {
         $_ = <STDIN>;
         ($dummy, $dummy, $freq) = split (/\s+/, $_);
         $suit[$i][$player][$suit] = $freq;
      }
   } elsif ($_ =~ "Generated") {
      ($dummy, $constant, $dummy) = split (/\s+/, $_);
   }
}

print "Generated $constant hands\n\n";
$constant /= 100;

print "HCP Distribution\n================\n";
print " #     N           E           S           W\n";
print "       %  diff     %  diff     %  diff     %  diff\n";

for ($i=0 ; $i<38 ; $i++) {
  printf ("%2d ", $i);
  for ($j=0 ; $j<4; $j++) {
     printf ("%5.2f ", $hcp[$i][$j]/$constant);
     printf ("%5.2f ", $hcp[$i][$j]/$constant - $rawhcp[$i]);
  }
  printf ("%5.2f", $rawhcp[$i]);
  print "\n";
}

foreach $k ("Spades", "Hearts", "Diamonds", "Clubs") {
  print "\n$k Distribution\n=====================\n";
  print " #     N            E            S            W\n";
  print "       %   diff     %   diff     %   diff     %   diff\n";
  $k1 = $suit_to_int{$k};
  for ($i=0 ; $i<14 ; $i++) {
    printf ("%2d ", $i);
    for ($j=0 ; $j<4; $j++) {
       printf ("%5.2f ", $suit[$i][$j][$k1]/$constant);
       printf ("%6.3f ", $suit[$i][$j][$k1]/$constant-$suitlen[$i]);
    }
    printf ("%5.2f", $suitlen[$i]);
    print "\n";
  }
}

print "\nIndividual Cards\n================\n";

print "  Spades                  Hearts                  Diamonds                Clubs\n";
print "#     N     E     S     W     N     E     S     W     N     E     S     W     N     E     S     W\n";

for ($rank = 2 ; $rank < 15 ; $rank++) {

   printf ("%s ", substr("23456789TJQKA", $rank-2, 1));
   for ($suit = 0 ; $suit < 4; $suit++) {

      for ($player = 0 ; $player < 4 ; $player++) {
         printf ("%5.2f ", $hascard [$player][$suit][$rank]/$constant-25.00);
      }
   }

   print "\n";
}
