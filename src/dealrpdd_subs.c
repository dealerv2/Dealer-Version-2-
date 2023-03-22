/* File dealrpdd_subs.c -- Code for LIB mode usign rpdd.zrd files *
 * Date        Version     Author      Comment
 * 2023/03/11  1.0.0       JGM         Adapted from show_rpdd.c
 */
#define GNU_SOURCE
// #define JGMDBG 1              /* so the dbgprt_macros.h works ok */

#include "../include/std_hdrs.h"

#include "../include/dealdefs.h"
#include "../include/dealtypes.h"
#include "../include/dealexterns.h"
#include "../include/dealprotos.h"
#include "../include/dbgprt_macros.h"    /* JGMDPRT other DBGLOC etc. */
#include "../include/dbgprt_macros.h"    /* JGMDPRT other DBGLOC etc. */
         /* --- types ---*/
typedef unsigned char BYTE_k;
typedef char DEAL_k[52];         /* char instead of unsigned char; easier compatibility and cards never use hi-bit anyway*/
struct rpdd_st {
     BYTE_k card[13] ;           /* being inside a struct these names have their own namespace do not conflict with others*/
     BYTE_k trick[10];
} ;
union zsep_ut {
   BYTE_k zerobyte[4];
   unsigned int izero;
} ;

enum rp_seat_ek {KIBTZ=-1, WEST=0, NORTH, EAST, SOUTH } ;         /* RP numbering */
enum card_rank_ek {spot=-1, Two_rk=0, Three_rk, Four_rk, Five_rk, Six_rk, Seven_rk, Eight_rk, Nine_rk, Ten_rk, Jack_rk, Queen_rk, King_rk, Ace_rk };

         /* --- Defines ---*/

         /* --- Globals ---*/
int ddres[4][5] = {0} ; /* results per seat/strain -- dealer numbering North=0, West=3;  Clubs=0, NT=4*/
union zsep_ut null_sep;
struct rpdd_st rp_rec;
         /* --- Protos  ---*/
void sr_deal_show( char *dl  ) ;

         /* --- Subroutines  ---*/
void hex_show_rprec(int recnum, struct rpdd_st *rprec ) {
   int k ;
   fprintf(stderr,"RPREC[%8d] ", recnum ) ;
   for (k=0; k<13; k++ ) {
      fprintf(stderr, "%02x ",rprec->card[k] ) ;
   }
   fprintf(stderr, " :: " ) ;
   for (k=0; k<10; k++ ) {
      fprintf(stderr, "%02x ",rprec->trick[k] ) ;
   }
   fprintf(stderr, "\n");
} /* end hex_show_rprec */

int set_rpdd_lib_vars(FILE *rpdd_file, struct options_st *opts) {
   long int fpos = 0 ;
   int rc ;
   int lib_recs ;
   rc = fseek(rpdd_file, 0 , SEEK_END) ;
   if (rc == 0 ) fpos = ftell(rpdd_file) ;
   lib_recs = fpos / RPDD_REC_SIZE ;
   if (lib_recs < 1000) {             /* Testing Library */
      rplib_blk_sz = 10 ;    /* */
      rplib_max_seed = lib_recs / rplib_blk_sz - 1 ;
   }
   else if (lib_recs < 10000) {  /* partial library */
      rplib_blk_sz = 100 ;
      rplib_max_seed = lib_recs / rplib_blk_sz - 1 ;
   }
   else {                        /* Big or full size Library */
      rplib_blk_sz = RP_BLOCKSIZE ;
      rplib_max_seed = (MAX_RP_SEED < lib_recs/RP_BLOCKSIZE ) ? MAX_RP_SEED : lib_recs/RP_BLOCKSIZE - 1 ;
   }


   rc = fseek(rpdd_file, 0 , SEEK_SET) ; /* set back to beginning or we get immediate EOF on first read */
   JGMDPRT(3,"RPLIB Globals. lib_recs=%d, rplib_blk_sz=%d, rplib_max_seed=%d, Fsize=%ld, libfile=%s\n",
                           lib_recs,rplib_blk_sz,rplib_max_seed,fpos, opts->rplib_fname );
   return lib_recs;
}

long int seek_rpdd_pos(FILE *rpdd_file, long int seed ) {
   /* 'seed' var in this case is the number of 1000 record blocks to skip from the start of the file.
    * as the biggest the file can be is 10,485,760 deals, seed should be 0 <= seed <= 10,485
    */
   long int myseed = seed ;
   if (seed < 0 || seed > rplib_max_seed ) {
      fprintf(stderr, "Seed=%ld must be between zero and %d when using this Library file. Continuing from record zero. \n" ,
            seed, rplib_max_seed ) ;
      myseed = 0 ;
   }
   int rc ;
   long int rpdd_pos ;
   rpdd_pos = myseed * rplib_blk_sz * RPDD_REC_SIZE ;
   rc = fseek(rpdd_file, rpdd_pos, SEEK_SET ) ;
   rp_recnum = myseed * rplib_blk_sz +1; /* track the actual record number in case of errors */
   JGMDPRT(2,"SEEK_rpdd: Seed=%ld,recnum = %d, rpdd_pos=%ld, fseek_rc=%d\n",seed,rp_recnum,rpdd_pos,rc) ;
   if (rc != 0 ) {
      perror("Seek to pos calculated from seed on rpdd.zrd file FAILED! Reset-ing to start of file (pos=0)\n");
      fprintf(stderr, "Orig-Seed=%ld My-Seed=%ld, calculated position=%ld, calc_blk_sz=%d, rplib_recs=%d \n",
               seed, myseed, rpdd_pos, rplib_blk_sz, rplib_recs ) ;
      rpdd_pos = 0 ;
      fseek(rpdd_file, rpdd_pos, SEEK_SET) ;
      rp_recnum = 0 ;
   }

   return rpdd_pos ;
} /* end seek_rpdd_pos */


int rp_read_err(int byte_cnt, int rsize, int recnum ) {
      int m = 13 ;
      if (byte_cnt == rsize || 0 == byte_cnt) { return 0 ;} // Zero at EOF ; 23 otherwise OK. No error.
      fprintf(stderr, "Error! %d Bytes READ at rp_recnum=%d: Cards Read = ",byte_cnt, recnum);
      if (byte_cnt < 13) {
         for (int x = 0 ; x < byte_cnt ; x++ ) {fprintf(stderr, "%02x ", rp_rec.card[x] ); }
         fprintf(stderr, " \n");
      }
      else {
         for (int x = 0 ; x < 13 ; x++ ) {fprintf(stderr, "%02x ", rp_rec.card[x] ); }
         fprintf(stderr, " :: " ) ;
          m = byte_cnt - 13 ;
          for (int x = 0 ; x < m ; x++ ) {fprintf(stderr, "%02x ", rp_rec.trick[x] ); }
         fprintf(stderr, " \n");
      }
      return -2  ;
} /* end rp_read_err */

/* read a binary record from the rpdd.zrd database Wrap around if not enough records obtained */
int get_rprec (FILE *rpfile, struct rpdd_st *rp_rec, struct options_st *opts ) {
   int byte_cnt;
   int read_err = 0 ;

   byte_cnt = fread(rp_rec, 1, sizeof(struct rpdd_st), rp_file) ; /* read 23 bytes into rprec */
   JGMDPRT(7,"get_rprec fread byte_cnt=%d ngen=%d\n",byte_cnt, ngen );
   DBGDO(7,hex_show_rprec(rp_recnum, rp_rec) ) ;
   read_err = rp_read_err(byte_cnt, 23, rp_recnum) ;
   if (read_err != 0 ) {
      fprintf(stderr, "RP READ_Err returns: %d for byte_cnt=%d at recnum=%d Returning to main with -2 \n",
                                          read_err,      byte_cnt,  rp_recnum);
      return -2 ;
   }
   // byte_cnt = fread(rprec, struct rpdd_st, 1, rpfile) ; /* read one struct rpdd_st into rprec */
   if( feof(rpfile) && rp_cnt < opts->max_produce) { /* Reached eof before necessary records got */
      fseek(rpfile, 0, SEEK_SET ) ;             /* rewind to beginning */
      wrap_cnt++;
      byte_cnt = fread(rp_rec, 1, sizeof(struct rpdd_st), rpfile) ;
      read_err = rp_read_err(byte_cnt, 23, rp_recnum) ;
      if (read_err != 0 ) {
         fprintf(stderr, "RP READ_Err returns: %d for byte_cnt=%d at recnum=%d Returning to main with -2 \n",
                                          read_err,      byte_cnt,  rp_recnum);
         return -2 ;
      }
      rp_recnum = 1 ;                              /* recnum tracks errors; want to know actual one in the file */
      fprintf(stderr, "Wrap Around # %d occurred at rp_cnt=%d, rp_recnum=%d ngen=%d\n",wrap_cnt,rp_cnt,rp_recnum,ngen);
   }
   JGMDPRT(8, "Get_rp_rec returns to get_rpdeal byte_cnt = %d. rp_rec[%02x %02x %02x %02x] \n",
               byte_cnt, rp_rec->card[0],rp_rec->card[1],rp_rec->card[2],rp_rec->card[3]);
   return byte_cnt ;  /* zero at eof; -ve if error ; 23 all other times except maybe separator record.... */
} /* end get_rprec */

int get_rpdeal(struct options_st *opts, char *dl ) { /* outputs to dl and the global dds_res_bin[4][5] */
   struct rpdd_st rp_rec ;
   union zsep_ut null_sep;
   int ddsres[4][5] ;
   int crdpos, tpos;
   short int tval ;
   int npos, epos, spos, wpos ;
   int pnum,j,k,strain,dealer_pnum;
   int rank, suit ;
   char rp_deal[52] ;
   char kard ;
   BYTE_k px4, tx2 ;
   int byte_cnt = 0 ;

   /* If 1st 16 'cards' are zero it means West has them. Impossible.
    * So RP says it is some sort of separator record.
    */
   null_sep.izero = 0 ;
   while (null_sep.izero == 0 ) {
      byte_cnt =  get_rprec(rp_file, &rp_rec, opts);
      if (byte_cnt < 0 ) {fprintf(stderr, "get_rprec suffered an error = %d. Bypassing this record \n",byte_cnt ) ; }
      rp_recnum++;  /* global */
      /* check if a separation record */
      memcpy(&null_sep.zerobyte, &rp_rec.card[0], 4 ) ;
      if (0 == null_sep.izero  ) {
         fprintf (stderr,"Found a null separator at record number %d rp_rec.card=[%0x, %0x, %0x, %0x ]\n",
                                    rp_recnum, rp_rec.card[0],rp_rec.card[1],rp_rec.card[2],rp_rec.card[3] );
         fprintf (stderr, ".izero=%d, zerobyte=[%0x, %0x, %0x, %0x ]\n",
                     null_sep.izero, null_sep.zerobyte[0],null_sep.zerobyte[1],null_sep.zerobyte[2],null_sep.zerobyte[3] );
      }
   } /* end while */
      rp_cnt++;
   /* An RP dbase record starts with Spade Ace, then Spade King, and so on down to Club Deuce */
      suit = SPADES ;
      rank = Ace_rk ;
      kard = MAKECARD(suit, rank ) ;
      npos = 0; epos=13; spos=26; wpos=39 ; /* The slots in the Deal52 buffer where each hand begins. */

      JGMDPRT(7, "Recnum %d: Card[0]=%02x, card[12]=%02x, trick[0]=%02x, trick[9]=%02x\n",
                  rp_recnum,rp_rec.card[0],rp_rec.card[12],rp_rec.trick[0],rp_rec.trick[9] ) ;
      for (crdpos=0 ; crdpos < 13 ; crdpos++ ) {  /* the whole deal is done in 13 bytes 4 cards each byte*/
         px4 = rp_rec.card[crdpos] ;
         JGMDPRT(9, "Crdpos=%d, cardByte=%02x\n" ,crdpos, px4) ;
         for (j = 0 ; j<4 ; j++ ) {  /* each byte holds 4 cards */
            pnum = (px4 & 0x03 ); /* process the right two bits -- little endian fmt*/
            px4  = px4 >> 2 ;
            kard = MAKECARD(suit, rank ) ; /* set the current card for this crdpos */
            switch (pnum) {                /* put the current card into the designated hand */
               case WEST : rp_deal[wpos++] = kard ; break ;
               case EAST : rp_deal[epos++] = kard ; break ;
               case NORTH: rp_deal[npos++] = kard ; break ;
               case SOUTH: rp_deal[spos++] = kard ; break ;
               default : printf("Cant happen in switch pnum = %d \n", pnum ) ;
            }
           JGMDPRT(8, " Card %c%c at pos %d:%d assigned to %c; seatcounts: N=%d,E=%d,S=%d,W=%d \n",
                     "CDHS"[suit],rank_ids[rank],crdpos,j, "WNES"[pnum],npos,(epos-13),(spos-26),(wpos-39) ) ;
            if (--rank < 0 ) { /* have we done all of current suit?  */
               rank = Ace_rk;    /* then start new suit */
               suit-- ;
            }
         } /* end for J 0 .. 3 */
      } /* end for crdpos 0 .. 12 -- all 52 cards done*/
      memcpy(dl, rp_deal, 52) ;
      DBGDO(8, sr_deal_show( (char *)dl ) );
      strain = SUIT_NT ;
      tpos = 0 ;
      memset(ddsres, 0, sizeof(ddsres) );
      while(tpos < 10 ) { /* Do the tricks:: NT for W,N,E,S, then Spades for W,N,E,S downto Clubs for W,N,E,S*/
         pnum=WEST ;
         for (k = 0 ; k < 2 ; k++ ) {           /* two bytes per strain */
            tx2 = rp_rec.trick[tpos++] ;
            for (j = 0 ; j < 2 ; j++ ) {        /* two players per byte */
               tval = tx2 & 0x0F ;
               dealer_pnum = (pnum+3)&0x00000003 ;  /* convert PAV player to Dealer Player. */
               ddsres[dealer_pnum][strain] = tval ;
               JGMDPRT(9, "TPOS=%d,      strain=%c,  pnum=%d, dealer_pnum=%d, tval=%d, tx2=%02x \n",
                        (tpos-1), "CDHSN"[strain],pnum,   dealer_pnum,     tval,     tx2 );
               tx2 = tx2 >> 4 ;
               pnum++ ;
            } /* end for two players */
         } /* end a set of 4 results for a strain */
         strain-- ;
      } /* end while tpos */
      JGMDPRT(8, " ----- Record %d all done. npos=%d, epos=%d, spos=%d, wpos=%d tpos=%d --------\n",rp_recnum, npos, epos, spos, wpos,tpos ) ;
      /* now use the local results to setup the global results area as if we had solved the board in mode 2 */
      dds_res_bin.errcode = 1 ; /* RETURN_NO_FAULT */
      memcpy(dds_res_bin.tricks, ddsres, sizeof(ddsres) ) ;
      dds_res_bin.parScore_NS = 0 ; /* par score will be calculated if input file calls for it. */
      dds_res_bin.CacheStatus = CACHE_OK ;
      dds_dealnum = ngen ;
      return 1 ;
} /* end get_rp_deal */



