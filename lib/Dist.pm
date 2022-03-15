############################################################
## Dist.pm package, v1.2A, (c) 1997/2022 François DELLACHERIE
##
## Feel free to distribute as long as you keep
##  the names of all contributors to this piece of perl in
##  this header.
## If you encounter bugs, mail me at
##
############################################################
## Provided to JGM in March 2022
# 2022/03/13 JGM Changed Language to English
# 2022/03/13 JGM Changed _to_dealer call and the join statements to allow for minus signs

package Dist;
use strict;
use Carp;

my $DefaultLanguage = 'English';
my $Language = $DefaultLanguage;
my %SuitLanguage = (French => 'pckt', English => 'shdc');
my $Suits = $SuitLanguage{$DefaultLanguage}; # default mode

my %Permut = (1 => [ qw /0/ ],
      2 => [ qw /01 10/ ],
      3 => [ qw /012 021 102 120 201 210/ ],
      4 => [ qw /0123 0132 0213 0231 0312 0321 1023 1032 1230 1203 1320 1302 2013 2031 2130 2103 2301 2310 3012 3021 3120 3102 3201 3210/ ]);

my $joiner_sign;
{ # protection block?
  my @dev_results;
  sub _recursively_develops {
     my $one_suit_length;
     my ($joker,$shape,$count,$depth,$nb_suits,$min,$max,@ranges) = @_;
     $depth || (@dev_results = ()) ;
     for $one_suit_length (split('', $ranges[$depth])){
       my $mycount;
       my $myshape;
       $mycount = $count + ($one_suit_length eq 'x' ? 0 : $one_suit_length);
       next if $mycount > $max;
       $myshape = $shape.$one_suit_length;
       if($depth + 1 == $nb_suits) {
         next unless ($joker || $mycount >= $min);
         push(@dev_results, $myshape);
       }
       else { &_recursively_develops($joker,$myshape,$mycount,$depth+1,$nb_suits,$min,$max,@ranges) }
    } # end for one_suit_length
    @dev_results;  # this gets returned
  } # end recursively develops
}  # end protection block

sub _permute_one_shape { # permute one shape w/ the permutation (01...n)->($permutation)
    my ($shape, $permutation) = @_;
    my $len = length($shape);
    $len == length($permutation) || carp "** Warning **: trying to permute $shape w/ $permutation : wrong length ?";
    my $permuted = ' ' x $len;
    while($len--){substr($permuted, substr($permutation, $len, 1), 1) = substr($shape, $len, 1)}
    return($permuted);
}


sub _permute_shapes { # permute an array of shapes w/ all the possible permutations
    my %shapes;
    my @shapes = @_;
    my $shape;
    for $shape (@shapes) {
      for (@{$Permut{length($shape)}}) {
         $shapes{_permute_one_shape($shape, $_)}++;
      }
    } #end for $shape
    return(sort keys %shapes);  # JGM added Debian Mod to sort
} # end permute shapes


sub _expand_shape { # low-level parsing : no permutations made in this function !
    my $nb_suits = 0;
    my $joker;
    my $max_in_one_suit = 9;  # not considering hands w/ a very very long suit !!
    my @ranges;
    my ($regular_shape, $min, $max) = @_;
    $max && ($max_in_one_suit = ($max < 10 ? $max : 9));
    while($regular_shape ne '') {
      my $range;
      (($_) = $regular_shape =~ /^(\d[-+]*|\[[-\d]+\]|x)/) ||
         confess "Could not parse beginning of $regular_shape";
      $regular_shape = $';
      $range = $_;
      /x/ and $joker = 'yes';        # 'x' is what we call a joker
      s/(\d)\+/[$1-9]/ unless /^\[/; # \d+ means 'at least \d cards in that suit'
      s/(\d)\-/[0-$1]/ unless /^\[/; # \d+ means 'at  most \d cards in that suit'
      s/^\[/\[^/;                    # the garbage that follows in order to develop
      /^\[/ and do {                 #  the perl-like range into a string containing
          $range = '0123456789';     #  all the length-in-one-suit possibilities
          $range =~ s/$_//g;
      }; # end do
      $nb_suits++;
      push(@ranges, $range);
    } # end while regular shape
    return( &_recursively_develops($joker,'',0,0,$nb_suits,$min,$max,@ranges) );
} #end expand_shape

sub _count_shape { # Counting the number of known cards in a shape
    my $shape = shift;
    my $count;
    for(split ('', $shape)) {$count += $_ if /\d/}
    $count > 13 && confess "Rock Bottom !! I got >13 cards here in shape $shape !!"; # should not happen ?
    return $count;
}

sub _reduce { # permute a set of shapes w/ a given permutation, replacing the joker by the
              # known number of cards (e.g: there remains only one joker in a shape),
              # and finally eliminates the duplicatas w/ a jivaro reduction...
    my ($shape_ref, $permutation) = @_;
    my %jivaro;
    my $replace_the_joker_by_;

    for (@$shape_ref){
      my $permuted = '';
      $replace_the_joker_by_ = 13 - _count_shape($_);
      (tr /x// == 1 || ($replace_the_joker_by_ == 0)) && $replace_the_joker_by_ < 10 && (s/x/$replace_the_joker_by_/g);
      $permutation && ($permuted = _permute_one_shape($_, $permutation));
      $jivaro{$permuted || $_}++;
    } #end for shape_ref
    @$shape_ref = sort keys %jivaro; # Debian Mod added by JGM 2022/03/13
}  #end _reduce

sub _reduce_by_ordering_suits {
    my %jivaro;
    for (@_) {
         $jivaro{join '', sort {$b cmp $a} split('', $_)}++;
    }
    return sort keys %jivaro; # Debian Mod added by JGM 2022/03/13
}


sub _extract_permutation_from_shape { # returns the permutation to apply on the results if one
                                      # uses a shape like (example in english...) 5h4c(xx)...
    my $shape = shift;
    my $permutation = '';
    if($shape =~ /[$Suits]/i) {
      $shape =~ s/[^$Suits]//gi;
      $shape =~ lc($shape);
      ($permutation = $Suits) =~ s/[$shape]//gi;
      eval "(\$permutation = \$shape.\$permutation) =~ tr/$Suits/0123/;";
    }
    return $permutation;
} #end sub _extract_permutation_from_shape

sub _parse_shape { # parses a 'regular expression' of a shape, returning 'staveren's-dealer
                   # -compatible' shapes
    my $shape_exp = shift;
    my $shape = $shape_exp;
    my $logical = '';
    my $suit_order_permutation;
    my @results_wo_logical;
    my @results;

    ($shape =~ /:/) && ( ($shape, $logical) = ($shape =~ m/^(.+):(.+)/) );
    $suit_order_permutation = _extract_permutation_from_shape($shape); # treating expressions like 5s4c(31)...
    $logical =~ s/\s*,\s*/ ~ /g; # ',' stands for 'and' in a regshape...replace by ~ for hack-value
    $shape =~ s/[$Suits]//gi; # Removes the (if present) suit permutation signs
    $shape =~ s/\s+//g;       # Remove useless \s

    if($shape =~ m/^\((.*)\)$/) { # Complete permutation on the 4 suits
      my $shape = $1;
      return (map {"any $_"} &_reduce_by_ordering_suits(&_expand_shape($shape, 13, 13)) ) unless ($logical) ;
      @results_wo_logical =  _permute_shapes (&_expand_shape($shape, 13, 13));

    }
    elsif ($shape =~ /^(.+)\((.+)\)$/) { # Partial permutation
      my ($fixed, $float) = ($1, $2);
      my $fx;
      for $fx (&_expand_shape($fixed, 0, 13)) {
         my ($min, $max) = (0, 0);
         $max = 13 - _count_shape($fx);
         $min = $max unless ($fx =~ /x/);
         push(@results_wo_logical, map {$fx.$_} _permute_shapes( &_expand_shape($float, $min, $max) ) ) ;
      } # end for $fx
    }
    elsif ($shape !~ /[()]/) {  # w/o any permutation at all
      @results_wo_logical = &_expand_shape($shape, 13, 13);
    }
    else { # There must be something wrong...
      confess "Couldn't understand this shape : $shape";
    } # end if-elsif-elsif-else chain

    _reduce(\@results_wo_logical, $suit_order_permutation);
    if($logical) {
      @results = ();
      for(@results_wo_logical){
         my $i;
         if( /x/ ){
            for $i(0..3) {
               my $s = substr($Suits, $i, 1);
               $logical =~ /$s/i and substr($_, $i, 1) eq 'x' and substr($_, $i, 1) = '=';
            }
         }
         push(@results, s/=/0+/g ? &_expand_shape($_, 13, 13) : $_);
       } #end for @results_wo_logical
      $logical =~ s/([$Suits])/sprintf "substr(\$_,%d,1)",index($Suits,$1)/gie;
      $logical =~ s/ ~ / and /g;
      return grep {eval "no strict;$logical"} &_reduce(\@results);
    } # end if $logical
    return @results_wo_logical;
}  # end _parse_shape

sub _staveren_expand {
    my $shape_list = shift;
    my $shape;
    my @stav_results;
    my $majors = substr($Suits, 0, 2);
    my $minors = substr($Suits, 2, 2);
    my ($diamond, $club) = split '', $minors;
    my ($spade, $heart) = split '', $majors;

    $shape_list =~ /([^|\#]*m[^|\#]*m[^|\#]*)/ and confess "$1: Multiple <m> not implemented ! (use permutations instead)";
    $shape_list =~ /([^|\#]*[$minors][^|\#:]*m[^|\#]*)/ and confess "$1: Multiple <m> not implemented ! (use permutations instead)";
    $shape_list =~ /([^|\#]*m[^|\#:]*[$minors][^|\#]*)/ and confess "$1: Multiple <m> not implemented ! (use permutations instead)";
    $shape_list =~ /([^|\#]*[$majors][^:|\#]*M[^|\#]*)/ and confess "$1: Multiple <M> not implemented ! (use permutations instead)";
    $shape_list =~ /([^|\#]*M[^|\#:]*[$majors][^|\#]*)/ and confess "$1: Multiple <M> not implemented ! (use permutations instead)";
    $shape_list =~ /([^|\#]*M[^|\#]*M[^|\#]*)/ and confess "$1: Multiple <M> not implemented ! (use permutations instead)";
    $shape_list =~ s/([^|\#]*)m([^|\#]*)/$1$diamond$2|$1$club$2/g;
    $shape_list =~ s/([^|\#]*)M([^|\#]*)/$1$spade$2|$1$heart$2/g;

    for $shape (split /[|\#]+/, $shape_list) {
      push(@stav_results,   &_parse_shape($shape));
    }
    return join $joiner_sign, &_reduce( \@stav_results ); #JGM added joiner sign
} #_staveren_expand

###################################################################################
############################################ Methods ##############################
###################################################################################

sub language {
    my $self = shift;
    ref $self && confess "You can use ->language method ONLY on the whole class !";
    @_ > 1 && confess "Usage: Class->language(LANG)";
    if(@_) {
         my $lang = shift;
         my $suits = $SuitLanguage{$lang} or confess "Language $lang not implemented";
         $Suits = $suits;
         $Language = $lang;
    }
    return $Language;
} #end sub language

sub shape {
    my $self = shift;
    (ref $self && @_ < 2) || confess "Usage: Object->shape(SHAPE)";
    $self->{SHAPE} = shift if @_;
    return $self->{SHAPE};
}

# sub zone not used in Dealer FDP, but in another FD project
sub zone {
    my $self = shift;
    (ref $self && @_ < 2) || confess "Usage: Object->zone(ZONE)";
    $self->{ZONE} = shift if @_;
    return $self->{ZONE};
}



sub to_dealer {
    my $self = shift;
    $joiner_sign = shift ;
    ref $self || confess "Usage: Object->to_dealer()";
    @_ && carp "Ignoring Args:: @_ in to_dealer, no options implemented yet !";
    return &_staveren_expand($self->shape);
}

sub new {
    my $self = shift;
    ref $self && confess "Cannot call method new on object";
    my $dist_ref = {};

    $dist_ref->{SHAPE} = undef;
    $dist_ref->{ZONE} = undef;
    return bless $dist_ref, $self;
}

1;
###########################################
############################ Comments #####
###########################################
#
# This is part of a big bridge-package I'm developping and I cannot release yet.
# It allows to specify complex types of shapes for a bridge hand, and convert them
# into shapes compatible with Hans van Staveren's Dealer program.
#
# Methods:
#  Dist->new                          : creates a Dist-object
#  Dist->language('English'|'French') : sets the language for suits (shdc or pckt)
#  Obj->shape('shape')                : creates a shape
#  Obj->to_dealer                     : produces a list of shape suitable for 'dealer'
#  Obj->zone                          : used for another package
















