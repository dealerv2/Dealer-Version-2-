# No shebang since this file brought in by require.
## Author      Version  Date           Comment
#  JGM         1.0      2020/10/22     Subroutines for DOP : Parent Require File.
#  JGM         9.0      2020/10/22     Created so we can have just one require statement in most cases.
#  JGM        10.2      2020/11/04     updated to version with FILEHANDLES  and fixed parser.
#  JGM        13.0      2021/01/22     Updated to version 13, for Mods from Competition Book.
#  JGM        21.0      2021/08/29     Updated to vers 21 for terse report.
#  JGM        21.1      2022/01/10     Eliminated lib dir. Moved to BridgeSW/DOPDealer and copy to /usr/local/bin/DOP
package main;
 #use Smart::Comments  ;            # '###', '####', '#####'  3 is high level, 5 is verbose
 # ----------------------------------------------------------------------
 our $VERSION = "V21.1";
 require  "DOP_Var_Defns_V21.pl";
 require  "DOP_Util_Subs_V21.pl";
 require  "DOP_Subs_V21.pl";
 require  "DOP_Subs2_V21.pl";
 require  "DOP_Calculation_Subs_V21.pl";
 require  "DOP_Competition_Subs_V21.pl";
 # -----------------------------------------------------------------------
 our $Require_OK = 6 ;
 1 ;

