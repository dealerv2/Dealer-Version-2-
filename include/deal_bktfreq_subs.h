#ifndef DEAL_BKTFREQ_SUBS_H
#define DEAL_BKTFREQ_SUBS_H

#ifndef DEALTYPES_H
  #include "../include/dealtypes.h"  /* we need the various action struct definitions here. */
#endif
// Globals
extern int *freq2D ;              // ptr to area assigned by calloc for the Freq Counters
extern int *freq1D ;              //   Ditto -- we maybe don't need these as the stuct has the ptr and use these only as temps

// Protos
extern int *alloc_bkts1D(struct bucket_st *bkt1D) ;
extern int  upd_bkt1D( int val, int *freq1D, struct bucket_st *bkt1D ) ;
extern int  show_freq1D (int *freq, char *descr, struct bucket_st *bkt, char direction ) ;
extern int  fill_bkt_names( struct bucket_st *bkt )  ; /* used by both 1D and 2D */
extern int *alloc_bkts2D(struct bucket_st *dwn_bkt, struct bucket_st *acr_bkt ) ;
extern int  find_bkt_id(int val, struct bucket_st *bkt ) ; /* used only in 2D case */
extern int  upd_bkt2D(int val_dwn, int val_acr, int *freq2D, struct bucket_st *dwn_bkt, struct bucket_st *acr_bkt) ;
extern int  show_freq2D(int *freq2D, char *description, struct bucket_st *d_bkt, struct bucket_st *a_bkt ) ;


#endif // end file guard
