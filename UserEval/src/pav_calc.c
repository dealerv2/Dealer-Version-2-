/* Redo 2023-04-02 because Dblton A's and Dblton Kx not valued correctly.
 * Now using ss_values[][][] array. This eliminates need for adjustments after the fact
 */
int pav_calc ( int side ) {  /* Tag Number: 10 */
   float_t f_pav_Dpts_suit[2][4] = { {0.0}, {0.0} };
   float_t f_pav_NTpts_suit[2][4]= { {0.0}, {0.0} };
   float_t f_pav_Dpts[2] = {0.0} ;
   float_t f_pav_NTpts[2]= {0.0};

   int pav_NTpts[2]     = {0}; /* Hand value if played in NT */
   int pav_Dpts[2]      = {0}; /* Hand value if played in a suit */
   int  hcp_suit[2][4]  = { {0},{0} }; /* suit value if played in NT */
   int  dpts_suit[2][4] = { {0},{0} }; /* suit value if played in a suit */
   int h, s , body_cnt;
   int h_NTdummy = 1 ;              /* will be set by LptsPAV function */
   int tmp_pts;
   int slen ;
   HANDSTAT_k *p_hs, *phs_dummy, *phs_decl ;

   prolog( side ) ;  /* zero globals, set the two handstat pointers, the two seats, and the pointer to the usereval results area */
   JGMDPRT( 7 , "++++++++++ pav_calc prolog done for side= %d; compass[0]=%c, compass[1]=%c, phs[0]=%p, phs[1]=%p, hcp[0]=%d, hcp[1]=%d\n",
               side, compass[0],compass[1],(void *)phs[0], (void *)phs[1], phs[0]->hs_totalpoints, phs[1]->hs_totalpoints ) ;
   JGMDPRT( 9 ,"Check Globals Init fhcp_adj[0]=%g, pav_pts[0]=%d, fhcp_suit_adj[1][3]=%g \n",
                                   fhcp_adj[0],pav_pts[0],fhcp_suit_adj[1][3] );
   for (h = 0 ; h < 2 ; h++) {         /* for each hand */

      p_hs = phs[h] ; /* phs array set by prolog to point to hs[north] and hs[south] OR to hs[east] and hs[west] */
      for (s = CLUBS ; s<= SPADES ; s++ ) {
         slen = p_hs->hs_length[s] ;
         if (0 == slen ) {
            dpts_suit[h][s] = 3 ; f_pav_Dpts_suit[h][s] = 3.0 ;
            hcp_suit[h][s]  = 0 ; f_pav_NTpts_suit[h][s]= 0.0 ;
         }
         else if ( slen <= 2 ) { /* lookup the total value of shortness and honors in the 3D array */
            f_pav_Dpts_suit[h][s] = get_ss_value(PAV, p_hs, s, 1 ) ;
            f_pav_NTpts_suit[h][s]= get_ss_value(PAV, p_hs, s, 0 ) ;
            dpts_suit[h][s] = roundf(f_pav_Dpts_suit[h][s] );
            hcp_suit[h][s] =  roundf(f_pav_NTpts_suit[h][s]);
         }
         else { /* no shortness pts; straight HCP */
            dpts_suit[h][s] = 0 ;
            hcp_suit[h][s]  = p_hs->points[s] ;
            f_pav_Dpts_suit[h][s] = 0.0 ;
            f_pav_NTpts_suit[h][s]= p_hs->points[s] ;
         }
         pav_NTpts[h]  += hcp_suit[h][s] ;
         f_pav_NTpts[h]+= f_pav_NTpts_suit[h][s] ;
         pav_Dpts[h]   += dpts_suit[h][s] ;
         f_pav_Dpts[h] +=  f_pav_Dpts_suit[h][s] ;

         JGMDPRT(8,"AdjPAV, Hand=%d, suit=%d, SuitLen=%d, hcp[h][s]=%d, dpts_suit[h][s]=%d, f_pav_Dpts_suit=%g \n",
                     h, s, slen, hcp_suit[h][s], dpts_suit[h][s], f_pav_Dpts_suit[h][s] );
      } /* end for s = CLUBS to Spades*/

      body_cnt = p_hs->hs_totalcounts[Aces] + p_hs->hs_totalcounts[Tens] ;
      body_pts[h] = body_cnt / 4 ; /* +1 when  4<= A+T <=7 and +2 if 8 == A+T */
      /* In NT PAV does not count Dpts; so we just need the NTpts + body */
      pav_NTpts[h] += body_pts[h]   ; /* NT points for hand h -- dont count Dpts for NT */
      JGMDPRT(7,"PAV NT Result: Hand=%d, NT_pts=%d,  body_pts[h]=%d, body_cnt=%d \n",
                     h,  pav_NTpts[h], body_pts[h],  body_cnt );
      UEv.nt_pts_seat[h] = pav_NTpts[h] ;
    /* Set the value for a suit contract */
      pav_Dpts[h] += body_pts[h];
      JGMDPRT(7,"PAV Suit Prelim Val[%d]=%d pav_Dpts[h]=%d, body_pts=%d, body_cnt=%d\n",
                     h,  pav_Dpts[h], body_pts[h],  body_cnt );
   } /* end for each hand */
   /* add +1 for dummy's 5+ suit in NT */
