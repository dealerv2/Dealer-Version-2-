/* File deal_dds.h -- to export dealdds_subs IF to Dealer code */
#ifndef DEAL_DDS_H
#define DEAL_DDS_H
struct Dealer_Results_st {      // Struct passed back from IF to Dealer
    int CacheStatus ;  // 0 = stale, 1=update mode 1 2= update mode 2
    int parScore_NS ;
    // int parScore_NS[4] ;     // One for each Vulnerability in case user asks.
    int tricks[4][5] ;          // [hands=n,e,s,w][strains = c,d,h,s,n]
    int errcode ; // 1 = ok, -1 = failure
    char ddsmsg[40];  // in case DDS fails get some of the errmsg. Also used for contract strings
} ;
typedef struct Dealer_Results_st DDSRES_k ;
struct Dealer_deal_st { unsigned int d[52] ; }; // Maybe no longer needed; we just pass in the curdeal ... */
typedef struct Dealer_deal_st Dealer_deal_k;
extern DDSRES_k dds_res_bin;
extern DDSRES_k dds_res;
extern int dds_mode;
extern int dds_tricks(int compass, int strain );
extern int dds_parscore(int compass );


#endif
