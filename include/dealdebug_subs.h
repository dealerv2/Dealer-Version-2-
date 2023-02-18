#ifndef DEALDBG_SUBS_H
#define DEALDBG_SUBS_H
extern void dump_curdeal(deal dl);
extern void hexdeal_show(deal dlx, int sz );
extern void sr_deal_show(deal dlx );
extern void sr_hand_show(int h, deal dlx) ;
extern void show_hands_pbn( int mask, deal d ) ;
extern void showtreenode(int tlev, struct tree *tr);
extern void showvartree(struct tree *t) ;
extern void showvarlist(struct var *v ) ;
extern void showdecisiontree(struct tree *t);
extern void showactionexpr( struct action *acp ) ;
extern void showactionlist(struct action *a );
extern void showdistrbits(int ***distrbitmaps[14] ) ;
extern void showAltCounts( void ) ;
#endif

