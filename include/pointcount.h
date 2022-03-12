/* Move this file to dealtypes.h */

#ifndef POINTCOUNT_H
#define POINTCOUNT_H
/* Indexes into the pointcount array */

/* JGM has redone this so that when the user enters "altcount n" it is "pt n" that is redefined */
enum idxPointcount {
    idxTens = 0 ,       /* pt0 is Tens */
    idxJacks ,
    idxQueens ,
    idxKings ,
    idxAces ,           /* pt4 is Aces */
    idxTop2 ,
    idxTop3 ,
    idxWinners = idxTop3 ,
    idxTop4 ,
    idxTop5 ,
    idxC13  ,           /* pt9 is C13 pts */
    idxHcp  ,           /* Last one since it has its own keyword, and does not need numeric tag */
    idxEnd
} ;
enum idxCardAttrRO {
      idxControls = 0,
      idxLTCwts,
      idxKleinman,
      idxEndRO
} ;
/* the pointcount array itself */
extern int tblPointcount[idxEnd][13] ;
extern int CardAttr_RO[idxEndRO][13] ;

#endif /* POINTCOUNT_H */
