/* JGM -- File dealdebug_subs.c -- Routines for debugging output mostly */
/* Try not to include any functions that are defined elsewhere in dealer
 * so that including this one will not result in conflicts
 */

#include <stdio.h>          /* for perror() */
#include <stdlib.h>         /* for srand48, mrand48 and friends */
#include <string.h>
#include <sys/random.h>    /* for getrandom() */
#include <assert.h>

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"

#include "../include/dealdebug_subs.h"
void dump_curdeal(deal d) { /* ouput deal in easy to read, verbose fmt */
    int  pnum , cardcount;
    char *pname="NESW";
    char *sname="CDHS";
    char *rname="23456789TJQKA";
    int r, s;
    char sid ;
    pnum = 0 ;

    cardcount = 0;
    unsigned int crd ;
    char pn, rn ;
    pn='*'; rn='-';
    /* this is already a debugging routine; no need to use DBGPRT */
    fprintf(stderr, "Debugging Deal in dump_curdeal");
 //    for (pnum = COMPASS_NORTH; pnum <= COMPASS_NORTH; pnum++) {
    for (pnum = COMPASS_NORTH; pnum <= COMPASS_WEST; pnum++) {
        cardcount = 0;
        pn = pname[pnum];
        fprintf(stderr, "\ndump_curdeal Looping for Player number %d : %c ",pnum, pname[pnum]);
        fprintf (stderr,  " Player[%c ] \n", pname[pnum]);
        for (s = SPADES; s >= CLUBS; s--) {  /* s must be signed for this to work */
          assert(CLUBS <= s && s <= SPADES); /* if s is not signed this will not be true */
          sid = sname[s];
          fprintf(stderr, "Suit %2d=[%c] { ", s, sid);
          for (r = ACE; r >= TWO; r--) {     /* r goes from Ace to Deuce. r must be a signed int, not an unsigned */
              assert(TWO <= r && r<= ACE );  /* if r is not signed this will not be true */
              rn = rname[r];
              crd = (unsigned int)MAKECARD (s, r);
              // fprintf(stderr, "%02X => %d:%c ", crd, r, rn);
              if (hascard(d, pnum, crd)) {
                     fprintf(stderr, "%c%c ",sid, rn );
                  cardcount++ ;
              } /* end if hascard */
            } /* end for r = ACE */
            fprintf (stderr, "}\n");
        } /* end for s suit */
        fprintf(stderr, "Done for player%d=%c, found=%d\n", pnum, pn, cardcount);
        assert(cardcount == 13 );  /* must find 13 cards for each player */
  } /* end for player */
  fprintf(stderr, "----------------dump curr deal done ----------------\n");
} /* end dump curr deal */

void hexdeal_show(deal dl ) { /* one line output of the coded cards in hex */
    int i ;
    i = 0;
    fprintf (stderr, " dl=[");
    for (i=0; i<52 ; i++ ) {  /* print hex deal */
        fprintf(stderr, "%02x ", dl[i] );
    } /* end hex deal */
    fprintf(stderr, "/]\n");
} /* end hexdeal_show */

void sr_deal_show(deal dl ) { /* two line output of the cards in SuitRank */
    char rns[] = "23456789TJQKA-";
    char sns[] ="CDHS";
    char rn, sn , sp;
    int s, r , i ;
    i = 0;
    fprintf (stderr," NOCARD=[%02x] Showing Deal using CARD_SUIT and CARD_RANK in sr_deal_show\n",NO_CARD);
    fprintf (stderr,"SR1=[");
   for (i=0; i<52 ; i++ ) {
       if (dl[i] == NO_CARD) { sn='-';rn='x' ; }
       else {
          s=CARD_SUIT(dl[i]); sn=sns[s];
          r=CARD_RANK(dl[i]); rn=rns[r];
       }
       sp = ((i+1)%13 == 0) ? ':' : ' ';
       fprintf(stderr,"%c%c%c", sn,rn,sp );
       if ( 25 == i ) fprintf(stderr, "\n     ");

    }
    fprintf (stderr,"]\n");
} /* end sr_deal_show */

void sr_hand_show(int p, deal dl ) {  /* two line output of the cards in SuitRank */
    char rns[] = "23456789TJQKA";
    char sns[] ="CDHS";
    char rn, sn ;
    int s, r ,  di ;
    di = p*13 ;
    fprintf (stderr," Showing Hand  using CARD_SUIT and CARD_RANK in sr_hand_show\n");
    for (s=0; s<p+1 ; s++ ) {fprintf(stderr, "        "); }
    fprintf (stderr,"SR1=[");
    for (di = p*13 ; di < (p+1)*13 ; di++ ) {
             s=C_SUIT(dl[di]); sn=sns[s];
             r=C_RANK(dl[di]); rn=rns[r];
             fprintf (stderr,"%c%c ", sn,rn );
    }
    printf ("]\n");
} /* end sr_hand_show */
/* These next routines to display the structures that yyparse builds. Helps understand how Dealer works. */
    int var_lev = 0;  /* file level global for the showvar_tree routines */
void showtreenode(int tlev, struct tree *tr) {
    int prt_cnt ;
    int lf1, lf2 ;
    fprintf(stderr, ":%3d: ",tlev);
    for (prt_cnt = 1; prt_cnt < tlev; prt_cnt++) { fprintf(stderr, "  ") ; } // indent two spaces per level
    lf1 =  (tr->tr_leaf1 != NULL ) ? tr->tr_leaf1->tr_type : -1 ;
    lf2 =  (tr->tr_leaf2 != NULL ) ? tr->tr_leaf2->tr_type : -1 ;
    fprintf(stderr,"Typ=[%2d], Int1=[%2d],Int2=[%2d],Int3=[%2d], lf1_Typ=[%2d] ptr1=[%p],lf2_typ=[%2d] ptr2=[%p]\n",
        tr->tr_type, tr->tr_int1, tr->tr_int2, tr->tr_int3, lf1,(void *)tr->tr_leaf1, lf2, (void *)tr->tr_leaf2 ) ;
    return ;
} /* end showtreenode() */

void showvartree(struct tree *t) {
    var_lev++ ;
    if (t == NULL ) { fprintf(stderr, "Var tree level %d is NULL \n", var_lev); return ; }
    showtreenode(var_lev, t);
    if (t->tr_leaf1) showvartree(t->tr_leaf1) ;
    if (t->tr_leaf2) showvartree(t->tr_leaf2) ;
    --var_lev;
    return ;
}

void showvarlist(struct var *v ) {
    struct var *v_ptr ;
    int var_expr_type ;
    v_ptr = v ;
    fprintf(stderr, "+----------------- VAR LIST -------------------------------------+\n");
    fprintf(stderr, "[        NAME        ] [Value] [ Pointer Addr ]  [ Next Var Addr ]\n") ;
    if (v == NULL ) {fprintf(stderr, "Var List is NULL \n"); return ; }
    while ( v_ptr != NULL ) {
         var_expr_type =  (v_ptr->v_tree != NULL ) ? v_ptr->v_tree->tr_type : -1 ;
         // var_expr_value = evaltree(v_ptr->v_tree ) ; /* Cant do this before certain amt of setup done */
         fprintf(stderr, "[%-20s] ExprType=[ %3d ] [%p] ->[%p]\n",
                     v_ptr->v_ident, var_expr_type, (void *)v_ptr, (void *)v_ptr->v_next );
         var_lev = 0 ;   /* start at 0 to get one level at least of indent */
         showvartree(v_ptr->v_tree) ;
         // showtreenode(1, v_ptr->v_tree);
         v_ptr=v_ptr->v_next ;
    } /* end while v_ptr */
    return ;
} /* end showvarlist */

void showdecisiontree(struct tree *t) {
    static int tr_lev = -1;
    tr_lev++ ;
    if (t == NULL ) { fprintf(stderr, "decision tree level %d is NULL \n", tr_lev); return ; }
    showtreenode(tr_lev, t);
    if (t->tr_leaf1) showdecisiontree(t->tr_leaf1) ;
    if (t->tr_leaf2) showdecisiontree(t->tr_leaf2) ;
    --tr_lev;
    return ;
} /* end showdecisiontree() */

void showactionexpr( struct action *acp ) {
    // int expr1, expr2, etype ;
    int  tr_type ;
    struct expr *pex ;
    fprintf(stderr, "Showing Action Expression for Type=[%d] and A-Ptr=[%p] and E-Ptr=[%p]\n",
                    acp->ac_type, (void *)acp, (void *)acp->ac_expr1 );
    switch (acp->ac_type) {
      case ACT_PRINTCOMPACT:
      case ACT_PRINTONELINE:
      case ACT_AVERAGE:
      case ACT_FREQUENCY:
            if (acp->ac_expr1) {
              // expr1 = evaltree (acp->ac_expr1); /* cant evaltree if no deals or other setup done */
              fprintf (stderr, "    Expr1_TRT=[%d] Expr1_ptr=[%p]\n",
                                    acp->ac_expr1->tr_type, (void *)acp->ac_expr1 );
            }
           break;      /* end several ACT cases */
      case ACT_PRINTES:
            pex = (struct expr *) acp->ac_expr1;
            while (pex) {
              if (pex->ex_ch) {
                fprintf (stderr, "    Expr1 Str=[%s]\n", pex->ex_ch);
              }
              if (pex->ex_tr) {
                // expr1 = evaltree (pex->ex_tr); /* cant evaltree if no deals or other setup done */
                tr_type = pex->ex_tr->tr_type;
                fprintf (stderr, "    Expr_TRT=[%d] Expr_ptr=[%p]\n", tr_type, (void *)pex );
              }
              pex = pex->next;
            } /* end while */
            break;      /* end ACT_PRINTES */
      case ACT_FREQUENCY2D:
            if (acp->ac_expr1) {
              // expr1 = evaltree (pex->ex_tr); /* cant evaltree if no deals or other setup done */
              fprintf (stderr, "    Expr1_TRT=[%d] Expr1_ptr=[%p]\n",
                                     acp->ac_expr1->tr_type, (void *)acp->ac_expr1 );
            }
            if (acp->ac_expr1) {
              // expr2 = evaltree (acp->ac_expr2); /* cant evaltree if no deals or other setup done */
              fprintf (stderr, "    Expr2_TRT=[%d] Expr2_ptr=[%p]\n",
                                    acp->ac_expr2->tr_type, (void *)acp->ac_expr2 );
            }
            break;    /* end ACT_FREQENCY2D */
    } /* end switch acp->ac_type */
    return ;
} /* end showactionexpr() */

void showactionlist(struct action *a ) {
    struct action *a_ptr ;
    a_ptr = a ;
    int ac_tr1_type = -9 ;
    int ac_tr2_type = -8 ;
    char *ac_str10 = "Not there";
    char *ac_ch1 ;
    fprintf(stderr, "+--------- ACTION LIST ---------+\n[Type] [Exp1-Type] [Exp2-Type] [Int1] [String] \n" ) ;
    if (a == NULL ) {fprintf(stderr, "Action List is NULL \n"); return ; }
    while ( a_ptr != NULL ) {
         ac_tr1_type = (a_ptr->ac_expr1 != NULL ) ? a_ptr->ac_expr1->tr_type : -1 ;
         ac_tr2_type = (a_ptr->ac_expr2 != NULL ) ? a_ptr->ac_expr2->tr_type : -2 ;
         ac_ch1 = (a_ptr->ac_str1 != NULL ) ? a_ptr->ac_str1 : ac_str10 ;
         fprintf(stderr, "[%4d] [%4d]      [%4d]      [%4d] [%s], ptr=[%p]\n",
                        a_ptr->ac_type, ac_tr1_type, ac_tr2_type, a_ptr->ac_int1, ac_ch1, (void *)a_ptr ) ;
          if (a_ptr->ac_expr1 || a_ptr->ac_expr2) showactionexpr(a_ptr) ;
          a_ptr = a_ptr->ac_next ;
    } /* end while */
    return ;
    // I need to modify the above like for showvarlist to call showactiontree, which will call showtreennode
    // if the ac_expr1 and/or ac_expr2 are not null

} /* end showactionlist */

void showdistrbits ( int ***distrbitmaps[14] ) {
   int c,d,h,s ;
   int dist_val ;
   fprintf(stderr, "Max Shapeno =[%d], Max Shapebit=[%08X] \n", (shapeno-1), (1<<(shapeno-1) ) ) ;
   for (c = 0 ; c <=13 ; c++ ) {
      for (d = 0 ; d <= 13 - c ; d++ ) {
         for ( h = 0 ; h <= 13 - d - c ; h++ ) {
            s = 13 -h - d - c ;
            dist_val = distrbitmaps[c][d][h][s] ;
            if ( dist_val != 0 ) {
               fprintf(stderr, "Shape[%x%x%x%x] => %04x \n",s,h,d,c,dist_val ) ;
            }
         } /* end for h */
      } /* end for d */
   } /* end for c */
   return ;
}  /* end show distrbitmaps */

void showAltCounts( void ) {
   int tbl,r ;
   fprintf(stderr, "Points Table, a copy of tblPointcount[idxHcp] table \n Pts:  ");
   for (r=0; r<13 ; r++ ) {
      fprintf(stderr, "%4d ", points[r] ) ;
   }
   fprintf(stderr, "\n");
   fprintf(stderr, "tblPointcount Tables in order Ten, Jack, Queen, King, Ace, Top2, Top3, Top4, Top5, C13, HCP\n");
   for (tbl=0 ; tbl < idxEnd ; tbl++ ) {
      fprintf(stderr, "Tbl%2d:", tbl );
      for (r=0 ; r < 13 ; r++ ) {
         fprintf(stderr, "%4d ", tblPointcount[tbl][r] );
      }
      fprintf(stderr, "\n");
   }
} /* end ShowAltCounts */

