#include "chemistry_file.H"

#ifndef AMREX_USE_CUDA
namespace thermo
{
    /* Inverse molecular weights */
    std::vector<double> imw;
    double fwd_A[0], fwd_beta[0], fwd_Ea[0];
    double low_A[0], low_beta[0], low_Ea[0];
    double rev_A[0], rev_beta[0], rev_Ea[0];
    double troe_a[0],troe_Ts[0], troe_Tss[0], troe_Tsss[0];
    double sri_a[0], sri_b[0], sri_c[0], sri_d[0], sri_e[0];
    double activation_units[0], prefactor_units[0], phase_units[0];
    int is_PD[0], troe_len[0], sri_len[0], nTB[0], *TBid[0];
    double *TB[0];

    double fwd_A_DEF[0], fwd_beta_DEF[0], fwd_Ea_DEF[0];
    double low_A_DEF[0], low_beta_DEF[0], low_Ea_DEF[0];
    double rev_A_DEF[0], rev_beta_DEF[0], rev_Ea_DEF[0];
    double troe_a_DEF[0],troe_Ts_DEF[0], troe_Tss_DEF[0], troe_Tsss_DEF[0];
    double sri_a_DEF[0], sri_b_DEF[0], sri_c_DEF[0], sri_d_DEF[0], sri_e_DEF[0];
    double activation_units_DEF[0], prefactor_units_DEF[0], phase_units_DEF[0];
    int is_PD_DEF[0], troe_len_DEF[0], sri_len_DEF[0], nTB_DEF[0], *TBid_DEF[0];
    double *TB_DEF[0];
    std::vector<int> rxn_map;
};

using namespace thermo;
#endif

/* Inverse molecular weights */
/* TODO: check necessity on CPU */
static AMREX_GPU_DEVICE_MANAGED double inv_molecular_weights[35] = {
    1.0 / 170.341020,  /*NC12H26 */
    1.0 / 1.007970,  /*H */
    1.0 / 15.999400,  /*O */
    1.0 / 17.007370,  /*OH */
    1.0 / 33.006770,  /*HO2 */
    1.0 / 2.015940,  /*H2 */
    1.0 / 18.015340,  /*H2O */
    1.0 / 34.014740,  /*H2O2 */
    1.0 / 31.998800,  /*O2 */
    1.0 / 15.035060,  /*CH3 */
    1.0 / 16.043030,  /*CH4 */
    1.0 / 30.026490,  /*CH2O */
    1.0 / 28.010550,  /*CO */
    1.0 / 44.009950,  /*CO2 */
    1.0 / 26.038240,  /*C2H2 */
    1.0 / 28.054180,  /*C2H4 */
    1.0 / 30.070120,  /*C2H6 */
    1.0 / 43.045610,  /*CH2CHO */
    1.0 / 41.073300,  /*aC3H5 */
    1.0 / 42.081270,  /*C3H6 */
    1.0 / 56.064730,  /*C2H3CHO */
    1.0 / 55.100390,  /*C4H7 */
    1.0 / 56.108360,  /*C4H81 */
    1.0 / 69.127480,  /*C5H9 */
    1.0 / 70.135450,  /*C5H10 */
    1.0 / 84.162540,  /*C6H12 */
    1.0 / 98.189630,  /*C7H14 */
    1.0 / 112.216720,  /*C8H16 */
    1.0 / 126.243810,  /*C9H18 */
    1.0 / 127.251780,  /*PXC9H19 */
    1.0 / 140.270900,  /*C10H20 */
    1.0 / 168.325080,  /*C12H24 */
    1.0 / 201.331850,  /*C12H25O2 */
    1.0 / 216.323280,  /*OC12H23OOH */
    1.0 / 28.013400};  /*N2 */

/* Inverse molecular weights */
/* TODO: check necessity because redundant with molecularWeight */
static AMREX_GPU_DEVICE_MANAGED double molecular_weights[35] = {
    170.341020,  /*NC12H26 */
    1.007970,  /*H */
    15.999400,  /*O */
    17.007370,  /*OH */
    33.006770,  /*HO2 */
    2.015940,  /*H2 */
    18.015340,  /*H2O */
    34.014740,  /*H2O2 */
    31.998800,  /*O2 */
    15.035060,  /*CH3 */
    16.043030,  /*CH4 */
    30.026490,  /*CH2O */
    28.010550,  /*CO */
    44.009950,  /*CO2 */
    26.038240,  /*C2H2 */
    28.054180,  /*C2H4 */
    30.070120,  /*C2H6 */
    43.045610,  /*CH2CHO */
    41.073300,  /*aC3H5 */
    42.081270,  /*C3H6 */
    56.064730,  /*C2H3CHO */
    55.100390,  /*C4H7 */
    56.108360,  /*C4H81 */
    69.127480,  /*C5H9 */
    70.135450,  /*C5H10 */
    84.162540,  /*C6H12 */
    98.189630,  /*C7H14 */
    112.216720,  /*C8H16 */
    126.243810,  /*C9H18 */
    127.251780,  /*PXC9H19 */
    140.270900,  /*C10H20 */
    168.325080,  /*C12H24 */
    201.331850,  /*C12H25O2 */
    216.323280,  /*OC12H23OOH */
    28.013400};  /*N2 */

AMREX_GPU_HOST_DEVICE
void get_imw(double imw_new[]){
    for(int i = 0; i<35; ++i) imw_new[i] = inv_molecular_weights[i];
}

/* TODO: check necessity because redundant with CKWT */
AMREX_GPU_HOST_DEVICE
void get_mw(double mw_new[]){
    for(int i = 0; i<35; ++i) mw_new[i] = molecular_weights[i];
}


#ifndef AMREX_USE_CUDA
/* Initializes parameter database */
void CKINIT()
{

    /* Inverse molecular weights */
    imw = {
        1.0 / 170.341020,  /*NC12H26 */
        1.0 / 1.007970,  /*H */
        1.0 / 15.999400,  /*O */
        1.0 / 17.007370,  /*OH */
        1.0 / 33.006770,  /*HO2 */
        1.0 / 2.015940,  /*H2 */
        1.0 / 18.015340,  /*H2O */
        1.0 / 34.014740,  /*H2O2 */
        1.0 / 31.998800,  /*O2 */
        1.0 / 15.035060,  /*CH3 */
        1.0 / 16.043030,  /*CH4 */
        1.0 / 30.026490,  /*CH2O */
        1.0 / 28.010550,  /*CO */
        1.0 / 44.009950,  /*CO2 */
        1.0 / 26.038240,  /*C2H2 */
        1.0 / 28.054180,  /*C2H4 */
        1.0 / 30.070120,  /*C2H6 */
        1.0 / 43.045610,  /*CH2CHO */
        1.0 / 41.073300,  /*aC3H5 */
        1.0 / 42.081270,  /*C3H6 */
        1.0 / 56.064730,  /*C2H3CHO */
        1.0 / 55.100390,  /*C4H7 */
        1.0 / 56.108360,  /*C4H81 */
        1.0 / 69.127480,  /*C5H9 */
        1.0 / 70.135450,  /*C5H10 */
        1.0 / 84.162540,  /*C6H12 */
        1.0 / 98.189630,  /*C7H14 */
        1.0 / 112.216720,  /*C8H16 */
        1.0 / 126.243810,  /*C9H18 */
        1.0 / 127.251780,  /*PXC9H19 */
        1.0 / 140.270900,  /*C10H20 */
        1.0 / 168.325080,  /*C12H24 */
        1.0 / 201.331850,  /*C12H25O2 */
        1.0 / 216.323280,  /*OC12H23OOH */
        1.0 / 28.013400};  /*N2 */

    rxn_map = {};

    SetAllDefaults();
}

void GET_REACTION_MAP(int *rmap)
{
    for (int i=0; i<0; ++i) {
        rmap[i] = rxn_map[i];
    }
}

#include <ReactionData.H>
double* GetParamPtr(int                reaction_id,
                    REACTION_PARAMETER param_id,
                    int                species_id,
                    int                get_default)
{
  double* ret = 0;
  if (reaction_id<0 || reaction_id>=0) {
    printf("Bad reaction id = %d",reaction_id);
    abort();
  };
  int mrid = rxn_map[reaction_id];

  if (param_id == THIRD_BODY) {
    if (species_id<0 || species_id>=35) {
      printf("GetParamPtr: Bad species id = %d",species_id);
      abort();
    }
    if (get_default) {
      for (int i=0; i<nTB_DEF[mrid]; ++i) {
        if (species_id == TBid_DEF[mrid][i]) {
          ret = &(TB_DEF[mrid][i]);
        }
      }
    }
    else {
      for (int i=0; i<nTB[mrid]; ++i) {
        if (species_id == TBid[mrid][i]) {
          ret = &(TB[mrid][i]);
        }
      }
    }
    if (ret == 0) {
      printf("GetParamPtr: No TB for reaction id = %d",reaction_id);
      abort();
    }
  }
  else {
    if (     param_id == FWD_A)     {ret = (get_default ? &(fwd_A_DEF[mrid]) : &(fwd_A[mrid]));}
      else if (param_id == FWD_BETA)  {ret = (get_default ? &(fwd_beta_DEF[mrid]) : &(fwd_beta[mrid]));}
      else if (param_id == FWD_EA)    {ret = (get_default ? &(fwd_Ea_DEF[mrid]) : &(fwd_Ea[mrid]));}
      else if (param_id == LOW_A)     {ret = (get_default ? &(low_A_DEF[mrid]) : &(low_A[mrid]));}
      else if (param_id == LOW_BETA)  {ret = (get_default ? &(low_beta_DEF[mrid]) : &(low_beta[mrid]));}
      else if (param_id == LOW_EA)    {ret = (get_default ? &(low_Ea_DEF[mrid]) : &(low_Ea[mrid]));}
      else if (param_id == REV_A)     {ret = (get_default ? &(rev_A_DEF[mrid]) : &(rev_A[mrid]));}
      else if (param_id == REV_BETA)  {ret = (get_default ? &(rev_beta_DEF[mrid]) : &(rev_beta[mrid]));}
      else if (param_id == REV_EA)    {ret = (get_default ? &(rev_Ea_DEF[mrid]) : &(rev_Ea[mrid]));}
      else if (param_id == TROE_A)    {ret = (get_default ? &(troe_a_DEF[mrid]) : &(troe_a[mrid]));}
      else if (param_id == TROE_TS)   {ret = (get_default ? &(troe_Ts_DEF[mrid]) : &(troe_Ts[mrid]));}
      else if (param_id == TROE_TSS)  {ret = (get_default ? &(troe_Tss_DEF[mrid]) : &(troe_Tss[mrid]));}
      else if (param_id == TROE_TSSS) {ret = (get_default ? &(troe_Tsss_DEF[mrid]) : &(troe_Tsss[mrid]));}
      else if (param_id == SRI_A)     {ret = (get_default ? &(sri_a_DEF[mrid]) : &(sri_a[mrid]));}
      else if (param_id == SRI_B)     {ret = (get_default ? &(sri_b_DEF[mrid]) : &(sri_b[mrid]));}
      else if (param_id == SRI_C)     {ret = (get_default ? &(sri_c_DEF[mrid]) : &(sri_c[mrid]));}
      else if (param_id == SRI_D)     {ret = (get_default ? &(sri_d_DEF[mrid]) : &(sri_d[mrid]));}
      else if (param_id == SRI_E)     {ret = (get_default ? &(sri_e_DEF[mrid]) : &(sri_e[mrid]));}
    else {
      printf("GetParamPtr: Unknown parameter id");
      abort();
    }
  }
  return ret;
}

void ResetAllParametersToDefault()
{
    for (int i=0; i<0; i++) {
        if (nTB[i] != 0) {
            nTB[i] = 0;
            free(TB[i]);
            free(TBid[i]);
        }

        fwd_A[i]    = fwd_A_DEF[i];
        fwd_beta[i] = fwd_beta_DEF[i];
        fwd_Ea[i]   = fwd_Ea_DEF[i];

        low_A[i]    = low_A_DEF[i];
        low_beta[i] = low_beta_DEF[i];
        low_Ea[i]   = low_Ea_DEF[i];

        rev_A[i]    = rev_A_DEF[i];
        rev_beta[i] = rev_beta_DEF[i];
        rev_Ea[i]   = rev_Ea_DEF[i];

        troe_a[i]    = troe_a_DEF[i];
        troe_Ts[i]   = troe_Ts_DEF[i];
        troe_Tss[i]  = troe_Tss_DEF[i];
        troe_Tsss[i] = troe_Tsss_DEF[i];

        sri_a[i] = sri_a_DEF[i];
        sri_b[i] = sri_b_DEF[i];
        sri_c[i] = sri_c_DEF[i];
        sri_d[i] = sri_d_DEF[i];
        sri_e[i] = sri_e_DEF[i];

        is_PD[i]    = is_PD_DEF[i];
        troe_len[i] = troe_len_DEF[i];
        sri_len[i]  = sri_len_DEF[i];

        activation_units[i] = activation_units_DEF[i];
        prefactor_units[i]  = prefactor_units_DEF[i];
        phase_units[i]      = phase_units_DEF[i];

        nTB[i]  = nTB_DEF[i];
        if (nTB[i] != 0) {
           TB[i] = (double *) malloc(sizeof(double) * nTB[i]);
           TBid[i] = (int *) malloc(sizeof(int) * nTB[i]);
           for (int j=0; j<nTB[i]; j++) {
             TB[i][j] = TB_DEF[i][j];
             TBid[i][j] = TBid_DEF[i][j];
           }
        }
    }
}

void SetAllDefaults()
{
    for (int i=0; i<0; i++) {
        if (nTB_DEF[i] != 0) {
            nTB_DEF[i] = 0;
            free(TB_DEF[i]);
            free(TBid_DEF[i]);
        }

        fwd_A_DEF[i]    = fwd_A[i];
        fwd_beta_DEF[i] = fwd_beta[i];
        fwd_Ea_DEF[i]   = fwd_Ea[i];

        low_A_DEF[i]    = low_A[i];
        low_beta_DEF[i] = low_beta[i];
        low_Ea_DEF[i]   = low_Ea[i];

        rev_A_DEF[i]    = rev_A[i];
        rev_beta_DEF[i] = rev_beta[i];
        rev_Ea_DEF[i]   = rev_Ea[i];

        troe_a_DEF[i]    = troe_a[i];
        troe_Ts_DEF[i]   = troe_Ts[i];
        troe_Tss_DEF[i]  = troe_Tss[i];
        troe_Tsss_DEF[i] = troe_Tsss[i];

        sri_a_DEF[i] = sri_a[i];
        sri_b_DEF[i] = sri_b[i];
        sri_c_DEF[i] = sri_c[i];
        sri_d_DEF[i] = sri_d[i];
        sri_e_DEF[i] = sri_e[i];

        is_PD_DEF[i]    = is_PD[i];
        troe_len_DEF[i] = troe_len[i];
        sri_len_DEF[i]  = sri_len[i];

        activation_units_DEF[i] = activation_units[i];
        prefactor_units_DEF[i]  = prefactor_units[i];
        phase_units_DEF[i]      = phase_units[i];

        nTB_DEF[i]  = nTB[i];
        if (nTB_DEF[i] != 0) {
           TB_DEF[i] = (double *) malloc(sizeof(double) * nTB_DEF[i]);
           TBid_DEF[i] = (int *) malloc(sizeof(int) * nTB_DEF[i]);
           for (int j=0; j<nTB_DEF[i]; j++) {
             TB_DEF[i][j] = TB[i][j];
             TBid_DEF[i][j] = TBid[i][j];
           }
        }
    }
}

/* Finalizes parameter database */
void CKFINALIZE()
{
  for (int i=0; i<0; ++i) {
    free(TB[i]); TB[i] = 0; 
    free(TBid[i]); TBid[i] = 0;
    nTB[i] = 0;

    free(TB_DEF[i]); TB_DEF[i] = 0; 
    free(TBid_DEF[i]); TBid_DEF[i] = 0;
    nTB_DEF[i] = 0;
  }
}

#else
/* TODO: Remove on GPU, right now needed by chemistry_module on FORTRAN */
AMREX_GPU_HOST_DEVICE void CKINIT()
{
}

AMREX_GPU_HOST_DEVICE void CKFINALIZE()
{
}

#endif


/*A few mechanism parameters */
void CKINDX(int * mm, int * kk, int * ii, int * nfit)
{
    *mm = 4;
    *kk = 35;
    *ii = 0;
    *nfit = -1; /*Why do you need this anyway ?  */
}



/* ckxnum... for parsing strings  */
void CKXNUM(char * line, int * nexp, int * lout, int * nval, double *  rval, int * kerr, int lenline )
{
    int n,i; /*Loop Counters */
    char cstr[1000];
    char *saveptr;
    char *p; /*String Tokens */
    /* Strip Comments  */
    for (i=0; i<lenline; ++i) {
        if (line[i]=='!') {
            break;
        }
        cstr[i] = line[i];
    }
    cstr[i] = '\0';

    p = strtok_r(cstr," ", &saveptr);
    if (!p) {
        *nval = 0;
        *kerr = 1;
        return;
    }
    for (n=0; n<*nexp; ++n) {
        rval[n] = atof(p);
        p = strtok_r(NULL, " ", &saveptr);
        if (!p) break;
    }
    *nval = n+1;
    if (*nval < *nexp) *kerr = 1;
    return;
}


/* cksnum... for parsing strings  */
void CKSNUM(char * line, int * nexp, int * lout, char * kray, int * nn, int * knum, int * nval, double *  rval, int * kerr, int lenline, int lenkray)
{
    /*Not done yet ... */
}


/* Returns the char strings of element names */
void CKSYME(int * kname, int * plenkname )
{
    int i; /*Loop Counter */
    int lenkname = *plenkname;
    /*clear kname */
    for (i=0; i<lenkname*4; i++) {
        kname[i] = ' ';
    }

    /* O  */
    kname[ 0*lenkname + 0 ] = 'O';
    kname[ 0*lenkname + 1 ] = ' ';

    /* H  */
    kname[ 1*lenkname + 0 ] = 'H';
    kname[ 1*lenkname + 1 ] = ' ';

    /* C  */
    kname[ 2*lenkname + 0 ] = 'C';
    kname[ 2*lenkname + 1 ] = ' ';

    /* N  */
    kname[ 3*lenkname + 0 ] = 'N';
    kname[ 3*lenkname + 1 ] = ' ';

}


/* Returns the char strings of species names */
void CKSYMS(int * kname, int * plenkname )
{
    int i; /*Loop Counter */
    int lenkname = *plenkname;
    /*clear kname */
    for (i=0; i<lenkname*35; i++) {
        kname[i] = ' ';
    }

    /* NC12H26  */
    kname[ 0*lenkname + 0 ] = 'N';
    kname[ 0*lenkname + 1 ] = 'C';
    kname[ 0*lenkname + 2 ] = '1';
    kname[ 0*lenkname + 3 ] = '2';
    kname[ 0*lenkname + 4 ] = 'H';
    kname[ 0*lenkname + 5 ] = '2';
    kname[ 0*lenkname + 6 ] = '6';
    kname[ 0*lenkname + 7 ] = ' ';

    /* H  */
    kname[ 1*lenkname + 0 ] = 'H';
    kname[ 1*lenkname + 1 ] = ' ';

    /* O  */
    kname[ 2*lenkname + 0 ] = 'O';
    kname[ 2*lenkname + 1 ] = ' ';

    /* OH  */
    kname[ 3*lenkname + 0 ] = 'O';
    kname[ 3*lenkname + 1 ] = 'H';
    kname[ 3*lenkname + 2 ] = ' ';

    /* HO2  */
    kname[ 4*lenkname + 0 ] = 'H';
    kname[ 4*lenkname + 1 ] = 'O';
    kname[ 4*lenkname + 2 ] = '2';
    kname[ 4*lenkname + 3 ] = ' ';

    /* H2  */
    kname[ 5*lenkname + 0 ] = 'H';
    kname[ 5*lenkname + 1 ] = '2';
    kname[ 5*lenkname + 2 ] = ' ';

    /* H2O  */
    kname[ 6*lenkname + 0 ] = 'H';
    kname[ 6*lenkname + 1 ] = '2';
    kname[ 6*lenkname + 2 ] = 'O';
    kname[ 6*lenkname + 3 ] = ' ';

    /* H2O2  */
    kname[ 7*lenkname + 0 ] = 'H';
    kname[ 7*lenkname + 1 ] = '2';
    kname[ 7*lenkname + 2 ] = 'O';
    kname[ 7*lenkname + 3 ] = '2';
    kname[ 7*lenkname + 4 ] = ' ';

    /* O2  */
    kname[ 8*lenkname + 0 ] = 'O';
    kname[ 8*lenkname + 1 ] = '2';
    kname[ 8*lenkname + 2 ] = ' ';

    /* CH3  */
    kname[ 9*lenkname + 0 ] = 'C';
    kname[ 9*lenkname + 1 ] = 'H';
    kname[ 9*lenkname + 2 ] = '3';
    kname[ 9*lenkname + 3 ] = ' ';

    /* CH4  */
    kname[ 10*lenkname + 0 ] = 'C';
    kname[ 10*lenkname + 1 ] = 'H';
    kname[ 10*lenkname + 2 ] = '4';
    kname[ 10*lenkname + 3 ] = ' ';

    /* CH2O  */
    kname[ 11*lenkname + 0 ] = 'C';
    kname[ 11*lenkname + 1 ] = 'H';
    kname[ 11*lenkname + 2 ] = '2';
    kname[ 11*lenkname + 3 ] = 'O';
    kname[ 11*lenkname + 4 ] = ' ';

    /* CO  */
    kname[ 12*lenkname + 0 ] = 'C';
    kname[ 12*lenkname + 1 ] = 'O';
    kname[ 12*lenkname + 2 ] = ' ';

    /* CO2  */
    kname[ 13*lenkname + 0 ] = 'C';
    kname[ 13*lenkname + 1 ] = 'O';
    kname[ 13*lenkname + 2 ] = '2';
    kname[ 13*lenkname + 3 ] = ' ';

    /* C2H2  */
    kname[ 14*lenkname + 0 ] = 'C';
    kname[ 14*lenkname + 1 ] = '2';
    kname[ 14*lenkname + 2 ] = 'H';
    kname[ 14*lenkname + 3 ] = '2';
    kname[ 14*lenkname + 4 ] = ' ';

    /* C2H4  */
    kname[ 15*lenkname + 0 ] = 'C';
    kname[ 15*lenkname + 1 ] = '2';
    kname[ 15*lenkname + 2 ] = 'H';
    kname[ 15*lenkname + 3 ] = '4';
    kname[ 15*lenkname + 4 ] = ' ';

    /* C2H6  */
    kname[ 16*lenkname + 0 ] = 'C';
    kname[ 16*lenkname + 1 ] = '2';
    kname[ 16*lenkname + 2 ] = 'H';
    kname[ 16*lenkname + 3 ] = '6';
    kname[ 16*lenkname + 4 ] = ' ';

    /* CH2CHO  */
    kname[ 17*lenkname + 0 ] = 'C';
    kname[ 17*lenkname + 1 ] = 'H';
    kname[ 17*lenkname + 2 ] = '2';
    kname[ 17*lenkname + 3 ] = 'C';
    kname[ 17*lenkname + 4 ] = 'H';
    kname[ 17*lenkname + 5 ] = 'O';
    kname[ 17*lenkname + 6 ] = ' ';

    /* aC3H5  */
    kname[ 18*lenkname + 0 ] = 'A';
    kname[ 18*lenkname + 1 ] = 'C';
    kname[ 18*lenkname + 2 ] = '3';
    kname[ 18*lenkname + 3 ] = 'H';
    kname[ 18*lenkname + 4 ] = '5';
    kname[ 18*lenkname + 5 ] = ' ';

    /* C3H6  */
    kname[ 19*lenkname + 0 ] = 'C';
    kname[ 19*lenkname + 1 ] = '3';
    kname[ 19*lenkname + 2 ] = 'H';
    kname[ 19*lenkname + 3 ] = '6';
    kname[ 19*lenkname + 4 ] = ' ';

    /* C2H3CHO  */
    kname[ 20*lenkname + 0 ] = 'C';
    kname[ 20*lenkname + 1 ] = '2';
    kname[ 20*lenkname + 2 ] = 'H';
    kname[ 20*lenkname + 3 ] = '3';
    kname[ 20*lenkname + 4 ] = 'C';
    kname[ 20*lenkname + 5 ] = 'H';
    kname[ 20*lenkname + 6 ] = 'O';
    kname[ 20*lenkname + 7 ] = ' ';

    /* C4H7  */
    kname[ 21*lenkname + 0 ] = 'C';
    kname[ 21*lenkname + 1 ] = '4';
    kname[ 21*lenkname + 2 ] = 'H';
    kname[ 21*lenkname + 3 ] = '7';
    kname[ 21*lenkname + 4 ] = ' ';

    /* C4H81  */
    kname[ 22*lenkname + 0 ] = 'C';
    kname[ 22*lenkname + 1 ] = '4';
    kname[ 22*lenkname + 2 ] = 'H';
    kname[ 22*lenkname + 3 ] = '8';
    kname[ 22*lenkname + 4 ] = '1';
    kname[ 22*lenkname + 5 ] = ' ';

    /* C5H9  */
    kname[ 23*lenkname + 0 ] = 'C';
    kname[ 23*lenkname + 1 ] = '5';
    kname[ 23*lenkname + 2 ] = 'H';
    kname[ 23*lenkname + 3 ] = '9';
    kname[ 23*lenkname + 4 ] = ' ';

    /* C5H10  */
    kname[ 24*lenkname + 0 ] = 'C';
    kname[ 24*lenkname + 1 ] = '5';
    kname[ 24*lenkname + 2 ] = 'H';
    kname[ 24*lenkname + 3 ] = '1';
    kname[ 24*lenkname + 4 ] = '0';
    kname[ 24*lenkname + 5 ] = ' ';

    /* C6H12  */
    kname[ 25*lenkname + 0 ] = 'C';
    kname[ 25*lenkname + 1 ] = '6';
    kname[ 25*lenkname + 2 ] = 'H';
    kname[ 25*lenkname + 3 ] = '1';
    kname[ 25*lenkname + 4 ] = '2';
    kname[ 25*lenkname + 5 ] = ' ';

    /* C7H14  */
    kname[ 26*lenkname + 0 ] = 'C';
    kname[ 26*lenkname + 1 ] = '7';
    kname[ 26*lenkname + 2 ] = 'H';
    kname[ 26*lenkname + 3 ] = '1';
    kname[ 26*lenkname + 4 ] = '4';
    kname[ 26*lenkname + 5 ] = ' ';

    /* C8H16  */
    kname[ 27*lenkname + 0 ] = 'C';
    kname[ 27*lenkname + 1 ] = '8';
    kname[ 27*lenkname + 2 ] = 'H';
    kname[ 27*lenkname + 3 ] = '1';
    kname[ 27*lenkname + 4 ] = '6';
    kname[ 27*lenkname + 5 ] = ' ';

    /* C9H18  */
    kname[ 28*lenkname + 0 ] = 'C';
    kname[ 28*lenkname + 1 ] = '9';
    kname[ 28*lenkname + 2 ] = 'H';
    kname[ 28*lenkname + 3 ] = '1';
    kname[ 28*lenkname + 4 ] = '8';
    kname[ 28*lenkname + 5 ] = ' ';

    /* PXC9H19  */
    kname[ 29*lenkname + 0 ] = 'P';
    kname[ 29*lenkname + 1 ] = 'X';
    kname[ 29*lenkname + 2 ] = 'C';
    kname[ 29*lenkname + 3 ] = '9';
    kname[ 29*lenkname + 4 ] = 'H';
    kname[ 29*lenkname + 5 ] = '1';
    kname[ 29*lenkname + 6 ] = '9';
    kname[ 29*lenkname + 7 ] = ' ';

    /* C10H20  */
    kname[ 30*lenkname + 0 ] = 'C';
    kname[ 30*lenkname + 1 ] = '1';
    kname[ 30*lenkname + 2 ] = '0';
    kname[ 30*lenkname + 3 ] = 'H';
    kname[ 30*lenkname + 4 ] = '2';
    kname[ 30*lenkname + 5 ] = '0';
    kname[ 30*lenkname + 6 ] = ' ';

    /* C12H24  */
    kname[ 31*lenkname + 0 ] = 'C';
    kname[ 31*lenkname + 1 ] = '1';
    kname[ 31*lenkname + 2 ] = '2';
    kname[ 31*lenkname + 3 ] = 'H';
    kname[ 31*lenkname + 4 ] = '2';
    kname[ 31*lenkname + 5 ] = '4';
    kname[ 31*lenkname + 6 ] = ' ';

    /* C12H25O2  */
    kname[ 32*lenkname + 0 ] = 'C';
    kname[ 32*lenkname + 1 ] = '1';
    kname[ 32*lenkname + 2 ] = '2';
    kname[ 32*lenkname + 3 ] = 'H';
    kname[ 32*lenkname + 4 ] = '2';
    kname[ 32*lenkname + 5 ] = '5';
    kname[ 32*lenkname + 6 ] = 'O';
    kname[ 32*lenkname + 7 ] = '2';
    kname[ 32*lenkname + 8 ] = ' ';

    /* OC12H23OOH  */
    kname[ 33*lenkname + 0 ] = 'O';
    kname[ 33*lenkname + 1 ] = 'C';
    kname[ 33*lenkname + 2 ] = '1';
    kname[ 33*lenkname + 3 ] = '2';
    kname[ 33*lenkname + 4 ] = 'H';
    kname[ 33*lenkname + 5 ] = '2';
    kname[ 33*lenkname + 6 ] = '3';
    kname[ 33*lenkname + 7 ] = 'O';
    kname[ 33*lenkname + 8 ] = 'O';
    kname[ 33*lenkname + 9 ] = 'H';
    kname[ 33*lenkname + 10 ] = ' ';

    /* N2  */
    kname[ 34*lenkname + 0 ] = 'N';
    kname[ 34*lenkname + 1 ] = '2';
    kname[ 34*lenkname + 2 ] = ' ';

}


/* Returns R, Rc, Patm */
void CKRP(double *  ru, double *  ruc, double *  pa)
{
     *ru  = 8.31451e+07; 
     *ruc = 1.98721558317399615845; 
     *pa  = 1.01325e+06; 
}


/*Compute P = rhoRT/W(x) */
void CKPX(double *  rho, double *  T, double *  x, double *  P)
{
    double XW = 0;/* To hold mean molecular wt */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    *P = *rho * 8.31451e+07 * (*T) / XW; /*P = rho*R*T/W */

    return;
}


/*Compute P = rhoRT/W(y) */
AMREX_GPU_HOST_DEVICE void CKPY(double *  rho, double *  T, double *  y,  double *  P)
{
    double YOW = 0;/* for computing mean MW */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    *P = *rho * 8.31451e+07 * (*T) * YOW; /*P = rho*R*T/W */

    return;
}


#ifndef AMREX_USE_CUDA
/*Compute P = rhoRT/W(y) */
void VCKPY(int *  np, double *  rho, double *  T, double *  y,  double *  P)
{
    double YOW[*np];
    for (int i=0; i<(*np); i++) {
        YOW[i] = 0.0;
    }

    for (int n=0; n<35; n++) {
        for (int i=0; i<(*np); i++) {
            YOW[i] += y[n*(*np)+i] * imw[n];
        }
    }

    for (int i=0; i<(*np); i++) {
        P[i] = rho[i] * 8.31451e+07 * T[i] * YOW[i]; /*P = rho*R*T/W */
    }

    return;
}
#endif


/*Compute P = rhoRT/W(c) */
void CKPC(double *  rho, double *  T, double *  c,  double *  P)
{
    int id; /*loop counter */
    /*See Eq 5 in CK Manual */
    double W = 0;
    double sumC = 0;
    W += c[0]*170.341020; /*NC12H26 */
    W += c[1]*1.007970; /*H */
    W += c[2]*15.999400; /*O */
    W += c[3]*17.007370; /*OH */
    W += c[4]*33.006770; /*HO2 */
    W += c[5]*2.015940; /*H2 */
    W += c[6]*18.015340; /*H2O */
    W += c[7]*34.014740; /*H2O2 */
    W += c[8]*31.998800; /*O2 */
    W += c[9]*15.035060; /*CH3 */
    W += c[10]*16.043030; /*CH4 */
    W += c[11]*30.026490; /*CH2O */
    W += c[12]*28.010550; /*CO */
    W += c[13]*44.009950; /*CO2 */
    W += c[14]*26.038240; /*C2H2 */
    W += c[15]*28.054180; /*C2H4 */
    W += c[16]*30.070120; /*C2H6 */
    W += c[17]*43.045610; /*CH2CHO */
    W += c[18]*41.073300; /*aC3H5 */
    W += c[19]*42.081270; /*C3H6 */
    W += c[20]*56.064730; /*C2H3CHO */
    W += c[21]*55.100390; /*C4H7 */
    W += c[22]*56.108360; /*C4H81 */
    W += c[23]*69.127480; /*C5H9 */
    W += c[24]*70.135450; /*C5H10 */
    W += c[25]*84.162540; /*C6H12 */
    W += c[26]*98.189630; /*C7H14 */
    W += c[27]*112.216720; /*C8H16 */
    W += c[28]*126.243810; /*C9H18 */
    W += c[29]*127.251780; /*PXC9H19 */
    W += c[30]*140.270900; /*C10H20 */
    W += c[31]*168.325080; /*C12H24 */
    W += c[32]*201.331850; /*C12H25O2 */
    W += c[33]*216.323280; /*OC12H23OOH */
    W += c[34]*28.013400; /*N2 */

    for (id = 0; id < 35; ++id) {
        sumC += c[id];
    }
    *P = *rho * 8.31451e+07 * (*T) * sumC / W; /*P = rho*R*T/W */

    return;
}


/*Compute rho = PW(x)/RT */
void CKRHOX(double *  P, double *  T, double *  x,  double *  rho)
{
    double XW = 0;/* To hold mean molecular wt */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    *rho = *P * XW / (8.31451e+07 * (*T)); /*rho = P*W/(R*T) */

    return;
}


/*Compute rho = P*W(y)/RT */
void CKRHOY(double *  P, double *  T, double *  y,  double *  rho)
{
    double YOW = 0;
    double tmp[35];

    for (int i = 0; i < 35; i++)
    {
        tmp[i] = y[i]*imw[i];
    }
    for (int i = 0; i < 35; i++)
    {
        YOW += tmp[i];
    }

    *rho = *P / (8.31451e+07 * (*T) * YOW);/*rho = P*W/(R*T) */
    return;
}


/*Compute rho = P*W(c)/(R*T) */
void CKRHOC(double *  P, double *  T, double *  c,  double *  rho)
{
    int id; /*loop counter */
    /*See Eq 5 in CK Manual */
    double W = 0;
    double sumC = 0;
    W += c[0]*170.341020; /*NC12H26 */
    W += c[1]*1.007970; /*H */
    W += c[2]*15.999400; /*O */
    W += c[3]*17.007370; /*OH */
    W += c[4]*33.006770; /*HO2 */
    W += c[5]*2.015940; /*H2 */
    W += c[6]*18.015340; /*H2O */
    W += c[7]*34.014740; /*H2O2 */
    W += c[8]*31.998800; /*O2 */
    W += c[9]*15.035060; /*CH3 */
    W += c[10]*16.043030; /*CH4 */
    W += c[11]*30.026490; /*CH2O */
    W += c[12]*28.010550; /*CO */
    W += c[13]*44.009950; /*CO2 */
    W += c[14]*26.038240; /*C2H2 */
    W += c[15]*28.054180; /*C2H4 */
    W += c[16]*30.070120; /*C2H6 */
    W += c[17]*43.045610; /*CH2CHO */
    W += c[18]*41.073300; /*aC3H5 */
    W += c[19]*42.081270; /*C3H6 */
    W += c[20]*56.064730; /*C2H3CHO */
    W += c[21]*55.100390; /*C4H7 */
    W += c[22]*56.108360; /*C4H81 */
    W += c[23]*69.127480; /*C5H9 */
    W += c[24]*70.135450; /*C5H10 */
    W += c[25]*84.162540; /*C6H12 */
    W += c[26]*98.189630; /*C7H14 */
    W += c[27]*112.216720; /*C8H16 */
    W += c[28]*126.243810; /*C9H18 */
    W += c[29]*127.251780; /*PXC9H19 */
    W += c[30]*140.270900; /*C10H20 */
    W += c[31]*168.325080; /*C12H24 */
    W += c[32]*201.331850; /*C12H25O2 */
    W += c[33]*216.323280; /*OC12H23OOH */
    W += c[34]*28.013400; /*N2 */

    for (id = 0; id < 35; ++id) {
        sumC += c[id];
    }
    *rho = *P * W / (sumC * (*T) * 8.31451e+07); /*rho = PW/(R*T) */

    return;
}


/*get molecular weight for all species */
void CKWT( double *  wt)
{
    molecularWeight(wt);
}


/*get atomic weight for all elements */
void CKAWT( double *  awt)
{
    atomicWeight(awt);
}


/*given y[species]: mass fractions */
/*returns mean molecular weight (gm/mole) */
void CKMMWY(double *  y,  double *  wtm)
{
    double YOW = 0;
    double tmp[35];

    for (int i = 0; i < 35; i++)
    {
        tmp[i] = y[i]*imw[i];
    }
    for (int i = 0; i < 35; i++)
    {
        YOW += tmp[i];
    }

    *wtm = 1.0 / YOW;
    return;
}


/*given x[species]: mole fractions */
/*returns mean molecular weight (gm/mole) */
void CKMMWX(double *  x,  double *  wtm)
{
    double XW = 0;/* see Eq 4 in CK Manual */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    *wtm = XW;

    return;
}


/*given c[species]: molar concentration */
/*returns mean molecular weight (gm/mole) */
void CKMMWC(double *  c,  double *  wtm)
{
    int id; /*loop counter */
    /*See Eq 5 in CK Manual */
    double W = 0;
    double sumC = 0;
    W += c[0]*170.341020; /*NC12H26 */
    W += c[1]*1.007970; /*H */
    W += c[2]*15.999400; /*O */
    W += c[3]*17.007370; /*OH */
    W += c[4]*33.006770; /*HO2 */
    W += c[5]*2.015940; /*H2 */
    W += c[6]*18.015340; /*H2O */
    W += c[7]*34.014740; /*H2O2 */
    W += c[8]*31.998800; /*O2 */
    W += c[9]*15.035060; /*CH3 */
    W += c[10]*16.043030; /*CH4 */
    W += c[11]*30.026490; /*CH2O */
    W += c[12]*28.010550; /*CO */
    W += c[13]*44.009950; /*CO2 */
    W += c[14]*26.038240; /*C2H2 */
    W += c[15]*28.054180; /*C2H4 */
    W += c[16]*30.070120; /*C2H6 */
    W += c[17]*43.045610; /*CH2CHO */
    W += c[18]*41.073300; /*aC3H5 */
    W += c[19]*42.081270; /*C3H6 */
    W += c[20]*56.064730; /*C2H3CHO */
    W += c[21]*55.100390; /*C4H7 */
    W += c[22]*56.108360; /*C4H81 */
    W += c[23]*69.127480; /*C5H9 */
    W += c[24]*70.135450; /*C5H10 */
    W += c[25]*84.162540; /*C6H12 */
    W += c[26]*98.189630; /*C7H14 */
    W += c[27]*112.216720; /*C8H16 */
    W += c[28]*126.243810; /*C9H18 */
    W += c[29]*127.251780; /*PXC9H19 */
    W += c[30]*140.270900; /*C10H20 */
    W += c[31]*168.325080; /*C12H24 */
    W += c[32]*201.331850; /*C12H25O2 */
    W += c[33]*216.323280; /*OC12H23OOH */
    W += c[34]*28.013400; /*N2 */

    for (id = 0; id < 35; ++id) {
        sumC += c[id];
    }
    /* CK provides no guard against divison by zero */
    *wtm = W/sumC;

    return;
}


/*convert y[species] (mass fracs) to x[species] (mole fracs) */
AMREX_GPU_HOST_DEVICE void CKYTX(double *  y,  double *  x)
{
    double YOW = 0;
    double tmp[35];

    for (int i = 0; i < 35; i++)
    {
        tmp[i] = y[i]*imw[i];
    }
    for (int i = 0; i < 35; i++)
    {
        YOW += tmp[i];
    }

    double YOWINV = 1.0/YOW;

    for (int i = 0; i < 35; i++)
    {
        x[i] = y[i]*imw[i]*YOWINV;
    }
    return;
}


#ifndef AMREX_USE_CUDA
/*convert y[npoints*species] (mass fracs) to x[npoints*species] (mole fracs) */
void VCKYTX(int *  np, double *  y,  double *  x)
{
    double YOW[*np];
    for (int i=0; i<(*np); i++) {
        YOW[i] = 0.0;
    }

    for (int n=0; n<35; n++) {
        for (int i=0; i<(*np); i++) {
            x[n*(*np)+i] = y[n*(*np)+i] * imw[n];
            YOW[i] += x[n*(*np)+i];
        }
    }

    for (int i=0; i<(*np); i++) {
        YOW[i] = 1.0/YOW[i];
    }

    for (int n=0; n<35; n++) {
        for (int i=0; i<(*np); i++) {
            x[n*(*np)+i] *=  YOW[i];
        }
    }
}
#else
/*TODO: remove this on GPU */
void VCKYTX(int *  np, double *  y,  double *  x)
{
}
#endif


/*convert y[species] (mass fracs) to c[species] (molar conc) */
void CKYTCP(double *  P, double *  T, double *  y,  double *  c)
{
    double YOW = 0;
    double PWORT;

    /*Compute inverse of mean molecular wt first */
    for (int i = 0; i < 35; i++)
    {
        c[i] = y[i]*imw[i];
    }
    for (int i = 0; i < 35; i++)
    {
        YOW += c[i];
    }

    /*PW/RT (see Eq. 7) */
    PWORT = (*P)/(YOW * 8.31451e+07 * (*T)); 
    /*Now compute conversion */

    for (int i = 0; i < 35; i++)
    {
        c[i] = PWORT * y[i] * imw[i];
    }
    return;
}


/*convert y[species] (mass fracs) to c[species] (molar conc) */
void CKYTCR(double *  rho, double *  T, double *  y,  double *  c)
{
    for (int i = 0; i < 35; i++)
    {
        c[i] = (*rho)  * y[i] * imw[i];
    }
}


/*convert x[species] (mole fracs) to y[species] (mass fracs) */
void CKXTY(double *  x,  double *  y)
{
    double XW = 0; /*See Eq 4, 9 in CK Manual */
    /*Compute mean molecular wt first */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    /*Now compute conversion */
    double XWinv = 1.0/XW;
    y[0] = x[0]*170.341020*XWinv; 
    y[1] = x[1]*1.007970*XWinv; 
    y[2] = x[2]*15.999400*XWinv; 
    y[3] = x[3]*17.007370*XWinv; 
    y[4] = x[4]*33.006770*XWinv; 
    y[5] = x[5]*2.015940*XWinv; 
    y[6] = x[6]*18.015340*XWinv; 
    y[7] = x[7]*34.014740*XWinv; 
    y[8] = x[8]*31.998800*XWinv; 
    y[9] = x[9]*15.035060*XWinv; 
    y[10] = x[10]*16.043030*XWinv; 
    y[11] = x[11]*30.026490*XWinv; 
    y[12] = x[12]*28.010550*XWinv; 
    y[13] = x[13]*44.009950*XWinv; 
    y[14] = x[14]*26.038240*XWinv; 
    y[15] = x[15]*28.054180*XWinv; 
    y[16] = x[16]*30.070120*XWinv; 
    y[17] = x[17]*43.045610*XWinv; 
    y[18] = x[18]*41.073300*XWinv; 
    y[19] = x[19]*42.081270*XWinv; 
    y[20] = x[20]*56.064730*XWinv; 
    y[21] = x[21]*55.100390*XWinv; 
    y[22] = x[22]*56.108360*XWinv; 
    y[23] = x[23]*69.127480*XWinv; 
    y[24] = x[24]*70.135450*XWinv; 
    y[25] = x[25]*84.162540*XWinv; 
    y[26] = x[26]*98.189630*XWinv; 
    y[27] = x[27]*112.216720*XWinv; 
    y[28] = x[28]*126.243810*XWinv; 
    y[29] = x[29]*127.251780*XWinv; 
    y[30] = x[30]*140.270900*XWinv; 
    y[31] = x[31]*168.325080*XWinv; 
    y[32] = x[32]*201.331850*XWinv; 
    y[33] = x[33]*216.323280*XWinv; 
    y[34] = x[34]*28.013400*XWinv; 

    return;
}


/*convert x[species] (mole fracs) to c[species] (molar conc) */
void CKXTCP(double *  P, double *  T, double *  x,  double *  c)
{
    int id; /*loop counter */
    double PORT = (*P)/(8.31451e+07 * (*T)); /*P/RT */

    /*Compute conversion, see Eq 10 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*PORT;
    }

    return;
}


/*convert x[species] (mole fracs) to c[species] (molar conc) */
void CKXTCR(double *  rho, double *  T, double *  x, double *  c)
{
    int id; /*loop counter */
    double XW = 0; /*See Eq 4, 11 in CK Manual */
    double ROW; 
    /*Compute mean molecular wt first */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    ROW = (*rho) / XW;

    /*Compute conversion, see Eq 11 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*ROW;
    }

    return;
}


/*convert c[species] (molar conc) to x[species] (mole fracs) */
void CKCTX(double *  c, double *  x)
{
    int id; /*loop counter */
    double sumC = 0; 

    /*compute sum of c  */
    for (id = 0; id < 35; ++id) {
        sumC += c[id];
    }

    /* See Eq 13  */
    double sumCinv = 1.0/sumC;
    for (id = 0; id < 35; ++id) {
        x[id] = c[id]*sumCinv;
    }

    return;
}


/*convert c[species] (molar conc) to y[species] (mass fracs) */
void CKCTY(double *  c, double *  y)
{
    double CW = 0; /*See Eq 12 in CK Manual */
    /*compute denominator in eq 12 first */
    CW += c[0]*170.341020; /*NC12H26 */
    CW += c[1]*1.007970; /*H */
    CW += c[2]*15.999400; /*O */
    CW += c[3]*17.007370; /*OH */
    CW += c[4]*33.006770; /*HO2 */
    CW += c[5]*2.015940; /*H2 */
    CW += c[6]*18.015340; /*H2O */
    CW += c[7]*34.014740; /*H2O2 */
    CW += c[8]*31.998800; /*O2 */
    CW += c[9]*15.035060; /*CH3 */
    CW += c[10]*16.043030; /*CH4 */
    CW += c[11]*30.026490; /*CH2O */
    CW += c[12]*28.010550; /*CO */
    CW += c[13]*44.009950; /*CO2 */
    CW += c[14]*26.038240; /*C2H2 */
    CW += c[15]*28.054180; /*C2H4 */
    CW += c[16]*30.070120; /*C2H6 */
    CW += c[17]*43.045610; /*CH2CHO */
    CW += c[18]*41.073300; /*aC3H5 */
    CW += c[19]*42.081270; /*C3H6 */
    CW += c[20]*56.064730; /*C2H3CHO */
    CW += c[21]*55.100390; /*C4H7 */
    CW += c[22]*56.108360; /*C4H81 */
    CW += c[23]*69.127480; /*C5H9 */
    CW += c[24]*70.135450; /*C5H10 */
    CW += c[25]*84.162540; /*C6H12 */
    CW += c[26]*98.189630; /*C7H14 */
    CW += c[27]*112.216720; /*C8H16 */
    CW += c[28]*126.243810; /*C9H18 */
    CW += c[29]*127.251780; /*PXC9H19 */
    CW += c[30]*140.270900; /*C10H20 */
    CW += c[31]*168.325080; /*C12H24 */
    CW += c[32]*201.331850; /*C12H25O2 */
    CW += c[33]*216.323280; /*OC12H23OOH */
    CW += c[34]*28.013400; /*N2 */
    /*Now compute conversion */
    double CWinv = 1.0/CW;
    y[0] = c[0]*170.341020*CWinv; 
    y[1] = c[1]*1.007970*CWinv; 
    y[2] = c[2]*15.999400*CWinv; 
    y[3] = c[3]*17.007370*CWinv; 
    y[4] = c[4]*33.006770*CWinv; 
    y[5] = c[5]*2.015940*CWinv; 
    y[6] = c[6]*18.015340*CWinv; 
    y[7] = c[7]*34.014740*CWinv; 
    y[8] = c[8]*31.998800*CWinv; 
    y[9] = c[9]*15.035060*CWinv; 
    y[10] = c[10]*16.043030*CWinv; 
    y[11] = c[11]*30.026490*CWinv; 
    y[12] = c[12]*28.010550*CWinv; 
    y[13] = c[13]*44.009950*CWinv; 
    y[14] = c[14]*26.038240*CWinv; 
    y[15] = c[15]*28.054180*CWinv; 
    y[16] = c[16]*30.070120*CWinv; 
    y[17] = c[17]*43.045610*CWinv; 
    y[18] = c[18]*41.073300*CWinv; 
    y[19] = c[19]*42.081270*CWinv; 
    y[20] = c[20]*56.064730*CWinv; 
    y[21] = c[21]*55.100390*CWinv; 
    y[22] = c[22]*56.108360*CWinv; 
    y[23] = c[23]*69.127480*CWinv; 
    y[24] = c[24]*70.135450*CWinv; 
    y[25] = c[25]*84.162540*CWinv; 
    y[26] = c[26]*98.189630*CWinv; 
    y[27] = c[27]*112.216720*CWinv; 
    y[28] = c[28]*126.243810*CWinv; 
    y[29] = c[29]*127.251780*CWinv; 
    y[30] = c[30]*140.270900*CWinv; 
    y[31] = c[31]*168.325080*CWinv; 
    y[32] = c[32]*201.331850*CWinv; 
    y[33] = c[33]*216.323280*CWinv; 
    y[34] = c[34]*28.013400*CWinv; 

    return;
}


/*get Cp/R as a function of T  */
/*for all species (Eq 19) */
void CKCPOR(double *  T, double *  cpor)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    cp_R(cpor, tc);
}


/*get H/RT as a function of T  */
/*for all species (Eq 20) */
void CKHORT(double *  T, double *  hort)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    speciesEnthalpy(hort, tc);
}


/*get S/R as a function of T  */
/*for all species (Eq 21) */
void CKSOR(double *  T, double *  sor)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    speciesEntropy(sor, tc);
}


/*get specific heat at constant volume as a function  */
/*of T for all species (molar units) */
void CKCVML(double *  T,  double *  cvml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    cv_R(cvml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        cvml[id] *= 8.31451e+07;
    }
}


/*get specific heat at constant pressure as a  */
/*function of T for all species (molar units) */
void CKCPML(double *  T,  double *  cpml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    cp_R(cpml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        cpml[id] *= 8.31451e+07;
    }
}


/*get internal energy as a function  */
/*of T for all species (molar units) */
void CKUML(double *  T,  double *  uml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesInternalEnergy(uml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        uml[id] *= RT;
    }
}


/*get enthalpy as a function  */
/*of T for all species (molar units) */
void CKHML(double *  T,  double *  hml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesEnthalpy(hml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        hml[id] *= RT;
    }
}


/*get standard-state Gibbs energy as a function  */
/*of T for all species (molar units) */
void CKGML(double *  T,  double *  gml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    gibbs(gml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        gml[id] *= RT;
    }
}


/*get standard-state Helmholtz free energy as a  */
/*function of T for all species (molar units) */
void CKAML(double *  T,  double *  aml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    helmholtz(aml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        aml[id] *= RT;
    }
}


/*Returns the standard-state entropies in molar units */
void CKSML(double *  T,  double *  sml)
{
    int id; /*loop counter */
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    speciesEntropy(sml, tc);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        sml[id] *= 8.31451e+07;
    }
}


/*Returns the specific heats at constant volume */
/*in mass units (Eq. 29) */
AMREX_GPU_HOST_DEVICE void CKCVMS(double *  T,  double *  cvms)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    cv_R(cvms, tc);
    /*multiply by R/molecularweight */
    cvms[0] *= 4.881096755203180e+05; /*NC12H26 */
    cvms[1] *= 8.248767324424338e+07; /*H */
    cvms[2] *= 5.196763628636074e+06; /*O */
    cvms[3] *= 4.888768810227566e+06; /*OH */
    cvms[4] *= 2.519031701678171e+06; /*HO2 */
    cvms[5] *= 4.124383662212169e+07; /*H2 */
    cvms[6] *= 4.615239012974499e+06; /*H2O */
    cvms[7] *= 2.444384405113783e+06; /*H2O2 */
    cvms[8] *= 2.598381814318037e+06; /*O2 */
    cvms[9] *= 5.530081023953346e+06; /*CH3 */
    cvms[10] *= 5.182630712527496e+06; /*CH4 */
    cvms[11] *= 2.769058254894261e+06; /*CH2O */
    cvms[12] *= 2.968349425484326e+06; /*CO */
    cvms[13] *= 1.889234139098090e+06; /*CO2 */
    cvms[14] *= 3.193192012977835e+06; /*C2H2 */
    cvms[15] *= 2.963733033722604e+06; /*C2H4 */
    cvms[16] *= 2.765040511976673e+06; /*C2H6 */
    cvms[17] *= 1.931558177477332e+06; /*CH2CHO */
    cvms[18] *= 2.024310196648431e+06; /*aC3H5 */
    cvms[19] *= 1.975822022481736e+06; /*C3H6 */
    cvms[20] *= 1.483019716673923e+06; /*C2H3CHO */
    cvms[21] *= 1.508974800359852e+06; /*C4H7 */
    cvms[22] *= 1.481866516861302e+06; /*C4H81 */
    cvms[23] *= 1.202779270993243e+06; /*C5H9 */
    cvms[24] *= 1.185493213489041e+06; /*C5H10 */
    cvms[25] *= 9.879110112408679e+05; /*C6H12 */
    cvms[26] *= 8.467808667778868e+05; /*C7H14 */
    cvms[27] *= 7.409332584306509e+05; /*C8H16 */
    cvms[28] *= 6.586073408272453e+05; /*C9H18 */
    cvms[29] *= 6.533904673081979e+05; /*PXC9H19 */
    cvms[30] *= 5.927466067445207e+05; /*C10H20 */
    cvms[31] *= 4.939555056204339e+05; /*C12H24 */
    cvms[32] *= 4.129753936101019e+05; /*C12H25O2 */
    cvms[33] *= 3.843557660553224e+05; /*OC12H23OOH */
    cvms[34] *= 2.968047434442088e+06; /*N2 */
}


/*Returns the specific heats at constant pressure */
/*in mass units (Eq. 26) */
AMREX_GPU_HOST_DEVICE void CKCPMS(double *  T,  double *  cpms)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    cp_R(cpms, tc);
    /*multiply by R/molecularweight */
    cpms[0] *= 4.881096755203180e+05; /*NC12H26 */
    cpms[1] *= 8.248767324424338e+07; /*H */
    cpms[2] *= 5.196763628636074e+06; /*O */
    cpms[3] *= 4.888768810227566e+06; /*OH */
    cpms[4] *= 2.519031701678171e+06; /*HO2 */
    cpms[5] *= 4.124383662212169e+07; /*H2 */
    cpms[6] *= 4.615239012974499e+06; /*H2O */
    cpms[7] *= 2.444384405113783e+06; /*H2O2 */
    cpms[8] *= 2.598381814318037e+06; /*O2 */
    cpms[9] *= 5.530081023953346e+06; /*CH3 */
    cpms[10] *= 5.182630712527496e+06; /*CH4 */
    cpms[11] *= 2.769058254894261e+06; /*CH2O */
    cpms[12] *= 2.968349425484326e+06; /*CO */
    cpms[13] *= 1.889234139098090e+06; /*CO2 */
    cpms[14] *= 3.193192012977835e+06; /*C2H2 */
    cpms[15] *= 2.963733033722604e+06; /*C2H4 */
    cpms[16] *= 2.765040511976673e+06; /*C2H6 */
    cpms[17] *= 1.931558177477332e+06; /*CH2CHO */
    cpms[18] *= 2.024310196648431e+06; /*aC3H5 */
    cpms[19] *= 1.975822022481736e+06; /*C3H6 */
    cpms[20] *= 1.483019716673923e+06; /*C2H3CHO */
    cpms[21] *= 1.508974800359852e+06; /*C4H7 */
    cpms[22] *= 1.481866516861302e+06; /*C4H81 */
    cpms[23] *= 1.202779270993243e+06; /*C5H9 */
    cpms[24] *= 1.185493213489041e+06; /*C5H10 */
    cpms[25] *= 9.879110112408679e+05; /*C6H12 */
    cpms[26] *= 8.467808667778868e+05; /*C7H14 */
    cpms[27] *= 7.409332584306509e+05; /*C8H16 */
    cpms[28] *= 6.586073408272453e+05; /*C9H18 */
    cpms[29] *= 6.533904673081979e+05; /*PXC9H19 */
    cpms[30] *= 5.927466067445207e+05; /*C10H20 */
    cpms[31] *= 4.939555056204339e+05; /*C12H24 */
    cpms[32] *= 4.129753936101019e+05; /*C12H25O2 */
    cpms[33] *= 3.843557660553224e+05; /*OC12H23OOH */
    cpms[34] *= 2.968047434442088e+06; /*N2 */
}


/*Returns internal energy in mass units (Eq 30.) */
AMREX_GPU_HOST_DEVICE void CKUMS(double *  T,  double *  ums)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesInternalEnergy(ums, tc);
    for (int i = 0; i < 35; i++)
    {
        ums[i] *= RT*imw[i];
    }
}


/*Returns enthalpy in mass units (Eq 27.) */
AMREX_GPU_HOST_DEVICE void CKHMS(double *  T,  double *  hms)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesEnthalpy(hms, tc);
    for (int i = 0; i < 35; i++)
    {
        hms[i] *= RT*imw[i];
    }
}


#ifndef AMREX_USE_CUDA
/*Returns enthalpy in mass units (Eq 27.) */
void VCKHMS(int *  np, double *  T,  double *  hms)
{
    double tc[5], h[35];

    for (int i=0; i<(*np); i++) {
        tc[0] = 0.0;
        tc[1] = T[i];
        tc[2] = T[i]*T[i];
        tc[3] = T[i]*T[i]*T[i];
        tc[4] = T[i]*T[i]*T[i]*T[i];

        speciesEnthalpy(h, tc);

        hms[0*(*np)+i] = h[0];
        hms[1*(*np)+i] = h[1];
        hms[2*(*np)+i] = h[2];
        hms[3*(*np)+i] = h[3];
        hms[4*(*np)+i] = h[4];
        hms[5*(*np)+i] = h[5];
        hms[6*(*np)+i] = h[6];
        hms[7*(*np)+i] = h[7];
        hms[8*(*np)+i] = h[8];
        hms[9*(*np)+i] = h[9];
        hms[10*(*np)+i] = h[10];
        hms[11*(*np)+i] = h[11];
        hms[12*(*np)+i] = h[12];
        hms[13*(*np)+i] = h[13];
        hms[14*(*np)+i] = h[14];
        hms[15*(*np)+i] = h[15];
        hms[16*(*np)+i] = h[16];
        hms[17*(*np)+i] = h[17];
        hms[18*(*np)+i] = h[18];
        hms[19*(*np)+i] = h[19];
        hms[20*(*np)+i] = h[20];
        hms[21*(*np)+i] = h[21];
        hms[22*(*np)+i] = h[22];
        hms[23*(*np)+i] = h[23];
        hms[24*(*np)+i] = h[24];
        hms[25*(*np)+i] = h[25];
        hms[26*(*np)+i] = h[26];
        hms[27*(*np)+i] = h[27];
        hms[28*(*np)+i] = h[28];
        hms[29*(*np)+i] = h[29];
        hms[30*(*np)+i] = h[30];
        hms[31*(*np)+i] = h[31];
        hms[32*(*np)+i] = h[32];
        hms[33*(*np)+i] = h[33];
        hms[34*(*np)+i] = h[34];
    }

    for (int n=0; n<35; n++) {
        for (int i=0; i<(*np); i++) {
            hms[n*(*np)+i] *= 8.31451e+07 * T[i] * imw[n];
        }
    }
}
#else
/*TODO: remove this on GPU */
void VCKHMS(int *  np, double *  T,  double *  hms)
{
}
#endif


/*Returns gibbs in mass units (Eq 31.) */
void CKGMS(double *  T,  double *  gms)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    gibbs(gms, tc);
    for (int i = 0; i < 35; i++)
    {
        gms[i] *= RT*imw[i];
    }
}


/*Returns helmholtz in mass units (Eq 32.) */
void CKAMS(double *  T,  double *  ams)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    helmholtz(ams, tc);
    for (int i = 0; i < 35; i++)
    {
        ams[i] *= RT*imw[i];
    }
}


/*Returns the entropies in mass units (Eq 28.) */
void CKSMS(double *  T,  double *  sms)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    speciesEntropy(sms, tc);
    /*multiply by R/molecularweight */
    sms[0] *= 4.881096755203180e+05; /*NC12H26 */
    sms[1] *= 8.248767324424338e+07; /*H */
    sms[2] *= 5.196763628636074e+06; /*O */
    sms[3] *= 4.888768810227566e+06; /*OH */
    sms[4] *= 2.519031701678171e+06; /*HO2 */
    sms[5] *= 4.124383662212169e+07; /*H2 */
    sms[6] *= 4.615239012974499e+06; /*H2O */
    sms[7] *= 2.444384405113783e+06; /*H2O2 */
    sms[8] *= 2.598381814318037e+06; /*O2 */
    sms[9] *= 5.530081023953346e+06; /*CH3 */
    sms[10] *= 5.182630712527496e+06; /*CH4 */
    sms[11] *= 2.769058254894261e+06; /*CH2O */
    sms[12] *= 2.968349425484326e+06; /*CO */
    sms[13] *= 1.889234139098090e+06; /*CO2 */
    sms[14] *= 3.193192012977835e+06; /*C2H2 */
    sms[15] *= 2.963733033722604e+06; /*C2H4 */
    sms[16] *= 2.765040511976673e+06; /*C2H6 */
    sms[17] *= 1.931558177477332e+06; /*CH2CHO */
    sms[18] *= 2.024310196648431e+06; /*aC3H5 */
    sms[19] *= 1.975822022481736e+06; /*C3H6 */
    sms[20] *= 1.483019716673923e+06; /*C2H3CHO */
    sms[21] *= 1.508974800359852e+06; /*C4H7 */
    sms[22] *= 1.481866516861302e+06; /*C4H81 */
    sms[23] *= 1.202779270993243e+06; /*C5H9 */
    sms[24] *= 1.185493213489041e+06; /*C5H10 */
    sms[25] *= 9.879110112408679e+05; /*C6H12 */
    sms[26] *= 8.467808667778868e+05; /*C7H14 */
    sms[27] *= 7.409332584306509e+05; /*C8H16 */
    sms[28] *= 6.586073408272453e+05; /*C9H18 */
    sms[29] *= 6.533904673081979e+05; /*PXC9H19 */
    sms[30] *= 5.927466067445207e+05; /*C10H20 */
    sms[31] *= 4.939555056204339e+05; /*C12H24 */
    sms[32] *= 4.129753936101019e+05; /*C12H25O2 */
    sms[33] *= 3.843557660553224e+05; /*OC12H23OOH */
    sms[34] *= 2.968047434442088e+06; /*N2 */
}


/*Returns the mean specific heat at CP (Eq. 33) */
void CKCPBL(double *  T, double *  x,  double *  cpbl)
{
    int id; /*loop counter */
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double cpor[35]; /* temporary storage */
    cp_R(cpor, tc);

    /*perform dot product */
    for (id = 0; id < 35; ++id) {
        result += x[id]*cpor[id];
    }

    *cpbl = result * 8.31451e+07;
}


/*Returns the mean specific heat at CP (Eq. 34) */
void CKCPBS(double *  T, double *  y,  double *  cpbs)
{
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double cpor[35], tresult[35]; /* temporary storage */
    cp_R(cpor, tc);
    for (int i = 0; i < 35; i++)
    {
        tresult[i] = cpor[i]*y[i]*imw[i];

    }
    for (int i = 0; i < 35; i++)
    {
        result += tresult[i];
    }

    *cpbs = result * 8.31451e+07;
}


/*Returns the mean specific heat at CV (Eq. 35) */
void CKCVBL(double *  T, double *  x,  double *  cvbl)
{
    int id; /*loop counter */
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double cvor[35]; /* temporary storage */
    cv_R(cvor, tc);

    /*perform dot product */
    for (id = 0; id < 35; ++id) {
        result += x[id]*cvor[id];
    }

    *cvbl = result * 8.31451e+07;
}


/*Returns the mean specific heat at CV (Eq. 36) */
AMREX_GPU_HOST_DEVICE void CKCVBS(double *  T, double *  y,  double *  cvbs)
{
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double cvor[35]; /* temporary storage */
    cv_R(cvor, tc);
    /*multiply by y/molecularweight */
    result += cvor[0]*y[0]*imw[0]; /*NC12H26 */
    result += cvor[1]*y[1]*imw[1]; /*H */
    result += cvor[2]*y[2]*imw[2]; /*O */
    result += cvor[3]*y[3]*imw[3]; /*OH */
    result += cvor[4]*y[4]*imw[4]; /*HO2 */
    result += cvor[5]*y[5]*imw[5]; /*H2 */
    result += cvor[6]*y[6]*imw[6]; /*H2O */
    result += cvor[7]*y[7]*imw[7]; /*H2O2 */
    result += cvor[8]*y[8]*imw[8]; /*O2 */
    result += cvor[9]*y[9]*imw[9]; /*CH3 */
    result += cvor[10]*y[10]*imw[10]; /*CH4 */
    result += cvor[11]*y[11]*imw[11]; /*CH2O */
    result += cvor[12]*y[12]*imw[12]; /*CO */
    result += cvor[13]*y[13]*imw[13]; /*CO2 */
    result += cvor[14]*y[14]*imw[14]; /*C2H2 */
    result += cvor[15]*y[15]*imw[15]; /*C2H4 */
    result += cvor[16]*y[16]*imw[16]; /*C2H6 */
    result += cvor[17]*y[17]*imw[17]; /*CH2CHO */
    result += cvor[18]*y[18]*imw[18]; /*aC3H5 */
    result += cvor[19]*y[19]*imw[19]; /*C3H6 */
    result += cvor[20]*y[20]*imw[20]; /*C2H3CHO */
    result += cvor[21]*y[21]*imw[21]; /*C4H7 */
    result += cvor[22]*y[22]*imw[22]; /*C4H81 */
    result += cvor[23]*y[23]*imw[23]; /*C5H9 */
    result += cvor[24]*y[24]*imw[24]; /*C5H10 */
    result += cvor[25]*y[25]*imw[25]; /*C6H12 */
    result += cvor[26]*y[26]*imw[26]; /*C7H14 */
    result += cvor[27]*y[27]*imw[27]; /*C8H16 */
    result += cvor[28]*y[28]*imw[28]; /*C9H18 */
    result += cvor[29]*y[29]*imw[29]; /*PXC9H19 */
    result += cvor[30]*y[30]*imw[30]; /*C10H20 */
    result += cvor[31]*y[31]*imw[31]; /*C12H24 */
    result += cvor[32]*y[32]*imw[32]; /*C12H25O2 */
    result += cvor[33]*y[33]*imw[33]; /*OC12H23OOH */
    result += cvor[34]*y[34]*imw[34]; /*N2 */

    *cvbs = result * 8.31451e+07;
}


/*Returns the mean enthalpy of the mixture in molar units */
void CKHBML(double *  T, double *  x,  double *  hbml)
{
    int id; /*loop counter */
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double hml[35]; /* temporary storage */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesEnthalpy(hml, tc);

    /*perform dot product */
    for (id = 0; id < 35; ++id) {
        result += x[id]*hml[id];
    }

    *hbml = result * RT;
}


/*Returns mean enthalpy of mixture in mass units */
void CKHBMS(double *  T, double *  y,  double *  hbms)
{
    double result = 0;
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double hml[35], tmp[35]; /* temporary storage */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesEnthalpy(hml, tc);
    int id;
    for (id = 0; id < 35; ++id) {
        tmp[id] = y[id]*hml[id]*imw[id];
    }
    for (id = 0; id < 35; ++id) {
        result += tmp[id];
    }

    *hbms = result * RT;
}


/*get mean internal energy in molar units */
void CKUBML(double *  T, double *  x,  double *  ubml)
{
    int id; /*loop counter */
    double result = 0; 
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double uml[35]; /* temporary energy array */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesInternalEnergy(uml, tc);

    /*perform dot product */
    for (id = 0; id < 35; ++id) {
        result += x[id]*uml[id];
    }

    *ubml = result * RT;
}


/*get mean internal energy in mass units */
AMREX_GPU_HOST_DEVICE void CKUBMS(double *  T, double *  y,  double *  ubms)
{
    double result = 0;
    double tT = *T; /*temporary temperature */
    double tc[] = { 0, tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double ums[35]; /* temporary energy array */
    double RT = 8.31451e+07*tT; /*R*T */
    speciesInternalEnergy(ums, tc);
    /*perform dot product + scaling by wt */
    result += y[0]*ums[0]*imw[0]; /*NC12H26 */
    result += y[1]*ums[1]*imw[1]; /*H */
    result += y[2]*ums[2]*imw[2]; /*O */
    result += y[3]*ums[3]*imw[3]; /*OH */
    result += y[4]*ums[4]*imw[4]; /*HO2 */
    result += y[5]*ums[5]*imw[5]; /*H2 */
    result += y[6]*ums[6]*imw[6]; /*H2O */
    result += y[7]*ums[7]*imw[7]; /*H2O2 */
    result += y[8]*ums[8]*imw[8]; /*O2 */
    result += y[9]*ums[9]*imw[9]; /*CH3 */
    result += y[10]*ums[10]*imw[10]; /*CH4 */
    result += y[11]*ums[11]*imw[11]; /*CH2O */
    result += y[12]*ums[12]*imw[12]; /*CO */
    result += y[13]*ums[13]*imw[13]; /*CO2 */
    result += y[14]*ums[14]*imw[14]; /*C2H2 */
    result += y[15]*ums[15]*imw[15]; /*C2H4 */
    result += y[16]*ums[16]*imw[16]; /*C2H6 */
    result += y[17]*ums[17]*imw[17]; /*CH2CHO */
    result += y[18]*ums[18]*imw[18]; /*aC3H5 */
    result += y[19]*ums[19]*imw[19]; /*C3H6 */
    result += y[20]*ums[20]*imw[20]; /*C2H3CHO */
    result += y[21]*ums[21]*imw[21]; /*C4H7 */
    result += y[22]*ums[22]*imw[22]; /*C4H81 */
    result += y[23]*ums[23]*imw[23]; /*C5H9 */
    result += y[24]*ums[24]*imw[24]; /*C5H10 */
    result += y[25]*ums[25]*imw[25]; /*C6H12 */
    result += y[26]*ums[26]*imw[26]; /*C7H14 */
    result += y[27]*ums[27]*imw[27]; /*C8H16 */
    result += y[28]*ums[28]*imw[28]; /*C9H18 */
    result += y[29]*ums[29]*imw[29]; /*PXC9H19 */
    result += y[30]*ums[30]*imw[30]; /*C10H20 */
    result += y[31]*ums[31]*imw[31]; /*C12H24 */
    result += y[32]*ums[32]*imw[32]; /*C12H25O2 */
    result += y[33]*ums[33]*imw[33]; /*OC12H23OOH */
    result += y[34]*ums[34]*imw[34]; /*N2 */

    *ubms = result * RT;
}


/*get mixture entropy in molar units */
void CKSBML(double *  P, double *  T, double *  x,  double *  sbml)
{
    int id; /*loop counter */
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double sor[35]; /* temporary storage */
    speciesEntropy(sor, tc);

    /*Compute Eq 42 */
    for (id = 0; id < 35; ++id) {
        result += x[id]*(sor[id]-log((x[id]+1e-100))-logPratio);
    }

    *sbml = result * 8.31451e+07;
}


/*get mixture entropy in mass units */
void CKSBMS(double *  P, double *  T, double *  y,  double *  sbms)
{
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double sor[35]; /* temporary storage */
    double x[35]; /* need a ytx conversion */
    double YOW = 0; /*See Eq 4, 6 in CK Manual */
    /*Compute inverse of mean molecular wt first */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    /*Now compute y to x conversion */
    x[0] = y[0]/(170.341020*YOW); 
    x[1] = y[1]/(1.007970*YOW); 
    x[2] = y[2]/(15.999400*YOW); 
    x[3] = y[3]/(17.007370*YOW); 
    x[4] = y[4]/(33.006770*YOW); 
    x[5] = y[5]/(2.015940*YOW); 
    x[6] = y[6]/(18.015340*YOW); 
    x[7] = y[7]/(34.014740*YOW); 
    x[8] = y[8]/(31.998800*YOW); 
    x[9] = y[9]/(15.035060*YOW); 
    x[10] = y[10]/(16.043030*YOW); 
    x[11] = y[11]/(30.026490*YOW); 
    x[12] = y[12]/(28.010550*YOW); 
    x[13] = y[13]/(44.009950*YOW); 
    x[14] = y[14]/(26.038240*YOW); 
    x[15] = y[15]/(28.054180*YOW); 
    x[16] = y[16]/(30.070120*YOW); 
    x[17] = y[17]/(43.045610*YOW); 
    x[18] = y[18]/(41.073300*YOW); 
    x[19] = y[19]/(42.081270*YOW); 
    x[20] = y[20]/(56.064730*YOW); 
    x[21] = y[21]/(55.100390*YOW); 
    x[22] = y[22]/(56.108360*YOW); 
    x[23] = y[23]/(69.127480*YOW); 
    x[24] = y[24]/(70.135450*YOW); 
    x[25] = y[25]/(84.162540*YOW); 
    x[26] = y[26]/(98.189630*YOW); 
    x[27] = y[27]/(112.216720*YOW); 
    x[28] = y[28]/(126.243810*YOW); 
    x[29] = y[29]/(127.251780*YOW); 
    x[30] = y[30]/(140.270900*YOW); 
    x[31] = y[31]/(168.325080*YOW); 
    x[32] = y[32]/(201.331850*YOW); 
    x[33] = y[33]/(216.323280*YOW); 
    x[34] = y[34]/(28.013400*YOW); 
    speciesEntropy(sor, tc);
    /*Perform computation in Eq 42 and 43 */
    result += x[0]*(sor[0]-log((x[0]+1e-100))-logPratio);
    result += x[1]*(sor[1]-log((x[1]+1e-100))-logPratio);
    result += x[2]*(sor[2]-log((x[2]+1e-100))-logPratio);
    result += x[3]*(sor[3]-log((x[3]+1e-100))-logPratio);
    result += x[4]*(sor[4]-log((x[4]+1e-100))-logPratio);
    result += x[5]*(sor[5]-log((x[5]+1e-100))-logPratio);
    result += x[6]*(sor[6]-log((x[6]+1e-100))-logPratio);
    result += x[7]*(sor[7]-log((x[7]+1e-100))-logPratio);
    result += x[8]*(sor[8]-log((x[8]+1e-100))-logPratio);
    result += x[9]*(sor[9]-log((x[9]+1e-100))-logPratio);
    result += x[10]*(sor[10]-log((x[10]+1e-100))-logPratio);
    result += x[11]*(sor[11]-log((x[11]+1e-100))-logPratio);
    result += x[12]*(sor[12]-log((x[12]+1e-100))-logPratio);
    result += x[13]*(sor[13]-log((x[13]+1e-100))-logPratio);
    result += x[14]*(sor[14]-log((x[14]+1e-100))-logPratio);
    result += x[15]*(sor[15]-log((x[15]+1e-100))-logPratio);
    result += x[16]*(sor[16]-log((x[16]+1e-100))-logPratio);
    result += x[17]*(sor[17]-log((x[17]+1e-100))-logPratio);
    result += x[18]*(sor[18]-log((x[18]+1e-100))-logPratio);
    result += x[19]*(sor[19]-log((x[19]+1e-100))-logPratio);
    result += x[20]*(sor[20]-log((x[20]+1e-100))-logPratio);
    result += x[21]*(sor[21]-log((x[21]+1e-100))-logPratio);
    result += x[22]*(sor[22]-log((x[22]+1e-100))-logPratio);
    result += x[23]*(sor[23]-log((x[23]+1e-100))-logPratio);
    result += x[24]*(sor[24]-log((x[24]+1e-100))-logPratio);
    result += x[25]*(sor[25]-log((x[25]+1e-100))-logPratio);
    result += x[26]*(sor[26]-log((x[26]+1e-100))-logPratio);
    result += x[27]*(sor[27]-log((x[27]+1e-100))-logPratio);
    result += x[28]*(sor[28]-log((x[28]+1e-100))-logPratio);
    result += x[29]*(sor[29]-log((x[29]+1e-100))-logPratio);
    result += x[30]*(sor[30]-log((x[30]+1e-100))-logPratio);
    result += x[31]*(sor[31]-log((x[31]+1e-100))-logPratio);
    result += x[32]*(sor[32]-log((x[32]+1e-100))-logPratio);
    result += x[33]*(sor[33]-log((x[33]+1e-100))-logPratio);
    result += x[34]*(sor[34]-log((x[34]+1e-100))-logPratio);
    /*Scale by R/W */
    *sbms = result * 8.31451e+07 * YOW;
}


/*Returns mean gibbs free energy in molar units */
void CKGBML(double *  P, double *  T, double *  x,  double *  gbml)
{
    int id; /*loop counter */
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    double gort[35]; /* temporary storage */
    /*Compute g/RT */
    gibbs(gort, tc);

    /*Compute Eq 44 */
    for (id = 0; id < 35; ++id) {
        result += x[id]*(gort[id]+log((x[id]+1e-100))+logPratio);
    }

    *gbml = result * RT;
}


/*Returns mixture gibbs free energy in mass units */
void CKGBMS(double *  P, double *  T, double *  y,  double *  gbms)
{
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    double gort[35]; /* temporary storage */
    double x[35]; /* need a ytx conversion */
    double YOW = 0; /*To hold 1/molecularweight */
    /*Compute inverse of mean molecular wt first */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    /*Now compute y to x conversion */
    x[0] = y[0]/(170.341020*YOW); 
    x[1] = y[1]/(1.007970*YOW); 
    x[2] = y[2]/(15.999400*YOW); 
    x[3] = y[3]/(17.007370*YOW); 
    x[4] = y[4]/(33.006770*YOW); 
    x[5] = y[5]/(2.015940*YOW); 
    x[6] = y[6]/(18.015340*YOW); 
    x[7] = y[7]/(34.014740*YOW); 
    x[8] = y[8]/(31.998800*YOW); 
    x[9] = y[9]/(15.035060*YOW); 
    x[10] = y[10]/(16.043030*YOW); 
    x[11] = y[11]/(30.026490*YOW); 
    x[12] = y[12]/(28.010550*YOW); 
    x[13] = y[13]/(44.009950*YOW); 
    x[14] = y[14]/(26.038240*YOW); 
    x[15] = y[15]/(28.054180*YOW); 
    x[16] = y[16]/(30.070120*YOW); 
    x[17] = y[17]/(43.045610*YOW); 
    x[18] = y[18]/(41.073300*YOW); 
    x[19] = y[19]/(42.081270*YOW); 
    x[20] = y[20]/(56.064730*YOW); 
    x[21] = y[21]/(55.100390*YOW); 
    x[22] = y[22]/(56.108360*YOW); 
    x[23] = y[23]/(69.127480*YOW); 
    x[24] = y[24]/(70.135450*YOW); 
    x[25] = y[25]/(84.162540*YOW); 
    x[26] = y[26]/(98.189630*YOW); 
    x[27] = y[27]/(112.216720*YOW); 
    x[28] = y[28]/(126.243810*YOW); 
    x[29] = y[29]/(127.251780*YOW); 
    x[30] = y[30]/(140.270900*YOW); 
    x[31] = y[31]/(168.325080*YOW); 
    x[32] = y[32]/(201.331850*YOW); 
    x[33] = y[33]/(216.323280*YOW); 
    x[34] = y[34]/(28.013400*YOW); 
    gibbs(gort, tc);
    /*Perform computation in Eq 44 */
    result += x[0]*(gort[0]+log((x[0]+1e-100))+logPratio);
    result += x[1]*(gort[1]+log((x[1]+1e-100))+logPratio);
    result += x[2]*(gort[2]+log((x[2]+1e-100))+logPratio);
    result += x[3]*(gort[3]+log((x[3]+1e-100))+logPratio);
    result += x[4]*(gort[4]+log((x[4]+1e-100))+logPratio);
    result += x[5]*(gort[5]+log((x[5]+1e-100))+logPratio);
    result += x[6]*(gort[6]+log((x[6]+1e-100))+logPratio);
    result += x[7]*(gort[7]+log((x[7]+1e-100))+logPratio);
    result += x[8]*(gort[8]+log((x[8]+1e-100))+logPratio);
    result += x[9]*(gort[9]+log((x[9]+1e-100))+logPratio);
    result += x[10]*(gort[10]+log((x[10]+1e-100))+logPratio);
    result += x[11]*(gort[11]+log((x[11]+1e-100))+logPratio);
    result += x[12]*(gort[12]+log((x[12]+1e-100))+logPratio);
    result += x[13]*(gort[13]+log((x[13]+1e-100))+logPratio);
    result += x[14]*(gort[14]+log((x[14]+1e-100))+logPratio);
    result += x[15]*(gort[15]+log((x[15]+1e-100))+logPratio);
    result += x[16]*(gort[16]+log((x[16]+1e-100))+logPratio);
    result += x[17]*(gort[17]+log((x[17]+1e-100))+logPratio);
    result += x[18]*(gort[18]+log((x[18]+1e-100))+logPratio);
    result += x[19]*(gort[19]+log((x[19]+1e-100))+logPratio);
    result += x[20]*(gort[20]+log((x[20]+1e-100))+logPratio);
    result += x[21]*(gort[21]+log((x[21]+1e-100))+logPratio);
    result += x[22]*(gort[22]+log((x[22]+1e-100))+logPratio);
    result += x[23]*(gort[23]+log((x[23]+1e-100))+logPratio);
    result += x[24]*(gort[24]+log((x[24]+1e-100))+logPratio);
    result += x[25]*(gort[25]+log((x[25]+1e-100))+logPratio);
    result += x[26]*(gort[26]+log((x[26]+1e-100))+logPratio);
    result += x[27]*(gort[27]+log((x[27]+1e-100))+logPratio);
    result += x[28]*(gort[28]+log((x[28]+1e-100))+logPratio);
    result += x[29]*(gort[29]+log((x[29]+1e-100))+logPratio);
    result += x[30]*(gort[30]+log((x[30]+1e-100))+logPratio);
    result += x[31]*(gort[31]+log((x[31]+1e-100))+logPratio);
    result += x[32]*(gort[32]+log((x[32]+1e-100))+logPratio);
    result += x[33]*(gort[33]+log((x[33]+1e-100))+logPratio);
    result += x[34]*(gort[34]+log((x[34]+1e-100))+logPratio);
    /*Scale by RT/W */
    *gbms = result * RT * YOW;
}


/*Returns mean helmholtz free energy in molar units */
void CKABML(double *  P, double *  T, double *  x,  double *  abml)
{
    int id; /*loop counter */
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    double aort[35]; /* temporary storage */
    /*Compute g/RT */
    helmholtz(aort, tc);

    /*Compute Eq 44 */
    for (id = 0; id < 35; ++id) {
        result += x[id]*(aort[id]+log((x[id]+1e-100))+logPratio);
    }

    *abml = result * RT;
}


/*Returns mixture helmholtz free energy in mass units */
void CKABMS(double *  P, double *  T, double *  y,  double *  abms)
{
    double result = 0; 
    /*Log of normalized pressure in cgs units dynes/cm^2 by Patm */
    double logPratio = log ( *P / 1013250.0 ); 
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double RT = 8.31451e+07*tT; /*R*T */
    double aort[35]; /* temporary storage */
    double x[35]; /* need a ytx conversion */
    double YOW = 0; /*To hold 1/molecularweight */
    /*Compute inverse of mean molecular wt first */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    /*Now compute y to x conversion */
    x[0] = y[0]/(170.341020*YOW); 
    x[1] = y[1]/(1.007970*YOW); 
    x[2] = y[2]/(15.999400*YOW); 
    x[3] = y[3]/(17.007370*YOW); 
    x[4] = y[4]/(33.006770*YOW); 
    x[5] = y[5]/(2.015940*YOW); 
    x[6] = y[6]/(18.015340*YOW); 
    x[7] = y[7]/(34.014740*YOW); 
    x[8] = y[8]/(31.998800*YOW); 
    x[9] = y[9]/(15.035060*YOW); 
    x[10] = y[10]/(16.043030*YOW); 
    x[11] = y[11]/(30.026490*YOW); 
    x[12] = y[12]/(28.010550*YOW); 
    x[13] = y[13]/(44.009950*YOW); 
    x[14] = y[14]/(26.038240*YOW); 
    x[15] = y[15]/(28.054180*YOW); 
    x[16] = y[16]/(30.070120*YOW); 
    x[17] = y[17]/(43.045610*YOW); 
    x[18] = y[18]/(41.073300*YOW); 
    x[19] = y[19]/(42.081270*YOW); 
    x[20] = y[20]/(56.064730*YOW); 
    x[21] = y[21]/(55.100390*YOW); 
    x[22] = y[22]/(56.108360*YOW); 
    x[23] = y[23]/(69.127480*YOW); 
    x[24] = y[24]/(70.135450*YOW); 
    x[25] = y[25]/(84.162540*YOW); 
    x[26] = y[26]/(98.189630*YOW); 
    x[27] = y[27]/(112.216720*YOW); 
    x[28] = y[28]/(126.243810*YOW); 
    x[29] = y[29]/(127.251780*YOW); 
    x[30] = y[30]/(140.270900*YOW); 
    x[31] = y[31]/(168.325080*YOW); 
    x[32] = y[32]/(201.331850*YOW); 
    x[33] = y[33]/(216.323280*YOW); 
    x[34] = y[34]/(28.013400*YOW); 
    helmholtz(aort, tc);
    /*Perform computation in Eq 44 */
    result += x[0]*(aort[0]+log((x[0]+1e-100))+logPratio);
    result += x[1]*(aort[1]+log((x[1]+1e-100))+logPratio);
    result += x[2]*(aort[2]+log((x[2]+1e-100))+logPratio);
    result += x[3]*(aort[3]+log((x[3]+1e-100))+logPratio);
    result += x[4]*(aort[4]+log((x[4]+1e-100))+logPratio);
    result += x[5]*(aort[5]+log((x[5]+1e-100))+logPratio);
    result += x[6]*(aort[6]+log((x[6]+1e-100))+logPratio);
    result += x[7]*(aort[7]+log((x[7]+1e-100))+logPratio);
    result += x[8]*(aort[8]+log((x[8]+1e-100))+logPratio);
    result += x[9]*(aort[9]+log((x[9]+1e-100))+logPratio);
    result += x[10]*(aort[10]+log((x[10]+1e-100))+logPratio);
    result += x[11]*(aort[11]+log((x[11]+1e-100))+logPratio);
    result += x[12]*(aort[12]+log((x[12]+1e-100))+logPratio);
    result += x[13]*(aort[13]+log((x[13]+1e-100))+logPratio);
    result += x[14]*(aort[14]+log((x[14]+1e-100))+logPratio);
    result += x[15]*(aort[15]+log((x[15]+1e-100))+logPratio);
    result += x[16]*(aort[16]+log((x[16]+1e-100))+logPratio);
    result += x[17]*(aort[17]+log((x[17]+1e-100))+logPratio);
    result += x[18]*(aort[18]+log((x[18]+1e-100))+logPratio);
    result += x[19]*(aort[19]+log((x[19]+1e-100))+logPratio);
    result += x[20]*(aort[20]+log((x[20]+1e-100))+logPratio);
    result += x[21]*(aort[21]+log((x[21]+1e-100))+logPratio);
    result += x[22]*(aort[22]+log((x[22]+1e-100))+logPratio);
    result += x[23]*(aort[23]+log((x[23]+1e-100))+logPratio);
    result += x[24]*(aort[24]+log((x[24]+1e-100))+logPratio);
    result += x[25]*(aort[25]+log((x[25]+1e-100))+logPratio);
    result += x[26]*(aort[26]+log((x[26]+1e-100))+logPratio);
    result += x[27]*(aort[27]+log((x[27]+1e-100))+logPratio);
    result += x[28]*(aort[28]+log((x[28]+1e-100))+logPratio);
    result += x[29]*(aort[29]+log((x[29]+1e-100))+logPratio);
    result += x[30]*(aort[30]+log((x[30]+1e-100))+logPratio);
    result += x[31]*(aort[31]+log((x[31]+1e-100))+logPratio);
    result += x[32]*(aort[32]+log((x[32]+1e-100))+logPratio);
    result += x[33]*(aort[33]+log((x[33]+1e-100))+logPratio);
    result += x[34]*(aort[34]+log((x[34]+1e-100))+logPratio);
    /*Scale by RT/W */
    *abms = result * RT * YOW;
}



/*Returns the molar production rate of species */
/*Given P, T, and mass fractions */
void CKWYP(double *  P, double *  T, double *  y,  double *  wdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double YOW = 0; 
    double PWORT; 
    /*Compute inverse of mean molecular wt first */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    /*PW/RT (see Eq. 7) */
    PWORT = (*P)/(YOW * 8.31451e+07 * (*T)); 
    /*multiply by 1e6 so c goes to SI */
    PWORT *= 1e6; 
    /*Now compute conversion (and go to SI) */
    c[0] = PWORT * y[0]*imw[0]; 
    c[1] = PWORT * y[1]*imw[1]; 
    c[2] = PWORT * y[2]*imw[2]; 
    c[3] = PWORT * y[3]*imw[3]; 
    c[4] = PWORT * y[4]*imw[4]; 
    c[5] = PWORT * y[5]*imw[5]; 
    c[6] = PWORT * y[6]*imw[6]; 
    c[7] = PWORT * y[7]*imw[7]; 
    c[8] = PWORT * y[8]*imw[8]; 
    c[9] = PWORT * y[9]*imw[9]; 
    c[10] = PWORT * y[10]*imw[10]; 
    c[11] = PWORT * y[11]*imw[11]; 
    c[12] = PWORT * y[12]*imw[12]; 
    c[13] = PWORT * y[13]*imw[13]; 
    c[14] = PWORT * y[14]*imw[14]; 
    c[15] = PWORT * y[15]*imw[15]; 
    c[16] = PWORT * y[16]*imw[16]; 
    c[17] = PWORT * y[17]*imw[17]; 
    c[18] = PWORT * y[18]*imw[18]; 
    c[19] = PWORT * y[19]*imw[19]; 
    c[20] = PWORT * y[20]*imw[20]; 
    c[21] = PWORT * y[21]*imw[21]; 
    c[22] = PWORT * y[22]*imw[22]; 
    c[23] = PWORT * y[23]*imw[23]; 
    c[24] = PWORT * y[24]*imw[24]; 
    c[25] = PWORT * y[25]*imw[25]; 
    c[26] = PWORT * y[26]*imw[26]; 
    c[27] = PWORT * y[27]*imw[27]; 
    c[28] = PWORT * y[28]*imw[28]; 
    c[29] = PWORT * y[29]*imw[29]; 
    c[30] = PWORT * y[30]*imw[30]; 
    c[31] = PWORT * y[31]*imw[31]; 
    c[32] = PWORT * y[32]*imw[32]; 
    c[33] = PWORT * y[33]*imw[33]; 
    c[34] = PWORT * y[34]*imw[34]; 

    /*convert to chemkin units */
    productionRate(wdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        wdot[id] *= 1.0e-6;
    }
}


/*Returns the molar production rate of species */
/*Given P, T, and mole fractions */
void CKWXP(double *  P, double *  T, double *  x,  double *  wdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double PORT = 1e6 * (*P)/(8.31451e+07 * (*T)); /*1e6 * P/RT so c goes to SI units */

    /*Compute conversion, see Eq 10 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*PORT;
    }

    /*convert to chemkin units */
    productionRate(wdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        wdot[id] *= 1.0e-6;
    }
}


/*Returns the molar production rate of species */
/*Given rho, T, and mass fractions */
AMREX_GPU_HOST_DEVICE void CKWYR(double *  rho, double *  T, double *  y,  double *  wdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    /*See Eq 8 with an extra 1e6 so c goes to SI */
    c[0] = 1e6 * (*rho) * y[0]*imw[0]; 
    c[1] = 1e6 * (*rho) * y[1]*imw[1]; 
    c[2] = 1e6 * (*rho) * y[2]*imw[2]; 
    c[3] = 1e6 * (*rho) * y[3]*imw[3]; 
    c[4] = 1e6 * (*rho) * y[4]*imw[4]; 
    c[5] = 1e6 * (*rho) * y[5]*imw[5]; 
    c[6] = 1e6 * (*rho) * y[6]*imw[6]; 
    c[7] = 1e6 * (*rho) * y[7]*imw[7]; 
    c[8] = 1e6 * (*rho) * y[8]*imw[8]; 
    c[9] = 1e6 * (*rho) * y[9]*imw[9]; 
    c[10] = 1e6 * (*rho) * y[10]*imw[10]; 
    c[11] = 1e6 * (*rho) * y[11]*imw[11]; 
    c[12] = 1e6 * (*rho) * y[12]*imw[12]; 
    c[13] = 1e6 * (*rho) * y[13]*imw[13]; 
    c[14] = 1e6 * (*rho) * y[14]*imw[14]; 
    c[15] = 1e6 * (*rho) * y[15]*imw[15]; 
    c[16] = 1e6 * (*rho) * y[16]*imw[16]; 
    c[17] = 1e6 * (*rho) * y[17]*imw[17]; 
    c[18] = 1e6 * (*rho) * y[18]*imw[18]; 
    c[19] = 1e6 * (*rho) * y[19]*imw[19]; 
    c[20] = 1e6 * (*rho) * y[20]*imw[20]; 
    c[21] = 1e6 * (*rho) * y[21]*imw[21]; 
    c[22] = 1e6 * (*rho) * y[22]*imw[22]; 
    c[23] = 1e6 * (*rho) * y[23]*imw[23]; 
    c[24] = 1e6 * (*rho) * y[24]*imw[24]; 
    c[25] = 1e6 * (*rho) * y[25]*imw[25]; 
    c[26] = 1e6 * (*rho) * y[26]*imw[26]; 
    c[27] = 1e6 * (*rho) * y[27]*imw[27]; 
    c[28] = 1e6 * (*rho) * y[28]*imw[28]; 
    c[29] = 1e6 * (*rho) * y[29]*imw[29]; 
    c[30] = 1e6 * (*rho) * y[30]*imw[30]; 
    c[31] = 1e6 * (*rho) * y[31]*imw[31]; 
    c[32] = 1e6 * (*rho) * y[32]*imw[32]; 
    c[33] = 1e6 * (*rho) * y[33]*imw[33]; 
    c[34] = 1e6 * (*rho) * y[34]*imw[34]; 

    /*call productionRate */
    productionRate(wdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        wdot[id] *= 1.0e-6;
    }
}


/*Returns the molar production rate of species */
/*Given rho, T, and mass fractions */
void VCKWYR(int *  np, double *  rho, double *  T,
	    double *  y,
	    double *  wdot)
{
#ifndef AMREX_USE_CUDA
    double c[35*(*np)]; /*temporary storage */
    /*See Eq 8 with an extra 1e6 so c goes to SI */
    for (int n=0; n<35; n++) {
        for (int i=0; i<(*np); i++) {
            c[n*(*np)+i] = 1.0e6 * rho[i] * y[n*(*np)+i] * imw[n];
        }
    }

    /*call productionRate */
    vproductionRate(*np, wdot, c, T);

    /*convert to chemkin units */
    for (int i=0; i<35*(*np); i++) {
        wdot[i] *= 1.0e-6;
    }
#endif
}


/*Returns the molar production rate of species */
/*Given rho, T, and mole fractions */
void CKWXR(double *  rho, double *  T, double *  x,  double *  wdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double XW = 0; /*See Eq 4, 11 in CK Manual */
    double ROW; 
    /*Compute mean molecular wt first */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    /*Extra 1e6 factor to take c to SI */
    ROW = 1e6*(*rho) / XW;

    /*Compute conversion, see Eq 11 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*ROW;
    }

    /*convert to chemkin units */
    productionRate(wdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        wdot[id] *= 1.0e-6;
    }
}


/*Returns the rate of progress for each reaction */
void CKQC(double *  T, double *  C, double *  qdot)
{
    int id; /*loop counter */

    /*convert to SI */
    for (id = 0; id < 35; ++id) {
        C[id] *= 1.0e6;
    }

    /*convert to chemkin units */
    progressRate(qdot, C, *T);

    /*convert to chemkin units */
    for (id = 0; id < 35; ++id) {
        C[id] *= 1.0e-6;
    }

    for (id = 0; id < 0; ++id) {
        qdot[id] *= 1.0e-6;
    }
}


/*Returns the progress rates of each reactions */
/*Given P, T, and mole fractions */
void CKKFKR(double *  P, double *  T, double *  x, double *  q_f, double *  q_r)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double PORT = 1e6 * (*P)/(8.31451e+07 * (*T)); /*1e6 * P/RT so c goes to SI units */

    /*Compute conversion, see Eq 10 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*PORT;
    }

    /*convert to chemkin units */
    progressRateFR(q_f, q_r, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 0; ++id) {
        q_f[id] *= 1.0e-6;
        q_r[id] *= 1.0e-6;
    }
}


/*Returns the progress rates of each reactions */
/*Given P, T, and mass fractions */
void CKQYP(double *  P, double *  T, double *  y, double *  qdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double YOW = 0; 
    double PWORT; 
    /*Compute inverse of mean molecular wt first */
    YOW += y[0]*imw[0]; /*NC12H26 */
    YOW += y[1]*imw[1]; /*H */
    YOW += y[2]*imw[2]; /*O */
    YOW += y[3]*imw[3]; /*OH */
    YOW += y[4]*imw[4]; /*HO2 */
    YOW += y[5]*imw[5]; /*H2 */
    YOW += y[6]*imw[6]; /*H2O */
    YOW += y[7]*imw[7]; /*H2O2 */
    YOW += y[8]*imw[8]; /*O2 */
    YOW += y[9]*imw[9]; /*CH3 */
    YOW += y[10]*imw[10]; /*CH4 */
    YOW += y[11]*imw[11]; /*CH2O */
    YOW += y[12]*imw[12]; /*CO */
    YOW += y[13]*imw[13]; /*CO2 */
    YOW += y[14]*imw[14]; /*C2H2 */
    YOW += y[15]*imw[15]; /*C2H4 */
    YOW += y[16]*imw[16]; /*C2H6 */
    YOW += y[17]*imw[17]; /*CH2CHO */
    YOW += y[18]*imw[18]; /*aC3H5 */
    YOW += y[19]*imw[19]; /*C3H6 */
    YOW += y[20]*imw[20]; /*C2H3CHO */
    YOW += y[21]*imw[21]; /*C4H7 */
    YOW += y[22]*imw[22]; /*C4H81 */
    YOW += y[23]*imw[23]; /*C5H9 */
    YOW += y[24]*imw[24]; /*C5H10 */
    YOW += y[25]*imw[25]; /*C6H12 */
    YOW += y[26]*imw[26]; /*C7H14 */
    YOW += y[27]*imw[27]; /*C8H16 */
    YOW += y[28]*imw[28]; /*C9H18 */
    YOW += y[29]*imw[29]; /*PXC9H19 */
    YOW += y[30]*imw[30]; /*C10H20 */
    YOW += y[31]*imw[31]; /*C12H24 */
    YOW += y[32]*imw[32]; /*C12H25O2 */
    YOW += y[33]*imw[33]; /*OC12H23OOH */
    YOW += y[34]*imw[34]; /*N2 */
    /*PW/RT (see Eq. 7) */
    PWORT = (*P)/(YOW * 8.31451e+07 * (*T)); 
    /*multiply by 1e6 so c goes to SI */
    PWORT *= 1e6; 
    /*Now compute conversion (and go to SI) */
    c[0] = PWORT * y[0]*imw[0]; 
    c[1] = PWORT * y[1]*imw[1]; 
    c[2] = PWORT * y[2]*imw[2]; 
    c[3] = PWORT * y[3]*imw[3]; 
    c[4] = PWORT * y[4]*imw[4]; 
    c[5] = PWORT * y[5]*imw[5]; 
    c[6] = PWORT * y[6]*imw[6]; 
    c[7] = PWORT * y[7]*imw[7]; 
    c[8] = PWORT * y[8]*imw[8]; 
    c[9] = PWORT * y[9]*imw[9]; 
    c[10] = PWORT * y[10]*imw[10]; 
    c[11] = PWORT * y[11]*imw[11]; 
    c[12] = PWORT * y[12]*imw[12]; 
    c[13] = PWORT * y[13]*imw[13]; 
    c[14] = PWORT * y[14]*imw[14]; 
    c[15] = PWORT * y[15]*imw[15]; 
    c[16] = PWORT * y[16]*imw[16]; 
    c[17] = PWORT * y[17]*imw[17]; 
    c[18] = PWORT * y[18]*imw[18]; 
    c[19] = PWORT * y[19]*imw[19]; 
    c[20] = PWORT * y[20]*imw[20]; 
    c[21] = PWORT * y[21]*imw[21]; 
    c[22] = PWORT * y[22]*imw[22]; 
    c[23] = PWORT * y[23]*imw[23]; 
    c[24] = PWORT * y[24]*imw[24]; 
    c[25] = PWORT * y[25]*imw[25]; 
    c[26] = PWORT * y[26]*imw[26]; 
    c[27] = PWORT * y[27]*imw[27]; 
    c[28] = PWORT * y[28]*imw[28]; 
    c[29] = PWORT * y[29]*imw[29]; 
    c[30] = PWORT * y[30]*imw[30]; 
    c[31] = PWORT * y[31]*imw[31]; 
    c[32] = PWORT * y[32]*imw[32]; 
    c[33] = PWORT * y[33]*imw[33]; 
    c[34] = PWORT * y[34]*imw[34]; 

    /*convert to chemkin units */
    progressRate(qdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 0; ++id) {
        qdot[id] *= 1.0e-6;
    }
}


/*Returns the progress rates of each reactions */
/*Given P, T, and mole fractions */
void CKQXP(double *  P, double *  T, double *  x, double *  qdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double PORT = 1e6 * (*P)/(8.31451e+07 * (*T)); /*1e6 * P/RT so c goes to SI units */

    /*Compute conversion, see Eq 10 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*PORT;
    }

    /*convert to chemkin units */
    progressRate(qdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 0; ++id) {
        qdot[id] *= 1.0e-6;
    }
}


/*Returns the progress rates of each reactions */
/*Given rho, T, and mass fractions */
void CKQYR(double *  rho, double *  T, double *  y, double *  qdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    /*See Eq 8 with an extra 1e6 so c goes to SI */
    c[0] = 1e6 * (*rho) * y[0]*imw[0]; 
    c[1] = 1e6 * (*rho) * y[1]*imw[1]; 
    c[2] = 1e6 * (*rho) * y[2]*imw[2]; 
    c[3] = 1e6 * (*rho) * y[3]*imw[3]; 
    c[4] = 1e6 * (*rho) * y[4]*imw[4]; 
    c[5] = 1e6 * (*rho) * y[5]*imw[5]; 
    c[6] = 1e6 * (*rho) * y[6]*imw[6]; 
    c[7] = 1e6 * (*rho) * y[7]*imw[7]; 
    c[8] = 1e6 * (*rho) * y[8]*imw[8]; 
    c[9] = 1e6 * (*rho) * y[9]*imw[9]; 
    c[10] = 1e6 * (*rho) * y[10]*imw[10]; 
    c[11] = 1e6 * (*rho) * y[11]*imw[11]; 
    c[12] = 1e6 * (*rho) * y[12]*imw[12]; 
    c[13] = 1e6 * (*rho) * y[13]*imw[13]; 
    c[14] = 1e6 * (*rho) * y[14]*imw[14]; 
    c[15] = 1e6 * (*rho) * y[15]*imw[15]; 
    c[16] = 1e6 * (*rho) * y[16]*imw[16]; 
    c[17] = 1e6 * (*rho) * y[17]*imw[17]; 
    c[18] = 1e6 * (*rho) * y[18]*imw[18]; 
    c[19] = 1e6 * (*rho) * y[19]*imw[19]; 
    c[20] = 1e6 * (*rho) * y[20]*imw[20]; 
    c[21] = 1e6 * (*rho) * y[21]*imw[21]; 
    c[22] = 1e6 * (*rho) * y[22]*imw[22]; 
    c[23] = 1e6 * (*rho) * y[23]*imw[23]; 
    c[24] = 1e6 * (*rho) * y[24]*imw[24]; 
    c[25] = 1e6 * (*rho) * y[25]*imw[25]; 
    c[26] = 1e6 * (*rho) * y[26]*imw[26]; 
    c[27] = 1e6 * (*rho) * y[27]*imw[27]; 
    c[28] = 1e6 * (*rho) * y[28]*imw[28]; 
    c[29] = 1e6 * (*rho) * y[29]*imw[29]; 
    c[30] = 1e6 * (*rho) * y[30]*imw[30]; 
    c[31] = 1e6 * (*rho) * y[31]*imw[31]; 
    c[32] = 1e6 * (*rho) * y[32]*imw[32]; 
    c[33] = 1e6 * (*rho) * y[33]*imw[33]; 
    c[34] = 1e6 * (*rho) * y[34]*imw[34]; 

    /*call progressRate */
    progressRate(qdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 0; ++id) {
        qdot[id] *= 1.0e-6;
    }
}


/*Returns the progress rates of each reactions */
/*Given rho, T, and mole fractions */
void CKQXR(double *  rho, double *  T, double *  x, double *  qdot)
{
    int id; /*loop counter */
    double c[35]; /*temporary storage */
    double XW = 0; /*See Eq 4, 11 in CK Manual */
    double ROW; 
    /*Compute mean molecular wt first */
    XW += x[0]*170.341020; /*NC12H26 */
    XW += x[1]*1.007970; /*H */
    XW += x[2]*15.999400; /*O */
    XW += x[3]*17.007370; /*OH */
    XW += x[4]*33.006770; /*HO2 */
    XW += x[5]*2.015940; /*H2 */
    XW += x[6]*18.015340; /*H2O */
    XW += x[7]*34.014740; /*H2O2 */
    XW += x[8]*31.998800; /*O2 */
    XW += x[9]*15.035060; /*CH3 */
    XW += x[10]*16.043030; /*CH4 */
    XW += x[11]*30.026490; /*CH2O */
    XW += x[12]*28.010550; /*CO */
    XW += x[13]*44.009950; /*CO2 */
    XW += x[14]*26.038240; /*C2H2 */
    XW += x[15]*28.054180; /*C2H4 */
    XW += x[16]*30.070120; /*C2H6 */
    XW += x[17]*43.045610; /*CH2CHO */
    XW += x[18]*41.073300; /*aC3H5 */
    XW += x[19]*42.081270; /*C3H6 */
    XW += x[20]*56.064730; /*C2H3CHO */
    XW += x[21]*55.100390; /*C4H7 */
    XW += x[22]*56.108360; /*C4H81 */
    XW += x[23]*69.127480; /*C5H9 */
    XW += x[24]*70.135450; /*C5H10 */
    XW += x[25]*84.162540; /*C6H12 */
    XW += x[26]*98.189630; /*C7H14 */
    XW += x[27]*112.216720; /*C8H16 */
    XW += x[28]*126.243810; /*C9H18 */
    XW += x[29]*127.251780; /*PXC9H19 */
    XW += x[30]*140.270900; /*C10H20 */
    XW += x[31]*168.325080; /*C12H24 */
    XW += x[32]*201.331850; /*C12H25O2 */
    XW += x[33]*216.323280; /*OC12H23OOH */
    XW += x[34]*28.013400; /*N2 */
    /*Extra 1e6 factor to take c to SI */
    ROW = 1e6*(*rho) / XW;

    /*Compute conversion, see Eq 11 */
    for (id = 0; id < 35; ++id) {
        c[id] = x[id]*ROW;
    }

    /*convert to chemkin units */
    progressRate(qdot, c, *T);

    /*convert to chemkin units */
    for (id = 0; id < 0; ++id) {
        qdot[id] *= 1.0e-6;
    }
}


/*Returns the stoichiometric coefficients */
/*of the reaction mechanism. (Eq 50) */
void CKNU(int * kdim,  int * nuki)
{
    int id; /*loop counter */
    int kd = (*kdim); 
    /*Zero nuki */
    for (id = 0; id < 35 * kd; ++ id) {
         nuki[id] = 0; 
    }
}


/*Returns the elemental composition  */
/*of the speciesi (mdim is num of elements) */
void CKNCF(int * mdim,  int * ncf)
{
    int id; /*loop counter */
    int kd = (*mdim); 
    /*Zero ncf */
    for (id = 0; id < kd * 35; ++ id) {
         ncf[id] = 0; 
    }

    /*NC12H26 */
    ncf[ 0 * kd + 2 ] = 12; /*C */
    ncf[ 0 * kd + 1 ] = 26; /*H */

    /*H */
    ncf[ 1 * kd + 1 ] = 1; /*H */

    /*O */
    ncf[ 2 * kd + 0 ] = 1; /*O */

    /*OH */
    ncf[ 3 * kd + 0 ] = 1; /*O */
    ncf[ 3 * kd + 1 ] = 1; /*H */

    /*HO2 */
    ncf[ 4 * kd + 1 ] = 1; /*H */
    ncf[ 4 * kd + 0 ] = 2; /*O */

    /*H2 */
    ncf[ 5 * kd + 1 ] = 2; /*H */

    /*H2O */
    ncf[ 6 * kd + 1 ] = 2; /*H */
    ncf[ 6 * kd + 0 ] = 1; /*O */

    /*H2O2 */
    ncf[ 7 * kd + 1 ] = 2; /*H */
    ncf[ 7 * kd + 0 ] = 2; /*O */

    /*O2 */
    ncf[ 8 * kd + 0 ] = 2; /*O */

    /*CH3 */
    ncf[ 9 * kd + 2 ] = 1; /*C */
    ncf[ 9 * kd + 1 ] = 3; /*H */

    /*CH4 */
    ncf[ 10 * kd + 2 ] = 1; /*C */
    ncf[ 10 * kd + 1 ] = 4; /*H */

    /*CH2O */
    ncf[ 11 * kd + 1 ] = 2; /*H */
    ncf[ 11 * kd + 2 ] = 1; /*C */
    ncf[ 11 * kd + 0 ] = 1; /*O */

    /*CO */
    ncf[ 12 * kd + 2 ] = 1; /*C */
    ncf[ 12 * kd + 0 ] = 1; /*O */

    /*CO2 */
    ncf[ 13 * kd + 2 ] = 1; /*C */
    ncf[ 13 * kd + 0 ] = 2; /*O */

    /*C2H2 */
    ncf[ 14 * kd + 2 ] = 2; /*C */
    ncf[ 14 * kd + 1 ] = 2; /*H */

    /*C2H4 */
    ncf[ 15 * kd + 2 ] = 2; /*C */
    ncf[ 15 * kd + 1 ] = 4; /*H */

    /*C2H6 */
    ncf[ 16 * kd + 2 ] = 2; /*C */
    ncf[ 16 * kd + 1 ] = 6; /*H */

    /*CH2CHO */
    ncf[ 17 * kd + 0 ] = 1; /*O */
    ncf[ 17 * kd + 1 ] = 3; /*H */
    ncf[ 17 * kd + 2 ] = 2; /*C */

    /*aC3H5 */
    ncf[ 18 * kd + 2 ] = 3; /*C */
    ncf[ 18 * kd + 1 ] = 5; /*H */

    /*C3H6 */
    ncf[ 19 * kd + 2 ] = 3; /*C */
    ncf[ 19 * kd + 1 ] = 6; /*H */

    /*C2H3CHO */
    ncf[ 20 * kd + 2 ] = 3; /*C */
    ncf[ 20 * kd + 1 ] = 4; /*H */
    ncf[ 20 * kd + 0 ] = 1; /*O */

    /*C4H7 */
    ncf[ 21 * kd + 2 ] = 4; /*C */
    ncf[ 21 * kd + 1 ] = 7; /*H */

    /*C4H81 */
    ncf[ 22 * kd + 2 ] = 4; /*C */
    ncf[ 22 * kd + 1 ] = 8; /*H */

    /*C5H9 */
    ncf[ 23 * kd + 1 ] = 9; /*H */
    ncf[ 23 * kd + 2 ] = 5; /*C */

    /*C5H10 */
    ncf[ 24 * kd + 2 ] = 5; /*C */
    ncf[ 24 * kd + 1 ] = 10; /*H */

    /*C6H12 */
    ncf[ 25 * kd + 2 ] = 6; /*C */
    ncf[ 25 * kd + 1 ] = 12; /*H */

    /*C7H14 */
    ncf[ 26 * kd + 2 ] = 7; /*C */
    ncf[ 26 * kd + 1 ] = 14; /*H */

    /*C8H16 */
    ncf[ 27 * kd + 2 ] = 8; /*C */
    ncf[ 27 * kd + 1 ] = 16; /*H */

    /*C9H18 */
    ncf[ 28 * kd + 2 ] = 9; /*C */
    ncf[ 28 * kd + 1 ] = 18; /*H */

    /*PXC9H19 */
    ncf[ 29 * kd + 2 ] = 9; /*C */
    ncf[ 29 * kd + 1 ] = 19; /*H */

    /*C10H20 */
    ncf[ 30 * kd + 2 ] = 10; /*C */
    ncf[ 30 * kd + 1 ] = 20; /*H */

    /*C12H24 */
    ncf[ 31 * kd + 2 ] = 12; /*C */
    ncf[ 31 * kd + 1 ] = 24; /*H */

    /*C12H25O2 */
    ncf[ 32 * kd + 1 ] = 25; /*H */
    ncf[ 32 * kd + 0 ] = 2; /*O */
    ncf[ 32 * kd + 2 ] = 12; /*C */

    /*OC12H23OOH */
    ncf[ 33 * kd + 1 ] = 24; /*H */
    ncf[ 33 * kd + 0 ] = 3; /*O */
    ncf[ 33 * kd + 2 ] = 12; /*C */

    /*N2 */
    ncf[ 34 * kd + 3 ] = 2; /*N */

}


/*Returns the arrehenius coefficients  */
/*for all reactions */
void CKABE( double *  a, double *  b, double *  e)
{

    return;
}


/*Returns the equil constants for each reaction */
void CKEQC(double *  T, double *  C, double *  eqcon)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double gort[35]; /* temporary storage */

    /*compute the Gibbs free energy */
    gibbs(gort, tc);

    /*compute the equilibrium constants */
    equilibriumConstants(eqcon, gort, tT);
}


/*Returns the equil constants for each reaction */
/*Given P, T, and mass fractions */
void CKEQYP(double *  P, double *  T, double *  y, double *  eqcon)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double gort[35]; /* temporary storage */

    /*compute the Gibbs free energy */
    gibbs(gort, tc);

    /*compute the equilibrium constants */
    equilibriumConstants(eqcon, gort, tT);
}


/*Returns the equil constants for each reaction */
/*Given P, T, and mole fractions */
void CKEQXP(double *  P, double *  T, double *  x, double *  eqcon)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double gort[35]; /* temporary storage */

    /*compute the Gibbs free energy */
    gibbs(gort, tc);

    /*compute the equilibrium constants */
    equilibriumConstants(eqcon, gort, tT);
}


/*Returns the equil constants for each reaction */
/*Given rho, T, and mass fractions */
void CKEQYR(double *  rho, double *  T, double *  y, double *  eqcon)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double gort[35]; /* temporary storage */

    /*compute the Gibbs free energy */
    gibbs(gort, tc);

    /*compute the equilibrium constants */
    equilibriumConstants(eqcon, gort, tT);
}


/*Returns the equil constants for each reaction */
/*Given rho, T, and mole fractions */
void CKEQXR(double *  rho, double *  T, double *  x, double *  eqcon)
{
    double tT = *T; /*temporary temperature */
    double tc[] = { log(tT), tT, tT*tT, tT*tT*tT, tT*tT*tT*tT }; /*temperature cache */
    double gort[35]; /* temporary storage */

    /*compute the Gibbs free energy */
    gibbs(gort, tc);

    /*compute the equilibrium constants */
    equilibriumConstants(eqcon, gort, tT);
}

#ifdef AMREX_USE_CUDA
/*GPU version of productionRate: no more use of thermo namespace vectors */
/*compute the production rate for each species */
AMREX_GPU_HOST_DEVICE inline void  productionRate(double * wdot, double * sc, double T)
{
    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];

    double qdot, q_f[0], q_r[0];
    comp_qfqr(q_f, q_r, sc, tc, invT);

    for (int i = 0; i < 35; ++i) {
        wdot[i] = 0.0;
    }

    return;
}

AMREX_GPU_HOST_DEVICE inline void comp_qfqr(double *  qf, double * qr, double * sc, double * tc, double invT)
{

    /*compute the mixture concentration */
    double mixture = 0.0;
    for (int i = 0; i < 35; ++i) {
        mixture += sc[i];
    }

    /*compute the Gibbs free energy */
    double g_RT[35];
    gibbs(g_RT, tc);

    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 * invT;
    double refCinv = 1 / refC;

    /* Evaluate the kfs */
    double k_f, Corr;



    return;
}
#endif


#ifndef AMREX_USE_CUDA
static double T_save = -1;
#ifdef _OPENMP
#pragma omp threadprivate(T_save)
#endif

static double k_f_save[0];
#ifdef _OPENMP
#pragma omp threadprivate(k_f_save)
#endif

static double Kc_save[0];
#ifdef _OPENMP
#pragma omp threadprivate(Kc_save)
#endif


/*compute the production rate for each species pointwise on CPU */
void productionRate(double *  wdot, double *  sc, double T)
{
    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];

    if (T != T_save)
    {
        T_save = T;
        comp_k_f(tc,invT,k_f_save);
        comp_Kc(tc,invT,Kc_save);
    }

    double qdot, q_f[0], q_r[0];
    comp_qfqr(q_f, q_r, sc, tc, invT);

    for (int i = 0; i < 35; ++i) {
        wdot[i] = 0.0;
    }

    return;
}

void comp_k_f(double *  tc, double invT, double *  k_f)
{
    for (int i=0; i<0; ++i) {
        k_f[i] = prefactor_units[i] * fwd_A[i]
                    * exp(fwd_beta[i] * tc[0] - activation_units[i] * fwd_Ea[i] * invT);
    };
    return;
}
void comp_Kc(double *  tc, double invT, double *  Kc)
{
    /*compute the Gibbs free energy */
    double g_RT[35];
    gibbs(g_RT, tc);


    for (int i=0; i<0; ++i) {
        Kc[i] = exp(Kc[i]);
    };

    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 * invT;
    double refCinv = 1 / refC;


    return;
}

void comp_qfqr(double *  qf, double *  qr, double *  sc, double *  tc, double invT)
{

    double T = tc[1];

    /*compute the mixture concentration */
    double mixture = 0.0;
    for (int i = 0; i < 35; ++i) {
        mixture += sc[i];
    }

    double Corr[0];
    for (int i = 0; i < 0; ++i) {
        Corr[i] = 1.0;
    }

    for (int i=0; i<0; i++)
    {
        qf[i] *= Corr[i] * k_f_save[i];
        qr[i] *= Corr[i] * k_f_save[i] / Kc_save[i];
    }

    return;
}
#endif


#ifndef AMREX_USE_CUDA
/*compute the production rate for each species */
void vproductionRate(int npt, double *  wdot, double *  sc, double *  T)
{
    double k_f_s[0*npt], Kc_s[0*npt], mixture[npt], g_RT[35*npt];
    double tc[5*npt], invT[npt];

#ifdef __INTEL_COMPILER
     #pragma simd
#endif
    for (int i=0; i<npt; i++) {
        tc[0*npt+i] = log(T[i]);
        tc[1*npt+i] = T[i];
        tc[2*npt+i] = T[i]*T[i];
        tc[3*npt+i] = T[i]*T[i]*T[i];
        tc[4*npt+i] = T[i]*T[i]*T[i]*T[i];
        invT[i] = 1.0 / T[i];
    }

    for (int i=0; i<npt; i++) {
        mixture[i] = 0.0;
    }

    for (int n=0; n<35; n++) {
        for (int i=0; i<npt; i++) {
            mixture[i] += sc[n*npt+i];
            wdot[n*npt+i] = 0.0;
        }
    }

    vcomp_k_f(npt, k_f_s, tc, invT);

    vcomp_gibbs(npt, g_RT, tc);

    vcomp_Kc(npt, Kc_s, g_RT, invT);

    vcomp_wdot(npt, wdot, mixture, sc, k_f_s, Kc_s, tc, invT, T);
}

void vcomp_k_f(int npt, double *  k_f_s, double *  tc, double *  invT)
{
}

void vcomp_gibbs(int npt, double *  g_RT, double *  tc)
{
    /*compute the Gibbs free energy */
    for (int i=0; i<npt; i++) {
        double tg[5], g[35];
        tg[0] = tc[0*npt+i];
        tg[1] = tc[1*npt+i];
        tg[2] = tc[2*npt+i];
        tg[3] = tc[3*npt+i];
        tg[4] = tc[4*npt+i];

        gibbs(g, tg);

        g_RT[0*npt+i] = g[0];
        g_RT[1*npt+i] = g[1];
        g_RT[2*npt+i] = g[2];
        g_RT[3*npt+i] = g[3];
        g_RT[4*npt+i] = g[4];
        g_RT[5*npt+i] = g[5];
        g_RT[6*npt+i] = g[6];
        g_RT[7*npt+i] = g[7];
        g_RT[8*npt+i] = g[8];
        g_RT[9*npt+i] = g[9];
        g_RT[10*npt+i] = g[10];
        g_RT[11*npt+i] = g[11];
        g_RT[12*npt+i] = g[12];
        g_RT[13*npt+i] = g[13];
        g_RT[14*npt+i] = g[14];
        g_RT[15*npt+i] = g[15];
        g_RT[16*npt+i] = g[16];
        g_RT[17*npt+i] = g[17];
        g_RT[18*npt+i] = g[18];
        g_RT[19*npt+i] = g[19];
        g_RT[20*npt+i] = g[20];
        g_RT[21*npt+i] = g[21];
        g_RT[22*npt+i] = g[22];
        g_RT[23*npt+i] = g[23];
        g_RT[24*npt+i] = g[24];
        g_RT[25*npt+i] = g[25];
        g_RT[26*npt+i] = g[26];
        g_RT[27*npt+i] = g[27];
        g_RT[28*npt+i] = g[28];
        g_RT[29*npt+i] = g[29];
        g_RT[30*npt+i] = g[30];
        g_RT[31*npt+i] = g[31];
        g_RT[32*npt+i] = g[32];
        g_RT[33*npt+i] = g[33];
        g_RT[34*npt+i] = g[34];
    }
}

void vcomp_Kc(int npt, double *  Kc_s, double *  g_RT, double *  invT)
{
}

void vcomp_wdot(int npt, double *  wdot, double *  mixture, double *  sc,
		double *  k_f_s, double *  Kc_s,
		double *  tc, double *  invT, double *  T)
{
}
#endif

/*compute an approx to the reaction Jacobian */
AMREX_GPU_HOST_DEVICE void DWDOT_PRECOND(double *  J, double *  sc, double *  Tp, int * HP)
{
    double c[35];

    for (int k=0; k<35; k++) {
        c[k] = 1.e6 * sc[k];
    }

    aJacobian_precond(J, c, *Tp, *HP);

    /* dwdot[k]/dT */
    /* dTdot/d[X] */
    for (int k=0; k<35; k++) {
        J[1260+k] *= 1.e-6;
        J[k*36+35] *= 1.e6;
    }

    return;
}

/*compute an approx to the SPS Jacobian */
AMREX_GPU_HOST_DEVICE void SLJ_PRECOND_CSC(double *  Jsps, int * indx, int * len, double * sc, double * Tp, int * HP, double * gamma)
{
    double c[35];
    double J[1296];
    double mwt[35];

    molecularWeight(mwt);

    for (int k=0; k<35; k++) {
        c[k] = 1.e6 * sc[k];
    }

    aJacobian_precond(J, c, *Tp, *HP);

    /* Change of coord */
    /* dwdot[k]/dT */
    /* dTdot/d[X] */
    for (int k=0; k<35; k++) {
        J[1260+k] = 1.e-6 * J[1260+k] * mwt[k];
        J[k*36+35] = 1.e6 * J[k*36+35] / mwt[k];
    }
    /* dTdot/dT */
    /* dwdot[l]/[k] */
    for (int k=0; k<35; k++) {
        for (int l=0; l<35; l++) {
            /* DIAG elem */
            if (k == l){
                J[ 36 * k + l] =  J[ 36 * k + l] * mwt[l] / mwt[k];
            /* NOT DIAG and not last column nor last row */
            } else {
                J[ 36 * k + l] =  J[ 36 * k + l] * mwt[l] / mwt[k];
            }
        }
    }

    for (int k=0; k<(*len); k++) {
        Jsps[k] = J[indx[k]];
    }

    return;
}

/*compute the reaction Jacobian */
AMREX_GPU_HOST_DEVICE void DWDOT(double *  J, double *  sc, double *  Tp, int * consP)
{
    double c[35];

    for (int k=0; k<35; k++) {
        c[k] = 1.e6 * sc[k];
    }

    aJacobian(J, c, *Tp, *consP);

    /* dwdot[k]/dT */
    /* dTdot/d[X] */
    for (int k=0; k<35; k++) {
        J[1260+k] *= 1.e-6;
        J[k*36+35] *= 1.e6;
    }

    return;
}

/*compute the sparsity pattern Jacobian */
AMREX_GPU_HOST_DEVICE void SPARSITY_INFO( int * nJdata, int * consP, int NCELLS)
{
    double c[35];
    double J[1296];

    for (int k=0; k<35; k++) {
        c[k] = 1.0/ 35.000000 ;
    }

    aJacobian(J, c, 1500.0, *consP);

    int nJdata_tmp = 0;
    for (int k=0; k<36; k++) {
        for (int l=0; l<36; l++) {
            if(J[ 36 * k + l] != 0.0){
                nJdata_tmp = nJdata_tmp + 1;
            }
        }
    }

    *nJdata = NCELLS * nJdata_tmp;

    return;
}



/*compute the sparsity pattern of simplified Jacobian */
AMREX_GPU_HOST_DEVICE void SPARSITY_INFO_PRECOND( int * nJdata, int * consP)
{
    double c[35];
    double J[1296];

    for (int k=0; k<35; k++) {
        c[k] = 1.0/ 35.000000 ;
    }

    aJacobian_precond(J, c, 1500.0, *consP);

    int nJdata_tmp = 0;
    for (int k=0; k<36; k++) {
        for (int l=0; l<36; l++) {
            if(k == l){
                nJdata_tmp = nJdata_tmp + 1;
            } else {
                if(J[ 36 * k + l] != 0.0){
                    nJdata_tmp = nJdata_tmp + 1;
                }
            }
        }
    }

    nJdata[0] = nJdata_tmp;

    return;
}


#ifndef AMREX_USE_CUDA
/*compute the sparsity pattern of the simplified precond Jacobian on CPU */
void SPARSITY_PREPROC_PRECOND(int * rowVals, int * colPtrs, int * indx, int * consP)
{
    double c[35];
    double J[1296];

    for (int k=0; k<35; k++) {
        c[k] = 1.0/ 35.000000 ;
    }

    aJacobian_precond(J, c, 1500.0, *consP);

    colPtrs[0] = 0;
    int nJdata_tmp = 0;
    for (int k=0; k<36; k++) {
        for (int l=0; l<36; l++) {
            if (k == l) {
                rowVals[nJdata_tmp] = l; 
                indx[nJdata_tmp] = 36*k + l;
                nJdata_tmp = nJdata_tmp + 1; 
            } else {
                if(J[36*k + l] != 0.0) {
                    rowVals[nJdata_tmp] = l; 
                    indx[nJdata_tmp] = 36*k + l;
                    nJdata_tmp = nJdata_tmp + 1; 
                }
            }
        }
        colPtrs[k+1] = nJdata_tmp;
    }

    return;
}
#else

/*compute the sparsity pattern of the simplified precond Jacobian on GPU */
AMREX_GPU_HOST_DEVICE void SPARSITY_PREPROC_PRECOND(int * rowPtr, int * colIndx, int * consP)
{
    double c[35];
    double J[1296];

    for (int k=0; k<35; k++) {
        c[k] = 1.0/ 35.000000 ;
    }

    aJacobian_precond(J, c, 1500.0, *consP);

    rowPtr[0] = 1;
    int nJdata_tmp = 1;
    for (int l=0; l<36; l++) {
        for (int k=0; k<36; k++) {
            if (k == l) {
                colIndx[nJdata_tmp-1] = l+1; 
                nJdata_tmp = nJdata_tmp + 1; 
            } else {
                if(J[36*k + l] != 0.0) {
                    colIndx[nJdata_tmp-1] = k+1; 
                    nJdata_tmp = nJdata_tmp + 1; 
                }
            }
        }
        rowPtr[l+1] = nJdata_tmp;
    }

    return;
}
#endif

/*compute the sparsity pattern of the Jacobian */
AMREX_GPU_HOST_DEVICE void SPARSITY_PREPROC(int *  rowVals, int *  colPtrs, int * consP, int NCELLS)
{
    double c[35];
    double J[1296];
    int offset_row;
    int offset_col;

    for (int k=0; k<35; k++) {
        c[k] = 1.0/ 35.000000 ;
    }

    aJacobian(J, c, 1500.0, *consP);

    colPtrs[0] = 0;
    int nJdata_tmp = 0;
    for (int nc=0; nc<NCELLS; nc++) {
        offset_row = nc * 36;
        offset_col = nc * 36;
        for (int k=0; k<36; k++) {
            for (int l=0; l<36; l++) {
                if(J[36*k + l] != 0.0) {
                    rowVals[nJdata_tmp] = l + offset_row; 
                    nJdata_tmp = nJdata_tmp + 1; 
                }
            }
            colPtrs[offset_col + (k + 1)] = nJdata_tmp;
        }
    }

    return;
}


#ifdef AMREX_USE_CUDA
/*compute the reaction Jacobian on GPU */
AMREX_GPU_HOST_DEVICE
void aJacobian(double * J, double * sc, double T, int consP)
{


    for (int i=0; i<1296; i++) {
        J[i] = 0.0;
    }

    double wdot[35];
    for (int k=0; k<35; k++) {
        wdot[k] = 0.0;
    }

    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];
    double invT2 = invT * invT;

    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 / T;
    double refCinv = 1.0 / refC;

    /*compute the mixture concentration */
    double mixture = 0.0;
    for (int k = 0; k < 35; ++k) {
        mixture += sc[k];
    }

    /*compute the Gibbs free energy */
    double g_RT[35];
    gibbs(g_RT, tc);

    /*compute the species enthalpy */
    double h_RT[35];
    speciesEnthalpy(h_RT, tc);

    double phi_f, k_f, k_r, phi_r, Kc, q, q_nocor, Corr, alpha;
    double dlnkfdT, dlnk0dT, dlnKcdT, dkrdT, dqdT;
    double dqdci, dcdc_fac, dqdc[35];
    double Pr, fPr, F, k_0, logPr;
    double logFcent, troe_c, troe_n, troePr_den, troePr, troe;
    double Fcent1, Fcent2, Fcent3, Fcent;
    double dlogFdc, dlogFdn, dlogFdcn_fac;
    double dlogPrdT, dlogfPrdT, dlogFdT, dlogFcentdT, dlogFdlogPr, dlnCorrdT;
    const double ln10 = log(10.0);
    const double log10e = 1.0/log(10.0);
    double c_R[35], dcRdT[35], e_RT[35];
    double * eh_RT;
    if (consP) {
        cp_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        eh_RT = &h_RT[0];
    }
    else {
        cv_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        speciesInternalEnergy(e_RT, tc);
        eh_RT = &e_RT[0];
    }

    double cmix = 0.0, ehmix = 0.0, dcmixdT=0.0, dehmixdT=0.0;
    for (int k = 0; k < 35; ++k) {
        cmix += c_R[k]*sc[k];
        dcmixdT += dcRdT[k]*sc[k];
        ehmix += eh_RT[k]*wdot[k];
        dehmixdT += invT*(c_R[k]-eh_RT[k])*wdot[k] + eh_RT[k]*J[1260+k];
    }

    double cmixinv = 1.0/cmix;
    double tmp1 = ehmix*cmixinv;
    double tmp3 = cmixinv*T;
    double tmp2 = tmp1*tmp3;
    double dehmixdc;
    /* dTdot/d[X] */
    for (int k = 0; k < 35; ++k) {
        dehmixdc = 0.0;
        for (int m = 0; m < 35; ++m) {
            dehmixdc += eh_RT[m]*J[k*36+m];
        }
        J[k*36+35] = tmp2*c_R[k] - tmp3*dehmixdc;
    }
    /* dTdot/dT */
    J[1295] = -tmp1 + tmp2*dcmixdT - tmp3*dehmixdT;

return;
}
#endif


#ifndef AMREX_USE_CUDA
/*compute the reaction Jacobian on CPU */
void aJacobian(double *  J, double *  sc, double T, int consP)
{
    for (int i=0; i<1296; i++) {
        J[i] = 0.0;
    }

    double wdot[35];
    for (int k=0; k<35; k++) {
        wdot[k] = 0.0;
    }

    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];
    double invT2 = invT * invT;

    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 / T;
    double refCinv = 1.0 / refC;

    /*compute the mixture concentration */
    double mixture = 0.0;
    for (int k = 0; k < 35; ++k) {
        mixture += sc[k];
    }

    /*compute the Gibbs free energy */
    double g_RT[35];
    gibbs(g_RT, tc);

    /*compute the species enthalpy */
    double h_RT[35];
    speciesEnthalpy(h_RT, tc);

    double phi_f, k_f, k_r, phi_r, Kc, q, q_nocor, Corr, alpha;
    double dlnkfdT, dlnk0dT, dlnKcdT, dkrdT, dqdT;
    double dqdci, dcdc_fac, dqdc[35];
    double Pr, fPr, F, k_0, logPr;
    double logFcent, troe_c, troe_n, troePr_den, troePr, troe;
    double Fcent1, Fcent2, Fcent3, Fcent;
    double dlogFdc, dlogFdn, dlogFdcn_fac;
    double dlogPrdT, dlogfPrdT, dlogFdT, dlogFcentdT, dlogFdlogPr, dlnCorrdT;
    const double ln10 = log(10.0);
    const double log10e = 1.0/log(10.0);
    double c_R[35], dcRdT[35], e_RT[35];
    double * eh_RT;
    if (consP) {
        cp_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        eh_RT = &h_RT[0];
    }
    else {
        cv_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        speciesInternalEnergy(e_RT, tc);
        eh_RT = &e_RT[0];
    }

    double cmix = 0.0, ehmix = 0.0, dcmixdT=0.0, dehmixdT=0.0;
    for (int k = 0; k < 35; ++k) {
        cmix += c_R[k]*sc[k];
        dcmixdT += dcRdT[k]*sc[k];
        ehmix += eh_RT[k]*wdot[k];
        dehmixdT += invT*(c_R[k]-eh_RT[k])*wdot[k] + eh_RT[k]*J[1260+k];
    }

    double cmixinv = 1.0/cmix;
    double tmp1 = ehmix*cmixinv;
    double tmp3 = cmixinv*T;
    double tmp2 = tmp1*tmp3;
    double dehmixdc;
    /* dTdot/d[X] */
    for (int k = 0; k < 35; ++k) {
        dehmixdc = 0.0;
        for (int m = 0; m < 35; ++m) {
            dehmixdc += eh_RT[m]*J[k*36+m];
        }
        J[k*36+35] = tmp2*c_R[k] - tmp3*dehmixdc;
    }
    /* dTdot/dT */
    J[1295] = -tmp1 + tmp2*dcmixdT - tmp3*dehmixdT;
}
#endif


/*compute an approx to the reaction Jacobian */
AMREX_GPU_HOST_DEVICE void aJacobian_precond(double *  J, double *  sc, double T, int HP)
{
    for (int i=0; i<1296; i++) {
        J[i] = 0.0;
    }

    double wdot[35];
    for (int k=0; k<35; k++) {
        wdot[k] = 0.0;
    }

    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];
    double invT2 = invT * invT;

    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 / T;
    double refCinv = 1.0 / refC;

    /*compute the mixture concentration */
    double mixture = 0.0;
    for (int k = 0; k < 35; ++k) {
        mixture += sc[k];
    }

    /*compute the Gibbs free energy */
    double g_RT[35];
    gibbs(g_RT, tc);

    /*compute the species enthalpy */
    double h_RT[35];
    speciesEnthalpy(h_RT, tc);

    double phi_f, k_f, k_r, phi_r, Kc, q, q_nocor, Corr, alpha;
    double dlnkfdT, dlnk0dT, dlnKcdT, dkrdT, dqdT;
    double dqdci, dcdc_fac, dqdc[35];
    double Pr, fPr, F, k_0, logPr;
    double logFcent, troe_c, troe_n, troePr_den, troePr, troe;
    double Fcent1, Fcent2, Fcent3, Fcent;
    double dlogFdc, dlogFdn, dlogFdcn_fac;
    double dlogPrdT, dlogfPrdT, dlogFdT, dlogFcentdT, dlogFdlogPr, dlnCorrdT;
    const double ln10 = log(10.0);
    const double log10e = 1.0/log(10.0);
    double c_R[35], dcRdT[35], e_RT[35];
    double * eh_RT;
    if (HP) {
        cp_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        eh_RT = &h_RT[0];
    }
    else {
        cv_R(c_R, tc);
        dcvpRdT(dcRdT, tc);
        speciesInternalEnergy(e_RT, tc);
        eh_RT = &e_RT[0];
    }

    double cmix = 0.0, ehmix = 0.0, dcmixdT=0.0, dehmixdT=0.0;
    for (int k = 0; k < 35; ++k) {
        cmix += c_R[k]*sc[k];
        dcmixdT += dcRdT[k]*sc[k];
        ehmix += eh_RT[k]*wdot[k];
        dehmixdT += invT*(c_R[k]-eh_RT[k])*wdot[k] + eh_RT[k]*J[1260+k];
    }

    double cmixinv = 1.0/cmix;
    double tmp1 = ehmix*cmixinv;
    double tmp3 = cmixinv*T;
    double tmp2 = tmp1*tmp3;
    double dehmixdc;
    /* dTdot/d[X] */
    for (int k = 0; k < 35; ++k) {
        dehmixdc = 0.0;
        for (int m = 0; m < 35; ++m) {
            dehmixdc += eh_RT[m]*J[k*36+m];
        }
        J[k*36+35] = tmp2*c_R[k] - tmp3*dehmixdc;
    }
    /* dTdot/dT */
    J[1295] = -tmp1 + tmp2*dcmixdT - tmp3*dehmixdT;
}


/*compute d(Cp/R)/dT and d(Cv/R)/dT at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void dcvpRdT(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +7.05332819e-13
            -3.99183928e-15 * tc[1]
            +6.90244896e-18 * tc[2]
            -3.71092933e-21 * tc[3];
        /*species 2: O */
        species[2] =
            -3.27931884e-03
            +1.32861279e-05 * tc[1]
            -1.83841987e-08 * tc[2]
            +8.45063884e-12 * tc[3];
        /*species 3: OH */
        species[3] =
            -3.22544939e-03
            +1.30552938e-05 * tc[1]
            -1.73956093e-08 * tc[2]
            +8.24949516e-12 * tc[3];
        /*species 4: HO2 */
        species[4] =
            -4.74912051e-03
            +4.23165782e-05 * tc[1]
            -7.28291682e-08 * tc[2]
            +3.71690050e-11 * tc[3];
        /*species 5: H2 */
        species[5] =
            +7.98052075e-03
            -3.89563020e-05 * tc[1]
            +6.04716282e-08 * tc[2]
            -2.95044704e-11 * tc[3];
        /*species 6: H2O */
        species[6] =
            -2.03643410e-03
            +1.30408042e-05 * tc[1]
            -1.64639119e-08 * tc[2]
            +7.08791268e-12 * tc[3];
        /*species 7: H2O2 */
        species[7] =
            -5.42822417e-04
            +3.34671402e-05 * tc[1]
            -6.47312439e-08 * tc[2]
            +3.44981745e-11 * tc[3];
        /*species 8: O2 */
        species[8] =
            -2.99673416e-03
            +1.96946040e-05 * tc[1]
            -2.90438853e-08 * tc[2]
            +1.29749135e-11 * tc[3];
        /*species 9: CH3 */
        species[9] =
            +2.01095175e-03
            +1.14604371e-05 * tc[1]
            -2.06135228e-08 * tc[2]
            +1.01754294e-11 * tc[3];
        /*species 10: CH4 */
        species[10] =
            -1.36709788e-02
            +9.83601198e-05 * tc[1]
            -1.45422908e-07 * tc[2]
            +6.66775824e-11 * tc[3];
        /*species 11: CH2O */
        species[11] =
            -9.90833369e-03
            +7.46440016e-05 * tc[1]
            -1.13785578e-07 * tc[2]
            +5.27090608e-11 * tc[3];
        /*species 12: CO */
        species[12] =
            -6.10353680e-04
            +2.03362866e-06 * tc[1]
            +2.72101765e-09 * tc[2]
            -3.61769800e-12 * tc[3];
        /*species 13: CO2 */
        species[13] =
            +8.98459677e-03
            -1.42471254e-05 * tc[1]
            +7.37757066e-09 * tc[2]
            -5.74798192e-13 * tc[3];
        /*species 14: C2H2 */
        species[14] =
            +2.33615629e-02
            -7.10343630e-05 * tc[1]
            +8.40457311e-08 * tc[2]
            -3.40029190e-11 * tc[3];
        /*species 15: C2H4 */
        species[15] =
            -7.57052247e-03
            +1.14198058e-04 * tc[1]
            -2.07476626e-07 * tc[2]
            +1.07953749e-10 * tc[3];
        /*species 16: C2H6 */
        species[16] =
            -5.50154270e-03
            +1.19887658e-04 * tc[1]
            -2.12539886e-07 * tc[2]
            +1.07474308e-10 * tc[3];
        /*species 17: CH2CHO */
        species[17] =
            +1.07385740e-02
            +3.78298500e-06 * tc[1]
            -2.14757493e-08 * tc[2]
            +1.14695404e-11 * tc[3];
        /*species 18: aC3H5 */
        species[18] =
            +1.98138210e-02
            +2.49941200e-05 * tc[1]
            -1.00066665e-07 * tc[2]
            +6.33862840e-11 * tc[3];
        /*species 19: C3H6 */
        species[19] =
            +2.09251800e-02
            +8.97358800e-06 * tc[1]
            -5.00673600e-08 * tc[2]
            +2.86325840e-11 * tc[3];
        /*species 20: C2H3CHO */
        species[20] =
            +2.62310540e-02
            -1.85824610e-05 * tc[1]
            -1.43511816e-08 * tc[2]
            +1.33922172e-11 * tc[3];
        /*species 21: C4H7 */
        species[21] =
            +3.96788570e-02
            -4.57961720e-05 * tc[1]
            +6.40589190e-09 * tc[2]
            +9.23855000e-12 * tc[3];
        /*species 22: C4H81 */
        species[22] =
            +3.08533800e-02
            +1.01730494e-05 * tc[1]
            -7.39646640e-08 * tc[2]
            +4.44407720e-11 * tc[3];
        /*species 23: C5H9 */
        species[23] =
            +4.04303890e-02
            +1.35604678e-05 * tc[1]
            -1.01174226e-07 * tc[2]
            +6.04668520e-11 * tc[3];
        /*species 32: C12H25O2 */
        species[32] =
            +8.93873000e-02
            +2.90702000e-05 * tc[1]
            -2.24775000e-07 * tc[2]
            +1.34130000e-10 * tc[3];
        /*species 33: OC12H23OOH */
        species[33] =
            +6.50623000e-02
            +1.39011600e-04 * tc[1]
            -3.80715000e-07 * tc[2]
            +2.04396400e-10 * tc[3];
        /*species 34: N2 */
        species[34] =
            +1.40824040e-03
            -7.92644400e-06 * tc[1]
            +1.69245450e-08 * tc[2]
            -9.77941600e-12 * tc[3];
    } else {
        /*species 1: H */
        species[1] =
            -2.30842973e-11
            +3.23123896e-14 * tc[1]
            -1.42054571e-17 * tc[2]
            +1.99278943e-21 * tc[3];
        /*species 2: O */
        species[2] =
            -8.59741137e-05
            +8.38969178e-08 * tc[1]
            -3.00533397e-11 * tc[2]
            +4.91334764e-15 * tc[3];
        /*species 3: OH */
        species[3] =
            +1.05650448e-03
            -5.18165516e-07 * tc[1]
            +9.15656022e-11 * tc[2]
            -5.32783504e-15 * tc[3];
        /*species 4: HO2 */
        species[4] =
            +2.23982013e-03
            -1.26731630e-06 * tc[1]
            +3.42739110e-10 * tc[2]
            -4.31634140e-14 * tc[3];
        /*species 5: H2 */
        species[5] =
            -4.94024731e-05
            +9.98913556e-07 * tc[1]
            -5.38699182e-10 * tc[2]
            +8.01021504e-14 * tc[3];
        /*species 6: H2O */
        species[6] =
            +2.17691804e-03
            -3.28145036e-07 * tc[1]
            -2.91125961e-10 * tc[2]
            +6.72803968e-14 * tc[3];
        /*species 7: H2O2 */
        species[7] =
            +4.90831694e-03
            -3.80278450e-06 * tc[1]
            +1.11355796e-09 * tc[2]
            -1.15163322e-13 * tc[3];
        /*species 8: O2 */
        species[8] =
            +1.48308754e-03
            -1.51593334e-06 * tc[1]
            +6.28411665e-10 * tc[2]
            -8.66871176e-14 * tc[3];
        /*species 9: CH3 */
        species[9] =
            +7.23990037e-03
            -5.97428696e-06 * tc[1]
            +1.78705393e-09 * tc[2]
            -1.86861758e-13 * tc[3];
        /*species 10: CH4 */
        species[10] =
            +1.33909467e-02
            -1.14657162e-05 * tc[1]
            +3.66877605e-09 * tc[2]
            -4.07260920e-13 * tc[3];
        /*species 11: CH2O */
        species[11] =
            +9.20000082e-03
            -8.84517626e-06 * tc[1]
            +3.01923636e-09 * tc[2]
            -3.53542256e-13 * tc[3];
        /*species 12: CO */
        species[12] =
            +2.06252743e-03
            -1.99765154e-06 * tc[1]
            +6.90159024e-10 * tc[2]
            -8.14590864e-14 * tc[3];
        /*species 13: CO2 */
        species[13] =
            +4.41437026e-03
            -4.42962808e-06 * tc[1]
            +1.57047056e-09 * tc[2]
            -1.88833666e-13 * tc[3];
        /*species 14: C2H2 */
        species[14] =
            +5.96166664e-03
            -4.74589704e-06 * tc[1]
            +1.40223651e-09 * tc[2]
            -1.44494085e-13 * tc[3];
        /*species 15: C2H4 */
        species[15] =
            +1.46454151e-02
            -1.34215583e-05 * tc[1]
            +4.41668769e-09 * tc[2]
            -5.02824244e-13 * tc[3];
        /*species 16: C2H6 */
        species[16] =
            +2.16852677e-02
            -2.00512134e-05 * tc[1]
            +6.64236003e-09 * tc[2]
            -7.60011560e-13 * tc[3];
        /*species 17: CH2CHO */
        species[17] =
            +8.13059140e-03
            -5.48724900e-06 * tc[1]
            +1.22109123e-09 * tc[2]
            -8.70406840e-14 * tc[3];
        /*species 18: aC3H5 */
        species[18] =
            +1.43247310e-02
            -1.13563264e-05 * tc[1]
            +3.32424030e-09 * tc[2]
            -3.61455548e-13 * tc[3];
        /*species 19: C3H6 */
        species[19] =
            +1.49083400e-02
            -9.89979800e-06 * tc[1]
            +2.16360660e-09 * tc[2]
            -1.50648160e-13 * tc[3];
        /*species 20: C2H3CHO */
        species[20] =
            +1.71142560e-02
            -1.49668322e-05 * tc[1]
            +4.27567470e-09 * tc[2]
            -3.66987364e-13 * tc[3];
        /*species 21: C4H7 */
        species[21] =
            +2.26345580e-02
            -1.85090940e-05 * tc[1]
            +5.04237810e-09 * tc[2]
            -4.16344680e-13 * tc[3];
        /*species 22: C4H81 */
        species[22] =
            +3.43505070e-02
            -3.17663940e-05 * tc[1]
            +9.92689860e-09 * tc[2]
            -1.01444180e-12 * tc[3];
        /*species 23: C5H9 */
        species[23] =
            +2.27141380e-02
            -1.55820926e-05 * tc[1]
            +3.56295660e-09 * tc[2]
            -2.63729792e-13 * tc[3];
        /*species 32: C12H25O2 */
        species[32] =
            +5.37539000e-02
            -3.36372000e-05 * tc[1]
            +7.54101000e-09 * tc[2]
            -5.88832000e-13 * tc[3];
        /*species 33: OC12H23OOH */
        species[33] =
            +6.16392000e-02
            -4.19672000e-05 * tc[1]
            +9.99498000e-09 * tc[2]
            -8.14360000e-13 * tc[3];
        /*species 34: N2 */
        species[34] =
            +1.48797680e-03
            -1.13695200e-06 * tc[1]
            +3.02911140e-10 * tc[2]
            -2.70134040e-14 * tc[3];
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            +5.74218294e-02
            -7.48973780e-05 * tc[1]
            +3.82094967e-08 * tc[2]
            -7.18439156e-12 * tc[3];
        /*species 25: C6H12 */
        species[25] =
            +6.98655426e-02
            -9.18816044e-05 * tc[1]
            +4.70902029e-08 * tc[2]
            -8.85184700e-12 * tc[3];
        /*species 26: C7H14 */
        species[26] =
            +8.24611601e-02
            -1.09300822e-04 * tc[1]
            +5.63586909e-08 * tc[2]
            -1.06295193e-11 * tc[3];
        /*species 27: C8H16 */
        species[27] =
            +9.46066357e-02
            -1.25477104e-04 * tc[1]
            +6.45474927e-08 * tc[2]
            -1.21087473e-11 * tc[3];
        /*species 28: C9H18 */
        species[28] =
            +1.06958297e-01
            -1.42194649e-04 * tc[1]
            +7.31913231e-08 * tc[2]
            -1.37108619e-11 * tc[3];
        /*species 30: C10H20 */
        species[30] =
            +1.19305598e-01
            -1.58897805e-04 * tc[1]
            +8.18209788e-08 * tc[2]
            -1.53087349e-11 * tc[3];
    } else {
        /*species 24: C5H10 */
        species[24] =
            +2.24072471e-02
            -1.52669605e-05 * tc[1]
            +3.54566898e-09 * tc[2]
            -2.73754056e-13 * tc[3];
        /*species 25: C6H12 */
        species[25] =
            +2.67377658e-02
            -1.82007355e-05 * tc[1]
            +4.22459304e-09 * tc[2]
            -3.26049698e-13 * tc[3];
        /*species 26: C7H14 */
        species[26] =
            +3.10607878e-02
            -2.11289586e-05 * tc[1]
            +4.90217340e-09 * tc[2]
            -3.78239288e-13 * tc[3];
        /*species 27: C8H16 */
        species[27] =
            +3.53666462e-02
            -2.40416776e-05 * tc[1]
            +5.57565159e-09 * tc[2]
            -4.30089048e-13 * tc[3];
        /*species 28: C9H18 */
        species[28] =
            +3.96825287e-02
            -2.69638892e-05 * tc[1]
            +6.25171356e-09 * tc[2]
            -4.82157176e-13 * tc[3];
        /*species 30: C10H20 */
        species[30] =
            +4.39971526e-02
            -2.98851060e-05 * tc[1]
            +6.92753034e-09 * tc[2]
            -5.34205908e-13 * tc[3];
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            +1.05617283e-01
            -1.33639994e-04 * tc[1]
            +6.43458498e-08 * tc[2]
            -1.10961710e-11 * tc[3];
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +4.12657344e-02
            -2.80766578e-05 * tc[1]
            +6.51524613e-09 * tc[2]
            -5.02769228e-13 * tc[3];
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            +1.47237711e-01
            -1.88794054e-04 * tc[1]
            +9.22323804e-08 * tc[2]
            -1.61440892e-11 * tc[3];
        /*species 31: C12H24 */
        species[31] =
            +1.43992360e-01
            -1.92276803e-04 * tc[1]
            +9.90523419e-08 * tc[2]
            -1.84959276e-11 * tc[3];
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +5.63550048e-02
            -3.82986400e-05 * tc[1]
            +8.88074586e-09 * tc[2]
            -6.84976600e-13 * tc[3];
        /*species 31: C12H24 */
        species[31] =
            +5.26230753e-02
            -3.57248638e-05 * tc[1]
            +8.27849589e-09 * tc[2]
            -6.38249996e-13 * tc[3];
    }
    return;
}


/*compute the progress rate for each reaction */
AMREX_GPU_HOST_DEVICE void progressRate(double *  qdot, double *  sc, double T)
{
    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];

#ifndef AMREX_USE_CUDA
    if (T != T_save)
    {
        T_save = T;
        comp_k_f(tc,invT,k_f_save);
        comp_Kc(tc,invT,Kc_save);
    }
#endif

    double q_f[0], q_r[0];
    comp_qfqr(q_f, q_r, sc, tc, invT);

    for (int i = 0; i < 0; ++i) {
        qdot[i] = q_f[i] - q_r[i];
    }

    return;
}


/*compute the progress rate for each reaction */
AMREX_GPU_HOST_DEVICE void progressRateFR(double *  q_f, double *  q_r, double *  sc, double T)
{
    double tc[] = { log(T), T, T*T, T*T*T, T*T*T*T }; /*temperature cache */
    double invT = 1.0 / tc[1];
#ifndef AMREX_USE_CUDA

    if (T != T_save)
    {
        T_save = T;
        comp_k_f(tc,invT,k_f_save);
        comp_Kc(tc,invT,Kc_save);
    }
#endif

    comp_qfqr(q_f, q_r, sc, tc, invT);

    return;
}


/*compute the equilibrium constants for each reaction */
void equilibriumConstants(double *  kc, double *  g_RT, double T)
{
    /*reference concentration: P_atm / (RT) in inverse mol/m^3 */
    double refC = 101325 / 8.31451 / T;

    return;
}


/*compute the g/(RT) at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void gibbs(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];
    double invT = 1 / T;

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +2.547365990000000e+04 * invT
            +2.946682853000000e+00
            -2.500000000000000e+00 * tc[0]
            -3.526664095000000e-13 * tc[1]
            +3.326532733333333e-16 * tc[2]
            -1.917346933333333e-19 * tc[3]
            +4.638661660000000e-23 * tc[4];
        /*species 2: O */
        species[2] =
            +2.912225920000000e+04 * invT
            +1.116333640000000e+00
            -3.168267100000000e+00 * tc[0]
            +1.639659420000000e-03 * tc[1]
            -1.107177326666667e-06 * tc[2]
            +5.106721866666666e-10 * tc[3]
            -1.056329855000000e-13 * tc[4];
        /*species 3: OH */
        species[3] =
            +3.381538120000000e+03 * invT
            +4.815738570000000e+00
            -4.125305610000000e+00 * tc[0]
            +1.612724695000000e-03 * tc[1]
            -1.087941151666667e-06 * tc[2]
            +4.832113691666666e-10 * tc[3]
            -1.031186895000000e-13 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +2.948080400000000e+02 * invT
            +5.851355599999999e-01
            -4.301798010000000e+00 * tc[0]
            +2.374560255000000e-03 * tc[1]
            -3.526381516666666e-06 * tc[2]
            +2.023032450000000e-09 * tc[3]
            -4.646125620000001e-13 * tc[4];
        /*species 5: H2 */
        species[5] =
            -9.179351730000000e+02 * invT
            +1.661320882000000e+00
            -2.344331120000000e+00 * tc[0]
            -3.990260375000000e-03 * tc[1]
            +3.246358500000000e-06 * tc[2]
            -1.679767450000000e-09 * tc[3]
            +3.688058805000000e-13 * tc[4];
        /*species 6: H2O */
        species[6] =
            -3.029372670000000e+04 * invT
            +5.047672768000000e+00
            -4.198640560000000e+00 * tc[0]
            +1.018217050000000e-03 * tc[1]
            -1.086733685000000e-06 * tc[2]
            +4.573308850000000e-10 * tc[3]
            -8.859890850000000e-14 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            -1.770258210000000e+04 * invT
            +8.410619499999998e-01
            -4.276112690000000e+00 * tc[0]
            +2.714112085000000e-04 * tc[1]
            -2.788928350000000e-06 * tc[2]
            +1.798090108333333e-09 * tc[3]
            -4.312271815000000e-13 * tc[4];
        /*species 8: O2 */
        species[8] =
            -1.063943560000000e+03 * invT
            +1.247806300000001e-01
            -3.782456360000000e+00 * tc[0]
            +1.498367080000000e-03 * tc[1]
            -1.641217001666667e-06 * tc[2]
            +8.067745908333334e-10 * tc[3]
            -1.621864185000000e-13 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +1.644499880000000e+04 * invT
            +2.069026070000000e+00
            -3.673590400000000e+00 * tc[0]
            -1.005475875000000e-03 * tc[1]
            -9.550364266666668e-07 * tc[2]
            +5.725978541666666e-10 * tc[3]
            -1.271928670000000e-13 * tc[4];
        /*species 10: CH4 */
        species[10] =
            -1.024664760000000e+04 * invT
            +9.791179889999999e+00
            -5.149876130000000e+00 * tc[0]
            +6.835489400000000e-03 * tc[1]
            -8.196676650000000e-06 * tc[2]
            +4.039525216666667e-09 * tc[3]
            -8.334697800000000e-13 * tc[4];
        /*species 11: CH2O */
        species[11] =
            -1.430895670000000e+04 * invT
            +4.190910250000000e+00
            -4.793723150000000e+00 * tc[0]
            +4.954166845000000e-03 * tc[1]
            -6.220333466666666e-06 * tc[2]
            +3.160710508333333e-09 * tc[3]
            -6.588632600000000e-13 * tc[4];
        /*species 12: CO */
        species[12] =
            -1.434408600000000e+04 * invT
            +7.112418999999992e-02
            -3.579533470000000e+00 * tc[0]
            +3.051768400000000e-04 * tc[1]
            -1.694690550000000e-07 * tc[2]
            -7.558382366666667e-11 * tc[3]
            +4.522122495000000e-14 * tc[4];
        /*species 13: CO2 */
        species[13] =
            -4.837196970000000e+04 * invT
            -7.544278700000000e+00
            -2.356773520000000e+00 * tc[0]
            -4.492298385000000e-03 * tc[1]
            +1.187260448333333e-06 * tc[2]
            -2.049325183333333e-10 * tc[3]
            +7.184977399999999e-15 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +2.642898070000000e+04 * invT
            -1.313102400600000e+01
            -8.086810940000000e-01 * tc[0]
            -1.168078145000000e-02 * tc[1]
            +5.919530250000000e-06 * tc[2]
            -2.334603641666667e-09 * tc[3]
            +4.250364870000000e-13 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +5.089775930000000e+03 * invT
            -1.381294799999999e-01
            -3.959201480000000e+00 * tc[0]
            +3.785261235000000e-03 * tc[1]
            -9.516504866666667e-06 * tc[2]
            +5.763239608333333e-09 * tc[3]
            -1.349421865000000e-12 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            -1.152220550000000e+04 * invT
            +1.624601760000000e+00
            -4.291424920000000e+00 * tc[0]
            +2.750771350000000e-03 * tc[1]
            -9.990638133333334e-06 * tc[2]
            +5.903885708333334e-09 * tc[3]
            -1.343428855000000e-12 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +6.200000000000000e+01 * invT
            -6.162391100000001e+00
            -3.409062400000000e+00 * tc[0]
            -5.369287000000000e-03 * tc[1]
            -3.152487500000000e-07 * tc[2]
            +5.965485916666667e-10 * tc[3]
            -1.433692550000000e-13 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +1.924562900000000e+04 * invT
            -1.581003050000000e+01
            -1.363183500000000e+00 * tc[0]
            -9.906910499999999e-03 * tc[1]
            -2.082843333333333e-06 * tc[2]
            +2.779629583333333e-09 * tc[3]
            -7.923285500000000e-13 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +1.074826000000000e+03 * invT
            -1.465203300000000e+01
            -1.493307000000000e+00 * tc[0]
            -1.046259000000000e-02 * tc[1]
            -7.477990000000000e-07 * tc[2]
            +1.390760000000000e-09 * tc[3]
            -3.579073000000000e-13 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            -9.335734399999999e+03 * invT
            -1.822672720000000e+01
            -1.271349800000000e+00 * tc[0]
            -1.311552700000000e-02 * tc[1]
            +1.548538416666667e-06 * tc[2]
            +3.986439333333333e-10 * tc[3]
            -1.674027150000000e-13 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +2.265332800000000e+04 * invT
            -2.269338368000000e+01
            -7.444943200000000e-01 * tc[0]
            -1.983942850000000e-02 * tc[1]
            +3.816347666666667e-06 * tc[2]
            -1.779414416666667e-10 * tc[3]
            -1.154818750000000e-13 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            -1.790400400000000e+03 * invT
            -1.988133100000000e+01
            -1.181138000000000e+00 * tc[0]
            -1.542669000000000e-02 * tc[1]
            -8.477541166666667e-07 * tc[2]
            +2.054574000000000e-09 * tc[3]
            -5.555096499999999e-13 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            +2.812188700000000e+03 * invT
            -3.887825510000000e+01
            +2.419011100000000e+00 * tc[0]
            -2.021519450000000e-02 * tc[1]
            -1.130038983333333e-06 * tc[2]
            +2.810395166666667e-09 * tc[3]
            -7.558356500000001e-13 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            -2.989180000000000e+04 * invT
            -1.166006000000000e+01
            -5.314040000000000e+00 * tc[0]
            -4.469365000000000e-02 * tc[1]
            -2.422516666666667e-06 * tc[2]
            +6.243750000000000e-09 * tc[3]
            -1.676625000000000e-12 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            -6.653610000000001e+04 * invT
            +1.965780000000001e+00
            -8.807330000000000e+00 * tc[0]
            -3.253115000000000e-02 * tc[1]
            -1.158430000000000e-05 * tc[2]
            +1.057541666666667e-08 * tc[3]
            -2.554955000000000e-12 * tc[4];
        /*species 34: N2 */
        species[34] =
            -1.020899900000000e+03 * invT
            -6.516950000000001e-01
            -3.298677000000000e+00 * tc[0]
            -7.041202000000000e-04 * tc[1]
            +6.605369999999999e-07 * tc[2]
            -4.701262500000001e-10 * tc[3]
            +1.222427000000000e-13 * tc[4];
    } else {
        /*species 1: H */
        species[1] =
            +2.547365990000000e+04 * invT
            +2.946682924000000e+00
            -2.500000010000000e+00 * tc[0]
            +1.154214865000000e-11 * tc[1]
            -2.692699133333334e-15 * tc[2]
            +3.945960291666667e-19 * tc[3]
            -2.490986785000000e-23 * tc[4];
        /*species 2: O */
        species[2] =
            +2.921757910000000e+04 * invT
            -2.214917859999999e+00
            -2.569420780000000e+00 * tc[0]
            +4.298705685000000e-05 * tc[1]
            -6.991409816666667e-09 * tc[2]
            +8.348149916666666e-13 * tc[3]
            -6.141684549999999e-17 * tc[4];
        /*species 3: OH */
        species[3] =
            +3.718857740000000e+03 * invT
            -2.836911870000000e+00
            -2.864728860000000e+00 * tc[0]
            -5.282522400000000e-04 * tc[1]
            +4.318045966666667e-08 * tc[2]
            -2.543488950000000e-12 * tc[3]
            +6.659793800000000e-17 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +1.118567130000000e+02 * invT
            +2.321087500000001e-01
            -4.017210900000000e+00 * tc[0]
            -1.119910065000000e-03 * tc[1]
            +1.056096916666667e-07 * tc[2]
            -9.520530833333334e-12 * tc[3]
            +5.395426750000000e-16 * tc[4];
        /*species 5: H2 */
        species[5] =
            -9.501589220000000e+02 * invT
            +6.542302510000000e+00
            -3.337279200000000e+00 * tc[0]
            +2.470123655000000e-05 * tc[1]
            -8.324279633333333e-08 * tc[2]
            +1.496386616666667e-11 * tc[3]
            -1.001276880000000e-15 * tc[4];
        /*species 6: H2O */
        species[6] =
            -3.000429710000000e+04 * invT
            -1.932777610000000e+00
            -3.033992490000000e+00 * tc[0]
            -1.088459020000000e-03 * tc[1]
            +2.734541966666666e-08 * tc[2]
            +8.086832250000000e-12 * tc[3]
            -8.410049600000000e-16 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            -1.786178770000000e+04 * invT
            +1.248846229999999e+00
            -4.165002850000000e+00 * tc[0]
            -2.454158470000000e-03 * tc[1]
            +3.168987083333333e-07 * tc[2]
            -3.093216550000000e-11 * tc[3]
            +1.439541525000000e-15 * tc[4];
        /*species 8: O2 */
        species[8] =
            -1.088457720000000e+03 * invT
            -2.170693450000000e+00
            -3.282537840000000e+00 * tc[0]
            -7.415437700000000e-04 * tc[1]
            +1.263277781666667e-07 * tc[2]
            -1.745587958333333e-11 * tc[3]
            +1.083588970000000e-15 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +1.677558430000000e+04 * invT
            -6.194354070000000e+00
            -2.285717720000000e+00 * tc[0]
            -3.619950185000000e-03 * tc[1]
            +4.978572466666667e-07 * tc[2]
            -4.964038700000000e-11 * tc[3]
            +2.335771970000000e-15 * tc[4];
        /*species 10: CH4 */
        species[10] =
            -9.468344590000001e+03 * invT
            -1.836246650500000e+01
            -7.485149500000000e-02 * tc[0]
            -6.695473350000000e-03 * tc[1]
            +9.554763483333333e-07 * tc[2]
            -1.019104458333333e-10 * tc[3]
            +5.090761500000000e-15 * tc[4];
        /*species 11: CH2O */
        species[11] =
            -1.399583230000000e+04 * invT
            -1.189563292000000e+01
            -1.760690080000000e+00 * tc[0]
            -4.600000410000000e-03 * tc[1]
            +7.370980216666666e-07 * tc[2]
            -8.386767666666666e-11 * tc[3]
            +4.419278200000001e-15 * tc[4];
        /*species 12: CO */
        species[12] =
            -1.415187240000000e+04 * invT
            -5.103502110000000e+00
            -2.715185610000000e+00 * tc[0]
            -1.031263715000000e-03 * tc[1]
            +1.664709618333334e-07 * tc[2]
            -1.917108400000000e-11 * tc[3]
            +1.018238580000000e-15 * tc[4];
        /*species 13: CO2 */
        species[13] =
            -4.875916600000000e+04 * invT
            +1.585822230000000e+00
            -3.857460290000000e+00 * tc[0]
            -2.207185130000000e-03 * tc[1]
            +3.691356733333334e-07 * tc[2]
            -4.362418233333334e-11 * tc[3]
            +2.360420820000000e-15 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +2.593599920000000e+04 * invT
            +5.377850850000001e+00
            -4.147569640000000e+00 * tc[0]
            -2.980833320000000e-03 * tc[1]
            +3.954914200000000e-07 * tc[2]
            -3.895101425000000e-11 * tc[3]
            +1.806176065000000e-15 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +4.939886140000000e+03 * invT
            -8.269258140000002e+00
            -2.036111160000000e+00 * tc[0]
            -7.322707550000000e-03 * tc[1]
            +1.118463191666667e-06 * tc[2]
            -1.226857691666667e-10 * tc[3]
            +6.285303050000000e-15 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            -1.142639320000000e+04 * invT
            -1.404372920000000e+01
            -1.071881500000000e+00 * tc[0]
            -1.084263385000000e-02 * tc[1]
            +1.670934450000000e-06 * tc[2]
            -1.845100008333333e-10 * tc[3]
            +9.500144500000000e-15 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            -9.695000000000000e+02 * invT
            +1.100775780000000e+01
            -5.975669900000000e+00 * tc[0]
            -4.065295700000000e-03 * tc[1]
            +4.572707500000000e-07 * tc[2]
            -3.391920083333333e-11 * tc[3]
            +1.088008550000000e-15 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +1.748244900000000e+04 * invT
            +1.774383770000000e+01
            -6.500787700000000e+00 * tc[0]
            -7.162365500000000e-03 * tc[1]
            +9.463605333333332e-07 * tc[2]
            -9.234000833333333e-11 * tc[3]
            +4.518194349999999e-15 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            -9.235703000000000e+02 * invT
            +2.004560700000000e+01
            -6.732257000000000e+00 * tc[0]
            -7.454170000000000e-03 * tc[1]
            +8.249831666666666e-07 * tc[2]
            -6.010018333333334e-11 * tc[3]
            +1.883102000000000e-15 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            -1.078405400000000e+04 * invT
            +1.066998720000000e+01
            -5.811186800000000e+00 * tc[0]
            -8.557128000000001e-03 * tc[1]
            +1.247236016666667e-06 * tc[2]
            -1.187687416666667e-10 * tc[3]
            +4.587342050000000e-15 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +2.095500800000000e+04 * invT
            +1.590279150000000e+01
            -7.013483500000000e+00 * tc[0]
            -1.131727900000000e-02 * tc[1]
            +1.542424500000000e-06 * tc[2]
            -1.400660583333333e-10 * tc[3]
            +5.204308500000000e-15 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            -2.139723100000000e+03 * invT
            -1.348961690000000e+01
            -2.053584100000000e+00 * tc[0]
            -1.717525350000000e-02 * tc[1]
            +2.647199500000000e-06 * tc[2]
            -2.757471833333334e-10 * tc[3]
            +1.268052250000000e-14 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            -1.721835900000000e+03 * invT
            +4.326452500000000e+01
            -1.013864000000000e+01 * tc[0]
            -1.135706900000000e-02 * tc[1]
            +1.298507716666667e-06 * tc[2]
            -9.897101666666667e-11 * tc[3]
            +3.296622400000000e-15 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            -3.741180000000000e+04 * invT
            +1.375992000000000e+02
            -2.847820000000000e+01 * tc[0]
            -2.687695000000000e-02 * tc[1]
            +2.803100000000000e-06 * tc[2]
            -2.094725000000000e-10 * tc[3]
            +7.360400000000000e-15 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            -7.182580000000000e+04 * invT
            +1.014393000000000e+02
            -2.367310000000000e+01 * tc[0]
            -3.081960000000000e-02 * tc[1]
            +3.497266666666667e-06 * tc[2]
            -2.776383333333333e-10 * tc[3]
            +1.017950000000000e-14 * tc[4];
        /*species 34: N2 */
        species[34] =
            -9.227977000000000e+02 * invT
            -3.053888000000000e+00
            -2.926640000000000e+00 * tc[0]
            -7.439884000000000e-04 * tc[1]
            +9.474600000000001e-08 * tc[2]
            -8.414198333333333e-12 * tc[3]
            +3.376675500000000e-16 * tc[4];
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -4.465466660000000e+03 * invT
            -3.333621381000000e+01
            +1.062234810000000e+00 * tc[0]
            -2.871091470000000e-02 * tc[1]
            +6.241448166666667e-06 * tc[2]
            -1.061374908333333e-09 * tc[3]
            +8.980489449999999e-14 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -7.343686170000000e+03 * invT
            -3.666482115000000e+01
            +1.352752050000000e+00 * tc[0]
            -3.493277130000000e-02 * tc[1]
            +7.656800366666667e-06 * tc[2]
            -1.308061191666667e-09 * tc[3]
            +1.106480875000000e-13 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -1.021686010000000e+04 * invT
            -4.018400869000000e+01
            +1.677205490000000e+00 * tc[0]
            -4.123058005000000e-02 * tc[1]
            +9.108401800000000e-06 * tc[2]
            -1.565519191666667e-09 * tc[3]
            +1.328689915000000e-13 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -1.310745590000000e+04 * invT
            -4.308016725000000e+01
            +1.892269150000000e+00 * tc[0]
            -4.730331785000000e-02 * tc[1]
            +1.045642535000000e-05 * tc[2]
            -1.792985908333333e-09 * tc[3]
            +1.513593415000000e-13 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -1.598908470000000e+04 * invT
            -4.628559543000000e+01
            +2.161082630000000e+00 * tc[0]
            -5.347914850000000e-02 * tc[1]
            +1.184955406666667e-05 * tc[2]
            -2.033092308333333e-09 * tc[3]
            +1.713857735000000e-13 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -1.887083650000000e+04 * invT
            -4.948615518000000e+01
            +2.429016880000000e+00 * tc[0]
            -5.965279900000000e-02 * tc[1]
            +1.324148375000000e-05 * tc[2]
            -2.272804966666667e-09 * tc[3]
            +1.913591865000000e-13 * tc[4];
    } else {
        /*species 24: C5H10 */
        species[24] =
            -1.008982050000000e+04 * invT
            +6.695354750000000e+01
            -1.458515390000000e+01 * tc[0]
            -1.120362355000000e-02 * tc[1]
            +1.272246708333333e-06 * tc[2]
            -9.849080500000001e-11 * tc[3]
            +3.421925695000000e-15 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -1.420628600000000e+04 * invT
            +8.621563800000001e+01
            -1.783375290000000e+01 * tc[0]
            -1.336888290000000e-02 * tc[1]
            +1.516727955000000e-06 * tc[2]
            -1.173498066666667e-10 * tc[3]
            +4.075621220000000e-15 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -1.832600650000000e+04 * invT
            +1.055289147000000e+02
            -2.108980390000000e+01 * tc[0]
            -1.553039390000000e-02 * tc[1]
            +1.760746550000000e-06 * tc[2]
            -1.361714833333333e-10 * tc[3]
            +4.727991095000000e-15 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -2.244856740000000e+04 * invT
            +1.248917285000000e+02
            -2.435401250000000e+01 * tc[0]
            -1.768332310000000e-02 * tc[1]
            +2.003473133333334e-06 * tc[2]
            -1.548792108333333e-10 * tc[3]
            +5.376113100000000e-15 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -2.657090610000000e+04 * invT
            +1.442328406000000e+02
            -2.761421760000000e+01 * tc[0]
            -1.984126435000000e-02 * tc[1]
            +2.246990766666667e-06 * tc[2]
            -1.736587100000000e-10 * tc[3]
            +6.026964700000000e-15 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -3.069373070000000e+04 * invT
            +1.635805623000000e+02
            -3.087539030000000e+01 * tc[0]
            -2.199857630000000e-02 * tc[1]
            +2.490425500000000e-06 * tc[2]
            -1.924313983333333e-10 * tc[3]
            +6.677573850000000e-15 * tc[4];
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -6.808185120000000e+03 * invT
            -4.339577212000000e+01
            +1.043872920000000e+00 * tc[0]
            -5.280864150000000e-02 * tc[1]
            +1.113666618333333e-05 * tc[2]
            -1.787384716666667e-09 * tc[3]
            +1.387021375000000e-13 * tc[4];
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            -1.745160300000000e+04 * invT
            +1.451476484000000e+02
            -2.830975140000000e+01 * tc[0]
            -2.063286720000000e-02 * tc[1]
            +2.339721483333333e-06 * tc[2]
            -1.809790591666667e-10 * tc[3]
            +6.284615349999999e-15 * tc[4];
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -4.006542530000000e+04 * invT
            -5.272127854000000e+01
            +2.621815940000000e+00 * tc[0]
            -7.361885550000000e-02 * tc[1]
            +1.573283785000000e-05 * tc[2]
            -2.562010566666667e-09 * tc[3]
            +2.018011150000000e-13 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -2.463452990000000e+04 * invT
            -5.587931381000000e+01
            +2.963426810000000e+00 * tc[0]
            -7.199618000000001e-02 * tc[1]
            +1.602306691666667e-05 * tc[2]
            -2.751453941666667e-09 * tc[3]
            +2.311990950000000e-13 * tc[4];
    } else {
        /*species 0: NC12H26 */
        species[0] =
            -5.488434650000000e+04 * invT
            +2.111804257000000e+02
            -3.850950370000000e+01 * tc[0]
            -2.817750240000000e-02 * tc[1]
            +3.191553333333333e-06 * tc[2]
            -2.466873850000000e-10 * tc[3]
            +8.562207500000000e-15 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -3.894059620000000e+04 * invT
            +2.022928741000000e+02
            -3.740021110000000e+01 * tc[0]
            -2.631153765000000e-02 * tc[1]
            +2.977071983333333e-06 * tc[2]
            -2.299582191666667e-10 * tc[3]
            +7.978124949999999e-15 * tc[4];
    }
    return;
}


/*compute the a/(RT) at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void helmholtz(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];
    double invT = 1 / T;

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +2.54736599e+04 * invT
            +1.94668285e+00
            -2.50000000e+00 * tc[0]
            -3.52666409e-13 * tc[1]
            +3.32653273e-16 * tc[2]
            -1.91734693e-19 * tc[3]
            +4.63866166e-23 * tc[4];
        /*species 2: O */
        species[2] =
            +2.91222592e+04 * invT
            +1.16333640e-01
            -3.16826710e+00 * tc[0]
            +1.63965942e-03 * tc[1]
            -1.10717733e-06 * tc[2]
            +5.10672187e-10 * tc[3]
            -1.05632985e-13 * tc[4];
        /*species 3: OH */
        species[3] =
            +3.38153812e+03 * invT
            +3.81573857e+00
            -4.12530561e+00 * tc[0]
            +1.61272470e-03 * tc[1]
            -1.08794115e-06 * tc[2]
            +4.83211369e-10 * tc[3]
            -1.03118689e-13 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +2.94808040e+02 * invT
            -4.14864440e-01
            -4.30179801e+00 * tc[0]
            +2.37456025e-03 * tc[1]
            -3.52638152e-06 * tc[2]
            +2.02303245e-09 * tc[3]
            -4.64612562e-13 * tc[4];
        /*species 5: H2 */
        species[5] =
            -9.17935173e+02 * invT
            +6.61320882e-01
            -2.34433112e+00 * tc[0]
            -3.99026037e-03 * tc[1]
            +3.24635850e-06 * tc[2]
            -1.67976745e-09 * tc[3]
            +3.68805881e-13 * tc[4];
        /*species 6: H2O */
        species[6] =
            -3.02937267e+04 * invT
            +4.04767277e+00
            -4.19864056e+00 * tc[0]
            +1.01821705e-03 * tc[1]
            -1.08673369e-06 * tc[2]
            +4.57330885e-10 * tc[3]
            -8.85989085e-14 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            -1.77025821e+04 * invT
            -1.58938050e-01
            -4.27611269e+00 * tc[0]
            +2.71411208e-04 * tc[1]
            -2.78892835e-06 * tc[2]
            +1.79809011e-09 * tc[3]
            -4.31227182e-13 * tc[4];
        /*species 8: O2 */
        species[8] =
            -1.06394356e+03 * invT
            -8.75219370e-01
            -3.78245636e+00 * tc[0]
            +1.49836708e-03 * tc[1]
            -1.64121700e-06 * tc[2]
            +8.06774591e-10 * tc[3]
            -1.62186418e-13 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +1.64449988e+04 * invT
            +1.06902607e+00
            -3.67359040e+00 * tc[0]
            -1.00547588e-03 * tc[1]
            -9.55036427e-07 * tc[2]
            +5.72597854e-10 * tc[3]
            -1.27192867e-13 * tc[4];
        /*species 10: CH4 */
        species[10] =
            -1.02466476e+04 * invT
            +8.79117989e+00
            -5.14987613e+00 * tc[0]
            +6.83548940e-03 * tc[1]
            -8.19667665e-06 * tc[2]
            +4.03952522e-09 * tc[3]
            -8.33469780e-13 * tc[4];
        /*species 11: CH2O */
        species[11] =
            -1.43089567e+04 * invT
            +3.19091025e+00
            -4.79372315e+00 * tc[0]
            +4.95416684e-03 * tc[1]
            -6.22033347e-06 * tc[2]
            +3.16071051e-09 * tc[3]
            -6.58863260e-13 * tc[4];
        /*species 12: CO */
        species[12] =
            -1.43440860e+04 * invT
            -9.28875810e-01
            -3.57953347e+00 * tc[0]
            +3.05176840e-04 * tc[1]
            -1.69469055e-07 * tc[2]
            -7.55838237e-11 * tc[3]
            +4.52212249e-14 * tc[4];
        /*species 13: CO2 */
        species[13] =
            -4.83719697e+04 * invT
            -8.54427870e+00
            -2.35677352e+00 * tc[0]
            -4.49229839e-03 * tc[1]
            +1.18726045e-06 * tc[2]
            -2.04932518e-10 * tc[3]
            +7.18497740e-15 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +2.64289807e+04 * invT
            -1.41310240e+01
            -8.08681094e-01 * tc[0]
            -1.16807815e-02 * tc[1]
            +5.91953025e-06 * tc[2]
            -2.33460364e-09 * tc[3]
            +4.25036487e-13 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +5.08977593e+03 * invT
            -1.13812948e+00
            -3.95920148e+00 * tc[0]
            +3.78526124e-03 * tc[1]
            -9.51650487e-06 * tc[2]
            +5.76323961e-09 * tc[3]
            -1.34942187e-12 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            -1.15222055e+04 * invT
            +6.24601760e-01
            -4.29142492e+00 * tc[0]
            +2.75077135e-03 * tc[1]
            -9.99063813e-06 * tc[2]
            +5.90388571e-09 * tc[3]
            -1.34342886e-12 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +6.20000000e+01 * invT
            -7.16239110e+00
            -3.40906240e+00 * tc[0]
            -5.36928700e-03 * tc[1]
            -3.15248750e-07 * tc[2]
            +5.96548592e-10 * tc[3]
            -1.43369255e-13 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +1.92456290e+04 * invT
            -1.68100305e+01
            -1.36318350e+00 * tc[0]
            -9.90691050e-03 * tc[1]
            -2.08284333e-06 * tc[2]
            +2.77962958e-09 * tc[3]
            -7.92328550e-13 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +1.07482600e+03 * invT
            -1.56520330e+01
            -1.49330700e+00 * tc[0]
            -1.04625900e-02 * tc[1]
            -7.47799000e-07 * tc[2]
            +1.39076000e-09 * tc[3]
            -3.57907300e-13 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            -9.33573440e+03 * invT
            -1.92267272e+01
            -1.27134980e+00 * tc[0]
            -1.31155270e-02 * tc[1]
            +1.54853842e-06 * tc[2]
            +3.98643933e-10 * tc[3]
            -1.67402715e-13 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +2.26533280e+04 * invT
            -2.36933837e+01
            -7.44494320e-01 * tc[0]
            -1.98394285e-02 * tc[1]
            +3.81634767e-06 * tc[2]
            -1.77941442e-10 * tc[3]
            -1.15481875e-13 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            -1.79040040e+03 * invT
            -2.08813310e+01
            -1.18113800e+00 * tc[0]
            -1.54266900e-02 * tc[1]
            -8.47754117e-07 * tc[2]
            +2.05457400e-09 * tc[3]
            -5.55509650e-13 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            +2.81218870e+03 * invT
            -3.98782551e+01
            +2.41901110e+00 * tc[0]
            -2.02151945e-02 * tc[1]
            -1.13003898e-06 * tc[2]
            +2.81039517e-09 * tc[3]
            -7.55835650e-13 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            -2.98918000e+04 * invT
            -1.26600600e+01
            -5.31404000e+00 * tc[0]
            -4.46936500e-02 * tc[1]
            -2.42251667e-06 * tc[2]
            +6.24375000e-09 * tc[3]
            -1.67662500e-12 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            -6.65361000e+04 * invT
            +9.65780000e-01
            -8.80733000e+00 * tc[0]
            -3.25311500e-02 * tc[1]
            -1.15843000e-05 * tc[2]
            +1.05754167e-08 * tc[3]
            -2.55495500e-12 * tc[4];
        /*species 34: N2 */
        species[34] =
            -1.02089990e+03 * invT
            -1.65169500e+00
            -3.29867700e+00 * tc[0]
            -7.04120200e-04 * tc[1]
            +6.60537000e-07 * tc[2]
            -4.70126250e-10 * tc[3]
            +1.22242700e-13 * tc[4];
    } else {
        /*species 1: H */
        species[1] =
            +2.54736599e+04 * invT
            +1.94668292e+00
            -2.50000001e+00 * tc[0]
            +1.15421486e-11 * tc[1]
            -2.69269913e-15 * tc[2]
            +3.94596029e-19 * tc[3]
            -2.49098679e-23 * tc[4];
        /*species 2: O */
        species[2] =
            +2.92175791e+04 * invT
            -3.21491786e+00
            -2.56942078e+00 * tc[0]
            +4.29870569e-05 * tc[1]
            -6.99140982e-09 * tc[2]
            +8.34814992e-13 * tc[3]
            -6.14168455e-17 * tc[4];
        /*species 3: OH */
        species[3] =
            +3.71885774e+03 * invT
            -3.83691187e+00
            -2.86472886e+00 * tc[0]
            -5.28252240e-04 * tc[1]
            +4.31804597e-08 * tc[2]
            -2.54348895e-12 * tc[3]
            +6.65979380e-17 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +1.11856713e+02 * invT
            -7.67891250e-01
            -4.01721090e+00 * tc[0]
            -1.11991006e-03 * tc[1]
            +1.05609692e-07 * tc[2]
            -9.52053083e-12 * tc[3]
            +5.39542675e-16 * tc[4];
        /*species 5: H2 */
        species[5] =
            -9.50158922e+02 * invT
            +5.54230251e+00
            -3.33727920e+00 * tc[0]
            +2.47012365e-05 * tc[1]
            -8.32427963e-08 * tc[2]
            +1.49638662e-11 * tc[3]
            -1.00127688e-15 * tc[4];
        /*species 6: H2O */
        species[6] =
            -3.00042971e+04 * invT
            -2.93277761e+00
            -3.03399249e+00 * tc[0]
            -1.08845902e-03 * tc[1]
            +2.73454197e-08 * tc[2]
            +8.08683225e-12 * tc[3]
            -8.41004960e-16 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            -1.78617877e+04 * invT
            +2.48846230e-01
            -4.16500285e+00 * tc[0]
            -2.45415847e-03 * tc[1]
            +3.16898708e-07 * tc[2]
            -3.09321655e-11 * tc[3]
            +1.43954153e-15 * tc[4];
        /*species 8: O2 */
        species[8] =
            -1.08845772e+03 * invT
            -3.17069345e+00
            -3.28253784e+00 * tc[0]
            -7.41543770e-04 * tc[1]
            +1.26327778e-07 * tc[2]
            -1.74558796e-11 * tc[3]
            +1.08358897e-15 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +1.67755843e+04 * invT
            -7.19435407e+00
            -2.28571772e+00 * tc[0]
            -3.61995018e-03 * tc[1]
            +4.97857247e-07 * tc[2]
            -4.96403870e-11 * tc[3]
            +2.33577197e-15 * tc[4];
        /*species 10: CH4 */
        species[10] =
            -9.46834459e+03 * invT
            -1.93624665e+01
            -7.48514950e-02 * tc[0]
            -6.69547335e-03 * tc[1]
            +9.55476348e-07 * tc[2]
            -1.01910446e-10 * tc[3]
            +5.09076150e-15 * tc[4];
        /*species 11: CH2O */
        species[11] =
            -1.39958323e+04 * invT
            -1.28956329e+01
            -1.76069008e+00 * tc[0]
            -4.60000041e-03 * tc[1]
            +7.37098022e-07 * tc[2]
            -8.38676767e-11 * tc[3]
            +4.41927820e-15 * tc[4];
        /*species 12: CO */
        species[12] =
            -1.41518724e+04 * invT
            -6.10350211e+00
            -2.71518561e+00 * tc[0]
            -1.03126372e-03 * tc[1]
            +1.66470962e-07 * tc[2]
            -1.91710840e-11 * tc[3]
            +1.01823858e-15 * tc[4];
        /*species 13: CO2 */
        species[13] =
            -4.87591660e+04 * invT
            +5.85822230e-01
            -3.85746029e+00 * tc[0]
            -2.20718513e-03 * tc[1]
            +3.69135673e-07 * tc[2]
            -4.36241823e-11 * tc[3]
            +2.36042082e-15 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +2.59359992e+04 * invT
            +4.37785085e+00
            -4.14756964e+00 * tc[0]
            -2.98083332e-03 * tc[1]
            +3.95491420e-07 * tc[2]
            -3.89510143e-11 * tc[3]
            +1.80617607e-15 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +4.93988614e+03 * invT
            -9.26925814e+00
            -2.03611116e+00 * tc[0]
            -7.32270755e-03 * tc[1]
            +1.11846319e-06 * tc[2]
            -1.22685769e-10 * tc[3]
            +6.28530305e-15 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            -1.14263932e+04 * invT
            -1.50437292e+01
            -1.07188150e+00 * tc[0]
            -1.08426339e-02 * tc[1]
            +1.67093445e-06 * tc[2]
            -1.84510001e-10 * tc[3]
            +9.50014450e-15 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            -9.69500000e+02 * invT
            +1.00077578e+01
            -5.97566990e+00 * tc[0]
            -4.06529570e-03 * tc[1]
            +4.57270750e-07 * tc[2]
            -3.39192008e-11 * tc[3]
            +1.08800855e-15 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +1.74824490e+04 * invT
            +1.67438377e+01
            -6.50078770e+00 * tc[0]
            -7.16236550e-03 * tc[1]
            +9.46360533e-07 * tc[2]
            -9.23400083e-11 * tc[3]
            +4.51819435e-15 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            -9.23570300e+02 * invT
            +1.90456070e+01
            -6.73225700e+00 * tc[0]
            -7.45417000e-03 * tc[1]
            +8.24983167e-07 * tc[2]
            -6.01001833e-11 * tc[3]
            +1.88310200e-15 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            -1.07840540e+04 * invT
            +9.66998720e+00
            -5.81118680e+00 * tc[0]
            -8.55712800e-03 * tc[1]
            +1.24723602e-06 * tc[2]
            -1.18768742e-10 * tc[3]
            +4.58734205e-15 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +2.09550080e+04 * invT
            +1.49027915e+01
            -7.01348350e+00 * tc[0]
            -1.13172790e-02 * tc[1]
            +1.54242450e-06 * tc[2]
            -1.40066058e-10 * tc[3]
            +5.20430850e-15 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            -2.13972310e+03 * invT
            -1.44896169e+01
            -2.05358410e+00 * tc[0]
            -1.71752535e-02 * tc[1]
            +2.64719950e-06 * tc[2]
            -2.75747183e-10 * tc[3]
            +1.26805225e-14 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            -1.72183590e+03 * invT
            +4.22645250e+01
            -1.01386400e+01 * tc[0]
            -1.13570690e-02 * tc[1]
            +1.29850772e-06 * tc[2]
            -9.89710167e-11 * tc[3]
            +3.29662240e-15 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            -3.74118000e+04 * invT
            +1.36599200e+02
            -2.84782000e+01 * tc[0]
            -2.68769500e-02 * tc[1]
            +2.80310000e-06 * tc[2]
            -2.09472500e-10 * tc[3]
            +7.36040000e-15 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            -7.18258000e+04 * invT
            +1.00439300e+02
            -2.36731000e+01 * tc[0]
            -3.08196000e-02 * tc[1]
            +3.49726667e-06 * tc[2]
            -2.77638333e-10 * tc[3]
            +1.01795000e-14 * tc[4];
        /*species 34: N2 */
        species[34] =
            -9.22797700e+02 * invT
            -4.05388800e+00
            -2.92664000e+00 * tc[0]
            -7.43988400e-04 * tc[1]
            +9.47460000e-08 * tc[2]
            -8.41419833e-12 * tc[3]
            +3.37667550e-16 * tc[4];
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -4.46546666e+03 * invT
            -3.43362138e+01
            +1.06223481e+00 * tc[0]
            -2.87109147e-02 * tc[1]
            +6.24144817e-06 * tc[2]
            -1.06137491e-09 * tc[3]
            +8.98048945e-14 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -7.34368617e+03 * invT
            -3.76648212e+01
            +1.35275205e+00 * tc[0]
            -3.49327713e-02 * tc[1]
            +7.65680037e-06 * tc[2]
            -1.30806119e-09 * tc[3]
            +1.10648088e-13 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -1.02168601e+04 * invT
            -4.11840087e+01
            +1.67720549e+00 * tc[0]
            -4.12305800e-02 * tc[1]
            +9.10840180e-06 * tc[2]
            -1.56551919e-09 * tc[3]
            +1.32868991e-13 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -1.31074559e+04 * invT
            -4.40801672e+01
            +1.89226915e+00 * tc[0]
            -4.73033178e-02 * tc[1]
            +1.04564254e-05 * tc[2]
            -1.79298591e-09 * tc[3]
            +1.51359342e-13 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -1.59890847e+04 * invT
            -4.72855954e+01
            +2.16108263e+00 * tc[0]
            -5.34791485e-02 * tc[1]
            +1.18495541e-05 * tc[2]
            -2.03309231e-09 * tc[3]
            +1.71385773e-13 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -1.88708365e+04 * invT
            -5.04861552e+01
            +2.42901688e+00 * tc[0]
            -5.96527990e-02 * tc[1]
            +1.32414838e-05 * tc[2]
            -2.27280497e-09 * tc[3]
            +1.91359186e-13 * tc[4];
    } else {
        /*species 24: C5H10 */
        species[24] =
            -1.00898205e+04 * invT
            +6.59535475e+01
            -1.45851539e+01 * tc[0]
            -1.12036235e-02 * tc[1]
            +1.27224671e-06 * tc[2]
            -9.84908050e-11 * tc[3]
            +3.42192569e-15 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -1.42062860e+04 * invT
            +8.52156380e+01
            -1.78337529e+01 * tc[0]
            -1.33688829e-02 * tc[1]
            +1.51672796e-06 * tc[2]
            -1.17349807e-10 * tc[3]
            +4.07562122e-15 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -1.83260065e+04 * invT
            +1.04528915e+02
            -2.10898039e+01 * tc[0]
            -1.55303939e-02 * tc[1]
            +1.76074655e-06 * tc[2]
            -1.36171483e-10 * tc[3]
            +4.72799110e-15 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -2.24485674e+04 * invT
            +1.23891728e+02
            -2.43540125e+01 * tc[0]
            -1.76833231e-02 * tc[1]
            +2.00347313e-06 * tc[2]
            -1.54879211e-10 * tc[3]
            +5.37611310e-15 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -2.65709061e+04 * invT
            +1.43232841e+02
            -2.76142176e+01 * tc[0]
            -1.98412643e-02 * tc[1]
            +2.24699077e-06 * tc[2]
            -1.73658710e-10 * tc[3]
            +6.02696470e-15 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -3.06937307e+04 * invT
            +1.62580562e+02
            -3.08753903e+01 * tc[0]
            -2.19985763e-02 * tc[1]
            +2.49042550e-06 * tc[2]
            -1.92431398e-10 * tc[3]
            +6.67757385e-15 * tc[4];
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -6.80818512e+03 * invT
            -4.43957721e+01
            +1.04387292e+00 * tc[0]
            -5.28086415e-02 * tc[1]
            +1.11366662e-05 * tc[2]
            -1.78738472e-09 * tc[3]
            +1.38702138e-13 * tc[4];
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            -1.74516030e+04 * invT
            +1.44147648e+02
            -2.83097514e+01 * tc[0]
            -2.06328672e-02 * tc[1]
            +2.33972148e-06 * tc[2]
            -1.80979059e-10 * tc[3]
            +6.28461535e-15 * tc[4];
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -4.00654253e+04 * invT
            -5.37212785e+01
            +2.62181594e+00 * tc[0]
            -7.36188555e-02 * tc[1]
            +1.57328378e-05 * tc[2]
            -2.56201057e-09 * tc[3]
            +2.01801115e-13 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -2.46345299e+04 * invT
            -5.68793138e+01
            +2.96342681e+00 * tc[0]
            -7.19961800e-02 * tc[1]
            +1.60230669e-05 * tc[2]
            -2.75145394e-09 * tc[3]
            +2.31199095e-13 * tc[4];
    } else {
        /*species 0: NC12H26 */
        species[0] =
            -5.48843465e+04 * invT
            +2.10180426e+02
            -3.85095037e+01 * tc[0]
            -2.81775024e-02 * tc[1]
            +3.19155333e-06 * tc[2]
            -2.46687385e-10 * tc[3]
            +8.56220750e-15 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -3.89405962e+04 * invT
            +2.01292874e+02
            -3.74002111e+01 * tc[0]
            -2.63115377e-02 * tc[1]
            +2.97707198e-06 * tc[2]
            -2.29958219e-10 * tc[3]
            +7.97812495e-15 * tc[4];
    }
    return;
}


/*compute Cv/R at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void cv_R(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +1.50000000e+00
            +7.05332819e-13 * tc[1]
            -1.99591964e-15 * tc[2]
            +2.30081632e-18 * tc[3]
            -9.27732332e-22 * tc[4];
        /*species 2: O */
        species[2] =
            +2.16826710e+00
            -3.27931884e-03 * tc[1]
            +6.64306396e-06 * tc[2]
            -6.12806624e-09 * tc[3]
            +2.11265971e-12 * tc[4];
        /*species 3: OH */
        species[3] =
            +3.12530561e+00
            -3.22544939e-03 * tc[1]
            +6.52764691e-06 * tc[2]
            -5.79853643e-09 * tc[3]
            +2.06237379e-12 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +3.30179801e+00
            -4.74912051e-03 * tc[1]
            +2.11582891e-05 * tc[2]
            -2.42763894e-08 * tc[3]
            +9.29225124e-12 * tc[4];
        /*species 5: H2 */
        species[5] =
            +1.34433112e+00
            +7.98052075e-03 * tc[1]
            -1.94781510e-05 * tc[2]
            +2.01572094e-08 * tc[3]
            -7.37611761e-12 * tc[4];
        /*species 6: H2O */
        species[6] =
            +3.19864056e+00
            -2.03643410e-03 * tc[1]
            +6.52040211e-06 * tc[2]
            -5.48797062e-09 * tc[3]
            +1.77197817e-12 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            +3.27611269e+00
            -5.42822417e-04 * tc[1]
            +1.67335701e-05 * tc[2]
            -2.15770813e-08 * tc[3]
            +8.62454363e-12 * tc[4];
        /*species 8: O2 */
        species[8] =
            +2.78245636e+00
            -2.99673416e-03 * tc[1]
            +9.84730201e-06 * tc[2]
            -9.68129509e-09 * tc[3]
            +3.24372837e-12 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +2.67359040e+00
            +2.01095175e-03 * tc[1]
            +5.73021856e-06 * tc[2]
            -6.87117425e-09 * tc[3]
            +2.54385734e-12 * tc[4];
        /*species 10: CH4 */
        species[10] =
            +4.14987613e+00
            -1.36709788e-02 * tc[1]
            +4.91800599e-05 * tc[2]
            -4.84743026e-08 * tc[3]
            +1.66693956e-11 * tc[4];
        /*species 11: CH2O */
        species[11] =
            +3.79372315e+00
            -9.90833369e-03 * tc[1]
            +3.73220008e-05 * tc[2]
            -3.79285261e-08 * tc[3]
            +1.31772652e-11 * tc[4];
        /*species 12: CO */
        species[12] =
            +2.57953347e+00
            -6.10353680e-04 * tc[1]
            +1.01681433e-06 * tc[2]
            +9.07005884e-10 * tc[3]
            -9.04424499e-13 * tc[4];
        /*species 13: CO2 */
        species[13] =
            +1.35677352e+00
            +8.98459677e-03 * tc[1]
            -7.12356269e-06 * tc[2]
            +2.45919022e-09 * tc[3]
            -1.43699548e-13 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            -1.91318906e-01
            +2.33615629e-02 * tc[1]
            -3.55171815e-05 * tc[2]
            +2.80152437e-08 * tc[3]
            -8.50072974e-12 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +2.95920148e+00
            -7.57052247e-03 * tc[1]
            +5.70990292e-05 * tc[2]
            -6.91588753e-08 * tc[3]
            +2.69884373e-11 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            +3.29142492e+00
            -5.50154270e-03 * tc[1]
            +5.99438288e-05 * tc[2]
            -7.08466285e-08 * tc[3]
            +2.68685771e-11 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +2.40906240e+00
            +1.07385740e-02 * tc[1]
            +1.89149250e-06 * tc[2]
            -7.15858310e-09 * tc[3]
            +2.86738510e-12 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +3.63183500e-01
            +1.98138210e-02 * tc[1]
            +1.24970600e-05 * tc[2]
            -3.33555550e-08 * tc[3]
            +1.58465710e-11 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +4.93307000e-01
            +2.09251800e-02 * tc[1]
            +4.48679400e-06 * tc[2]
            -1.66891200e-08 * tc[3]
            +7.15814600e-12 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            +2.71349800e-01
            +2.62310540e-02 * tc[1]
            -9.29123050e-06 * tc[2]
            -4.78372720e-09 * tc[3]
            +3.34805430e-12 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            -2.55505680e-01
            +3.96788570e-02 * tc[1]
            -2.28980860e-05 * tc[2]
            +2.13529730e-09 * tc[3]
            +2.30963750e-12 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            +1.81138000e-01
            +3.08533800e-02 * tc[1]
            +5.08652470e-06 * tc[2]
            -2.46548880e-08 * tc[3]
            +1.11101930e-11 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            -3.41901110e+00
            +4.04303890e-02 * tc[1]
            +6.78023390e-06 * tc[2]
            -3.37247420e-08 * tc[3]
            +1.51167130e-11 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            +4.31404000e+00
            +8.93873000e-02 * tc[1]
            +1.45351000e-05 * tc[2]
            -7.49250000e-08 * tc[3]
            +3.35325000e-11 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            +7.80733000e+00
            +6.50623000e-02 * tc[1]
            +6.95058000e-05 * tc[2]
            -1.26905000e-07 * tc[3]
            +5.10991000e-11 * tc[4];
        /*species 34: N2 */
        species[34] =
            +2.29867700e+00
            +1.40824040e-03 * tc[1]
            -3.96322200e-06 * tc[2]
            +5.64151500e-09 * tc[3]
            -2.44485400e-12 * tc[4];
    } else {
        /*species 1: H */
        species[1] =
            +1.50000001e+00
            -2.30842973e-11 * tc[1]
            +1.61561948e-14 * tc[2]
            -4.73515235e-18 * tc[3]
            +4.98197357e-22 * tc[4];
        /*species 2: O */
        species[2] =
            +1.56942078e+00
            -8.59741137e-05 * tc[1]
            +4.19484589e-08 * tc[2]
            -1.00177799e-11 * tc[3]
            +1.22833691e-15 * tc[4];
        /*species 3: OH */
        species[3] =
            +1.86472886e+00
            +1.05650448e-03 * tc[1]
            -2.59082758e-07 * tc[2]
            +3.05218674e-11 * tc[3]
            -1.33195876e-15 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +3.01721090e+00
            +2.23982013e-03 * tc[1]
            -6.33658150e-07 * tc[2]
            +1.14246370e-10 * tc[3]
            -1.07908535e-14 * tc[4];
        /*species 5: H2 */
        species[5] =
            +2.33727920e+00
            -4.94024731e-05 * tc[1]
            +4.99456778e-07 * tc[2]
            -1.79566394e-10 * tc[3]
            +2.00255376e-14 * tc[4];
        /*species 6: H2O */
        species[6] =
            +2.03399249e+00
            +2.17691804e-03 * tc[1]
            -1.64072518e-07 * tc[2]
            -9.70419870e-11 * tc[3]
            +1.68200992e-14 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            +3.16500285e+00
            +4.90831694e-03 * tc[1]
            -1.90139225e-06 * tc[2]
            +3.71185986e-10 * tc[3]
            -2.87908305e-14 * tc[4];
        /*species 8: O2 */
        species[8] =
            +2.28253784e+00
            +1.48308754e-03 * tc[1]
            -7.57966669e-07 * tc[2]
            +2.09470555e-10 * tc[3]
            -2.16717794e-14 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +1.28571772e+00
            +7.23990037e-03 * tc[1]
            -2.98714348e-06 * tc[2]
            +5.95684644e-10 * tc[3]
            -4.67154394e-14 * tc[4];
        /*species 10: CH4 */
        species[10] =
            -9.25148505e-01
            +1.33909467e-02 * tc[1]
            -5.73285809e-06 * tc[2]
            +1.22292535e-09 * tc[3]
            -1.01815230e-13 * tc[4];
        /*species 11: CH2O */
        species[11] =
            +7.60690080e-01
            +9.20000082e-03 * tc[1]
            -4.42258813e-06 * tc[2]
            +1.00641212e-09 * tc[3]
            -8.83855640e-14 * tc[4];
        /*species 12: CO */
        species[12] =
            +1.71518561e+00
            +2.06252743e-03 * tc[1]
            -9.98825771e-07 * tc[2]
            +2.30053008e-10 * tc[3]
            -2.03647716e-14 * tc[4];
        /*species 13: CO2 */
        species[13] =
            +2.85746029e+00
            +4.41437026e-03 * tc[1]
            -2.21481404e-06 * tc[2]
            +5.23490188e-10 * tc[3]
            -4.72084164e-14 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +3.14756964e+00
            +5.96166664e-03 * tc[1]
            -2.37294852e-06 * tc[2]
            +4.67412171e-10 * tc[3]
            -3.61235213e-14 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +1.03611116e+00
            +1.46454151e-02 * tc[1]
            -6.71077915e-06 * tc[2]
            +1.47222923e-09 * tc[3]
            -1.25706061e-13 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            +7.18815000e-02
            +2.16852677e-02 * tc[1]
            -1.00256067e-05 * tc[2]
            +2.21412001e-09 * tc[3]
            -1.90002890e-13 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +4.97566990e+00
            +8.13059140e-03 * tc[1]
            -2.74362450e-06 * tc[2]
            +4.07030410e-10 * tc[3]
            -2.17601710e-14 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +5.50078770e+00
            +1.43247310e-02 * tc[1]
            -5.67816320e-06 * tc[2]
            +1.10808010e-09 * tc[3]
            -9.03638870e-14 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +5.73225700e+00
            +1.49083400e-02 * tc[1]
            -4.94989900e-06 * tc[2]
            +7.21202200e-10 * tc[3]
            -3.76620400e-14 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            +4.81118680e+00
            +1.71142560e-02 * tc[1]
            -7.48341610e-06 * tc[2]
            +1.42522490e-09 * tc[3]
            -9.17468410e-14 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +6.01348350e+00
            +2.26345580e-02 * tc[1]
            -9.25454700e-06 * tc[2]
            +1.68079270e-09 * tc[3]
            -1.04086170e-13 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            +1.05358410e+00
            +3.43505070e-02 * tc[1]
            -1.58831970e-05 * tc[2]
            +3.30896620e-09 * tc[3]
            -2.53610450e-13 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            +9.13864000e+00
            +2.27141380e-02 * tc[1]
            -7.79104630e-06 * tc[2]
            +1.18765220e-09 * tc[3]
            -6.59324480e-14 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            +2.74782000e+01
            +5.37539000e-02 * tc[1]
            -1.68186000e-05 * tc[2]
            +2.51367000e-09 * tc[3]
            -1.47208000e-13 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            +2.26731000e+01
            +6.16392000e-02 * tc[1]
            -2.09836000e-05 * tc[2]
            +3.33166000e-09 * tc[3]
            -2.03590000e-13 * tc[4];
        /*species 34: N2 */
        species[34] =
            +1.92664000e+00
            +1.48797680e-03 * tc[1]
            -5.68476000e-07 * tc[2]
            +1.00970380e-10 * tc[3]
            -6.75335100e-15 * tc[4];
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -2.06223481e+00
            +5.74218294e-02 * tc[1]
            -3.74486890e-05 * tc[2]
            +1.27364989e-08 * tc[3]
            -1.79609789e-12 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -2.35275205e+00
            +6.98655426e-02 * tc[1]
            -4.59408022e-05 * tc[2]
            +1.56967343e-08 * tc[3]
            -2.21296175e-12 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -2.67720549e+00
            +8.24611601e-02 * tc[1]
            -5.46504108e-05 * tc[2]
            +1.87862303e-08 * tc[3]
            -2.65737983e-12 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -2.89226915e+00
            +9.46066357e-02 * tc[1]
            -6.27385521e-05 * tc[2]
            +2.15158309e-08 * tc[3]
            -3.02718683e-12 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -3.16108263e+00
            +1.06958297e-01 * tc[1]
            -7.10973244e-05 * tc[2]
            +2.43971077e-08 * tc[3]
            -3.42771547e-12 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -3.42901688e+00
            +1.19305598e-01 * tc[1]
            -7.94489025e-05 * tc[2]
            +2.72736596e-08 * tc[3]
            -3.82718373e-12 * tc[4];
    } else {
        /*species 24: C5H10 */
        species[24] =
            +1.35851539e+01
            +2.24072471e-02 * tc[1]
            -7.63348025e-06 * tc[2]
            +1.18188966e-09 * tc[3]
            -6.84385139e-14 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            +1.68337529e+01
            +2.67377658e-02 * tc[1]
            -9.10036773e-06 * tc[2]
            +1.40819768e-09 * tc[3]
            -8.15124244e-14 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            +2.00898039e+01
            +3.10607878e-02 * tc[1]
            -1.05644793e-05 * tc[2]
            +1.63405780e-09 * tc[3]
            -9.45598219e-14 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            +2.33540125e+01
            +3.53666462e-02 * tc[1]
            -1.20208388e-05 * tc[2]
            +1.85855053e-09 * tc[3]
            -1.07522262e-13 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            +2.66142176e+01
            +3.96825287e-02 * tc[1]
            -1.34819446e-05 * tc[2]
            +2.08390452e-09 * tc[3]
            -1.20539294e-13 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            +2.98753903e+01
            +4.39971526e-02 * tc[1]
            -1.49425530e-05 * tc[2]
            +2.30917678e-09 * tc[3]
            -1.33551477e-13 * tc[4];
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -2.04387292e+00
            +1.05617283e-01 * tc[1]
            -6.68199971e-05 * tc[2]
            +2.14486166e-08 * tc[3]
            -2.77404275e-12 * tc[4];
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +2.73097514e+01
            +4.12657344e-02 * tc[1]
            -1.40383289e-05 * tc[2]
            +2.17174871e-09 * tc[3]
            -1.25692307e-13 * tc[4];
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -3.62181594e+00
            +1.47237711e-01 * tc[1]
            -9.43970271e-05 * tc[2]
            +3.07441268e-08 * tc[3]
            -4.03602230e-12 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -3.96342681e+00
            +1.43992360e-01 * tc[1]
            -9.61384015e-05 * tc[2]
            +3.30174473e-08 * tc[3]
            -4.62398190e-12 * tc[4];
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +3.75095037e+01
            +5.63550048e-02 * tc[1]
            -1.91493200e-05 * tc[2]
            +2.96024862e-09 * tc[3]
            -1.71244150e-13 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            +3.64002111e+01
            +5.26230753e-02 * tc[1]
            -1.78624319e-05 * tc[2]
            +2.75949863e-09 * tc[3]
            -1.59562499e-13 * tc[4];
    }
    return;
}


/*compute Cp/R at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void cp_R(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +2.50000000e+00
            +7.05332819e-13 * tc[1]
            -1.99591964e-15 * tc[2]
            +2.30081632e-18 * tc[3]
            -9.27732332e-22 * tc[4];
        /*species 2: O */
        species[2] =
            +3.16826710e+00
            -3.27931884e-03 * tc[1]
            +6.64306396e-06 * tc[2]
            -6.12806624e-09 * tc[3]
            +2.11265971e-12 * tc[4];
        /*species 3: OH */
        species[3] =
            +4.12530561e+00
            -3.22544939e-03 * tc[1]
            +6.52764691e-06 * tc[2]
            -5.79853643e-09 * tc[3]
            +2.06237379e-12 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +4.30179801e+00
            -4.74912051e-03 * tc[1]
            +2.11582891e-05 * tc[2]
            -2.42763894e-08 * tc[3]
            +9.29225124e-12 * tc[4];
        /*species 5: H2 */
        species[5] =
            +2.34433112e+00
            +7.98052075e-03 * tc[1]
            -1.94781510e-05 * tc[2]
            +2.01572094e-08 * tc[3]
            -7.37611761e-12 * tc[4];
        /*species 6: H2O */
        species[6] =
            +4.19864056e+00
            -2.03643410e-03 * tc[1]
            +6.52040211e-06 * tc[2]
            -5.48797062e-09 * tc[3]
            +1.77197817e-12 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            +4.27611269e+00
            -5.42822417e-04 * tc[1]
            +1.67335701e-05 * tc[2]
            -2.15770813e-08 * tc[3]
            +8.62454363e-12 * tc[4];
        /*species 8: O2 */
        species[8] =
            +3.78245636e+00
            -2.99673416e-03 * tc[1]
            +9.84730201e-06 * tc[2]
            -9.68129509e-09 * tc[3]
            +3.24372837e-12 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +3.67359040e+00
            +2.01095175e-03 * tc[1]
            +5.73021856e-06 * tc[2]
            -6.87117425e-09 * tc[3]
            +2.54385734e-12 * tc[4];
        /*species 10: CH4 */
        species[10] =
            +5.14987613e+00
            -1.36709788e-02 * tc[1]
            +4.91800599e-05 * tc[2]
            -4.84743026e-08 * tc[3]
            +1.66693956e-11 * tc[4];
        /*species 11: CH2O */
        species[11] =
            +4.79372315e+00
            -9.90833369e-03 * tc[1]
            +3.73220008e-05 * tc[2]
            -3.79285261e-08 * tc[3]
            +1.31772652e-11 * tc[4];
        /*species 12: CO */
        species[12] =
            +3.57953347e+00
            -6.10353680e-04 * tc[1]
            +1.01681433e-06 * tc[2]
            +9.07005884e-10 * tc[3]
            -9.04424499e-13 * tc[4];
        /*species 13: CO2 */
        species[13] =
            +2.35677352e+00
            +8.98459677e-03 * tc[1]
            -7.12356269e-06 * tc[2]
            +2.45919022e-09 * tc[3]
            -1.43699548e-13 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +8.08681094e-01
            +2.33615629e-02 * tc[1]
            -3.55171815e-05 * tc[2]
            +2.80152437e-08 * tc[3]
            -8.50072974e-12 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +3.95920148e+00
            -7.57052247e-03 * tc[1]
            +5.70990292e-05 * tc[2]
            -6.91588753e-08 * tc[3]
            +2.69884373e-11 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            +4.29142492e+00
            -5.50154270e-03 * tc[1]
            +5.99438288e-05 * tc[2]
            -7.08466285e-08 * tc[3]
            +2.68685771e-11 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +3.40906240e+00
            +1.07385740e-02 * tc[1]
            +1.89149250e-06 * tc[2]
            -7.15858310e-09 * tc[3]
            +2.86738510e-12 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +1.36318350e+00
            +1.98138210e-02 * tc[1]
            +1.24970600e-05 * tc[2]
            -3.33555550e-08 * tc[3]
            +1.58465710e-11 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +1.49330700e+00
            +2.09251800e-02 * tc[1]
            +4.48679400e-06 * tc[2]
            -1.66891200e-08 * tc[3]
            +7.15814600e-12 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            +1.27134980e+00
            +2.62310540e-02 * tc[1]
            -9.29123050e-06 * tc[2]
            -4.78372720e-09 * tc[3]
            +3.34805430e-12 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +7.44494320e-01
            +3.96788570e-02 * tc[1]
            -2.28980860e-05 * tc[2]
            +2.13529730e-09 * tc[3]
            +2.30963750e-12 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            +1.18113800e+00
            +3.08533800e-02 * tc[1]
            +5.08652470e-06 * tc[2]
            -2.46548880e-08 * tc[3]
            +1.11101930e-11 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            -2.41901110e+00
            +4.04303890e-02 * tc[1]
            +6.78023390e-06 * tc[2]
            -3.37247420e-08 * tc[3]
            +1.51167130e-11 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            +5.31404000e+00
            +8.93873000e-02 * tc[1]
            +1.45351000e-05 * tc[2]
            -7.49250000e-08 * tc[3]
            +3.35325000e-11 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            +8.80733000e+00
            +6.50623000e-02 * tc[1]
            +6.95058000e-05 * tc[2]
            -1.26905000e-07 * tc[3]
            +5.10991000e-11 * tc[4];
        /*species 34: N2 */
        species[34] =
            +3.29867700e+00
            +1.40824040e-03 * tc[1]
            -3.96322200e-06 * tc[2]
            +5.64151500e-09 * tc[3]
            -2.44485400e-12 * tc[4];
    } else {
        /*species 1: H */
        species[1] =
            +2.50000001e+00
            -2.30842973e-11 * tc[1]
            +1.61561948e-14 * tc[2]
            -4.73515235e-18 * tc[3]
            +4.98197357e-22 * tc[4];
        /*species 2: O */
        species[2] =
            +2.56942078e+00
            -8.59741137e-05 * tc[1]
            +4.19484589e-08 * tc[2]
            -1.00177799e-11 * tc[3]
            +1.22833691e-15 * tc[4];
        /*species 3: OH */
        species[3] =
            +2.86472886e+00
            +1.05650448e-03 * tc[1]
            -2.59082758e-07 * tc[2]
            +3.05218674e-11 * tc[3]
            -1.33195876e-15 * tc[4];
        /*species 4: HO2 */
        species[4] =
            +4.01721090e+00
            +2.23982013e-03 * tc[1]
            -6.33658150e-07 * tc[2]
            +1.14246370e-10 * tc[3]
            -1.07908535e-14 * tc[4];
        /*species 5: H2 */
        species[5] =
            +3.33727920e+00
            -4.94024731e-05 * tc[1]
            +4.99456778e-07 * tc[2]
            -1.79566394e-10 * tc[3]
            +2.00255376e-14 * tc[4];
        /*species 6: H2O */
        species[6] =
            +3.03399249e+00
            +2.17691804e-03 * tc[1]
            -1.64072518e-07 * tc[2]
            -9.70419870e-11 * tc[3]
            +1.68200992e-14 * tc[4];
        /*species 7: H2O2 */
        species[7] =
            +4.16500285e+00
            +4.90831694e-03 * tc[1]
            -1.90139225e-06 * tc[2]
            +3.71185986e-10 * tc[3]
            -2.87908305e-14 * tc[4];
        /*species 8: O2 */
        species[8] =
            +3.28253784e+00
            +1.48308754e-03 * tc[1]
            -7.57966669e-07 * tc[2]
            +2.09470555e-10 * tc[3]
            -2.16717794e-14 * tc[4];
        /*species 9: CH3 */
        species[9] =
            +2.28571772e+00
            +7.23990037e-03 * tc[1]
            -2.98714348e-06 * tc[2]
            +5.95684644e-10 * tc[3]
            -4.67154394e-14 * tc[4];
        /*species 10: CH4 */
        species[10] =
            +7.48514950e-02
            +1.33909467e-02 * tc[1]
            -5.73285809e-06 * tc[2]
            +1.22292535e-09 * tc[3]
            -1.01815230e-13 * tc[4];
        /*species 11: CH2O */
        species[11] =
            +1.76069008e+00
            +9.20000082e-03 * tc[1]
            -4.42258813e-06 * tc[2]
            +1.00641212e-09 * tc[3]
            -8.83855640e-14 * tc[4];
        /*species 12: CO */
        species[12] =
            +2.71518561e+00
            +2.06252743e-03 * tc[1]
            -9.98825771e-07 * tc[2]
            +2.30053008e-10 * tc[3]
            -2.03647716e-14 * tc[4];
        /*species 13: CO2 */
        species[13] =
            +3.85746029e+00
            +4.41437026e-03 * tc[1]
            -2.21481404e-06 * tc[2]
            +5.23490188e-10 * tc[3]
            -4.72084164e-14 * tc[4];
        /*species 14: C2H2 */
        species[14] =
            +4.14756964e+00
            +5.96166664e-03 * tc[1]
            -2.37294852e-06 * tc[2]
            +4.67412171e-10 * tc[3]
            -3.61235213e-14 * tc[4];
        /*species 15: C2H4 */
        species[15] =
            +2.03611116e+00
            +1.46454151e-02 * tc[1]
            -6.71077915e-06 * tc[2]
            +1.47222923e-09 * tc[3]
            -1.25706061e-13 * tc[4];
        /*species 16: C2H6 */
        species[16] =
            +1.07188150e+00
            +2.16852677e-02 * tc[1]
            -1.00256067e-05 * tc[2]
            +2.21412001e-09 * tc[3]
            -1.90002890e-13 * tc[4];
        /*species 17: CH2CHO */
        species[17] =
            +5.97566990e+00
            +8.13059140e-03 * tc[1]
            -2.74362450e-06 * tc[2]
            +4.07030410e-10 * tc[3]
            -2.17601710e-14 * tc[4];
        /*species 18: aC3H5 */
        species[18] =
            +6.50078770e+00
            +1.43247310e-02 * tc[1]
            -5.67816320e-06 * tc[2]
            +1.10808010e-09 * tc[3]
            -9.03638870e-14 * tc[4];
        /*species 19: C3H6 */
        species[19] =
            +6.73225700e+00
            +1.49083400e-02 * tc[1]
            -4.94989900e-06 * tc[2]
            +7.21202200e-10 * tc[3]
            -3.76620400e-14 * tc[4];
        /*species 20: C2H3CHO */
        species[20] =
            +5.81118680e+00
            +1.71142560e-02 * tc[1]
            -7.48341610e-06 * tc[2]
            +1.42522490e-09 * tc[3]
            -9.17468410e-14 * tc[4];
        /*species 21: C4H7 */
        species[21] =
            +7.01348350e+00
            +2.26345580e-02 * tc[1]
            -9.25454700e-06 * tc[2]
            +1.68079270e-09 * tc[3]
            -1.04086170e-13 * tc[4];
        /*species 22: C4H81 */
        species[22] =
            +2.05358410e+00
            +3.43505070e-02 * tc[1]
            -1.58831970e-05 * tc[2]
            +3.30896620e-09 * tc[3]
            -2.53610450e-13 * tc[4];
        /*species 23: C5H9 */
        species[23] =
            +1.01386400e+01
            +2.27141380e-02 * tc[1]
            -7.79104630e-06 * tc[2]
            +1.18765220e-09 * tc[3]
            -6.59324480e-14 * tc[4];
        /*species 32: C12H25O2 */
        species[32] =
            +2.84782000e+01
            +5.37539000e-02 * tc[1]
            -1.68186000e-05 * tc[2]
            +2.51367000e-09 * tc[3]
            -1.47208000e-13 * tc[4];
        /*species 33: OC12H23OOH */
        species[33] =
            +2.36731000e+01
            +6.16392000e-02 * tc[1]
            -2.09836000e-05 * tc[2]
            +3.33166000e-09 * tc[3]
            -2.03590000e-13 * tc[4];
        /*species 34: N2 */
        species[34] =
            +2.92664000e+00
            +1.48797680e-03 * tc[1]
            -5.68476000e-07 * tc[2]
            +1.00970380e-10 * tc[3]
            -6.75335100e-15 * tc[4];
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -1.06223481e+00
            +5.74218294e-02 * tc[1]
            -3.74486890e-05 * tc[2]
            +1.27364989e-08 * tc[3]
            -1.79609789e-12 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            -1.35275205e+00
            +6.98655426e-02 * tc[1]
            -4.59408022e-05 * tc[2]
            +1.56967343e-08 * tc[3]
            -2.21296175e-12 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            -1.67720549e+00
            +8.24611601e-02 * tc[1]
            -5.46504108e-05 * tc[2]
            +1.87862303e-08 * tc[3]
            -2.65737983e-12 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            -1.89226915e+00
            +9.46066357e-02 * tc[1]
            -6.27385521e-05 * tc[2]
            +2.15158309e-08 * tc[3]
            -3.02718683e-12 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            -2.16108263e+00
            +1.06958297e-01 * tc[1]
            -7.10973244e-05 * tc[2]
            +2.43971077e-08 * tc[3]
            -3.42771547e-12 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            -2.42901688e+00
            +1.19305598e-01 * tc[1]
            -7.94489025e-05 * tc[2]
            +2.72736596e-08 * tc[3]
            -3.82718373e-12 * tc[4];
    } else {
        /*species 24: C5H10 */
        species[24] =
            +1.45851539e+01
            +2.24072471e-02 * tc[1]
            -7.63348025e-06 * tc[2]
            +1.18188966e-09 * tc[3]
            -6.84385139e-14 * tc[4];
        /*species 25: C6H12 */
        species[25] =
            +1.78337529e+01
            +2.67377658e-02 * tc[1]
            -9.10036773e-06 * tc[2]
            +1.40819768e-09 * tc[3]
            -8.15124244e-14 * tc[4];
        /*species 26: C7H14 */
        species[26] =
            +2.10898039e+01
            +3.10607878e-02 * tc[1]
            -1.05644793e-05 * tc[2]
            +1.63405780e-09 * tc[3]
            -9.45598219e-14 * tc[4];
        /*species 27: C8H16 */
        species[27] =
            +2.43540125e+01
            +3.53666462e-02 * tc[1]
            -1.20208388e-05 * tc[2]
            +1.85855053e-09 * tc[3]
            -1.07522262e-13 * tc[4];
        /*species 28: C9H18 */
        species[28] =
            +2.76142176e+01
            +3.96825287e-02 * tc[1]
            -1.34819446e-05 * tc[2]
            +2.08390452e-09 * tc[3]
            -1.20539294e-13 * tc[4];
        /*species 30: C10H20 */
        species[30] =
            +3.08753903e+01
            +4.39971526e-02 * tc[1]
            -1.49425530e-05 * tc[2]
            +2.30917678e-09 * tc[3]
            -1.33551477e-13 * tc[4];
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -1.04387292e+00
            +1.05617283e-01 * tc[1]
            -6.68199971e-05 * tc[2]
            +2.14486166e-08 * tc[3]
            -2.77404275e-12 * tc[4];
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +2.83097514e+01
            +4.12657344e-02 * tc[1]
            -1.40383289e-05 * tc[2]
            +2.17174871e-09 * tc[3]
            -1.25692307e-13 * tc[4];
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -2.62181594e+00
            +1.47237711e-01 * tc[1]
            -9.43970271e-05 * tc[2]
            +3.07441268e-08 * tc[3]
            -4.03602230e-12 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            -2.96342681e+00
            +1.43992360e-01 * tc[1]
            -9.61384015e-05 * tc[2]
            +3.30174473e-08 * tc[3]
            -4.62398190e-12 * tc[4];
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +3.85095037e+01
            +5.63550048e-02 * tc[1]
            -1.91493200e-05 * tc[2]
            +2.96024862e-09 * tc[3]
            -1.71244150e-13 * tc[4];
        /*species 31: C12H24 */
        species[31] =
            +3.74002111e+01
            +5.26230753e-02 * tc[1]
            -1.78624319e-05 * tc[2]
            +2.75949863e-09 * tc[3]
            -1.59562499e-13 * tc[4];
    }
    return;
}


/*compute the e/(RT) at the given temperature */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void speciesInternalEnergy(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];
    double invT = 1 / T;

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +1.50000000e+00
            +3.52666409e-13 * tc[1]
            -6.65306547e-16 * tc[2]
            +5.75204080e-19 * tc[3]
            -1.85546466e-22 * tc[4]
            +2.54736599e+04 * invT;
        /*species 2: O */
        species[2] =
            +2.16826710e+00
            -1.63965942e-03 * tc[1]
            +2.21435465e-06 * tc[2]
            -1.53201656e-09 * tc[3]
            +4.22531942e-13 * tc[4]
            +2.91222592e+04 * invT;
        /*species 3: OH */
        species[3] =
            +3.12530561e+00
            -1.61272470e-03 * tc[1]
            +2.17588230e-06 * tc[2]
            -1.44963411e-09 * tc[3]
            +4.12474758e-13 * tc[4]
            +3.38153812e+03 * invT;
        /*species 4: HO2 */
        species[4] =
            +3.30179801e+00
            -2.37456025e-03 * tc[1]
            +7.05276303e-06 * tc[2]
            -6.06909735e-09 * tc[3]
            +1.85845025e-12 * tc[4]
            +2.94808040e+02 * invT;
        /*species 5: H2 */
        species[5] =
            +1.34433112e+00
            +3.99026037e-03 * tc[1]
            -6.49271700e-06 * tc[2]
            +5.03930235e-09 * tc[3]
            -1.47522352e-12 * tc[4]
            -9.17935173e+02 * invT;
        /*species 6: H2O */
        species[6] =
            +3.19864056e+00
            -1.01821705e-03 * tc[1]
            +2.17346737e-06 * tc[2]
            -1.37199266e-09 * tc[3]
            +3.54395634e-13 * tc[4]
            -3.02937267e+04 * invT;
        /*species 7: H2O2 */
        species[7] =
            +3.27611269e+00
            -2.71411208e-04 * tc[1]
            +5.57785670e-06 * tc[2]
            -5.39427032e-09 * tc[3]
            +1.72490873e-12 * tc[4]
            -1.77025821e+04 * invT;
        /*species 8: O2 */
        species[8] =
            +2.78245636e+00
            -1.49836708e-03 * tc[1]
            +3.28243400e-06 * tc[2]
            -2.42032377e-09 * tc[3]
            +6.48745674e-13 * tc[4]
            -1.06394356e+03 * invT;
        /*species 9: CH3 */
        species[9] =
            +2.67359040e+00
            +1.00547588e-03 * tc[1]
            +1.91007285e-06 * tc[2]
            -1.71779356e-09 * tc[3]
            +5.08771468e-13 * tc[4]
            +1.64449988e+04 * invT;
        /*species 10: CH4 */
        species[10] =
            +4.14987613e+00
            -6.83548940e-03 * tc[1]
            +1.63933533e-05 * tc[2]
            -1.21185757e-08 * tc[3]
            +3.33387912e-12 * tc[4]
            -1.02466476e+04 * invT;
        /*species 11: CH2O */
        species[11] =
            +3.79372315e+00
            -4.95416684e-03 * tc[1]
            +1.24406669e-05 * tc[2]
            -9.48213152e-09 * tc[3]
            +2.63545304e-12 * tc[4]
            -1.43089567e+04 * invT;
        /*species 12: CO */
        species[12] =
            +2.57953347e+00
            -3.05176840e-04 * tc[1]
            +3.38938110e-07 * tc[2]
            +2.26751471e-10 * tc[3]
            -1.80884900e-13 * tc[4]
            -1.43440860e+04 * invT;
        /*species 13: CO2 */
        species[13] =
            +1.35677352e+00
            +4.49229839e-03 * tc[1]
            -2.37452090e-06 * tc[2]
            +6.14797555e-10 * tc[3]
            -2.87399096e-14 * tc[4]
            -4.83719697e+04 * invT;
        /*species 14: C2H2 */
        species[14] =
            -1.91318906e-01
            +1.16807815e-02 * tc[1]
            -1.18390605e-05 * tc[2]
            +7.00381092e-09 * tc[3]
            -1.70014595e-12 * tc[4]
            +2.64289807e+04 * invT;
        /*species 15: C2H4 */
        species[15] =
            +2.95920148e+00
            -3.78526124e-03 * tc[1]
            +1.90330097e-05 * tc[2]
            -1.72897188e-08 * tc[3]
            +5.39768746e-12 * tc[4]
            +5.08977593e+03 * invT;
        /*species 16: C2H6 */
        species[16] =
            +3.29142492e+00
            -2.75077135e-03 * tc[1]
            +1.99812763e-05 * tc[2]
            -1.77116571e-08 * tc[3]
            +5.37371542e-12 * tc[4]
            -1.15222055e+04 * invT;
        /*species 17: CH2CHO */
        species[17] =
            +2.40906240e+00
            +5.36928700e-03 * tc[1]
            +6.30497500e-07 * tc[2]
            -1.78964578e-09 * tc[3]
            +5.73477020e-13 * tc[4]
            +6.20000000e+01 * invT;
        /*species 18: aC3H5 */
        species[18] =
            +3.63183500e-01
            +9.90691050e-03 * tc[1]
            +4.16568667e-06 * tc[2]
            -8.33888875e-09 * tc[3]
            +3.16931420e-12 * tc[4]
            +1.92456290e+04 * invT;
        /*species 19: C3H6 */
        species[19] =
            +4.93307000e-01
            +1.04625900e-02 * tc[1]
            +1.49559800e-06 * tc[2]
            -4.17228000e-09 * tc[3]
            +1.43162920e-12 * tc[4]
            +1.07482600e+03 * invT;
        /*species 20: C2H3CHO */
        species[20] =
            +2.71349800e-01
            +1.31155270e-02 * tc[1]
            -3.09707683e-06 * tc[2]
            -1.19593180e-09 * tc[3]
            +6.69610860e-13 * tc[4]
            -9.33573440e+03 * invT;
        /*species 21: C4H7 */
        species[21] =
            -2.55505680e-01
            +1.98394285e-02 * tc[1]
            -7.63269533e-06 * tc[2]
            +5.33824325e-10 * tc[3]
            +4.61927500e-13 * tc[4]
            +2.26533280e+04 * invT;
        /*species 22: C4H81 */
        species[22] =
            +1.81138000e-01
            +1.54266900e-02 * tc[1]
            +1.69550823e-06 * tc[2]
            -6.16372200e-09 * tc[3]
            +2.22203860e-12 * tc[4]
            -1.79040040e+03 * invT;
        /*species 23: C5H9 */
        species[23] =
            -3.41901110e+00
            +2.02151945e-02 * tc[1]
            +2.26007797e-06 * tc[2]
            -8.43118550e-09 * tc[3]
            +3.02334260e-12 * tc[4]
            +2.81218870e+03 * invT;
        /*species 32: C12H25O2 */
        species[32] =
            +4.31404000e+00
            +4.46936500e-02 * tc[1]
            +4.84503333e-06 * tc[2]
            -1.87312500e-08 * tc[3]
            +6.70650000e-12 * tc[4]
            -2.98918000e+04 * invT;
        /*species 33: OC12H23OOH */
        species[33] =
            +7.80733000e+00
            +3.25311500e-02 * tc[1]
            +2.31686000e-05 * tc[2]
            -3.17262500e-08 * tc[3]
            +1.02198200e-11 * tc[4]
            -6.65361000e+04 * invT;
        /*species 34: N2 */
        species[34] =
            +2.29867700e+00
            +7.04120200e-04 * tc[1]
            -1.32107400e-06 * tc[2]
            +1.41037875e-09 * tc[3]
            -4.88970800e-13 * tc[4]
            -1.02089990e+03 * invT;
    } else {
        /*species 1: H */
        species[1] =
            +1.50000001e+00
            -1.15421486e-11 * tc[1]
            +5.38539827e-15 * tc[2]
            -1.18378809e-18 * tc[3]
            +9.96394714e-23 * tc[4]
            +2.54736599e+04 * invT;
        /*species 2: O */
        species[2] =
            +1.56942078e+00
            -4.29870569e-05 * tc[1]
            +1.39828196e-08 * tc[2]
            -2.50444497e-12 * tc[3]
            +2.45667382e-16 * tc[4]
            +2.92175791e+04 * invT;
        /*species 3: OH */
        species[3] =
            +1.86472886e+00
            +5.28252240e-04 * tc[1]
            -8.63609193e-08 * tc[2]
            +7.63046685e-12 * tc[3]
            -2.66391752e-16 * tc[4]
            +3.71885774e+03 * invT;
        /*species 4: HO2 */
        species[4] =
            +3.01721090e+00
            +1.11991006e-03 * tc[1]
            -2.11219383e-07 * tc[2]
            +2.85615925e-11 * tc[3]
            -2.15817070e-15 * tc[4]
            +1.11856713e+02 * invT;
        /*species 5: H2 */
        species[5] =
            +2.33727920e+00
            -2.47012365e-05 * tc[1]
            +1.66485593e-07 * tc[2]
            -4.48915985e-11 * tc[3]
            +4.00510752e-15 * tc[4]
            -9.50158922e+02 * invT;
        /*species 6: H2O */
        species[6] =
            +2.03399249e+00
            +1.08845902e-03 * tc[1]
            -5.46908393e-08 * tc[2]
            -2.42604967e-11 * tc[3]
            +3.36401984e-15 * tc[4]
            -3.00042971e+04 * invT;
        /*species 7: H2O2 */
        species[7] =
            +3.16500285e+00
            +2.45415847e-03 * tc[1]
            -6.33797417e-07 * tc[2]
            +9.27964965e-11 * tc[3]
            -5.75816610e-15 * tc[4]
            -1.78617877e+04 * invT;
        /*species 8: O2 */
        species[8] =
            +2.28253784e+00
            +7.41543770e-04 * tc[1]
            -2.52655556e-07 * tc[2]
            +5.23676387e-11 * tc[3]
            -4.33435588e-15 * tc[4]
            -1.08845772e+03 * invT;
        /*species 9: CH3 */
        species[9] =
            +1.28571772e+00
            +3.61995018e-03 * tc[1]
            -9.95714493e-07 * tc[2]
            +1.48921161e-10 * tc[3]
            -9.34308788e-15 * tc[4]
            +1.67755843e+04 * invT;
        /*species 10: CH4 */
        species[10] =
            -9.25148505e-01
            +6.69547335e-03 * tc[1]
            -1.91095270e-06 * tc[2]
            +3.05731338e-10 * tc[3]
            -2.03630460e-14 * tc[4]
            -9.46834459e+03 * invT;
        /*species 11: CH2O */
        species[11] =
            +7.60690080e-01
            +4.60000041e-03 * tc[1]
            -1.47419604e-06 * tc[2]
            +2.51603030e-10 * tc[3]
            -1.76771128e-14 * tc[4]
            -1.39958323e+04 * invT;
        /*species 12: CO */
        species[12] =
            +1.71518561e+00
            +1.03126372e-03 * tc[1]
            -3.32941924e-07 * tc[2]
            +5.75132520e-11 * tc[3]
            -4.07295432e-15 * tc[4]
            -1.41518724e+04 * invT;
        /*species 13: CO2 */
        species[13] =
            +2.85746029e+00
            +2.20718513e-03 * tc[1]
            -7.38271347e-07 * tc[2]
            +1.30872547e-10 * tc[3]
            -9.44168328e-15 * tc[4]
            -4.87591660e+04 * invT;
        /*species 14: C2H2 */
        species[14] =
            +3.14756964e+00
            +2.98083332e-03 * tc[1]
            -7.90982840e-07 * tc[2]
            +1.16853043e-10 * tc[3]
            -7.22470426e-15 * tc[4]
            +2.59359992e+04 * invT;
        /*species 15: C2H4 */
        species[15] =
            +1.03611116e+00
            +7.32270755e-03 * tc[1]
            -2.23692638e-06 * tc[2]
            +3.68057308e-10 * tc[3]
            -2.51412122e-14 * tc[4]
            +4.93988614e+03 * invT;
        /*species 16: C2H6 */
        species[16] =
            +7.18815000e-02
            +1.08426339e-02 * tc[1]
            -3.34186890e-06 * tc[2]
            +5.53530003e-10 * tc[3]
            -3.80005780e-14 * tc[4]
            -1.14263932e+04 * invT;
        /*species 17: CH2CHO */
        species[17] =
            +4.97566990e+00
            +4.06529570e-03 * tc[1]
            -9.14541500e-07 * tc[2]
            +1.01757603e-10 * tc[3]
            -4.35203420e-15 * tc[4]
            -9.69500000e+02 * invT;
        /*species 18: aC3H5 */
        species[18] =
            +5.50078770e+00
            +7.16236550e-03 * tc[1]
            -1.89272107e-06 * tc[2]
            +2.77020025e-10 * tc[3]
            -1.80727774e-14 * tc[4]
            +1.74824490e+04 * invT;
        /*species 19: C3H6 */
        species[19] =
            +5.73225700e+00
            +7.45417000e-03 * tc[1]
            -1.64996633e-06 * tc[2]
            +1.80300550e-10 * tc[3]
            -7.53240800e-15 * tc[4]
            -9.23570300e+02 * invT;
        /*species 20: C2H3CHO */
        species[20] =
            +4.81118680e+00
            +8.55712800e-03 * tc[1]
            -2.49447203e-06 * tc[2]
            +3.56306225e-10 * tc[3]
            -1.83493682e-14 * tc[4]
            -1.07840540e+04 * invT;
        /*species 21: C4H7 */
        species[21] =
            +6.01348350e+00
            +1.13172790e-02 * tc[1]
            -3.08484900e-06 * tc[2]
            +4.20198175e-10 * tc[3]
            -2.08172340e-14 * tc[4]
            +2.09550080e+04 * invT;
        /*species 22: C4H81 */
        species[22] =
            +1.05358410e+00
            +1.71752535e-02 * tc[1]
            -5.29439900e-06 * tc[2]
            +8.27241550e-10 * tc[3]
            -5.07220900e-14 * tc[4]
            -2.13972310e+03 * invT;
        /*species 23: C5H9 */
        species[23] =
            +9.13864000e+00
            +1.13570690e-02 * tc[1]
            -2.59701543e-06 * tc[2]
            +2.96913050e-10 * tc[3]
            -1.31864896e-14 * tc[4]
            -1.72183590e+03 * invT;
        /*species 32: C12H25O2 */
        species[32] =
            +2.74782000e+01
            +2.68769500e-02 * tc[1]
            -5.60620000e-06 * tc[2]
            +6.28417500e-10 * tc[3]
            -2.94416000e-14 * tc[4]
            -3.74118000e+04 * invT;
        /*species 33: OC12H23OOH */
        species[33] =
            +2.26731000e+01
            +3.08196000e-02 * tc[1]
            -6.99453333e-06 * tc[2]
            +8.32915000e-10 * tc[3]
            -4.07180000e-14 * tc[4]
            -7.18258000e+04 * invT;
        /*species 34: N2 */
        species[34] =
            +1.92664000e+00
            +7.43988400e-04 * tc[1]
            -1.89492000e-07 * tc[2]
            +2.52425950e-11 * tc[3]
            -1.35067020e-15 * tc[4]
            -9.22797700e+02 * invT;
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -2.06223481e+00
            +2.87109147e-02 * tc[1]
            -1.24828963e-05 * tc[2]
            +3.18412472e-09 * tc[3]
            -3.59219578e-13 * tc[4]
            -4.46546666e+03 * invT;
        /*species 25: C6H12 */
        species[25] =
            -2.35275205e+00
            +3.49327713e-02 * tc[1]
            -1.53136007e-05 * tc[2]
            +3.92418358e-09 * tc[3]
            -4.42592350e-13 * tc[4]
            -7.34368617e+03 * invT;
        /*species 26: C7H14 */
        species[26] =
            -2.67720549e+00
            +4.12305800e-02 * tc[1]
            -1.82168036e-05 * tc[2]
            +4.69655757e-09 * tc[3]
            -5.31475966e-13 * tc[4]
            -1.02168601e+04 * invT;
        /*species 27: C8H16 */
        species[27] =
            -2.89226915e+00
            +4.73033178e-02 * tc[1]
            -2.09128507e-05 * tc[2]
            +5.37895772e-09 * tc[3]
            -6.05437366e-13 * tc[4]
            -1.31074559e+04 * invT;
        /*species 28: C9H18 */
        species[28] =
            -3.16108263e+00
            +5.34791485e-02 * tc[1]
            -2.36991081e-05 * tc[2]
            +6.09927692e-09 * tc[3]
            -6.85543094e-13 * tc[4]
            -1.59890847e+04 * invT;
        /*species 30: C10H20 */
        species[30] =
            -3.42901688e+00
            +5.96527990e-02 * tc[1]
            -2.64829675e-05 * tc[2]
            +6.81841490e-09 * tc[3]
            -7.65436746e-13 * tc[4]
            -1.88708365e+04 * invT;
    } else {
        /*species 24: C5H10 */
        species[24] =
            +1.35851539e+01
            +1.12036235e-02 * tc[1]
            -2.54449342e-06 * tc[2]
            +2.95472415e-10 * tc[3]
            -1.36877028e-14 * tc[4]
            -1.00898205e+04 * invT;
        /*species 25: C6H12 */
        species[25] =
            +1.68337529e+01
            +1.33688829e-02 * tc[1]
            -3.03345591e-06 * tc[2]
            +3.52049420e-10 * tc[3]
            -1.63024849e-14 * tc[4]
            -1.42062860e+04 * invT;
        /*species 26: C7H14 */
        species[26] =
            +2.00898039e+01
            +1.55303939e-02 * tc[1]
            -3.52149310e-06 * tc[2]
            +4.08514450e-10 * tc[3]
            -1.89119644e-14 * tc[4]
            -1.83260065e+04 * invT;
        /*species 27: C8H16 */
        species[27] =
            +2.33540125e+01
            +1.76833231e-02 * tc[1]
            -4.00694627e-06 * tc[2]
            +4.64637633e-10 * tc[3]
            -2.15044524e-14 * tc[4]
            -2.24485674e+04 * invT;
        /*species 28: C9H18 */
        species[28] =
            +2.66142176e+01
            +1.98412643e-02 * tc[1]
            -4.49398153e-06 * tc[2]
            +5.20976130e-10 * tc[3]
            -2.41078588e-14 * tc[4]
            -2.65709061e+04 * invT;
        /*species 30: C10H20 */
        species[30] =
            +2.98753903e+01
            +2.19985763e-02 * tc[1]
            -4.98085100e-06 * tc[2]
            +5.77294195e-10 * tc[3]
            -2.67102954e-14 * tc[4]
            -3.06937307e+04 * invT;
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -2.04387292e+00
            +5.28086415e-02 * tc[1]
            -2.22733324e-05 * tc[2]
            +5.36215415e-09 * tc[3]
            -5.54808550e-13 * tc[4]
            -6.80818512e+03 * invT;
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +2.73097514e+01
            +2.06328672e-02 * tc[1]
            -4.67944297e-06 * tc[2]
            +5.42937177e-10 * tc[3]
            -2.51384614e-14 * tc[4]
            -1.74516030e+04 * invT;
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -3.62181594e+00
            +7.36188555e-02 * tc[1]
            -3.14656757e-05 * tc[2]
            +7.68603170e-09 * tc[3]
            -8.07204460e-13 * tc[4]
            -4.00654253e+04 * invT;
        /*species 31: C12H24 */
        species[31] =
            -3.96342681e+00
            +7.19961800e-02 * tc[1]
            -3.20461338e-05 * tc[2]
            +8.25436183e-09 * tc[3]
            -9.24796380e-13 * tc[4]
            -2.46345299e+04 * invT;
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +3.75095037e+01
            +2.81775024e-02 * tc[1]
            -6.38310667e-06 * tc[2]
            +7.40062155e-10 * tc[3]
            -3.42488300e-14 * tc[4]
            -5.48843465e+04 * invT;
        /*species 31: C12H24 */
        species[31] =
            +3.64002111e+01
            +2.63115377e-02 * tc[1]
            -5.95414397e-06 * tc[2]
            +6.89874658e-10 * tc[3]
            -3.19124998e-14 * tc[4]
            -3.89405962e+04 * invT;
    }
    return;
}


/*compute the h/(RT) at the given temperature (Eq 20) */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void speciesEnthalpy(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];
    double invT = 1 / T;

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +2.50000000e+00
            +3.52666409e-13 * tc[1]
            -6.65306547e-16 * tc[2]
            +5.75204080e-19 * tc[3]
            -1.85546466e-22 * tc[4]
            +2.54736599e+04 * invT;
        /*species 2: O */
        species[2] =
            +3.16826710e+00
            -1.63965942e-03 * tc[1]
            +2.21435465e-06 * tc[2]
            -1.53201656e-09 * tc[3]
            +4.22531942e-13 * tc[4]
            +2.91222592e+04 * invT;
        /*species 3: OH */
        species[3] =
            +4.12530561e+00
            -1.61272470e-03 * tc[1]
            +2.17588230e-06 * tc[2]
            -1.44963411e-09 * tc[3]
            +4.12474758e-13 * tc[4]
            +3.38153812e+03 * invT;
        /*species 4: HO2 */
        species[4] =
            +4.30179801e+00
            -2.37456025e-03 * tc[1]
            +7.05276303e-06 * tc[2]
            -6.06909735e-09 * tc[3]
            +1.85845025e-12 * tc[4]
            +2.94808040e+02 * invT;
        /*species 5: H2 */
        species[5] =
            +2.34433112e+00
            +3.99026037e-03 * tc[1]
            -6.49271700e-06 * tc[2]
            +5.03930235e-09 * tc[3]
            -1.47522352e-12 * tc[4]
            -9.17935173e+02 * invT;
        /*species 6: H2O */
        species[6] =
            +4.19864056e+00
            -1.01821705e-03 * tc[1]
            +2.17346737e-06 * tc[2]
            -1.37199266e-09 * tc[3]
            +3.54395634e-13 * tc[4]
            -3.02937267e+04 * invT;
        /*species 7: H2O2 */
        species[7] =
            +4.27611269e+00
            -2.71411208e-04 * tc[1]
            +5.57785670e-06 * tc[2]
            -5.39427032e-09 * tc[3]
            +1.72490873e-12 * tc[4]
            -1.77025821e+04 * invT;
        /*species 8: O2 */
        species[8] =
            +3.78245636e+00
            -1.49836708e-03 * tc[1]
            +3.28243400e-06 * tc[2]
            -2.42032377e-09 * tc[3]
            +6.48745674e-13 * tc[4]
            -1.06394356e+03 * invT;
        /*species 9: CH3 */
        species[9] =
            +3.67359040e+00
            +1.00547588e-03 * tc[1]
            +1.91007285e-06 * tc[2]
            -1.71779356e-09 * tc[3]
            +5.08771468e-13 * tc[4]
            +1.64449988e+04 * invT;
        /*species 10: CH4 */
        species[10] =
            +5.14987613e+00
            -6.83548940e-03 * tc[1]
            +1.63933533e-05 * tc[2]
            -1.21185757e-08 * tc[3]
            +3.33387912e-12 * tc[4]
            -1.02466476e+04 * invT;
        /*species 11: CH2O */
        species[11] =
            +4.79372315e+00
            -4.95416684e-03 * tc[1]
            +1.24406669e-05 * tc[2]
            -9.48213152e-09 * tc[3]
            +2.63545304e-12 * tc[4]
            -1.43089567e+04 * invT;
        /*species 12: CO */
        species[12] =
            +3.57953347e+00
            -3.05176840e-04 * tc[1]
            +3.38938110e-07 * tc[2]
            +2.26751471e-10 * tc[3]
            -1.80884900e-13 * tc[4]
            -1.43440860e+04 * invT;
        /*species 13: CO2 */
        species[13] =
            +2.35677352e+00
            +4.49229839e-03 * tc[1]
            -2.37452090e-06 * tc[2]
            +6.14797555e-10 * tc[3]
            -2.87399096e-14 * tc[4]
            -4.83719697e+04 * invT;
        /*species 14: C2H2 */
        species[14] =
            +8.08681094e-01
            +1.16807815e-02 * tc[1]
            -1.18390605e-05 * tc[2]
            +7.00381092e-09 * tc[3]
            -1.70014595e-12 * tc[4]
            +2.64289807e+04 * invT;
        /*species 15: C2H4 */
        species[15] =
            +3.95920148e+00
            -3.78526124e-03 * tc[1]
            +1.90330097e-05 * tc[2]
            -1.72897188e-08 * tc[3]
            +5.39768746e-12 * tc[4]
            +5.08977593e+03 * invT;
        /*species 16: C2H6 */
        species[16] =
            +4.29142492e+00
            -2.75077135e-03 * tc[1]
            +1.99812763e-05 * tc[2]
            -1.77116571e-08 * tc[3]
            +5.37371542e-12 * tc[4]
            -1.15222055e+04 * invT;
        /*species 17: CH2CHO */
        species[17] =
            +3.40906240e+00
            +5.36928700e-03 * tc[1]
            +6.30497500e-07 * tc[2]
            -1.78964578e-09 * tc[3]
            +5.73477020e-13 * tc[4]
            +6.20000000e+01 * invT;
        /*species 18: aC3H5 */
        species[18] =
            +1.36318350e+00
            +9.90691050e-03 * tc[1]
            +4.16568667e-06 * tc[2]
            -8.33888875e-09 * tc[3]
            +3.16931420e-12 * tc[4]
            +1.92456290e+04 * invT;
        /*species 19: C3H6 */
        species[19] =
            +1.49330700e+00
            +1.04625900e-02 * tc[1]
            +1.49559800e-06 * tc[2]
            -4.17228000e-09 * tc[3]
            +1.43162920e-12 * tc[4]
            +1.07482600e+03 * invT;
        /*species 20: C2H3CHO */
        species[20] =
            +1.27134980e+00
            +1.31155270e-02 * tc[1]
            -3.09707683e-06 * tc[2]
            -1.19593180e-09 * tc[3]
            +6.69610860e-13 * tc[4]
            -9.33573440e+03 * invT;
        /*species 21: C4H7 */
        species[21] =
            +7.44494320e-01
            +1.98394285e-02 * tc[1]
            -7.63269533e-06 * tc[2]
            +5.33824325e-10 * tc[3]
            +4.61927500e-13 * tc[4]
            +2.26533280e+04 * invT;
        /*species 22: C4H81 */
        species[22] =
            +1.18113800e+00
            +1.54266900e-02 * tc[1]
            +1.69550823e-06 * tc[2]
            -6.16372200e-09 * tc[3]
            +2.22203860e-12 * tc[4]
            -1.79040040e+03 * invT;
        /*species 23: C5H9 */
        species[23] =
            -2.41901110e+00
            +2.02151945e-02 * tc[1]
            +2.26007797e-06 * tc[2]
            -8.43118550e-09 * tc[3]
            +3.02334260e-12 * tc[4]
            +2.81218870e+03 * invT;
        /*species 32: C12H25O2 */
        species[32] =
            +5.31404000e+00
            +4.46936500e-02 * tc[1]
            +4.84503333e-06 * tc[2]
            -1.87312500e-08 * tc[3]
            +6.70650000e-12 * tc[4]
            -2.98918000e+04 * invT;
        /*species 33: OC12H23OOH */
        species[33] =
            +8.80733000e+00
            +3.25311500e-02 * tc[1]
            +2.31686000e-05 * tc[2]
            -3.17262500e-08 * tc[3]
            +1.02198200e-11 * tc[4]
            -6.65361000e+04 * invT;
        /*species 34: N2 */
        species[34] =
            +3.29867700e+00
            +7.04120200e-04 * tc[1]
            -1.32107400e-06 * tc[2]
            +1.41037875e-09 * tc[3]
            -4.88970800e-13 * tc[4]
            -1.02089990e+03 * invT;
    } else {
        /*species 1: H */
        species[1] =
            +2.50000001e+00
            -1.15421486e-11 * tc[1]
            +5.38539827e-15 * tc[2]
            -1.18378809e-18 * tc[3]
            +9.96394714e-23 * tc[4]
            +2.54736599e+04 * invT;
        /*species 2: O */
        species[2] =
            +2.56942078e+00
            -4.29870569e-05 * tc[1]
            +1.39828196e-08 * tc[2]
            -2.50444497e-12 * tc[3]
            +2.45667382e-16 * tc[4]
            +2.92175791e+04 * invT;
        /*species 3: OH */
        species[3] =
            +2.86472886e+00
            +5.28252240e-04 * tc[1]
            -8.63609193e-08 * tc[2]
            +7.63046685e-12 * tc[3]
            -2.66391752e-16 * tc[4]
            +3.71885774e+03 * invT;
        /*species 4: HO2 */
        species[4] =
            +4.01721090e+00
            +1.11991006e-03 * tc[1]
            -2.11219383e-07 * tc[2]
            +2.85615925e-11 * tc[3]
            -2.15817070e-15 * tc[4]
            +1.11856713e+02 * invT;
        /*species 5: H2 */
        species[5] =
            +3.33727920e+00
            -2.47012365e-05 * tc[1]
            +1.66485593e-07 * tc[2]
            -4.48915985e-11 * tc[3]
            +4.00510752e-15 * tc[4]
            -9.50158922e+02 * invT;
        /*species 6: H2O */
        species[6] =
            +3.03399249e+00
            +1.08845902e-03 * tc[1]
            -5.46908393e-08 * tc[2]
            -2.42604967e-11 * tc[3]
            +3.36401984e-15 * tc[4]
            -3.00042971e+04 * invT;
        /*species 7: H2O2 */
        species[7] =
            +4.16500285e+00
            +2.45415847e-03 * tc[1]
            -6.33797417e-07 * tc[2]
            +9.27964965e-11 * tc[3]
            -5.75816610e-15 * tc[4]
            -1.78617877e+04 * invT;
        /*species 8: O2 */
        species[8] =
            +3.28253784e+00
            +7.41543770e-04 * tc[1]
            -2.52655556e-07 * tc[2]
            +5.23676387e-11 * tc[3]
            -4.33435588e-15 * tc[4]
            -1.08845772e+03 * invT;
        /*species 9: CH3 */
        species[9] =
            +2.28571772e+00
            +3.61995018e-03 * tc[1]
            -9.95714493e-07 * tc[2]
            +1.48921161e-10 * tc[3]
            -9.34308788e-15 * tc[4]
            +1.67755843e+04 * invT;
        /*species 10: CH4 */
        species[10] =
            +7.48514950e-02
            +6.69547335e-03 * tc[1]
            -1.91095270e-06 * tc[2]
            +3.05731338e-10 * tc[3]
            -2.03630460e-14 * tc[4]
            -9.46834459e+03 * invT;
        /*species 11: CH2O */
        species[11] =
            +1.76069008e+00
            +4.60000041e-03 * tc[1]
            -1.47419604e-06 * tc[2]
            +2.51603030e-10 * tc[3]
            -1.76771128e-14 * tc[4]
            -1.39958323e+04 * invT;
        /*species 12: CO */
        species[12] =
            +2.71518561e+00
            +1.03126372e-03 * tc[1]
            -3.32941924e-07 * tc[2]
            +5.75132520e-11 * tc[3]
            -4.07295432e-15 * tc[4]
            -1.41518724e+04 * invT;
        /*species 13: CO2 */
        species[13] =
            +3.85746029e+00
            +2.20718513e-03 * tc[1]
            -7.38271347e-07 * tc[2]
            +1.30872547e-10 * tc[3]
            -9.44168328e-15 * tc[4]
            -4.87591660e+04 * invT;
        /*species 14: C2H2 */
        species[14] =
            +4.14756964e+00
            +2.98083332e-03 * tc[1]
            -7.90982840e-07 * tc[2]
            +1.16853043e-10 * tc[3]
            -7.22470426e-15 * tc[4]
            +2.59359992e+04 * invT;
        /*species 15: C2H4 */
        species[15] =
            +2.03611116e+00
            +7.32270755e-03 * tc[1]
            -2.23692638e-06 * tc[2]
            +3.68057308e-10 * tc[3]
            -2.51412122e-14 * tc[4]
            +4.93988614e+03 * invT;
        /*species 16: C2H6 */
        species[16] =
            +1.07188150e+00
            +1.08426339e-02 * tc[1]
            -3.34186890e-06 * tc[2]
            +5.53530003e-10 * tc[3]
            -3.80005780e-14 * tc[4]
            -1.14263932e+04 * invT;
        /*species 17: CH2CHO */
        species[17] =
            +5.97566990e+00
            +4.06529570e-03 * tc[1]
            -9.14541500e-07 * tc[2]
            +1.01757603e-10 * tc[3]
            -4.35203420e-15 * tc[4]
            -9.69500000e+02 * invT;
        /*species 18: aC3H5 */
        species[18] =
            +6.50078770e+00
            +7.16236550e-03 * tc[1]
            -1.89272107e-06 * tc[2]
            +2.77020025e-10 * tc[3]
            -1.80727774e-14 * tc[4]
            +1.74824490e+04 * invT;
        /*species 19: C3H6 */
        species[19] =
            +6.73225700e+00
            +7.45417000e-03 * tc[1]
            -1.64996633e-06 * tc[2]
            +1.80300550e-10 * tc[3]
            -7.53240800e-15 * tc[4]
            -9.23570300e+02 * invT;
        /*species 20: C2H3CHO */
        species[20] =
            +5.81118680e+00
            +8.55712800e-03 * tc[1]
            -2.49447203e-06 * tc[2]
            +3.56306225e-10 * tc[3]
            -1.83493682e-14 * tc[4]
            -1.07840540e+04 * invT;
        /*species 21: C4H7 */
        species[21] =
            +7.01348350e+00
            +1.13172790e-02 * tc[1]
            -3.08484900e-06 * tc[2]
            +4.20198175e-10 * tc[3]
            -2.08172340e-14 * tc[4]
            +2.09550080e+04 * invT;
        /*species 22: C4H81 */
        species[22] =
            +2.05358410e+00
            +1.71752535e-02 * tc[1]
            -5.29439900e-06 * tc[2]
            +8.27241550e-10 * tc[3]
            -5.07220900e-14 * tc[4]
            -2.13972310e+03 * invT;
        /*species 23: C5H9 */
        species[23] =
            +1.01386400e+01
            +1.13570690e-02 * tc[1]
            -2.59701543e-06 * tc[2]
            +2.96913050e-10 * tc[3]
            -1.31864896e-14 * tc[4]
            -1.72183590e+03 * invT;
        /*species 32: C12H25O2 */
        species[32] =
            +2.84782000e+01
            +2.68769500e-02 * tc[1]
            -5.60620000e-06 * tc[2]
            +6.28417500e-10 * tc[3]
            -2.94416000e-14 * tc[4]
            -3.74118000e+04 * invT;
        /*species 33: OC12H23OOH */
        species[33] =
            +2.36731000e+01
            +3.08196000e-02 * tc[1]
            -6.99453333e-06 * tc[2]
            +8.32915000e-10 * tc[3]
            -4.07180000e-14 * tc[4]
            -7.18258000e+04 * invT;
        /*species 34: N2 */
        species[34] =
            +2.92664000e+00
            +7.43988400e-04 * tc[1]
            -1.89492000e-07 * tc[2]
            +2.52425950e-11 * tc[3]
            -1.35067020e-15 * tc[4]
            -9.22797700e+02 * invT;
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -1.06223481e+00
            +2.87109147e-02 * tc[1]
            -1.24828963e-05 * tc[2]
            +3.18412472e-09 * tc[3]
            -3.59219578e-13 * tc[4]
            -4.46546666e+03 * invT;
        /*species 25: C6H12 */
        species[25] =
            -1.35275205e+00
            +3.49327713e-02 * tc[1]
            -1.53136007e-05 * tc[2]
            +3.92418358e-09 * tc[3]
            -4.42592350e-13 * tc[4]
            -7.34368617e+03 * invT;
        /*species 26: C7H14 */
        species[26] =
            -1.67720549e+00
            +4.12305800e-02 * tc[1]
            -1.82168036e-05 * tc[2]
            +4.69655757e-09 * tc[3]
            -5.31475966e-13 * tc[4]
            -1.02168601e+04 * invT;
        /*species 27: C8H16 */
        species[27] =
            -1.89226915e+00
            +4.73033178e-02 * tc[1]
            -2.09128507e-05 * tc[2]
            +5.37895772e-09 * tc[3]
            -6.05437366e-13 * tc[4]
            -1.31074559e+04 * invT;
        /*species 28: C9H18 */
        species[28] =
            -2.16108263e+00
            +5.34791485e-02 * tc[1]
            -2.36991081e-05 * tc[2]
            +6.09927692e-09 * tc[3]
            -6.85543094e-13 * tc[4]
            -1.59890847e+04 * invT;
        /*species 30: C10H20 */
        species[30] =
            -2.42901688e+00
            +5.96527990e-02 * tc[1]
            -2.64829675e-05 * tc[2]
            +6.81841490e-09 * tc[3]
            -7.65436746e-13 * tc[4]
            -1.88708365e+04 * invT;
    } else {
        /*species 24: C5H10 */
        species[24] =
            +1.45851539e+01
            +1.12036235e-02 * tc[1]
            -2.54449342e-06 * tc[2]
            +2.95472415e-10 * tc[3]
            -1.36877028e-14 * tc[4]
            -1.00898205e+04 * invT;
        /*species 25: C6H12 */
        species[25] =
            +1.78337529e+01
            +1.33688829e-02 * tc[1]
            -3.03345591e-06 * tc[2]
            +3.52049420e-10 * tc[3]
            -1.63024849e-14 * tc[4]
            -1.42062860e+04 * invT;
        /*species 26: C7H14 */
        species[26] =
            +2.10898039e+01
            +1.55303939e-02 * tc[1]
            -3.52149310e-06 * tc[2]
            +4.08514450e-10 * tc[3]
            -1.89119644e-14 * tc[4]
            -1.83260065e+04 * invT;
        /*species 27: C8H16 */
        species[27] =
            +2.43540125e+01
            +1.76833231e-02 * tc[1]
            -4.00694627e-06 * tc[2]
            +4.64637633e-10 * tc[3]
            -2.15044524e-14 * tc[4]
            -2.24485674e+04 * invT;
        /*species 28: C9H18 */
        species[28] =
            +2.76142176e+01
            +1.98412643e-02 * tc[1]
            -4.49398153e-06 * tc[2]
            +5.20976130e-10 * tc[3]
            -2.41078588e-14 * tc[4]
            -2.65709061e+04 * invT;
        /*species 30: C10H20 */
        species[30] =
            +3.08753903e+01
            +2.19985763e-02 * tc[1]
            -4.98085100e-06 * tc[2]
            +5.77294195e-10 * tc[3]
            -2.67102954e-14 * tc[4]
            -3.06937307e+04 * invT;
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -1.04387292e+00
            +5.28086415e-02 * tc[1]
            -2.22733324e-05 * tc[2]
            +5.36215415e-09 * tc[3]
            -5.54808550e-13 * tc[4]
            -6.80818512e+03 * invT;
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +2.83097514e+01
            +2.06328672e-02 * tc[1]
            -4.67944297e-06 * tc[2]
            +5.42937177e-10 * tc[3]
            -2.51384614e-14 * tc[4]
            -1.74516030e+04 * invT;
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -2.62181594e+00
            +7.36188555e-02 * tc[1]
            -3.14656757e-05 * tc[2]
            +7.68603170e-09 * tc[3]
            -8.07204460e-13 * tc[4]
            -4.00654253e+04 * invT;
        /*species 31: C12H24 */
        species[31] =
            -2.96342681e+00
            +7.19961800e-02 * tc[1]
            -3.20461338e-05 * tc[2]
            +8.25436183e-09 * tc[3]
            -9.24796380e-13 * tc[4]
            -2.46345299e+04 * invT;
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +3.85095037e+01
            +2.81775024e-02 * tc[1]
            -6.38310667e-06 * tc[2]
            +7.40062155e-10 * tc[3]
            -3.42488300e-14 * tc[4]
            -5.48843465e+04 * invT;
        /*species 31: C12H24 */
        species[31] =
            +3.74002111e+01
            +2.63115377e-02 * tc[1]
            -5.95414397e-06 * tc[2]
            +6.89874658e-10 * tc[3]
            -3.19124998e-14 * tc[4]
            -3.89405962e+04 * invT;
    }
    return;
}


/*compute the S/R at the given temperature (Eq 21) */
/*tc contains precomputed powers of T, tc[0] = log(T) */
AMREX_GPU_HOST_DEVICE void speciesEntropy(double * species, double *  tc)
{

    /*temperature */
    double T = tc[1];

    /*species with midpoint at T=1000 kelvin */
    if (T < 1000) {
        /*species 1: H */
        species[1] =
            +2.50000000e+00 * tc[0]
            +7.05332819e-13 * tc[1]
            -9.97959820e-16 * tc[2]
            +7.66938773e-19 * tc[3]
            -2.31933083e-22 * tc[4]
            -4.46682853e-01 ;
        /*species 2: O */
        species[2] =
            +3.16826710e+00 * tc[0]
            -3.27931884e-03 * tc[1]
            +3.32153198e-06 * tc[2]
            -2.04268875e-09 * tc[3]
            +5.28164927e-13 * tc[4]
            +2.05193346e+00 ;
        /*species 3: OH */
        species[3] =
            +4.12530561e+00 * tc[0]
            -3.22544939e-03 * tc[1]
            +3.26382346e-06 * tc[2]
            -1.93284548e-09 * tc[3]
            +5.15593447e-13 * tc[4]
            -6.90432960e-01 ;
        /*species 4: HO2 */
        species[4] =
            +4.30179801e+00 * tc[0]
            -4.74912051e-03 * tc[1]
            +1.05791445e-05 * tc[2]
            -8.09212980e-09 * tc[3]
            +2.32306281e-12 * tc[4]
            +3.71666245e+00 ;
        /*species 5: H2 */
        species[5] =
            +2.34433112e+00 * tc[0]
            +7.98052075e-03 * tc[1]
            -9.73907550e-06 * tc[2]
            +6.71906980e-09 * tc[3]
            -1.84402940e-12 * tc[4]
            +6.83010238e-01 ;
        /*species 6: H2O */
        species[6] =
            +4.19864056e+00 * tc[0]
            -2.03643410e-03 * tc[1]
            +3.26020105e-06 * tc[2]
            -1.82932354e-09 * tc[3]
            +4.42994543e-13 * tc[4]
            -8.49032208e-01 ;
        /*species 7: H2O2 */
        species[7] =
            +4.27611269e+00 * tc[0]
            -5.42822417e-04 * tc[1]
            +8.36678505e-06 * tc[2]
            -7.19236043e-09 * tc[3]
            +2.15613591e-12 * tc[4]
            +3.43505074e+00 ;
        /*species 8: O2 */
        species[8] =
            +3.78245636e+00 * tc[0]
            -2.99673416e-03 * tc[1]
            +4.92365101e-06 * tc[2]
            -3.22709836e-09 * tc[3]
            +8.10932092e-13 * tc[4]
            +3.65767573e+00 ;
        /*species 9: CH3 */
        species[9] =
            +3.67359040e+00 * tc[0]
            +2.01095175e-03 * tc[1]
            +2.86510928e-06 * tc[2]
            -2.29039142e-09 * tc[3]
            +6.35964335e-13 * tc[4]
            +1.60456433e+00 ;
        /*species 10: CH4 */
        species[10] =
            +5.14987613e+00 * tc[0]
            -1.36709788e-02 * tc[1]
            +2.45900299e-05 * tc[2]
            -1.61581009e-08 * tc[3]
            +4.16734890e-12 * tc[4]
            -4.64130376e+00 ;
        /*species 11: CH2O */
        species[11] =
            +4.79372315e+00 * tc[0]
            -9.90833369e-03 * tc[1]
            +1.86610004e-05 * tc[2]
            -1.26428420e-08 * tc[3]
            +3.29431630e-12 * tc[4]
            +6.02812900e-01 ;
        /*species 12: CO */
        species[12] =
            +3.57953347e+00 * tc[0]
            -6.10353680e-04 * tc[1]
            +5.08407165e-07 * tc[2]
            +3.02335295e-10 * tc[3]
            -2.26106125e-13 * tc[4]
            +3.50840928e+00 ;
        /*species 13: CO2 */
        species[13] =
            +2.35677352e+00 * tc[0]
            +8.98459677e-03 * tc[1]
            -3.56178134e-06 * tc[2]
            +8.19730073e-10 * tc[3]
            -3.59248870e-14 * tc[4]
            +9.90105222e+00 ;
        /*species 14: C2H2 */
        species[14] =
            +8.08681094e-01 * tc[0]
            +2.33615629e-02 * tc[1]
            -1.77585907e-05 * tc[2]
            +9.33841457e-09 * tc[3]
            -2.12518243e-12 * tc[4]
            +1.39397051e+01 ;
        /*species 15: C2H4 */
        species[15] =
            +3.95920148e+00 * tc[0]
            -7.57052247e-03 * tc[1]
            +2.85495146e-05 * tc[2]
            -2.30529584e-08 * tc[3]
            +6.74710933e-12 * tc[4]
            +4.09733096e+00 ;
        /*species 16: C2H6 */
        species[16] =
            +4.29142492e+00 * tc[0]
            -5.50154270e-03 * tc[1]
            +2.99719144e-05 * tc[2]
            -2.36155428e-08 * tc[3]
            +6.71714427e-12 * tc[4]
            +2.66682316e+00 ;
        /*species 17: CH2CHO */
        species[17] =
            +3.40906240e+00 * tc[0]
            +1.07385740e-02 * tc[1]
            +9.45746250e-07 * tc[2]
            -2.38619437e-09 * tc[3]
            +7.16846275e-13 * tc[4]
            +9.57145350e+00 ;
        /*species 18: aC3H5 */
        species[18] =
            +1.36318350e+00 * tc[0]
            +1.98138210e-02 * tc[1]
            +6.24853000e-06 * tc[2]
            -1.11185183e-08 * tc[3]
            +3.96164275e-12 * tc[4]
            +1.71732140e+01 ;
        /*species 19: C3H6 */
        species[19] =
            +1.49330700e+00 * tc[0]
            +2.09251800e-02 * tc[1]
            +2.24339700e-06 * tc[2]
            -5.56304000e-09 * tc[3]
            +1.78953650e-12 * tc[4]
            +1.61453400e+01 ;
        /*species 20: C2H3CHO */
        species[20] =
            +1.27134980e+00 * tc[0]
            +2.62310540e-02 * tc[1]
            -4.64561525e-06 * tc[2]
            -1.59457573e-09 * tc[3]
            +8.37013575e-13 * tc[4]
            +1.94980770e+01 ;
        /*species 21: C4H7 */
        species[21] =
            +7.44494320e-01 * tc[0]
            +3.96788570e-02 * tc[1]
            -1.14490430e-05 * tc[2]
            +7.11765767e-10 * tc[3]
            +5.77409375e-13 * tc[4]
            +2.34378780e+01 ;
        /*species 22: C4H81 */
        species[22] =
            +1.18113800e+00 * tc[0]
            +3.08533800e-02 * tc[1]
            +2.54326235e-06 * tc[2]
            -8.21829600e-09 * tc[3]
            +2.77754825e-12 * tc[4]
            +2.10624690e+01 ;
        /*species 23: C5H9 */
        species[23] =
            -2.41901110e+00 * tc[0]
            +4.04303890e-02 * tc[1]
            +3.39011695e-06 * tc[2]
            -1.12415807e-08 * tc[3]
            +3.77917825e-12 * tc[4]
            +3.64592440e+01 ;
        /*species 32: C12H25O2 */
        species[32] =
            +5.31404000e+00 * tc[0]
            +8.93873000e-02 * tc[1]
            +7.26755000e-06 * tc[2]
            -2.49750000e-08 * tc[3]
            +8.38312500e-12 * tc[4]
            +1.69741000e+01 ;
        /*species 33: OC12H23OOH */
        species[33] =
            +8.80733000e+00 * tc[0]
            +6.50623000e-02 * tc[1]
            +3.47529000e-05 * tc[2]
            -4.23016667e-08 * tc[3]
            +1.27747750e-11 * tc[4]
            +6.84155000e+00 ;
        /*species 34: N2 */
        species[34] =
            +3.29867700e+00 * tc[0]
            +1.40824040e-03 * tc[1]
            -1.98161100e-06 * tc[2]
            +1.88050500e-09 * tc[3]
            -6.11213500e-13 * tc[4]
            +3.95037200e+00 ;
    } else {
        /*species 1: H */
        species[1] =
            +2.50000001e+00 * tc[0]
            -2.30842973e-11 * tc[1]
            +8.07809740e-15 * tc[2]
            -1.57838412e-18 * tc[3]
            +1.24549339e-22 * tc[4]
            -4.46682914e-01 ;
        /*species 2: O */
        species[2] =
            +2.56942078e+00 * tc[0]
            -8.59741137e-05 * tc[1]
            +2.09742295e-08 * tc[2]
            -3.33925997e-12 * tc[3]
            +3.07084227e-16 * tc[4]
            +4.78433864e+00 ;
        /*species 3: OH */
        species[3] =
            +2.86472886e+00 * tc[0]
            +1.05650448e-03 * tc[1]
            -1.29541379e-07 * tc[2]
            +1.01739558e-11 * tc[3]
            -3.32989690e-16 * tc[4]
            +5.70164073e+00 ;
        /*species 4: HO2 */
        species[4] =
            +4.01721090e+00 * tc[0]
            +2.23982013e-03 * tc[1]
            -3.16829075e-07 * tc[2]
            +3.80821233e-11 * tc[3]
            -2.69771337e-15 * tc[4]
            +3.78510215e+00 ;
        /*species 5: H2 */
        species[5] =
            +3.33727920e+00 * tc[0]
            -4.94024731e-05 * tc[1]
            +2.49728389e-07 * tc[2]
            -5.98554647e-11 * tc[3]
            +5.00638440e-15 * tc[4]
            -3.20502331e+00 ;
        /*species 6: H2O */
        species[6] =
            +3.03399249e+00 * tc[0]
            +2.17691804e-03 * tc[1]
            -8.20362590e-08 * tc[2]
            -3.23473290e-11 * tc[3]
            +4.20502480e-15 * tc[4]
            +4.96677010e+00 ;
        /*species 7: H2O2 */
        species[7] =
            +4.16500285e+00 * tc[0]
            +4.90831694e-03 * tc[1]
            -9.50696125e-07 * tc[2]
            +1.23728662e-10 * tc[3]
            -7.19770763e-15 * tc[4]
            +2.91615662e+00 ;
        /*species 8: O2 */
        species[8] =
            +3.28253784e+00 * tc[0]
            +1.48308754e-03 * tc[1]
            -3.78983334e-07 * tc[2]
            +6.98235183e-11 * tc[3]
            -5.41794485e-15 * tc[4]
            +5.45323129e+00 ;
        /*species 9: CH3 */
        species[9] =
            +2.28571772e+00 * tc[0]
            +7.23990037e-03 * tc[1]
            -1.49357174e-06 * tc[2]
            +1.98561548e-10 * tc[3]
            -1.16788599e-14 * tc[4]
            +8.48007179e+00 ;
        /*species 10: CH4 */
        species[10] =
            +7.48514950e-02 * tc[0]
            +1.33909467e-02 * tc[1]
            -2.86642905e-06 * tc[2]
            +4.07641783e-10 * tc[3]
            -2.54538075e-14 * tc[4]
            +1.84373180e+01 ;
        /*species 11: CH2O */
        species[11] =
            +1.76069008e+00 * tc[0]
            +9.20000082e-03 * tc[1]
            -2.21129406e-06 * tc[2]
            +3.35470707e-10 * tc[3]
            -2.20963910e-14 * tc[4]
            +1.36563230e+01 ;
        /*species 12: CO */
        species[12] =
            +2.71518561e+00 * tc[0]
            +2.06252743e-03 * tc[1]
            -4.99412886e-07 * tc[2]
            +7.66843360e-11 * tc[3]
            -5.09119290e-15 * tc[4]
            +7.81868772e+00 ;
        /*species 13: CO2 */
        species[13] =
            +3.85746029e+00 * tc[0]
            +4.41437026e-03 * tc[1]
            -1.10740702e-06 * tc[2]
            +1.74496729e-10 * tc[3]
            -1.18021041e-14 * tc[4]
            +2.27163806e+00 ;
        /*species 14: C2H2 */
        species[14] =
            +4.14756964e+00 * tc[0]
            +5.96166664e-03 * tc[1]
            -1.18647426e-06 * tc[2]
            +1.55804057e-10 * tc[3]
            -9.03088033e-15 * tc[4]
            -1.23028121e+00 ;
        /*species 15: C2H4 */
        species[15] =
            +2.03611116e+00 * tc[0]
            +1.46454151e-02 * tc[1]
            -3.35538958e-06 * tc[2]
            +4.90743077e-10 * tc[3]
            -3.14265152e-14 * tc[4]
            +1.03053693e+01 ;
        /*species 16: C2H6 */
        species[16] =
            +1.07188150e+00 * tc[0]
            +2.16852677e-02 * tc[1]
            -5.01280335e-06 * tc[2]
            +7.38040003e-10 * tc[3]
            -4.75007225e-14 * tc[4]
            +1.51156107e+01 ;
        /*species 17: CH2CHO */
        species[17] =
            +5.97566990e+00 * tc[0]
            +8.13059140e-03 * tc[1]
            -1.37181225e-06 * tc[2]
            +1.35676803e-10 * tc[3]
            -5.44004275e-15 * tc[4]
            -5.03208790e+00 ;
        /*species 18: aC3H5 */
        species[18] =
            +6.50078770e+00 * tc[0]
            +1.43247310e-02 * tc[1]
            -2.83908160e-06 * tc[2]
            +3.69360033e-10 * tc[3]
            -2.25909717e-14 * tc[4]
            -1.12430500e+01 ;
        /*species 19: C3H6 */
        species[19] =
            +6.73225700e+00 * tc[0]
            +1.49083400e-02 * tc[1]
            -2.47494950e-06 * tc[2]
            +2.40400733e-10 * tc[3]
            -9.41551000e-15 * tc[4]
            -1.33133500e+01 ;
        /*species 20: C2H3CHO */
        species[20] =
            +5.81118680e+00 * tc[0]
            +1.71142560e-02 * tc[1]
            -3.74170805e-06 * tc[2]
            +4.75074967e-10 * tc[3]
            -2.29367102e-14 * tc[4]
            -4.85880040e+00 ;
        /*species 21: C4H7 */
        species[21] =
            +7.01348350e+00 * tc[0]
            +2.26345580e-02 * tc[1]
            -4.62727350e-06 * tc[2]
            +5.60264233e-10 * tc[3]
            -2.60215425e-14 * tc[4]
            -8.88930800e+00 ;
        /*species 22: C4H81 */
        species[22] =
            +2.05358410e+00 * tc[0]
            +3.43505070e-02 * tc[1]
            -7.94159850e-06 * tc[2]
            +1.10298873e-09 * tc[3]
            -6.34026125e-14 * tc[4]
            +1.55432010e+01 ;
        /*species 23: C5H9 */
        species[23] =
            +1.01386400e+01 * tc[0]
            +2.27141380e-02 * tc[1]
            -3.89552315e-06 * tc[2]
            +3.95884067e-10 * tc[3]
            -1.64831120e-14 * tc[4]
            -3.31258850e+01 ;
        /*species 32: C12H25O2 */
        species[32] =
            +2.84782000e+01 * tc[0]
            +5.37539000e-02 * tc[1]
            -8.40930000e-06 * tc[2]
            +8.37890000e-10 * tc[3]
            -3.68020000e-14 * tc[4]
            -1.09121000e+02 ;
        /*species 33: OC12H23OOH */
        species[33] =
            +2.36731000e+01 * tc[0]
            +6.16392000e-02 * tc[1]
            -1.04918000e-05 * tc[2]
            +1.11055333e-09 * tc[3]
            -5.08975000e-14 * tc[4]
            -7.77662000e+01 ;
        /*species 34: N2 */
        species[34] =
            +2.92664000e+00 * tc[0]
            +1.48797680e-03 * tc[1]
            -2.84238000e-07 * tc[2]
            +3.36567933e-11 * tc[3]
            -1.68833775e-15 * tc[4]
            +5.98052800e+00 ;
    }

    /*species with midpoint at T=1392 kelvin */
    if (T < 1392) {
        /*species 24: C5H10 */
        species[24] =
            -1.06223481e+00 * tc[0]
            +5.74218294e-02 * tc[1]
            -1.87243445e-05 * tc[2]
            +4.24549963e-09 * tc[3]
            -4.49024472e-13 * tc[4]
            +3.22739790e+01 ;
        /*species 25: C6H12 */
        species[25] =
            -1.35275205e+00 * tc[0]
            +6.98655426e-02 * tc[1]
            -2.29704011e-05 * tc[2]
            +5.23224477e-09 * tc[3]
            -5.53240438e-13 * tc[4]
            +3.53120691e+01 ;
        /*species 26: C7H14 */
        species[26] =
            -1.67720549e+00 * tc[0]
            +8.24611601e-02 * tc[1]
            -2.73252054e-05 * tc[2]
            +6.26207677e-09 * tc[3]
            -6.64344957e-13 * tc[4]
            +3.85068032e+01 ;
        /*species 27: C8H16 */
        species[27] =
            -1.89226915e+00 * tc[0]
            +9.46066357e-02 * tc[1]
            -3.13692761e-05 * tc[2]
            +7.17194363e-09 * tc[3]
            -7.56796708e-13 * tc[4]
            +4.11878981e+01 ;
        /*species 28: C9H18 */
        species[28] =
            -2.16108263e+00 * tc[0]
            +1.06958297e-01 * tc[1]
            -3.55486622e-05 * tc[2]
            +8.13236923e-09 * tc[3]
            -8.56928867e-13 * tc[4]
            +4.41245128e+01 ;
        /*species 30: C10H20 */
        species[30] =
            -2.42901688e+00 * tc[0]
            +1.19305598e-01 * tc[1]
            -3.97244513e-05 * tc[2]
            +9.09121987e-09 * tc[3]
            -9.56795932e-13 * tc[4]
            +4.70571383e+01 ;
    } else {
        /*species 24: C5H10 */
        species[24] =
            +1.45851539e+01 * tc[0]
            +2.24072471e-02 * tc[1]
            -3.81674012e-06 * tc[2]
            +3.93963220e-10 * tc[3]
            -1.71096285e-14 * tc[4]
            -5.23683936e+01 ;
        /*species 25: C6H12 */
        species[25] =
            +1.78337529e+01 * tc[0]
            +2.67377658e-02 * tc[1]
            -4.55018387e-06 * tc[2]
            +4.69399227e-10 * tc[3]
            -2.03781061e-14 * tc[4]
            -6.83818851e+01 ;
        /*species 26: C7H14 */
        species[26] =
            +2.10898039e+01 * tc[0]
            +3.10607878e-02 * tc[1]
            -5.28223965e-06 * tc[2]
            +5.44685933e-10 * tc[3]
            -2.36399555e-14 * tc[4]
            -8.44391108e+01 ;
        /*species 27: C8H16 */
        species[27] =
            +2.43540125e+01 * tc[0]
            +3.53666462e-02 * tc[1]
            -6.01041940e-06 * tc[2]
            +6.19516843e-10 * tc[3]
            -2.68805655e-14 * tc[4]
            -1.00537716e+02 ;
        /*species 28: C9H18 */
        species[28] =
            +2.76142176e+01 * tc[0]
            +3.96825287e-02 * tc[1]
            -6.74097230e-06 * tc[2]
            +6.94634840e-10 * tc[3]
            -3.01348235e-14 * tc[4]
            -1.16618623e+02 ;
        /*species 30: C10H20 */
        species[30] =
            +3.08753903e+01 * tc[0]
            +4.39971526e-02 * tc[1]
            -7.47127650e-06 * tc[2]
            +7.69725593e-10 * tc[3]
            -3.33878693e-14 * tc[4]
            -1.32705172e+02 ;
    }

    /*species with midpoint at T=1390 kelvin */
    if (T < 1390) {
        /*species 29: PXC9H19 */
        species[29] =
            -1.04387292e+00 * tc[0]
            +1.05617283e-01 * tc[1]
            -3.34099986e-05 * tc[2]
            +7.14953887e-09 * tc[3]
            -6.93510688e-13 * tc[4]
            +4.23518992e+01 ;
    } else {
        /*species 29: PXC9H19 */
        species[29] =
            +2.83097514e+01 * tc[0]
            +4.12657344e-02 * tc[1]
            -7.01916445e-06 * tc[2]
            +7.23916237e-10 * tc[3]
            -3.14230767e-14 * tc[4]
            -1.16837897e+02 ;
    }

    /*species with midpoint at T=1391 kelvin */
    if (T < 1391) {
        /*species 0: NC12H26 */
        species[0] =
            -2.62181594e+00 * tc[0]
            +1.47237711e-01 * tc[1]
            -4.71985136e-05 * tc[2]
            +1.02480423e-08 * tc[3]
            -1.00900558e-12 * tc[4]
            +5.00994626e+01 ;
        /*species 31: C12H24 */
        species[31] =
            -2.96342681e+00 * tc[0]
            +1.43992360e-01 * tc[1]
            -4.80692008e-05 * tc[2]
            +1.10058158e-08 * tc[3]
            -1.15599547e-12 * tc[4]
            +5.29158870e+01 ;
    } else {
        /*species 0: NC12H26 */
        species[0] =
            +3.85095037e+01 * tc[0]
            +5.63550048e-02 * tc[1]
            -9.57466000e-06 * tc[2]
            +9.86749540e-10 * tc[3]
            -4.28110375e-14 * tc[4]
            -1.72670922e+02 ;
        /*species 31: C12H24 */
        species[31] =
            +3.74002111e+01 * tc[0]
            +5.26230753e-02 * tc[1]
            -8.93121595e-06 * tc[2]
            +9.19832877e-10 * tc[3]
            -3.98906247e-14 * tc[4]
            -1.64892663e+02 ;
    }
    return;
}


/*save molecular weights into array */
void molecularWeight(double *  wt)
{
    wt[0] = 170.341020; /*NC12H26 */
    wt[1] = 1.007970; /*H */
    wt[2] = 15.999400; /*O */
    wt[3] = 17.007370; /*OH */
    wt[4] = 33.006770; /*HO2 */
    wt[5] = 2.015940; /*H2 */
    wt[6] = 18.015340; /*H2O */
    wt[7] = 34.014740; /*H2O2 */
    wt[8] = 31.998800; /*O2 */
    wt[9] = 15.035060; /*CH3 */
    wt[10] = 16.043030; /*CH4 */
    wt[11] = 30.026490; /*CH2O */
    wt[12] = 28.010550; /*CO */
    wt[13] = 44.009950; /*CO2 */
    wt[14] = 26.038240; /*C2H2 */
    wt[15] = 28.054180; /*C2H4 */
    wt[16] = 30.070120; /*C2H6 */
    wt[17] = 43.045610; /*CH2CHO */
    wt[18] = 41.073300; /*aC3H5 */
    wt[19] = 42.081270; /*C3H6 */
    wt[20] = 56.064730; /*C2H3CHO */
    wt[21] = 55.100390; /*C4H7 */
    wt[22] = 56.108360; /*C4H81 */
    wt[23] = 69.127480; /*C5H9 */
    wt[24] = 70.135450; /*C5H10 */
    wt[25] = 84.162540; /*C6H12 */
    wt[26] = 98.189630; /*C7H14 */
    wt[27] = 112.216720; /*C8H16 */
    wt[28] = 126.243810; /*C9H18 */
    wt[29] = 127.251780; /*PXC9H19 */
    wt[30] = 140.270900; /*C10H20 */
    wt[31] = 168.325080; /*C12H24 */
    wt[32] = 201.331850; /*C12H25O2 */
    wt[33] = 216.323280; /*OC12H23OOH */
    wt[34] = 28.013400; /*N2 */

    return;
}


/*save atomic weights into array */
void atomicWeight(double *  awt)
{
    awt[0] = 15.999400; /*O */
    awt[1] = 1.007970; /*H */
    awt[2] = 12.011150; /*C */
    awt[3] = 14.006700; /*N */

    return;
}


/* get temperature given internal energy in mass units and mass fracs */
AMREX_GPU_HOST_DEVICE void GET_T_GIVEN_EY(double *  e, double *  y, double *  t, int * ierr)
{
#ifdef CONVERGENCE
    const int maxiter = 5000;
    const double tol  = 1.e-12;
#else
    const int maxiter = 200;
    const double tol  = 1.e-6;
#endif
    double ein  = *e;
    double tmin = 90;/*max lower bound for thermo def */
    double tmax = 4000;/*min upper bound for thermo def */
    double e1,emin,emax,cv,t1,dt;
    int i;/* loop counter */
    CKUBMS(&tmin, y, &emin);
    CKUBMS(&tmax, y, &emax);
    if (ein < emin) {
        /*Linear Extrapolation below tmin */
        CKCVBS(&tmin, y, &cv);
        *t = tmin - (emin-ein)/cv;
        *ierr = 1;
        return;
    }
    if (ein > emax) {
        /*Linear Extrapolation above tmax */
        CKCVBS(&tmax, y, &cv);
        *t = tmax - (emax-ein)/cv;
        *ierr = 1;
        return;
    }
    t1 = *t;
    if (t1 < tmin || t1 > tmax) {
        t1 = tmin + (tmax-tmin)/(emax-emin)*(ein-emin);
    }
    for (i = 0; i < maxiter; ++i) {
        CKUBMS(&t1,y,&e1);
        CKCVBS(&t1,y,&cv);
        dt = (ein - e1) / cv;
        if (dt > 100.) { dt = 100.; }
        else if (dt < -100.) { dt = -100.; }
        else if (fabs(dt) < tol) break;
        else if (t1+dt == t1) break;
        t1 += dt;
    }
    *t = t1;
    *ierr = 0;
    return;
}

/* get temperature given enthalpy in mass units and mass fracs */
void GET_T_GIVEN_HY(double *  h, double *  y, double *  t, int * ierr)
{
#ifdef CONVERGENCE
    const int maxiter = 5000;
    const double tol  = 1.e-12;
#else
    const int maxiter = 200;
    const double tol  = 1.e-6;
#endif
    double hin  = *h;
    double tmin = 90;/*max lower bound for thermo def */
    double tmax = 4000;/*min upper bound for thermo def */
    double h1,hmin,hmax,cp,t1,dt;
    int i;/* loop counter */
    CKHBMS(&tmin, y, &hmin);
    CKHBMS(&tmax, y, &hmax);
    if (hin < hmin) {
        /*Linear Extrapolation below tmin */
        CKCPBS(&tmin, y, &cp);
        *t = tmin - (hmin-hin)/cp;
        *ierr = 1;
        return;
    }
    if (hin > hmax) {
        /*Linear Extrapolation above tmax */
        CKCPBS(&tmax, y, &cp);
        *t = tmax - (hmax-hin)/cp;
        *ierr = 1;
        return;
    }
    t1 = *t;
    if (t1 < tmin || t1 > tmax) {
        t1 = tmin + (tmax-tmin)/(hmax-hmin)*(hin-hmin);
    }
    for (i = 0; i < maxiter; ++i) {
        CKHBMS(&t1,y,&h1);
        CKCPBS(&t1,y,&cp);
        dt = (hin - h1) / cp;
        if (dt > 100.) { dt = 100.; }
        else if (dt < -100.) { dt = -100.; }
        else if (fabs(dt) < tol) break;
        else if (t1+dt == t1) break;
        t1 += dt;
    }
    *t = t1;
    *ierr = 0;
    return;
}


/*compute the critical parameters for each species */
void GET_CRITPARAMS(double *  Tci, double *  ai, double *  bi, double *  acentric_i)
{

    double   EPS[35];
    double   SIG[35];
    double    wt[35];
    double avogadro = 6.02214199e23;
    double boltzmann = 1.3806503e-16; //we work in CGS
    double Rcst = 83.144598; //in bar [CGS] !

    egtransetEPS(EPS);
    egtransetSIG(SIG);
    molecularWeight(wt);

    /*species 0: NC12H26 */
    Tci[0] = 1.316 * EPS[0] ; 
    ai[0] = (5.55 * pow(avogadro,2.0) * EPS[0]*boltzmann * pow(1e-8*SIG[0],3.0) ) / (pow(wt[0],2.0)); 
    bi[0] = 0.855 * avogadro * pow(1e-8*SIG[0],3.0) / (wt[0]); 
    acentric_i[0] = 0.0 ;

    /*species 1: H */
    Tci[1] = 1.316 * EPS[1] ; 
    ai[1] = (5.55 * pow(avogadro,2.0) * EPS[1]*boltzmann * pow(1e-8*SIG[1],3.0) ) / (pow(wt[1],2.0)); 
    bi[1] = 0.855 * avogadro * pow(1e-8*SIG[1],3.0) / (wt[1]); 
    acentric_i[1] = 0.0 ;

    /*species 2: O */
    Tci[2] = 1.316 * EPS[2] ; 
    ai[2] = (5.55 * pow(avogadro,2.0) * EPS[2]*boltzmann * pow(1e-8*SIG[2],3.0) ) / (pow(wt[2],2.0)); 
    bi[2] = 0.855 * avogadro * pow(1e-8*SIG[2],3.0) / (wt[2]); 
    acentric_i[2] = 0.0 ;

    /*species 3: OH */
    Tci[3] = 1.316 * EPS[3] ; 
    ai[3] = (5.55 * pow(avogadro,2.0) * EPS[3]*boltzmann * pow(1e-8*SIG[3],3.0) ) / (pow(wt[3],2.0)); 
    bi[3] = 0.855 * avogadro * pow(1e-8*SIG[3],3.0) / (wt[3]); 
    acentric_i[3] = 0.0 ;

    /*species 4: HO2 */
    Tci[4] = 1.316 * EPS[4] ; 
    ai[4] = (5.55 * pow(avogadro,2.0) * EPS[4]*boltzmann * pow(1e-8*SIG[4],3.0) ) / (pow(wt[4],2.0)); 
    bi[4] = 0.855 * avogadro * pow(1e-8*SIG[4],3.0) / (wt[4]); 
    acentric_i[4] = 0.0 ;

    /*species 5: H2 */
    /*Imported from NIST */
    Tci[5] = 33.145000 ; 
    ai[5] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[5],2.0) / (pow(2.015880,2.0) * 12.964000); 
    bi[5] = 0.08664 * Rcst * Tci[5] / (2.015880 * 12.964000); 
    acentric_i[5] = -0.219000 ;

    /*species 6: H2O */
    /*Imported from NIST */
    Tci[6] = 647.096000 ; 
    ai[6] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[6],2.0) / (pow(18.015340,2.0) * 220.640000); 
    bi[6] = 0.08664 * Rcst * Tci[6] / (18.015340 * 220.640000); 
    acentric_i[6] = 0.344300 ;

    /*species 7: H2O2 */
    Tci[7] = 1.316 * EPS[7] ; 
    ai[7] = (5.55 * pow(avogadro,2.0) * EPS[7]*boltzmann * pow(1e-8*SIG[7],3.0) ) / (pow(wt[7],2.0)); 
    bi[7] = 0.855 * avogadro * pow(1e-8*SIG[7],3.0) / (wt[7]); 
    acentric_i[7] = 0.0 ;

    /*species 8: O2 */
    /*Imported from NIST */
    Tci[8] = 154.581000 ; 
    ai[8] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[8],2.0) / (pow(31.998800,2.0) * 50.430466); 
    bi[8] = 0.08664 * Rcst * Tci[8] / (31.998800 * 50.430466); 
    acentric_i[8] = 0.022200 ;

    /*species 9: CH3 */
    Tci[9] = 1.316 * EPS[9] ; 
    ai[9] = (5.55 * pow(avogadro,2.0) * EPS[9]*boltzmann * pow(1e-8*SIG[9],3.0) ) / (pow(wt[9],2.0)); 
    bi[9] = 0.855 * avogadro * pow(1e-8*SIG[9],3.0) / (wt[9]); 
    acentric_i[9] = 0.0 ;

    /*species 10: CH4 */
    /*Imported from NIST */
    Tci[10] = 190.560000 ; 
    ai[10] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[10],2.0) / (pow(16.043030,2.0) * 45.990000); 
    bi[10] = 0.08664 * Rcst * Tci[10] / (16.043030 * 45.990000); 
    acentric_i[10] = 0.011000 ;

    /*species 11: CH2O */
    Tci[11] = 1.316 * EPS[11] ; 
    ai[11] = (5.55 * pow(avogadro,2.0) * EPS[11]*boltzmann * pow(1e-8*SIG[11],3.0) ) / (pow(wt[11],2.0)); 
    bi[11] = 0.855 * avogadro * pow(1e-8*SIG[11],3.0) / (wt[11]); 
    acentric_i[11] = 0.0 ;

    /*species 12: CO */
    /*Imported from NIST */
    Tci[12] = 132.850000 ; 
    ai[12] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[12],2.0) / (pow(28.010000,2.0) * 34.940000); 
    bi[12] = 0.08664 * Rcst * Tci[12] / (28.010000 * 34.940000); 
    acentric_i[12] = 0.045000 ;

    /*species 13: CO2 */
    /*Imported from NIST */
    Tci[13] = 304.120000 ; 
    ai[13] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[13],2.0) / (pow(44.009950,2.0) * 73.740000); 
    bi[13] = 0.08664 * Rcst * Tci[13] / (44.009950 * 73.740000); 
    acentric_i[13] = 0.225000 ;

    /*species 14: C2H2 */
    /*Imported from NIST */
    Tci[14] = 308.300000 ; 
    ai[14] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[14],2.0) / (pow(26.038000,2.0) * 61.140000); 
    bi[14] = 0.08664 * Rcst * Tci[14] / (26.038000 * 61.140000); 
    acentric_i[14] = 0.189000 ;

    /*species 15: C2H4 */
    /*Imported from NIST */
    Tci[15] = 282.340000 ; 
    ai[15] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[15],2.0) / (pow(28.054000,2.0) * 50.410000); 
    bi[15] = 0.08664 * Rcst * Tci[15] / (28.054000 * 50.410000); 
    acentric_i[15] = 0.087000 ;

    /*species 16: C2H6 */
    /*Imported from NIST */
    Tci[16] = 305.320000 ; 
    ai[16] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[16],2.0) / (pow(30.070120,2.0) * 48.720000); 
    bi[16] = 0.08664 * Rcst * Tci[16] / (30.070120 * 48.720000); 
    acentric_i[16] = 0.099000 ;

    /*species 17: CH2CHO */
    Tci[17] = 1.316 * EPS[17] ; 
    ai[17] = (5.55 * pow(avogadro,2.0) * EPS[17]*boltzmann * pow(1e-8*SIG[17],3.0) ) / (pow(wt[17],2.0)); 
    bi[17] = 0.855 * avogadro * pow(1e-8*SIG[17],3.0) / (wt[17]); 
    acentric_i[17] = 0.0 ;

    /*species 18: aC3H5 */
    Tci[18] = 1.316 * EPS[18] ; 
    ai[18] = (5.55 * pow(avogadro,2.0) * EPS[18]*boltzmann * pow(1e-8*SIG[18],3.0) ) / (pow(wt[18],2.0)); 
    bi[18] = 0.855 * avogadro * pow(1e-8*SIG[18],3.0) / (wt[18]); 
    acentric_i[18] = 0.0 ;

    /*species 19: C3H6 */
    Tci[19] = 1.316 * EPS[19] ; 
    ai[19] = (5.55 * pow(avogadro,2.0) * EPS[19]*boltzmann * pow(1e-8*SIG[19],3.0) ) / (pow(wt[19],2.0)); 
    bi[19] = 0.855 * avogadro * pow(1e-8*SIG[19],3.0) / (wt[19]); 
    acentric_i[19] = 0.0 ;

    /*species 20: C2H3CHO */
    Tci[20] = 1.316 * EPS[20] ; 
    ai[20] = (5.55 * pow(avogadro,2.0) * EPS[20]*boltzmann * pow(1e-8*SIG[20],3.0) ) / (pow(wt[20],2.0)); 
    bi[20] = 0.855 * avogadro * pow(1e-8*SIG[20],3.0) / (wt[20]); 
    acentric_i[20] = 0.0 ;

    /*species 21: C4H7 */
    Tci[21] = 1.316 * EPS[21] ; 
    ai[21] = (5.55 * pow(avogadro,2.0) * EPS[21]*boltzmann * pow(1e-8*SIG[21],3.0) ) / (pow(wt[21],2.0)); 
    bi[21] = 0.855 * avogadro * pow(1e-8*SIG[21],3.0) / (wt[21]); 
    acentric_i[21] = 0.0 ;

    /*species 22: C4H81 */
    Tci[22] = 1.316 * EPS[22] ; 
    ai[22] = (5.55 * pow(avogadro,2.0) * EPS[22]*boltzmann * pow(1e-8*SIG[22],3.0) ) / (pow(wt[22],2.0)); 
    bi[22] = 0.855 * avogadro * pow(1e-8*SIG[22],3.0) / (wt[22]); 
    acentric_i[22] = 0.0 ;

    /*species 23: C5H9 */
    Tci[23] = 1.316 * EPS[23] ; 
    ai[23] = (5.55 * pow(avogadro,2.0) * EPS[23]*boltzmann * pow(1e-8*SIG[23],3.0) ) / (pow(wt[23],2.0)); 
    bi[23] = 0.855 * avogadro * pow(1e-8*SIG[23],3.0) / (wt[23]); 
    acentric_i[23] = 0.0 ;

    /*species 24: C5H10 */
    Tci[24] = 1.316 * EPS[24] ; 
    ai[24] = (5.55 * pow(avogadro,2.0) * EPS[24]*boltzmann * pow(1e-8*SIG[24],3.0) ) / (pow(wt[24],2.0)); 
    bi[24] = 0.855 * avogadro * pow(1e-8*SIG[24],3.0) / (wt[24]); 
    acentric_i[24] = 0.0 ;

    /*species 25: C6H12 */
    Tci[25] = 1.316 * EPS[25] ; 
    ai[25] = (5.55 * pow(avogadro,2.0) * EPS[25]*boltzmann * pow(1e-8*SIG[25],3.0) ) / (pow(wt[25],2.0)); 
    bi[25] = 0.855 * avogadro * pow(1e-8*SIG[25],3.0) / (wt[25]); 
    acentric_i[25] = 0.0 ;

    /*species 26: C7H14 */
    Tci[26] = 1.316 * EPS[26] ; 
    ai[26] = (5.55 * pow(avogadro,2.0) * EPS[26]*boltzmann * pow(1e-8*SIG[26],3.0) ) / (pow(wt[26],2.0)); 
    bi[26] = 0.855 * avogadro * pow(1e-8*SIG[26],3.0) / (wt[26]); 
    acentric_i[26] = 0.0 ;

    /*species 27: C8H16 */
    Tci[27] = 1.316 * EPS[27] ; 
    ai[27] = (5.55 * pow(avogadro,2.0) * EPS[27]*boltzmann * pow(1e-8*SIG[27],3.0) ) / (pow(wt[27],2.0)); 
    bi[27] = 0.855 * avogadro * pow(1e-8*SIG[27],3.0) / (wt[27]); 
    acentric_i[27] = 0.0 ;

    /*species 28: C9H18 */
    Tci[28] = 1.316 * EPS[28] ; 
    ai[28] = (5.55 * pow(avogadro,2.0) * EPS[28]*boltzmann * pow(1e-8*SIG[28],3.0) ) / (pow(wt[28],2.0)); 
    bi[28] = 0.855 * avogadro * pow(1e-8*SIG[28],3.0) / (wt[28]); 
    acentric_i[28] = 0.0 ;

    /*species 29: PXC9H19 */
    Tci[29] = 1.316 * EPS[29] ; 
    ai[29] = (5.55 * pow(avogadro,2.0) * EPS[29]*boltzmann * pow(1e-8*SIG[29],3.0) ) / (pow(wt[29],2.0)); 
    bi[29] = 0.855 * avogadro * pow(1e-8*SIG[29],3.0) / (wt[29]); 
    acentric_i[29] = 0.0 ;

    /*species 30: C10H20 */
    Tci[30] = 1.316 * EPS[30] ; 
    ai[30] = (5.55 * pow(avogadro,2.0) * EPS[30]*boltzmann * pow(1e-8*SIG[30],3.0) ) / (pow(wt[30],2.0)); 
    bi[30] = 0.855 * avogadro * pow(1e-8*SIG[30],3.0) / (wt[30]); 
    acentric_i[30] = 0.0 ;

    /*species 31: C12H24 */
    Tci[31] = 1.316 * EPS[31] ; 
    ai[31] = (5.55 * pow(avogadro,2.0) * EPS[31]*boltzmann * pow(1e-8*SIG[31],3.0) ) / (pow(wt[31],2.0)); 
    bi[31] = 0.855 * avogadro * pow(1e-8*SIG[31],3.0) / (wt[31]); 
    acentric_i[31] = 0.0 ;

    /*species 32: C12H25O2 */
    Tci[32] = 1.316 * EPS[32] ; 
    ai[32] = (5.55 * pow(avogadro,2.0) * EPS[32]*boltzmann * pow(1e-8*SIG[32],3.0) ) / (pow(wt[32],2.0)); 
    bi[32] = 0.855 * avogadro * pow(1e-8*SIG[32],3.0) / (wt[32]); 
    acentric_i[32] = 0.0 ;

    /*species 33: OC12H23OOH */
    Tci[33] = 1.316 * EPS[33] ; 
    ai[33] = (5.55 * pow(avogadro,2.0) * EPS[33]*boltzmann * pow(1e-8*SIG[33],3.0) ) / (pow(wt[33],2.0)); 
    bi[33] = 0.855 * avogadro * pow(1e-8*SIG[33],3.0) / (wt[33]); 
    acentric_i[33] = 0.0 ;

    /*species 34: N2 */
    /*Imported from NIST */
    Tci[34] = 126.192000 ; 
    ai[34] = 1e6 * 0.42748 * pow(Rcst,2.0) * pow(Tci[34],2.0) / (pow(28.013400,2.0) * 33.958000); 
    bi[34] = 0.08664 * Rcst * Tci[34] / (28.013400 * 33.958000); 
    acentric_i[34] = 0.037200 ;

    return;
}


void egtransetLENIMC(int* LENIMC ) {
    *LENIMC = 142;}


void egtransetLENRMC(int* LENRMC ) {
    *LENRMC = 24500;}


void egtransetNO(int* NO ) {
    *NO = 4;}


void egtransetKK(int* KK ) {
    *KK = 35;}


void egtransetNLITE(int* NLITE ) {
    *NLITE = 2;}


/*Patm in ergs/cm3 */
void egtransetPATM(double* PATM) {
    *PATM =   0.1013250000000000E+07;}


/*the molecular weights in g/mol */
void egtransetWT(double* WT ) {
    WT[0] = 1.70341020E+02;
    WT[1] = 1.00797000E+00;
    WT[2] = 1.59994000E+01;
    WT[3] = 1.70073700E+01;
    WT[4] = 3.30067700E+01;
    WT[5] = 2.01594000E+00;
    WT[6] = 1.80153400E+01;
    WT[7] = 3.40147400E+01;
    WT[8] = 3.19988000E+01;
    WT[9] = 1.50350600E+01;
    WT[10] = 1.60430300E+01;
    WT[11] = 3.00264900E+01;
    WT[12] = 2.80105500E+01;
    WT[13] = 4.40099500E+01;
    WT[14] = 2.60382400E+01;
    WT[15] = 2.80541800E+01;
    WT[16] = 3.00701200E+01;
    WT[17] = 4.30456100E+01;
    WT[18] = 4.10733000E+01;
    WT[19] = 4.20812700E+01;
    WT[20] = 5.60647300E+01;
    WT[21] = 5.51003900E+01;
    WT[22] = 5.61083600E+01;
    WT[23] = 6.91274800E+01;
    WT[24] = 7.01354500E+01;
    WT[25] = 8.41625400E+01;
    WT[26] = 9.81896300E+01;
    WT[27] = 1.12216720E+02;
    WT[28] = 1.26243810E+02;
    WT[29] = 1.27251780E+02;
    WT[30] = 1.40270900E+02;
    WT[31] = 1.68325080E+02;
    WT[32] = 2.01331850E+02;
    WT[33] = 2.16323280E+02;
    WT[34] = 2.80134000E+01;
}


/*the lennard-jones potential well depth eps/kb in K */
void egtransetEPS(double* EPS ) {
    EPS[0] = 7.89980000E+02;
    EPS[1] = 1.45000000E+02;
    EPS[2] = 8.00000000E+01;
    EPS[3] = 8.00000000E+01;
    EPS[4] = 1.07400000E+02;
    EPS[5] = 3.80000000E+01;
    EPS[6] = 5.72400000E+02;
    EPS[7] = 1.07400000E+02;
    EPS[8] = 1.07400000E+02;
    EPS[9] = 1.44000000E+02;
    EPS[10] = 1.41400000E+02;
    EPS[11] = 4.98000000E+02;
    EPS[12] = 9.81000000E+01;
    EPS[13] = 2.44000000E+02;
    EPS[14] = 2.09000000E+02;
    EPS[15] = 2.80800000E+02;
    EPS[16] = 2.52300000E+02;
    EPS[17] = 4.36000000E+02;
    EPS[18] = 2.66800000E+02;
    EPS[19] = 2.66800000E+02;
    EPS[20] = 3.57000000E+02;
    EPS[21] = 3.57000000E+02;
    EPS[22] = 3.57000000E+02;
    EPS[23] = 4.48508000E+02;
    EPS[24] = 4.48508000E+02;
    EPS[25] = 5.04629000E+02;
    EPS[26] = 5.57947000E+02;
    EPS[27] = 6.08488000E+02;
    EPS[28] = 6.55390000E+02;
    EPS[29] = 6.60032000E+02;
    EPS[30] = 6.98122000E+02;
    EPS[31] = 7.75294000E+02;
    EPS[32] = 8.05031000E+02;
    EPS[33] = 8.39348000E+02;
    EPS[34] = 9.75300000E+01;
}


/*the lennard-jones collision diameter in Angstroms */
void egtransetSIG(double* SIG ) {
    SIG[0] = 7.04700000E+00;
    SIG[1] = 2.05000000E+00;
    SIG[2] = 2.75000000E+00;
    SIG[3] = 2.75000000E+00;
    SIG[4] = 3.45800000E+00;
    SIG[5] = 2.92000000E+00;
    SIG[6] = 2.60500000E+00;
    SIG[7] = 3.45800000E+00;
    SIG[8] = 3.45800000E+00;
    SIG[9] = 3.80000000E+00;
    SIG[10] = 3.74600000E+00;
    SIG[11] = 3.59000000E+00;
    SIG[12] = 3.65000000E+00;
    SIG[13] = 3.76300000E+00;
    SIG[14] = 4.10000000E+00;
    SIG[15] = 3.97100000E+00;
    SIG[16] = 4.30200000E+00;
    SIG[17] = 3.97000000E+00;
    SIG[18] = 4.98200000E+00;
    SIG[19] = 4.98200000E+00;
    SIG[20] = 5.17600000E+00;
    SIG[21] = 5.17600000E+00;
    SIG[22] = 5.17600000E+00;
    SIG[23] = 5.34200000E+00;
    SIG[24] = 5.34200000E+00;
    SIG[25] = 5.62800000E+00;
    SIG[26] = 5.87600000E+00;
    SIG[27] = 6.10000000E+00;
    SIG[28] = 6.33100000E+00;
    SIG[29] = 6.46700000E+00;
    SIG[30] = 6.57800000E+00;
    SIG[31] = 7.09000000E+00;
    SIG[32] = 7.10200000E+00;
    SIG[33] = 7.27300000E+00;
    SIG[34] = 3.62100000E+00;
}


/*the dipole moment in Debye */
void egtransetDIP(double* DIP ) {
    DIP[0] = 0.00000000E+00;
    DIP[1] = 0.00000000E+00;
    DIP[2] = 0.00000000E+00;
    DIP[3] = 0.00000000E+00;
    DIP[4] = 0.00000000E+00;
    DIP[5] = 0.00000000E+00;
    DIP[6] = 1.84400000E+00;
    DIP[7] = 0.00000000E+00;
    DIP[8] = 0.00000000E+00;
    DIP[9] = 0.00000000E+00;
    DIP[10] = 0.00000000E+00;
    DIP[11] = 0.00000000E+00;
    DIP[12] = 0.00000000E+00;
    DIP[13] = 0.00000000E+00;
    DIP[14] = 0.00000000E+00;
    DIP[15] = 0.00000000E+00;
    DIP[16] = 0.00000000E+00;
    DIP[17] = 0.00000000E+00;
    DIP[18] = 0.00000000E+00;
    DIP[19] = 0.00000000E+00;
    DIP[20] = 0.00000000E+00;
    DIP[21] = 0.00000000E+00;
    DIP[22] = 0.00000000E+00;
    DIP[23] = 0.00000000E+00;
    DIP[24] = 0.00000000E+00;
    DIP[25] = 0.00000000E+00;
    DIP[26] = 0.00000000E+00;
    DIP[27] = 0.00000000E+00;
    DIP[28] = 0.00000000E+00;
    DIP[29] = 0.00000000E+00;
    DIP[30] = 0.00000000E+00;
    DIP[31] = 0.00000000E+00;
    DIP[32] = 0.00000000E+00;
    DIP[33] = 0.00000000E+00;
    DIP[34] = 0.00000000E+00;
}


/*the polarizability in cubic Angstroms */
void egtransetPOL(double* POL ) {
    POL[0] = 0.00000000E+00;
    POL[1] = 0.00000000E+00;
    POL[2] = 0.00000000E+00;
    POL[3] = 0.00000000E+00;
    POL[4] = 0.00000000E+00;
    POL[5] = 7.90000000E-01;
    POL[6] = 0.00000000E+00;
    POL[7] = 0.00000000E+00;
    POL[8] = 1.60000000E+00;
    POL[9] = 0.00000000E+00;
    POL[10] = 2.60000000E+00;
    POL[11] = 0.00000000E+00;
    POL[12] = 1.95000000E+00;
    POL[13] = 2.65000000E+00;
    POL[14] = 0.00000000E+00;
    POL[15] = 0.00000000E+00;
    POL[16] = 0.00000000E+00;
    POL[17] = 0.00000000E+00;
    POL[18] = 0.00000000E+00;
    POL[19] = 0.00000000E+00;
    POL[20] = 0.00000000E+00;
    POL[21] = 0.00000000E+00;
    POL[22] = 0.00000000E+00;
    POL[23] = 0.00000000E+00;
    POL[24] = 0.00000000E+00;
    POL[25] = 0.00000000E+00;
    POL[26] = 0.00000000E+00;
    POL[27] = 0.00000000E+00;
    POL[28] = 0.00000000E+00;
    POL[29] = 0.00000000E+00;
    POL[30] = 0.00000000E+00;
    POL[31] = 0.00000000E+00;
    POL[32] = 0.00000000E+00;
    POL[33] = 0.00000000E+00;
    POL[34] = 1.76000000E+00;
}


/*the rotational relaxation collision number at 298 K */
void egtransetZROT(double* ZROT ) {
    ZROT[0] = 1.00000000E+00;
    ZROT[1] = 0.00000000E+00;
    ZROT[2] = 0.00000000E+00;
    ZROT[3] = 0.00000000E+00;
    ZROT[4] = 1.00000000E+00;
    ZROT[5] = 2.80000000E+02;
    ZROT[6] = 4.00000000E+00;
    ZROT[7] = 3.80000000E+00;
    ZROT[8] = 3.80000000E+00;
    ZROT[9] = 0.00000000E+00;
    ZROT[10] = 1.30000000E+01;
    ZROT[11] = 2.00000000E+00;
    ZROT[12] = 1.80000000E+00;
    ZROT[13] = 2.10000000E+00;
    ZROT[14] = 2.50000000E+00;
    ZROT[15] = 1.50000000E+00;
    ZROT[16] = 1.50000000E+00;
    ZROT[17] = 2.00000000E+00;
    ZROT[18] = 1.00000000E+00;
    ZROT[19] = 1.00000000E+00;
    ZROT[20] = 1.00000000E+00;
    ZROT[21] = 1.00000000E+00;
    ZROT[22] = 1.00000000E+00;
    ZROT[23] = 1.00000000E+00;
    ZROT[24] = 1.00000000E+00;
    ZROT[25] = 1.00000000E+00;
    ZROT[26] = 1.00000000E+00;
    ZROT[27] = 1.00000000E+00;
    ZROT[28] = 1.00000000E+00;
    ZROT[29] = 1.00000000E+00;
    ZROT[30] = 1.00000000E+00;
    ZROT[31] = 1.00000000E+00;
    ZROT[32] = 1.00000000E+00;
    ZROT[33] = 1.00000000E+00;
    ZROT[34] = 4.00000000E+00;
}


/*0: monoatomic, 1: linear, 2: nonlinear */
void egtransetNLIN(int* NLIN) {
    NLIN[0] = 2;
    NLIN[1] = 0;
    NLIN[2] = 0;
    NLIN[3] = 1;
    NLIN[4] = 2;
    NLIN[5] = 1;
    NLIN[6] = 2;
    NLIN[7] = 2;
    NLIN[8] = 1;
    NLIN[9] = 1;
    NLIN[10] = 2;
    NLIN[11] = 2;
    NLIN[12] = 1;
    NLIN[13] = 1;
    NLIN[14] = 1;
    NLIN[15] = 2;
    NLIN[16] = 2;
    NLIN[17] = 2;
    NLIN[18] = 2;
    NLIN[19] = 2;
    NLIN[20] = 2;
    NLIN[21] = 2;
    NLIN[22] = 2;
    NLIN[23] = 2;
    NLIN[24] = 2;
    NLIN[25] = 2;
    NLIN[26] = 2;
    NLIN[27] = 2;
    NLIN[28] = 2;
    NLIN[29] = 2;
    NLIN[30] = 2;
    NLIN[31] = 2;
    NLIN[32] = 2;
    NLIN[33] = 2;
    NLIN[34] = 1;
}


/*Poly fits for the viscosities, dim NO*KK */
void egtransetCOFETA(double* COFETA) {
    COFETA[0] = -7.22653896E+00;
    COFETA[1] = -3.12842246E+00;
    COFETA[2] = 6.69477571E-01;
    COFETA[3] = -3.60312934E-02;
    COFETA[4] = -2.04078397E+01;
    COFETA[5] = 3.65436395E+00;
    COFETA[6] = -3.98339635E-01;
    COFETA[7] = 1.75883009E-02;
    COFETA[8] = -1.50926240E+01;
    COFETA[9] = 1.92606504E+00;
    COFETA[10] = -1.73487476E-01;
    COFETA[11] = 7.82572931E-03;
    COFETA[12] = -1.50620763E+01;
    COFETA[13] = 1.92606504E+00;
    COFETA[14] = -1.73487476E-01;
    COFETA[15] = 7.82572931E-03;
    COFETA[16] = -1.71463238E+01;
    COFETA[17] = 2.68036374E+00;
    COFETA[18] = -2.72570227E-01;
    COFETA[19] = 1.21650964E-02;
    COFETA[20] = -1.38347699E+01;
    COFETA[21] = 1.00106621E+00;
    COFETA[22] = -4.98105694E-02;
    COFETA[23] = 2.31450475E-03;
    COFETA[24] = -1.05420863E+01;
    COFETA[25] = -1.37777096E+00;
    COFETA[26] = 4.20502308E-01;
    COFETA[27] = -2.40627230E-02;
    COFETA[28] = -1.71312832E+01;
    COFETA[29] = 2.68036374E+00;
    COFETA[30] = -2.72570227E-01;
    COFETA[31] = 1.21650964E-02;
    COFETA[32] = -1.71618309E+01;
    COFETA[33] = 2.68036374E+00;
    COFETA[34] = -2.72570227E-01;
    COFETA[35] = 1.21650964E-02;
    COFETA[36] = -2.02316497E+01;
    COFETA[37] = 3.63241793E+00;
    COFETA[38] = -3.95581049E-01;
    COFETA[39] = 1.74725495E-02;
    COFETA[40] = -2.00094664E+01;
    COFETA[41] = 3.57220167E+00;
    COFETA[42] = -3.87936446E-01;
    COFETA[43] = 1.71483254E-02;
    COFETA[44] = -1.98330577E+01;
    COFETA[45] = 2.69480162E+00;
    COFETA[46] = -1.65880845E-01;
    COFETA[47] = 3.14504769E-03;
    COFETA[48] = -1.66188336E+01;
    COFETA[49] = 2.40307799E+00;
    COFETA[50] = -2.36167638E-01;
    COFETA[51] = 1.05714061E-02;
    COFETA[52] = -2.40014975E+01;
    COFETA[53] = 5.14359547E+00;
    COFETA[54] = -5.74269731E-01;
    COFETA[55] = 2.44937679E-02;
    COFETA[56] = -2.33666446E+01;
    COFETA[57] = 4.80350223E+00;
    COFETA[58] = -5.38341336E-01;
    COFETA[59] = 2.32747213E-02;
    COFETA[60] = -2.50655444E+01;
    COFETA[61] = 5.33982977E+00;
    COFETA[62] = -5.89982992E-01;
    COFETA[63] = 2.47780650E-02;
    COFETA[64] = -2.46410937E+01;
    COFETA[65] = 5.19497183E+00;
    COFETA[66] = -5.78827339E-01;
    COFETA[67] = 2.46050921E-02;
    COFETA[68] = -2.23277173E+01;
    COFETA[69] = 3.86433912E+00;
    COFETA[70] = -3.41553983E-01;
    COFETA[71] = 1.17083447E-02;
    COFETA[72] = -2.51527853E+01;
    COFETA[73] = 5.30723075E+00;
    COFETA[74] = -5.89742369E-01;
    COFETA[75] = 2.49294033E-02;
    COFETA[76] = -2.51406631E+01;
    COFETA[77] = 5.30723075E+00;
    COFETA[78] = -5.89742369E-01;
    COFETA[79] = 2.49294033E-02;
    COFETA[80] = -2.48320121E+01;
    COFETA[81] = 4.94595777E+00;
    COFETA[82] = -5.12278955E-01;
    COFETA[83] = 2.03286378E-02;
    COFETA[84] = -2.48406871E+01;
    COFETA[85] = 4.94595777E+00;
    COFETA[86] = -5.12278955E-01;
    COFETA[87] = 2.03286378E-02;
    COFETA[88] = -2.48316231E+01;
    COFETA[89] = 4.94595777E+00;
    COFETA[90] = -5.12278955E-01;
    COFETA[91] = 2.03286378E-02;
    COFETA[92] = -2.22556470E+01;
    COFETA[93] = 3.65583269E+00;
    COFETA[94] = -3.09681973E-01;
    COFETA[95] = 1.01347367E-02;
    COFETA[96] = -2.22484090E+01;
    COFETA[97] = 3.65583269E+00;
    COFETA[98] = -3.09681973E-01;
    COFETA[99] = 1.01347367E-02;
    COFETA[100] = -1.99265579E+01;
    COFETA[101] = 2.56014100E+00;
    COFETA[102] = -1.45913962E-01;
    COFETA[103] = 2.18129625E-03;
    COFETA[104] = -1.75766471E+01;
    COFETA[105] = 1.47718088E+00;
    COFETA[106] = 1.33842258E-02;
    COFETA[107] = -5.45446078E-03;
    COFETA[108] = -1.51262692E+01;
    COFETA[109] = 3.71381902E-01;
    COFETA[110] = 1.73700335E-01;
    COFETA[111] = -1.30510322E-02;
    COFETA[112] = -1.29964167E+01;
    COFETA[113] = -5.82992847E-01;
    COFETA[114] = 3.10614522E-01;
    COFETA[115] = -1.94724805E-02;
    COFETA[116] = -1.28241825E+01;
    COFETA[117] = -6.75983155E-01;
    COFETA[118] = 3.23857491E-01;
    COFETA[119] = -2.00893320E-02;
    COFETA[120] = -1.10600818E+01;
    COFETA[121] = -1.44434970E+00;
    COFETA[122] = 4.32898375E-01;
    COFETA[123] = -2.51531493E-02;
    COFETA[124] = -7.82457830E+00;
    COFETA[125] = -2.87790249E+00;
    COFETA[126] = 6.34514847E-01;
    COFETA[127] = -3.44344136E-02;
    COFETA[128] = -6.55009356E+00;
    COFETA[129] = -3.39070398E+00;
    COFETA[130] = 7.06046571E-01;
    COFETA[131] = -3.77010172E-02;
    COFETA[132] = -5.21187446E+00;
    COFETA[133] = -3.96972532E+00;
    COFETA[134] = 7.86375770E-01;
    COFETA[135] = -4.13504402E-02;
    COFETA[136] = -1.65695594E+01;
    COFETA[137] = 2.39056562E+00;
    COFETA[138] = -2.34558144E-01;
    COFETA[139] = 1.05024037E-02;
}


/*Poly fits for the conductivities, dim NO*KK */
void egtransetCOFLAM(double* COFLAM) {
    COFLAM[0] = -9.35546325E+00;
    COFLAM[1] = 3.31628854E+00;
    COFLAM[2] = -1.07145802E-02;
    COFLAM[3] = -1.24086068E-02;
    COFLAM[4] = -8.57929284E-01;
    COFLAM[5] = 3.65436395E+00;
    COFLAM[6] = -3.98339635E-01;
    COFLAM[7] = 1.75883009E-02;
    COFLAM[8] = 1.69267361E+00;
    COFLAM[9] = 1.92606504E+00;
    COFLAM[10] = -1.73487476E-01;
    COFLAM[11] = 7.82572931E-03;
    COFLAM[12] = 1.50119731E+01;
    COFLAM[13] = -3.63267854E+00;
    COFLAM[14] = 5.92839101E-01;
    COFLAM[15] = -2.62920439E-02;
    COFLAM[16] = -1.12960913E+00;
    COFLAM[17] = 2.34014305E+00;
    COFLAM[18] = -1.63245030E-01;
    COFLAM[19] = 5.80319600E-03;
    COFLAM[20] = 9.24084392E+00;
    COFLAM[21] = -4.69568931E-01;
    COFLAM[22] = 1.15980279E-01;
    COFLAM[23] = -2.61033830E-03;
    COFLAM[24] = 2.33729817E+01;
    COFLAM[25] = -8.96536433E+00;
    COFLAM[26] = 1.52828665E+00;
    COFLAM[27] = -7.58551778E-02;
    COFLAM[28] = 8.83996545E-01;
    COFLAM[29] = 1.31525428E+00;
    COFLAM[30] = 1.91774576E-02;
    COFLAM[31] = -4.41642722E-03;
    COFLAM[32] = -1.93718739E+00;
    COFLAM[33] = 2.89110219E+00;
    COFLAM[34] = -2.71096923E-01;
    COFLAM[35] = 1.15344907E-02;
    COFLAM[36] = 1.39937901E+01;
    COFLAM[37] = -4.64256494E+00;
    COFLAM[38] = 9.07728674E-01;
    COFLAM[39] = -4.77274469E-02;
    COFLAM[40] = 1.33091602E+01;
    COFLAM[41] = -4.96140261E+00;
    COFLAM[42] = 1.03295505E+00;
    COFLAM[43] = -5.63420090E-02;
    COFLAM[44] = 5.39305086E+00;
    COFLAM[45] = -2.39312375E+00;
    COFLAM[46] = 7.39585221E-01;
    COFLAM[47] = -4.58435589E-02;
    COFLAM[48] = 1.18777264E+01;
    COFLAM[49] = -3.15463949E+00;
    COFLAM[50] = 6.01973268E-01;
    COFLAM[51] = -3.03211261E-02;
    COFLAM[52] = -1.13649314E+01;
    COFLAM[53] = 5.88177395E+00;
    COFLAM[54] = -5.68651819E-01;
    COFLAM[55] = 2.03561485E-02;
    COFLAM[56] = -7.70164502E+00;
    COFLAM[57] = 4.56884453E+00;
    COFLAM[58] = -4.04747583E-01;
    COFLAM[59] = 1.40841060E-02;
    COFLAM[60] = -1.46152839E+01;
    COFLAM[61] = 6.36251406E+00;
    COFLAM[62] = -5.03832130E-01;
    COFLAM[63] = 1.26121050E-02;
    COFLAM[64] = -1.09902209E+01;
    COFLAM[65] = 4.70647707E+00;
    COFLAM[66] = -2.52272495E-01;
    COFLAM[67] = 1.75193258E-04;
    COFLAM[68] = -6.27425585E+00;
    COFLAM[69] = 2.90471302E+00;
    COFLAM[70] = -4.35144316E-02;
    COFLAM[71] = -7.77922853E-03;
    COFLAM[72] = -2.14189975E+01;
    COFLAM[73] = 9.40841118E+00;
    COFLAM[74] = -9.66247514E-01;
    COFLAM[75] = 3.55085385E-02;
    COFLAM[76] = -1.70514683E+01;
    COFLAM[77] = 7.35906811E+00;
    COFLAM[78] = -6.52820880E-01;
    COFLAM[79] = 1.99982154E-02;
    COFLAM[80] = -1.81837138E+01;
    COFLAM[81] = 7.64582654E+00;
    COFLAM[82] = -6.80297048E-01;
    COFLAM[83] = 2.04082696E-02;
    COFLAM[84] = -1.65417640E+01;
    COFLAM[85] = 7.11744396E+00;
    COFLAM[86] = -6.16669198E-01;
    COFLAM[87] = 1.81367203E-02;
    COFLAM[88] = -1.45831906E+01;
    COFLAM[89] = 5.90742506E+00;
    COFLAM[90] = -3.94364916E-01;
    COFLAM[91] = 5.56233506E-03;
    COFLAM[92] = -3.46263181E+01;
    COFLAM[93] = 1.40704450E+01;
    COFLAM[94] = -1.51405291E+00;
    COFLAM[95] = 5.69632980E-02;
    COFLAM[96] = -1.86689228E+01;
    COFLAM[97] = 7.65011397E+00;
    COFLAM[98] = -6.46093197E-01;
    COFLAM[99] = 1.78356572E-02;
    COFLAM[100] = -1.75774990E+01;
    COFLAM[101] = 7.09949747E+00;
    COFLAM[102] = -5.61324125E-01;
    COFLAM[103] = 1.36473814E-02;
    COFLAM[104] = -1.64736897E+01;
    COFLAM[105] = 6.56031028E+00;
    COFLAM[106] = -4.79947853E-01;
    COFLAM[107] = 9.69177489E-03;
    COFLAM[108] = -1.49547492E+01;
    COFLAM[109] = 5.85879222E+00;
    COFLAM[110] = -3.77230132E-01;
    COFLAM[111] = 4.79862139E-03;
    COFLAM[112] = -1.37648736E+01;
    COFLAM[113] = 5.30662269E+00;
    COFLAM[114] = -2.96948984E-01;
    COFLAM[115] = 1.00864606E-03;
    COFLAM[116] = -1.20691382E+01;
    COFLAM[117] = 4.57989014E+00;
    COFLAM[118] = -1.94853954E-01;
    COFLAM[119] = -3.74099700E-03;
    COFLAM[120] = -1.25669899E+01;
    COFLAM[121] = 4.75862175E+00;
    COFLAM[122] = -2.18414418E-01;
    COFLAM[123] = -2.65674312E-03;
    COFLAM[124] = -1.05264381E+01;
    COFLAM[125] = 3.82519742E+00;
    COFLAM[126] = -8.57053717E-02;
    COFLAM[127] = -8.80476791E-03;
    COFLAM[128] = -1.09133652E+01;
    COFLAM[129] = 4.21990954E+00;
    COFLAM[130] = -1.87708552E-01;
    COFLAM[131] = -1.69129596E-03;
    COFLAM[132] = -6.84099409E+00;
    COFLAM[133] = 2.34795192E+00;
    COFLAM[134] = 8.90947541E-02;
    COFLAM[135] = -1.52021664E-02;
    COFLAM[136] = 1.29306158E+01;
    COFLAM[137] = -3.52817362E+00;
    COFLAM[138] = 6.45499013E-01;
    COFLAM[139] = -3.19375299E-02;
}


/*Poly fits for the diffusion coefficients, dim NO*KK*KK */
void egtransetCOFD(double* COFD) {
    COFD[0] = -1.28665164E+01;
    COFD[1] = 1.40407102E-01;
    COFD[2] = 3.41555146E-01;
    COFD[3] = -2.03644864E-02;
    COFD[4] = -2.09727773E+01;
    COFD[5] = 5.49979971E+00;
    COFD[6] = -4.62000942E-01;
    COFD[7] = 1.85549745E-02;
    COFD[8] = -2.17906852E+01;
    COFD[9] = 5.43781703E+00;
    COFD[10] = -4.75415362E-01;
    COFD[11] = 1.99887429E-02;
    COFD[12] = -2.18185356E+01;
    COFD[13] = 5.43781703E+00;
    COFD[14] = -4.75415362E-01;
    COFD[15] = 1.99887429E-02;
    COFD[16] = -2.28852654E+01;
    COFD[17] = 5.60324057E+00;
    COFD[18] = -4.88278590E-01;
    COFD[19] = 2.02076422E-02;
    COFD[20] = -1.85196491E+01;
    COFD[21] = 4.68077396E+00;
    COFD[22] = -3.91229141E-01;
    COFD[23] = 1.69020200E-02;
    COFD[24] = -1.47122821E+01;
    COFD[25] = 1.67339458E+00;
    COFD[26] = 1.22536705E-01;
    COFD[27] = -1.01692680E-02;
    COFD[28] = -2.28978338E+01;
    COFD[29] = 5.60324057E+00;
    COFD[30] = -4.88278590E-01;
    COFD[31] = 2.02076422E-02;
    COFD[32] = -2.28722429E+01;
    COFD[33] = 5.60324057E+00;
    COFD[34] = -4.88278590E-01;
    COFD[35] = 2.02076422E-02;
    COFD[36] = -2.26337136E+01;
    COFD[37] = 5.50133264E+00;
    COFD[38] = -4.62484498E-01;
    COFD[39] = 1.85871217E-02;
    COFD[40] = -2.26491367E+01;
    COFD[41] = 5.50690143E+00;
    COFD[42] = -4.64003349E-01;
    COFD[43] = 1.86846584E-02;
    COFD[44] = -1.64683757E+01;
    COFD[45] = 2.28170571E+00;
    COFD[46] = 3.44078407E-02;
    COFD[47] = -6.01703889E-03;
    COFD[48] = -2.27747816E+01;
    COFD[49] = 5.60263382E+00;
    COFD[50] = -4.91348327E-01;
    COFD[51] = 2.04651120E-02;
    COFD[52] = -2.17131617E+01;
    COFD[53] = 4.69815223E+00;
    COFD[54] = -3.28454103E-01;
    COFD[55] = 1.15970636E-02;
    COFD[56] = -2.22067747E+01;
    COFD[57] = 5.03874797E+00;
    COFD[58] = -3.82440213E-01;
    COFD[59] = 1.43232015E-02;
    COFD[60] = -2.08912827E+01;
    COFD[61] = 4.35422933E+00;
    COFD[62] = -2.74727512E-01;
    COFD[63] = 8.91328380E-03;
    COFD[64] = -2.15101033E+01;
    COFD[65] = 4.62311744E+00;
    COFD[66] = -3.16576672E-01;
    COFD[67] = 1.09982058E-02;
    COFD[68] = -1.78379251E+01;
    COFD[69] = 2.81667592E+00;
    COFD[70] = -4.44112417E-02;
    COFD[71] = -2.24836515E-03;
    COFD[72] = -2.15057026E+01;
    COFD[73] = 4.49401210E+00;
    COFD[74] = -2.96300228E-01;
    COFD[75] = 9.98228772E-03;
    COFD[76] = -2.15154466E+01;
    COFD[77] = 4.49401210E+00;
    COFD[78] = -2.96300228E-01;
    COFD[79] = 9.98228772E-03;
    COFD[80] = -1.98610372E+01;
    COFD[81] = 3.61510430E+00;
    COFD[82] = -1.62429647E-01;
    COFD[83] = 3.41379096E-03;
    COFD[84] = -1.98544963E+01;
    COFD[85] = 3.61510430E+00;
    COFD[86] = -1.62429647E-01;
    COFD[87] = 3.41379096E-03;
    COFD[88] = -1.98613298E+01;
    COFD[89] = 3.61510430E+00;
    COFD[90] = -1.62429647E-01;
    COFD[91] = 3.41379096E-03;
    COFD[92] = -1.79641972E+01;
    COFD[93] = 2.68406243E+00;
    COFD[94] = -2.49058117E-02;
    COFD[95] = -3.18217796E-03;
    COFD[96] = -1.79693351E+01;
    COFD[97] = 2.68406243E+00;
    COFD[98] = -2.49058117E-02;
    COFD[99] = -3.18217796E-03;
    COFD[100] = -1.71023566E+01;
    COFD[101] = 2.22992608E+00;
    COFD[102] = 4.19679647E-02;
    COFD[103] = -6.37519657E-03;
    COFD[104] = -1.62450439E+01;
    COFD[105] = 1.79898308E+00;
    COFD[106] = 1.04422970E-01;
    COFD[107] = -9.31788260E-03;
    COFD[108] = -1.54004394E+01;
    COFD[109] = 1.38485345E+00;
    COFD[110] = 1.63989484E-01;
    COFD[111] = -1.21088577E-02;
    COFD[112] = -1.46746719E+01;
    COFD[113] = 1.02870676E+00;
    COFD[114] = 2.14984638E-01;
    COFD[115] = -1.44879090E-02;
    COFD[116] = -1.46203257E+01;
    COFD[117] = 9.94330848E-01;
    COFD[118] = 2.19902430E-01;
    COFD[119] = -1.47171735E-02;
    COFD[120] = -1.40460932E+01;
    COFD[121] = 7.18763378E-01;
    COFD[122] = 2.59237872E-01;
    COFD[123] = -1.65477278E-02;
    COFD[124] = -1.30635390E+01;
    COFD[125] = 2.26455242E-01;
    COFD[126] = 3.29362482E-01;
    COFD[127] = -1.98017964E-02;
    COFD[128] = -1.27059857E+01;
    COFD[129] = 4.81478906E-02;
    COFD[130] = 3.54630676E-01;
    COFD[131] = -2.09687597E-02;
    COFD[132] = -1.22701336E+01;
    COFD[133] = -1.61500371E-01;
    COFD[134] = 3.84217824E-01;
    COFD[135] = -2.23309656E-02;
    COFD[136] = -2.27628163E+01;
    COFD[137] = 5.60192583E+00;
    COFD[138] = -4.91455403E-01;
    COFD[139] = 2.04776556E-02;
    COFD[140] = -2.09727773E+01;
    COFD[141] = 5.49979971E+00;
    COFD[142] = -4.62000942E-01;
    COFD[143] = 1.85549745E-02;
    COFD[144] = -1.47968712E+01;
    COFD[145] = 4.23027636E+00;
    COFD[146] = -3.36139991E-01;
    COFD[147] = 1.46507621E-02;
    COFD[148] = -1.34230272E+01;
    COFD[149] = 3.48624238E+00;
    COFD[150] = -2.41554467E-01;
    COFD[151] = 1.06263545E-02;
    COFD[152] = -1.34247866E+01;
    COFD[153] = 3.48624238E+00;
    COFD[154] = -2.41554467E-01;
    COFD[155] = 1.06263545E-02;
    COFD[156] = -1.46554748E+01;
    COFD[157] = 3.83606243E+00;
    COFD[158] = -2.86076532E-01;
    COFD[159] = 1.25205829E-02;
    COFD[160] = -1.14366381E+01;
    COFD[161] = 2.78323501E+00;
    COFD[162] = -1.51214064E-01;
    COFD[163] = 6.75150012E-03;
    COFD[164] = -1.95739570E+01;
    COFD[165] = 5.61113230E+00;
    COFD[166] = -4.90190187E-01;
    COFD[167] = 2.03260675E-02;
    COFD[168] = -1.46559141E+01;
    COFD[169] = 3.83606243E+00;
    COFD[170] = -2.86076532E-01;
    COFD[171] = 1.25205829E-02;
    COFD[172] = -1.46550083E+01;
    COFD[173] = 3.83606243E+00;
    COFD[174] = -2.86076532E-01;
    COFD[175] = 1.25205829E-02;
    COFD[176] = -1.57994893E+01;
    COFD[177] = 4.22225052E+00;
    COFD[178] = -3.35156428E-01;
    COFD[179] = 1.46104855E-02;
    COFD[180] = -1.57199037E+01;
    COFD[181] = 4.19936335E+00;
    COFD[182] = -3.32311009E-01;
    COFD[183] = 1.44921003E-02;
    COFD[184] = -1.97550088E+01;
    COFD[185] = 5.56931926E+00;
    COFD[186] = -4.89105511E-01;
    COFD[187] = 2.04493129E-02;
    COFD[188] = -1.43151174E+01;
    COFD[189] = 3.68038508E+00;
    COFD[190] = -2.65779346E-01;
    COFD[191] = 1.16360771E-02;
    COFD[192] = -1.76147026E+01;
    COFD[193] = 4.86049500E+00;
    COFD[194] = -4.12200578E-01;
    COFD[195] = 1.77160971E-02;
    COFD[196] = -1.72232223E+01;
    COFD[197] = 4.69060745E+00;
    COFD[198] = -3.92369888E-01;
    COFD[199] = 1.69459661E-02;
    COFD[200] = -1.82251914E+01;
    COFD[201] = 5.05237312E+00;
    COFD[202] = -4.35182396E-01;
    COFD[203] = 1.86363074E-02;
    COFD[204] = -1.79344949E+01;
    COFD[205] = 4.91373893E+00;
    COFD[206] = -4.18747629E-01;
    COFD[207] = 1.79856610E-02;
    COFD[208] = -1.94691430E+01;
    COFD[209] = 5.43830787E+00;
    COFD[210] = -4.75472880E-01;
    COFD[211] = 1.99909996E-02;
    COFD[212] = -1.83539686E+01;
    COFD[213] = 4.98756925E+00;
    COFD[214] = -4.27526072E-01;
    COFD[215] = 1.83341755E-02;
    COFD[216] = -1.83542556E+01;
    COFD[217] = 4.98756925E+00;
    COFD[218] = -4.27526072E-01;
    COFD[219] = 1.83341755E-02;
    COFD[220] = -1.94091087E+01;
    COFD[221] = 5.32291505E+00;
    COFD[222] = -4.65883522E-01;
    COFD[223] = 1.97916109E-02;
    COFD[224] = -1.94089542E+01;
    COFD[225] = 5.32291505E+00;
    COFD[226] = -4.65883522E-01;
    COFD[227] = 1.97916109E-02;
    COFD[228] = -1.94091156E+01;
    COFD[229] = 5.32291505E+00;
    COFD[230] = -4.65883522E-01;
    COFD[231] = 1.97916109E-02;
    COFD[232] = -1.99878729E+01;
    COFD[233] = 5.47248098E+00;
    COFD[234] = -4.79290468E-01;
    COFD[235] = 2.01323903E-02;
    COFD[236] = -1.99879761E+01;
    COFD[237] = 5.47248098E+00;
    COFD[238] = -4.79290468E-01;
    COFD[239] = 2.01323903E-02;
    COFD[240] = -2.04125610E+01;
    COFD[241] = 5.57739710E+00;
    COFD[242] = -4.89778630E-01;
    COFD[243] = 2.04637924E-02;
    COFD[244] = -2.06420287E+01;
    COFD[245] = 5.61233640E+00;
    COFD[246] = -4.91245632E-01;
    COFD[247] = 2.04073252E-02;
    COFD[248] = -2.07167874E+01;
    COFD[249] = 5.58797624E+00;
    COFD[250] = -4.84623564E-01;
    COFD[251] = 1.99817352E-02;
    COFD[252] = -2.08358871E+01;
    COFD[253] = 5.58518783E+00;
    COFD[254] = -4.81282144E-01;
    COFD[255] = 1.97176495E-02;
    COFD[256] = -2.08746196E+01;
    COFD[257] = 5.58521622E+00;
    COFD[258] = -4.81014671E-01;
    COFD[259] = 1.96952964E-02;
    COFD[260] = -2.09388289E+01;
    COFD[261] = 5.57974996E+00;
    COFD[262] = -4.78112864E-01;
    COFD[263] = 1.94826410E-02;
    COFD[264] = -2.09750828E+01;
    COFD[265] = 5.50429364E+00;
    COFD[266] = -4.63372495E-01;
    COFD[267] = 1.86455814E-02;
    COFD[268] = -2.09816297E+01;
    COFD[269] = 5.49082104E+00;
    COFD[270] = -4.60014303E-01;
    COFD[271] = 1.84365627E-02;
    COFD[272] = -2.09649830E+01;
    COFD[273] = 5.45055984E+00;
    COFD[274] = -4.52626477E-01;
    COFD[275] = 1.80297457E-02;
    COFD[276] = -1.42894441E+01;
    COFD[277] = 3.67490723E+00;
    COFD[278] = -2.65114792E-01;
    COFD[279] = 1.16092671E-02;
    COFD[280] = -2.17906852E+01;
    COFD[281] = 5.43781703E+00;
    COFD[282] = -4.75415362E-01;
    COFD[283] = 1.99887429E-02;
    COFD[284] = -1.34230272E+01;
    COFD[285] = 3.48624238E+00;
    COFD[286] = -2.41554467E-01;
    COFD[287] = 1.06263545E-02;
    COFD[288] = -1.32093628E+01;
    COFD[289] = 2.90778936E+00;
    COFD[290] = -1.67388544E-01;
    COFD[291] = 7.45220609E-03;
    COFD[292] = -1.32244035E+01;
    COFD[293] = 2.90778936E+00;
    COFD[294] = -1.67388544E-01;
    COFD[295] = 7.45220609E-03;
    COFD[296] = -1.43190389E+01;
    COFD[297] = 3.17651319E+00;
    COFD[298] = -2.02028974E-01;
    COFD[299] = 8.94232502E-03;
    COFD[300] = -1.09595712E+01;
    COFD[301] = 2.30836460E+00;
    COFD[302] = -8.76339315E-02;
    COFD[303] = 3.90878445E-03;
    COFD[304] = -1.94093572E+01;
    COFD[305] = 5.16013126E+00;
    COFD[306] = -4.46824543E-01;
    COFD[307] = 1.90464887E-02;
    COFD[308] = -1.43238998E+01;
    COFD[309] = 3.17651319E+00;
    COFD[310] = -2.02028974E-01;
    COFD[311] = 8.94232502E-03;
    COFD[312] = -1.43139231E+01;
    COFD[313] = 3.17651319E+00;
    COFD[314] = -2.02028974E-01;
    COFD[315] = 8.94232502E-03;
    COFD[316] = -1.50766130E+01;
    COFD[317] = 3.47945612E+00;
    COFD[318] = -2.40703722E-01;
    COFD[319] = 1.05907441E-02;
    COFD[320] = -1.50270339E+01;
    COFD[321] = 3.46140064E+00;
    COFD[322] = -2.38440092E-01;
    COFD[323] = 1.04960087E-02;
    COFD[324] = -1.94373127E+01;
    COFD[325] = 5.02567894E+00;
    COFD[326] = -4.32045169E-01;
    COFD[327] = 1.85132214E-02;
    COFD[328] = -1.40999008E+01;
    COFD[329] = 3.08120012E+00;
    COFD[330] = -1.89629903E-01;
    COFD[331] = 8.40361952E-03;
    COFD[332] = -1.70534856E+01;
    COFD[333] = 4.14240922E+00;
    COFD[334] = -3.25239774E-01;
    COFD[335] = 1.41980687E-02;
    COFD[336] = -1.65488358E+01;
    COFD[337] = 3.95035840E+00;
    COFD[338] = -3.00959418E-01;
    COFD[339] = 1.31692593E-02;
    COFD[340] = -1.74792112E+01;
    COFD[341] = 4.29676909E+00;
    COFD[342] = -3.44085306E-01;
    COFD[343] = 1.49671135E-02;
    COFD[344] = -1.72556499E+01;
    COFD[345] = 4.17889917E+00;
    COFD[346] = -3.29752510E-01;
    COFD[347] = 1.43850275E-02;
    COFD[348] = -1.90915649E+01;
    COFD[349] = 4.84384483E+00;
    COFD[350] = -4.10265575E-01;
    COFD[351] = 1.76414287E-02;
    COFD[352] = -1.76775033E+01;
    COFD[353] = 4.24719726E+00;
    COFD[354] = -3.38206061E-01;
    COFD[355] = 1.47350654E-02;
    COFD[356] = -1.76808721E+01;
    COFD[357] = 4.24719726E+00;
    COFD[358] = -3.38206061E-01;
    COFD[359] = 1.47350654E-02;
    COFD[360] = -1.87877985E+01;
    COFD[361] = 4.61260432E+00;
    COFD[362] = -3.82854484E-01;
    COFD[363] = 1.65575163E-02;
    COFD[364] = -1.87858595E+01;
    COFD[365] = 4.61260432E+00;
    COFD[366] = -3.82854484E-01;
    COFD[367] = 1.65575163E-02;
    COFD[368] = -1.87878849E+01;
    COFD[369] = 4.61260432E+00;
    COFD[370] = -3.82854484E-01;
    COFD[371] = 1.65575163E-02;
    COFD[372] = -1.96267288E+01;
    COFD[373] = 4.88238340E+00;
    COFD[374] = -4.14880316E-01;
    COFD[375] = 1.78258928E-02;
    COFD[376] = -1.96280812E+01;
    COFD[377] = 4.88238340E+00;
    COFD[378] = -4.14880316E-01;
    COFD[379] = 1.78258928E-02;
    COFD[380] = -2.01678848E+01;
    COFD[381] = 5.04165097E+00;
    COFD[382] = -4.33913744E-01;
    COFD[383] = 1.85861246E-02;
    COFD[384] = -2.05574296E+01;
    COFD[385] = 5.14795860E+00;
    COFD[386] = -4.45935789E-01;
    COFD[387] = 1.90364341E-02;
    COFD[388] = -2.08840973E+01;
    COFD[389] = 5.23394577E+00;
    COFD[390] = -4.55294957E-01;
    COFD[391] = 1.93696950E-02;
    COFD[392] = -2.12416202E+01;
    COFD[393] = 5.33751161E+00;
    COFD[394] = -4.67531962E-01;
    COFD[395] = 1.98534391E-02;
    COFD[396] = -2.12927812E+01;
    COFD[397] = 5.34414631E+00;
    COFD[398] = -4.68238030E-01;
    COFD[399] = 1.98779007E-02;
    COFD[400] = -2.14367598E+01;
    COFD[401] = 5.37405372E+00;
    COFD[402] = -4.70540525E-01;
    COFD[403] = 1.99137160E-02;
    COFD[404] = -2.17411496E+01;
    COFD[405] = 5.41958585E+00;
    COFD[406] = -4.73477571E-01;
    COFD[407] = 1.99215358E-02;
    COFD[408] = -2.18770418E+01;
    COFD[409] = 5.46038651E+00;
    COFD[410] = -4.77939293E-01;
    COFD[411] = 2.00823652E-02;
    COFD[412] = -2.20665373E+01;
    COFD[413] = 5.51007247E+00;
    COFD[414] = -4.83411907E-01;
    COFD[415] = 2.02812924E-02;
    COFD[416] = -1.40756935E+01;
    COFD[417] = 3.07549274E+00;
    COFD[418] = -1.88889344E-01;
    COFD[419] = 8.37152866E-03;
    COFD[420] = -2.18185356E+01;
    COFD[421] = 5.43781703E+00;
    COFD[422] = -4.75415362E-01;
    COFD[423] = 1.99887429E-02;
    COFD[424] = -1.34247866E+01;
    COFD[425] = 3.48624238E+00;
    COFD[426] = -2.41554467E-01;
    COFD[427] = 1.06263545E-02;
    COFD[428] = -1.32244035E+01;
    COFD[429] = 2.90778936E+00;
    COFD[430] = -1.67388544E-01;
    COFD[431] = 7.45220609E-03;
    COFD[432] = -1.32399106E+01;
    COFD[433] = 2.90778936E+00;
    COFD[434] = -1.67388544E-01;
    COFD[435] = 7.45220609E-03;
    COFD[436] = -1.43394069E+01;
    COFD[437] = 3.17651319E+00;
    COFD[438] = -2.02028974E-01;
    COFD[439] = 8.94232502E-03;
    COFD[440] = -1.09628982E+01;
    COFD[441] = 2.30836460E+00;
    COFD[442] = -8.76339315E-02;
    COFD[443] = 3.90878445E-03;
    COFD[444] = -1.94253036E+01;
    COFD[445] = 5.16013126E+00;
    COFD[446] = -4.46824543E-01;
    COFD[447] = 1.90464887E-02;
    COFD[448] = -1.43444709E+01;
    COFD[449] = 3.17651319E+00;
    COFD[450] = -2.02028974E-01;
    COFD[451] = 8.94232502E-03;
    COFD[452] = -1.43340796E+01;
    COFD[453] = 3.17651319E+00;
    COFD[454] = -2.02028974E-01;
    COFD[455] = 8.94232502E-03;
    COFD[456] = -1.50911794E+01;
    COFD[457] = 3.47945612E+00;
    COFD[458] = -2.40703722E-01;
    COFD[459] = 1.05907441E-02;
    COFD[460] = -1.50420953E+01;
    COFD[461] = 3.46140064E+00;
    COFD[462] = -2.38440092E-01;
    COFD[463] = 1.04960087E-02;
    COFD[464] = -1.94570287E+01;
    COFD[465] = 5.02567894E+00;
    COFD[466] = -4.32045169E-01;
    COFD[467] = 1.85132214E-02;
    COFD[468] = -1.41191261E+01;
    COFD[469] = 3.08120012E+00;
    COFD[470] = -1.89629903E-01;
    COFD[471] = 8.40361952E-03;
    COFD[472] = -1.70757047E+01;
    COFD[473] = 4.14240922E+00;
    COFD[474] = -3.25239774E-01;
    COFD[475] = 1.41980687E-02;
    COFD[476] = -1.65675362E+01;
    COFD[477] = 3.95035840E+00;
    COFD[478] = -3.00959418E-01;
    COFD[479] = 1.31692593E-02;
    COFD[480] = -1.74984476E+01;
    COFD[481] = 4.29676909E+00;
    COFD[482] = -3.44085306E-01;
    COFD[483] = 1.49671135E-02;
    COFD[484] = -1.72753760E+01;
    COFD[485] = 4.17889917E+00;
    COFD[486] = -3.29752510E-01;
    COFD[487] = 1.43850275E-02;
    COFD[488] = -1.91136491E+01;
    COFD[489] = 4.84384483E+00;
    COFD[490] = -4.10265575E-01;
    COFD[491] = 1.76414287E-02;
    COFD[492] = -1.76992976E+01;
    COFD[493] = 4.24719726E+00;
    COFD[494] = -3.38206061E-01;
    COFD[495] = 1.47350654E-02;
    COFD[496] = -1.77028170E+01;
    COFD[497] = 4.24719726E+00;
    COFD[498] = -3.38206061E-01;
    COFD[499] = 1.47350654E-02;
    COFD[500] = -1.88114012E+01;
    COFD[501] = 4.61260432E+00;
    COFD[502] = -3.82854484E-01;
    COFD[503] = 1.65575163E-02;
    COFD[504] = -1.88093686E+01;
    COFD[505] = 4.61260432E+00;
    COFD[506] = -3.82854484E-01;
    COFD[507] = 1.65575163E-02;
    COFD[508] = -1.88114917E+01;
    COFD[509] = 4.61260432E+00;
    COFD[510] = -3.82854484E-01;
    COFD[511] = 1.65575163E-02;
    COFD[512] = -1.96513910E+01;
    COFD[513] = 4.88238340E+00;
    COFD[514] = -4.14880316E-01;
    COFD[515] = 1.78258928E-02;
    COFD[516] = -1.96528119E+01;
    COFD[517] = 4.88238340E+00;
    COFD[518] = -4.14880316E-01;
    COFD[519] = 1.78258928E-02;
    COFD[520] = -2.01934260E+01;
    COFD[521] = 5.04165097E+00;
    COFD[522] = -4.33913744E-01;
    COFD[523] = 1.85861246E-02;
    COFD[524] = -2.05835832E+01;
    COFD[525] = 5.14795860E+00;
    COFD[526] = -4.45935789E-01;
    COFD[527] = 1.90364341E-02;
    COFD[528] = -2.09107297E+01;
    COFD[529] = 5.23394577E+00;
    COFD[530] = -4.55294957E-01;
    COFD[531] = 1.93696950E-02;
    COFD[532] = -2.12686373E+01;
    COFD[533] = 5.33751161E+00;
    COFD[534] = -4.67531962E-01;
    COFD[535] = 1.98534391E-02;
    COFD[536] = -2.13198231E+01;
    COFD[537] = 5.34414631E+00;
    COFD[538] = -4.68238030E-01;
    COFD[539] = 1.98779007E-02;
    COFD[540] = -2.14640929E+01;
    COFD[541] = 5.37405372E+00;
    COFD[542] = -4.70540525E-01;
    COFD[543] = 1.99137160E-02;
    COFD[544] = -2.17689706E+01;
    COFD[545] = 5.41958585E+00;
    COFD[546] = -4.73477571E-01;
    COFD[547] = 1.99215358E-02;
    COFD[548] = -2.19052760E+01;
    COFD[549] = 5.46038651E+00;
    COFD[550] = -4.77939293E-01;
    COFD[551] = 2.00823652E-02;
    COFD[552] = -2.20949205E+01;
    COFD[553] = 5.51007247E+00;
    COFD[554] = -4.83411907E-01;
    COFD[555] = 2.02812924E-02;
    COFD[556] = -1.40949196E+01;
    COFD[557] = 3.07549274E+00;
    COFD[558] = -1.88889344E-01;
    COFD[559] = 8.37152866E-03;
    COFD[560] = -2.28852654E+01;
    COFD[561] = 5.60324057E+00;
    COFD[562] = -4.88278590E-01;
    COFD[563] = 2.02076422E-02;
    COFD[564] = -1.46554748E+01;
    COFD[565] = 3.83606243E+00;
    COFD[566] = -2.86076532E-01;
    COFD[567] = 1.25205829E-02;
    COFD[568] = -1.43190389E+01;
    COFD[569] = 3.17651319E+00;
    COFD[570] = -2.02028974E-01;
    COFD[571] = 8.94232502E-03;
    COFD[572] = -1.43394069E+01;
    COFD[573] = 3.17651319E+00;
    COFD[574] = -2.02028974E-01;
    COFD[575] = 8.94232502E-03;
    COFD[576] = -1.55666415E+01;
    COFD[577] = 3.48070094E+00;
    COFD[578] = -2.40859499E-01;
    COFD[579] = 1.05972514E-02;
    COFD[580] = -1.18998012E+01;
    COFD[581] = 2.57507000E+00;
    COFD[582] = -1.24033737E-01;
    COFD[583] = 5.56694959E-03;
    COFD[584] = -2.06463744E+01;
    COFD[585] = 5.41688482E+00;
    COFD[586] = -4.73387188E-01;
    COFD[587] = 1.99280175E-02;
    COFD[588] = -1.55741053E+01;
    COFD[589] = 3.48070094E+00;
    COFD[590] = -2.40859499E-01;
    COFD[591] = 1.05972514E-02;
    COFD[592] = -1.55588279E+01;
    COFD[593] = 3.48070094E+00;
    COFD[594] = -2.40859499E-01;
    COFD[595] = 1.05972514E-02;
    COFD[596] = -1.63542394E+01;
    COFD[597] = 3.82388595E+00;
    COFD[598] = -2.84480724E-01;
    COFD[599] = 1.24506311E-02;
    COFD[600] = -1.62775714E+01;
    COFD[601] = 3.79163564E+00;
    COFD[602] = -2.80257365E-01;
    COFD[603] = 1.22656902E-02;
    COFD[604] = -2.08367725E+01;
    COFD[605] = 5.35267674E+00;
    COFD[606] = -4.69010505E-01;
    COFD[607] = 1.98979152E-02;
    COFD[608] = -1.52792891E+01;
    COFD[609] = 3.36790500E+00;
    COFD[610] = -2.26321740E-01;
    COFD[611] = 9.97135055E-03;
    COFD[612] = -1.84777607E+01;
    COFD[613] = 4.49330851E+00;
    COFD[614] = -3.68208715E-01;
    COFD[615] = 1.59565402E-02;
    COFD[616] = -1.78903913E+01;
    COFD[617] = 4.29613154E+00;
    COFD[618] = -3.44012526E-01;
    COFD[619] = 1.49643715E-02;
    COFD[620] = -1.89616623E+01;
    COFD[621] = 4.68595732E+00;
    COFD[622] = -3.91842840E-01;
    COFD[623] = 1.69262542E-02;
    COFD[624] = -1.86499071E+01;
    COFD[625] = 4.53572533E+00;
    COFD[626] = -3.73386925E-01;
    COFD[627] = 1.61678881E-02;
    COFD[628] = -2.05324091E+01;
    COFD[629] = 5.18417470E+00;
    COFD[630] = -4.49491573E-01;
    COFD[631] = 1.91438508E-02;
    COFD[632] = -1.91208314E+01;
    COFD[633] = 4.61801405E+00;
    COFD[634] = -3.83535652E-01;
    COFD[635] = 1.65862513E-02;
    COFD[636] = -1.91261963E+01;
    COFD[637] = 4.61801405E+00;
    COFD[638] = -3.83535652E-01;
    COFD[639] = 1.65862513E-02;
    COFD[640] = -2.02564783E+01;
    COFD[641] = 4.97613338E+00;
    COFD[642] = -4.26175206E-01;
    COFD[643] = 1.82809270E-02;
    COFD[644] = -2.02532461E+01;
    COFD[645] = 4.97613338E+00;
    COFD[646] = -4.26175206E-01;
    COFD[647] = 1.82809270E-02;
    COFD[648] = -2.02566224E+01;
    COFD[649] = 4.97613338E+00;
    COFD[650] = -4.26175206E-01;
    COFD[651] = 1.82809270E-02;
    COFD[652] = -2.10673266E+01;
    COFD[653] = 5.22065615E+00;
    COFD[654] = -4.53751850E-01;
    COFD[655] = 1.93099506E-02;
    COFD[656] = -2.10696543E+01;
    COFD[657] = 5.22065615E+00;
    COFD[658] = -4.53751850E-01;
    COFD[659] = 1.93099506E-02;
    COFD[660] = -2.15776583E+01;
    COFD[661] = 5.35995365E+00;
    COFD[662] = -4.69587823E-01;
    COFD[663] = 1.99080702E-02;
    COFD[664] = -2.18432549E+01;
    COFD[665] = 5.40768793E+00;
    COFD[666] = -4.72942538E-01;
    COFD[667] = 1.99389038E-02;
    COFD[668] = -2.21426312E+01;
    COFD[669] = 5.47842731E+00;
    COFD[670] = -4.79952894E-01;
    COFD[671] = 2.01568406E-02;
    COFD[672] = -2.24428140E+01;
    COFD[673] = 5.55319328E+00;
    COFD[674] = -4.87759313E-01;
    COFD[675] = 2.04205124E-02;
    COFD[676] = -2.24883982E+01;
    COFD[677] = 5.55793029E+00;
    COFD[678] = -4.88170533E-01;
    COFD[679] = 2.04302911E-02;
    COFD[680] = -2.26548043E+01;
    COFD[681] = 5.59528609E+00;
    COFD[682] = -4.91473190E-01;
    COFD[683] = 2.05131236E-02;
    COFD[684] = -2.28924481E+01;
    COFD[685] = 5.61013561E+00;
    COFD[686] = -4.89933558E-01;
    COFD[687] = 2.03098561E-02;
    COFD[688] = -2.29058275E+01;
    COFD[689] = 5.59504443E+00;
    COFD[690] = -4.86415566E-01;
    COFD[691] = 2.00947399E-02;
    COFD[692] = -2.29515301E+01;
    COFD[693] = 5.58219321E+00;
    COFD[694] = -4.82941269E-01;
    COFD[695] = 1.98711890E-02;
    COFD[696] = -1.52486273E+01;
    COFD[697] = 3.35922578E+00;
    COFD[698] = -2.25181399E-01;
    COFD[699] = 9.92132878E-03;
    COFD[700] = -1.85196491E+01;
    COFD[701] = 4.68077396E+00;
    COFD[702] = -3.91229141E-01;
    COFD[703] = 1.69020200E-02;
    COFD[704] = -1.14366381E+01;
    COFD[705] = 2.78323501E+00;
    COFD[706] = -1.51214064E-01;
    COFD[707] = 6.75150012E-03;
    COFD[708] = -1.09595712E+01;
    COFD[709] = 2.30836460E+00;
    COFD[710] = -8.76339315E-02;
    COFD[711] = 3.90878445E-03;
    COFD[712] = -1.09628982E+01;
    COFD[713] = 2.30836460E+00;
    COFD[714] = -8.76339315E-02;
    COFD[715] = 3.90878445E-03;
    COFD[716] = -1.18998012E+01;
    COFD[717] = 2.57507000E+00;
    COFD[718] = -1.24033737E-01;
    COFD[719] = 5.56694959E-03;
    COFD[720] = -1.03270606E+01;
    COFD[721] = 2.19285409E+00;
    COFD[722] = -7.54492786E-02;
    COFD[723] = 3.51398213E-03;
    COFD[724] = -1.71982995E+01;
    COFD[725] = 4.63881404E+00;
    COFD[726] = -3.86139633E-01;
    COFD[727] = 1.66955081E-02;
    COFD[728] = -1.19006548E+01;
    COFD[729] = 2.57507000E+00;
    COFD[730] = -1.24033737E-01;
    COFD[731] = 5.56694959E-03;
    COFD[732] = -1.18988955E+01;
    COFD[733] = 2.57507000E+00;
    COFD[734] = -1.24033737E-01;
    COFD[735] = 5.56694959E-03;
    COFD[736] = -1.25141260E+01;
    COFD[737] = 2.77873601E+00;
    COFD[738] = -1.50637360E-01;
    COFD[739] = 6.72684281E-03;
    COFD[740] = -1.24693568E+01;
    COFD[741] = 2.76686648E+00;
    COFD[742] = -1.49120141E-01;
    COFD[743] = 6.66220432E-03;
    COFD[744] = -1.60528285E+01;
    COFD[745] = 4.11188603E+00;
    COFD[746] = -3.21540884E-01;
    COFD[747] = 1.40482564E-02;
    COFD[748] = -1.17159737E+01;
    COFD[749] = 2.48123210E+00;
    COFD[750] = -1.11322604E-01;
    COFD[751] = 4.99282389E-03;
    COFD[752] = -1.37794315E+01;
    COFD[753] = 3.23973858E+00;
    COFD[754] = -2.09989036E-01;
    COFD[755] = 9.27667906E-03;
    COFD[756] = -1.34709807E+01;
    COFD[757] = 3.09379603E+00;
    COFD[758] = -1.91268635E-01;
    COFD[759] = 8.47480224E-03;
    COFD[760] = -1.42229194E+01;
    COFD[761] = 3.38669384E+00;
    COFD[762] = -2.28784122E-01;
    COFD[763] = 1.00790953E-02;
    COFD[764] = -1.39924781E+01;
    COFD[765] = 3.26384506E+00;
    COFD[766] = -2.12947087E-01;
    COFD[767] = 9.39743888E-03;
    COFD[768] = -1.57040212E+01;
    COFD[769] = 3.93614244E+00;
    COFD[770] = -2.99111497E-01;
    COFD[771] = 1.30888229E-02;
    COFD[772] = -1.43129712E+01;
    COFD[773] = 3.31177824E+00;
    COFD[774] = -2.18945280E-01;
    COFD[775] = 9.64764419E-03;
    COFD[776] = -1.43135319E+01;
    COFD[777] = 3.31177824E+00;
    COFD[778] = -2.18945280E-01;
    COFD[779] = 9.64764419E-03;
    COFD[780] = -1.52614053E+01;
    COFD[781] = 3.64565939E+00;
    COFD[782] = -2.61726871E-01;
    COFD[783] = 1.14799244E-02;
    COFD[784] = -1.52611017E+01;
    COFD[785] = 3.64565939E+00;
    COFD[786] = -2.61726871E-01;
    COFD[787] = 1.14799244E-02;
    COFD[788] = -1.52614188E+01;
    COFD[789] = 3.64565939E+00;
    COFD[790] = -2.61726871E-01;
    COFD[791] = 1.14799244E-02;
    COFD[792] = -1.61865953E+01;
    COFD[793] = 3.97811734E+00;
    COFD[794] = -3.04543772E-01;
    COFD[795] = 1.33243270E-02;
    COFD[796] = -1.61867990E+01;
    COFD[797] = 3.97811734E+00;
    COFD[798] = -3.04543772E-01;
    COFD[799] = 1.33243270E-02;
    COFD[800] = -1.66550826E+01;
    COFD[801] = 4.12489968E+00;
    COFD[802] = -3.23119338E-01;
    COFD[803] = 1.41122711E-02;
    COFD[804] = -1.70329091E+01;
    COFD[805] = 4.23969256E+00;
    COFD[806] = -3.37288904E-01;
    COFD[807] = 1.46975974E-02;
    COFD[808] = -1.73560790E+01;
    COFD[809] = 4.33584314E+00;
    COFD[810] = -3.48938603E-01;
    COFD[811] = 1.51686902E-02;
    COFD[812] = -1.76841226E+01;
    COFD[813] = 4.43623609E+00;
    COFD[814] = -3.61391880E-01;
    COFD[815] = 1.56853368E-02;
    COFD[816] = -1.77370270E+01;
    COFD[817] = 4.44459213E+00;
    COFD[818] = -3.62399607E-01;
    COFD[819] = 1.57258496E-02;
    COFD[820] = -1.79421151E+01;
    COFD[821] = 4.50796148E+00;
    COFD[822] = -3.69986604E-01;
    COFD[823] = 1.60286525E-02;
    COFD[824] = -1.84634235E+01;
    COFD[825] = 4.65797225E+00;
    COFD[826] = -3.88483952E-01;
    COFD[827] = 1.67915384E-02;
    COFD[828] = -1.85877596E+01;
    COFD[829] = 4.69975029E+00;
    COFD[830] = -3.93412211E-01;
    COFD[831] = 1.69852534E-02;
    COFD[832] = -1.87542611E+01;
    COFD[833] = 4.74513688E+00;
    COFD[834] = -3.98725524E-01;
    COFD[835] = 1.71922391E-02;
    COFD[836] = -1.16906297E+01;
    COFD[837] = 2.47469981E+00;
    COFD[838] = -1.10436257E-01;
    COFD[839] = 4.95273813E-03;
    COFD[840] = -1.47122821E+01;
    COFD[841] = 1.67339458E+00;
    COFD[842] = 1.22536705E-01;
    COFD[843] = -1.01692680E-02;
    COFD[844] = -1.95739570E+01;
    COFD[845] = 5.61113230E+00;
    COFD[846] = -4.90190187E-01;
    COFD[847] = 2.03260675E-02;
    COFD[848] = -1.94093572E+01;
    COFD[849] = 5.16013126E+00;
    COFD[850] = -4.46824543E-01;
    COFD[851] = 1.90464887E-02;
    COFD[852] = -1.94253036E+01;
    COFD[853] = 5.16013126E+00;
    COFD[854] = -4.46824543E-01;
    COFD[855] = 1.90464887E-02;
    COFD[856] = -2.06463744E+01;
    COFD[857] = 5.41688482E+00;
    COFD[858] = -4.73387188E-01;
    COFD[859] = 1.99280175E-02;
    COFD[860] = -1.71982995E+01;
    COFD[861] = 4.63881404E+00;
    COFD[862] = -3.86139633E-01;
    COFD[863] = 1.66955081E-02;
    COFD[864] = -1.19157919E+01;
    COFD[865] = 9.28955130E-01;
    COFD[866] = 2.42107090E-01;
    COFD[867] = -1.59823963E-02;
    COFD[868] = -2.06516336E+01;
    COFD[869] = 5.41688482E+00;
    COFD[870] = -4.73387188E-01;
    COFD[871] = 1.99280175E-02;
    COFD[872] = -2.12652533E+01;
    COFD[873] = 5.59961818E+00;
    COFD[874] = -4.91624858E-01;
    COFD[875] = 2.05035550E-02;
    COFD[876] = -2.12831323E+01;
    COFD[877] = 5.61184117E+00;
    COFD[878] = -4.90532156E-01;
    COFD[879] = 2.03507922E-02;
    COFD[880] = -2.14087397E+01;
    COFD[881] = 5.57282008E+00;
    COFD[882] = -4.76690890E-01;
    COFD[883] = 1.94000719E-02;
    COFD[884] = -1.77563250E+01;
    COFD[885] = 3.57475686E+00;
    COFD[886] = -1.56396297E-01;
    COFD[887] = 3.12157721E-03;
    COFD[888] = -2.11388331E+01;
    COFD[889] = 5.55529675E+00;
    COFD[890] = -4.87942518E-01;
    COFD[891] = 2.04249054E-02;
    COFD[892] = -2.07653719E+01;
    COFD[893] = 5.01092022E+00;
    COFD[894] = -3.77985635E-01;
    COFD[895] = 1.40968645E-02;
    COFD[896] = -2.15095980E+01;
    COFD[897] = 5.46737673E+00;
    COFD[898] = -4.55696085E-01;
    COFD[899] = 1.81982625E-02;
    COFD[900] = -2.08812333E+01;
    COFD[901] = 5.08859217E+00;
    COFD[902] = -3.90525428E-01;
    COFD[903] = 1.47376395E-02;
    COFD[904] = -2.12661865E+01;
    COFD[905] = 5.24930667E+00;
    COFD[906] = -4.17435088E-01;
    COFD[907] = 1.61434424E-02;
    COFD[908] = -1.87419199E+01;
    COFD[909] = 3.96926341E+00;
    COFD[910] = -2.16412264E-01;
    COFD[911] = 6.06012078E-03;
    COFD[912] = -2.13847439E+01;
    COFD[913] = 5.17440955E+00;
    COFD[914] = -4.04678430E-01;
    COFD[915] = 1.54706350E-02;
    COFD[916] = -2.13884087E+01;
    COFD[917] = 5.17440955E+00;
    COFD[918] = -4.04678430E-01;
    COFD[919] = 1.54706350E-02;
    COFD[920] = -2.03436890E+01;
    COFD[921] = 4.57152878E+00;
    COFD[922] = -3.08371263E-01;
    COFD[923] = 1.05838559E-02;
    COFD[924] = -2.03415655E+01;
    COFD[925] = 4.57152878E+00;
    COFD[926] = -3.08371263E-01;
    COFD[927] = 1.05838559E-02;
    COFD[928] = -2.03437836E+01;
    COFD[929] = 4.57152878E+00;
    COFD[930] = -3.08371263E-01;
    COFD[931] = 1.05838559E-02;
    COFD[932] = -1.90109377E+01;
    COFD[933] = 3.88639599E+00;
    COFD[934] = -2.03781856E-01;
    COFD[935] = 5.44050761E-03;
    COFD[936] = -1.90124255E+01;
    COFD[937] = 3.88639599E+00;
    COFD[938] = -2.03781856E-01;
    COFD[939] = 5.44050761E-03;
    COFD[940] = -1.83523887E+01;
    COFD[941] = 3.52256138E+00;
    COFD[942] = -1.48611326E-01;
    COFD[943] = 2.74505262E-03;
    COFD[944] = -1.76500883E+01;
    COFD[945] = 3.15831209E+00;
    COFD[946] = -9.45393054E-02;
    COFD[947] = 1.45090308E-04;
    COFD[948] = -1.68531596E+01;
    COFD[949] = 2.76314344E+00;
    COFD[950] = -3.65464735E-02;
    COFD[951] = -2.62462500E-03;
    COFD[952] = -1.62777238E+01;
    COFD[953] = 2.46665777E+00;
    COFD[954] = 7.25684399E-03;
    COFD[955] = -4.72424494E-03;
    COFD[956] = -1.62527734E+01;
    COFD[957] = 2.44062989E+00;
    COFD[958] = 1.10981222E-02;
    COFD[959] = -4.90788998E-03;
    COFD[960] = -1.58057876E+01;
    COFD[961] = 2.22000952E+00;
    COFD[962] = 4.34143206E-02;
    COFD[963] = -6.44371681E-03;
    COFD[964] = -1.49248401E+01;
    COFD[965] = 1.76546946E+00;
    COFD[966] = 1.09258240E-01;
    COFD[967] = -9.54522038E-03;
    COFD[968] = -1.45392473E+01;
    COFD[969] = 1.58706272E+00;
    COFD[970] = 1.34937972E-01;
    COFD[971] = -1.07491300E-02;
    COFD[972] = -1.41324994E+01;
    COFD[973] = 1.38727136E+00;
    COFD[974] = 1.63642976E-01;
    COFD[975] = -1.20926895E-02;
    COFD[976] = -2.10643259E+01;
    COFD[977] = 5.53614847E+00;
    COFD[978] = -4.86046736E-01;
    COFD[979] = 2.03659188E-02;
    COFD[980] = -2.28978338E+01;
    COFD[981] = 5.60324057E+00;
    COFD[982] = -4.88278590E-01;
    COFD[983] = 2.02076422E-02;
    COFD[984] = -1.46559141E+01;
    COFD[985] = 3.83606243E+00;
    COFD[986] = -2.86076532E-01;
    COFD[987] = 1.25205829E-02;
    COFD[988] = -1.43238998E+01;
    COFD[989] = 3.17651319E+00;
    COFD[990] = -2.02028974E-01;
    COFD[991] = 8.94232502E-03;
    COFD[992] = -1.43444709E+01;
    COFD[993] = 3.17651319E+00;
    COFD[994] = -2.02028974E-01;
    COFD[995] = 8.94232502E-03;
    COFD[996] = -1.55741053E+01;
    COFD[997] = 3.48070094E+00;
    COFD[998] = -2.40859499E-01;
    COFD[999] = 1.05972514E-02;
    COFD[1000] = -1.19006548E+01;
    COFD[1001] = 2.57507000E+00;
    COFD[1002] = -1.24033737E-01;
    COFD[1003] = 5.56694959E-03;
    COFD[1004] = -2.06516336E+01;
    COFD[1005] = 5.41688482E+00;
    COFD[1006] = -4.73387188E-01;
    COFD[1007] = 1.99280175E-02;
    COFD[1008] = -1.55816822E+01;
    COFD[1009] = 3.48070094E+00;
    COFD[1010] = -2.40859499E-01;
    COFD[1011] = 1.05972514E-02;
    COFD[1012] = -1.55661750E+01;
    COFD[1013] = 3.48070094E+00;
    COFD[1014] = -2.40859499E-01;
    COFD[1015] = 1.05972514E-02;
    COFD[1016] = -1.63588981E+01;
    COFD[1017] = 3.82388595E+00;
    COFD[1018] = -2.84480724E-01;
    COFD[1019] = 1.24506311E-02;
    COFD[1020] = -1.62824412E+01;
    COFD[1021] = 3.79163564E+00;
    COFD[1022] = -2.80257365E-01;
    COFD[1023] = 1.22656902E-02;
    COFD[1024] = -2.08438809E+01;
    COFD[1025] = 5.35267674E+00;
    COFD[1026] = -4.69010505E-01;
    COFD[1027] = 1.98979152E-02;
    COFD[1028] = -1.52861376E+01;
    COFD[1029] = 3.36790500E+00;
    COFD[1030] = -2.26321740E-01;
    COFD[1031] = 9.97135055E-03;
    COFD[1032] = -1.84863000E+01;
    COFD[1033] = 4.49330851E+00;
    COFD[1034] = -3.68208715E-01;
    COFD[1035] = 1.59565402E-02;
    COFD[1036] = -1.78969684E+01;
    COFD[1037] = 4.29613154E+00;
    COFD[1038] = -3.44012526E-01;
    COFD[1039] = 1.49643715E-02;
    COFD[1040] = -1.89685165E+01;
    COFD[1041] = 4.68595732E+00;
    COFD[1042] = -3.91842840E-01;
    COFD[1043] = 1.69262542E-02;
    COFD[1044] = -1.86570209E+01;
    COFD[1045] = 4.53572533E+00;
    COFD[1046] = -3.73386925E-01;
    COFD[1047] = 1.61678881E-02;
    COFD[1048] = -2.05408665E+01;
    COFD[1049] = 5.18417470E+00;
    COFD[1050] = -4.49491573E-01;
    COFD[1051] = 1.91438508E-02;
    COFD[1052] = -1.91291147E+01;
    COFD[1053] = 4.61801405E+00;
    COFD[1054] = -3.83535652E-01;
    COFD[1055] = 1.65862513E-02;
    COFD[1056] = -1.91345696E+01;
    COFD[1057] = 4.61801405E+00;
    COFD[1058] = -3.83535652E-01;
    COFD[1059] = 1.65862513E-02;
    COFD[1060] = -2.02658925E+01;
    COFD[1061] = 4.97613338E+00;
    COFD[1062] = -4.26175206E-01;
    COFD[1063] = 1.82809270E-02;
    COFD[1064] = -2.02625990E+01;
    COFD[1065] = 4.97613338E+00;
    COFD[1066] = -4.26175206E-01;
    COFD[1067] = 1.82809270E-02;
    COFD[1068] = -2.02660394E+01;
    COFD[1069] = 4.97613338E+00;
    COFD[1070] = -4.26175206E-01;
    COFD[1071] = 1.82809270E-02;
    COFD[1072] = -2.10774569E+01;
    COFD[1073] = 5.22065615E+00;
    COFD[1074] = -4.53751850E-01;
    COFD[1075] = 1.93099506E-02;
    COFD[1076] = -2.10798324E+01;
    COFD[1077] = 5.22065615E+00;
    COFD[1078] = -4.53751850E-01;
    COFD[1079] = 1.93099506E-02;
    COFD[1080] = -2.15884160E+01;
    COFD[1081] = 5.35995365E+00;
    COFD[1082] = -4.69587823E-01;
    COFD[1083] = 1.99080702E-02;
    COFD[1084] = -2.18544688E+01;
    COFD[1085] = 5.40768793E+00;
    COFD[1086] = -4.72942538E-01;
    COFD[1087] = 1.99389038E-02;
    COFD[1088] = -2.21542134E+01;
    COFD[1089] = 5.47842731E+00;
    COFD[1090] = -4.79952894E-01;
    COFD[1091] = 2.01568406E-02;
    COFD[1092] = -2.24546999E+01;
    COFD[1093] = 5.55319328E+00;
    COFD[1094] = -4.87759313E-01;
    COFD[1095] = 2.04205124E-02;
    COFD[1096] = -2.25003038E+01;
    COFD[1097] = 5.55793029E+00;
    COFD[1098] = -4.88170533E-01;
    COFD[1099] = 2.04302911E-02;
    COFD[1100] = -2.26669448E+01;
    COFD[1101] = 5.59528609E+00;
    COFD[1102] = -4.91473190E-01;
    COFD[1103] = 2.05131236E-02;
    COFD[1104] = -2.29049917E+01;
    COFD[1105] = 5.61013561E+00;
    COFD[1106] = -4.89933558E-01;
    COFD[1107] = 2.03098561E-02;
    COFD[1108] = -2.29187221E+01;
    COFD[1109] = 5.59504443E+00;
    COFD[1110] = -4.86415566E-01;
    COFD[1111] = 2.00947399E-02;
    COFD[1112] = -2.29645534E+01;
    COFD[1113] = 5.58219321E+00;
    COFD[1114] = -4.82941269E-01;
    COFD[1115] = 1.98711890E-02;
    COFD[1116] = -1.52554761E+01;
    COFD[1117] = 3.35922578E+00;
    COFD[1118] = -2.25181399E-01;
    COFD[1119] = 9.92132878E-03;
    COFD[1120] = -2.28722429E+01;
    COFD[1121] = 5.60324057E+00;
    COFD[1122] = -4.88278590E-01;
    COFD[1123] = 2.02076422E-02;
    COFD[1124] = -1.46550083E+01;
    COFD[1125] = 3.83606243E+00;
    COFD[1126] = -2.86076532E-01;
    COFD[1127] = 1.25205829E-02;
    COFD[1128] = -1.43139231E+01;
    COFD[1129] = 3.17651319E+00;
    COFD[1130] = -2.02028974E-01;
    COFD[1131] = 8.94232502E-03;
    COFD[1132] = -1.43340796E+01;
    COFD[1133] = 3.17651319E+00;
    COFD[1134] = -2.02028974E-01;
    COFD[1135] = 8.94232502E-03;
    COFD[1136] = -1.55588279E+01;
    COFD[1137] = 3.48070094E+00;
    COFD[1138] = -2.40859499E-01;
    COFD[1139] = 1.05972514E-02;
    COFD[1140] = -1.18988955E+01;
    COFD[1141] = 2.57507000E+00;
    COFD[1142] = -1.24033737E-01;
    COFD[1143] = 5.56694959E-03;
    COFD[1144] = -2.12652533E+01;
    COFD[1145] = 5.59961818E+00;
    COFD[1146] = -4.91624858E-01;
    COFD[1147] = 2.05035550E-02;
    COFD[1148] = -1.55661750E+01;
    COFD[1149] = 3.48070094E+00;
    COFD[1150] = -2.40859499E-01;
    COFD[1151] = 1.05972514E-02;
    COFD[1152] = -1.55511344E+01;
    COFD[1153] = 3.48070094E+00;
    COFD[1154] = -2.40859499E-01;
    COFD[1155] = 1.05972514E-02;
    COFD[1156] = -1.63493345E+01;
    COFD[1157] = 3.82388595E+00;
    COFD[1158] = -2.84480724E-01;
    COFD[1159] = 1.24506311E-02;
    COFD[1160] = -1.62724462E+01;
    COFD[1161] = 3.79163564E+00;
    COFD[1162] = -2.80257365E-01;
    COFD[1163] = 1.22656902E-02;
    COFD[1164] = -2.08293255E+01;
    COFD[1165] = 5.35267674E+00;
    COFD[1166] = -4.69010505E-01;
    COFD[1167] = 1.98979152E-02;
    COFD[1168] = -1.52721107E+01;
    COFD[1169] = 3.36790500E+00;
    COFD[1170] = -2.26321740E-01;
    COFD[1171] = 9.97135055E-03;
    COFD[1172] = -1.84688406E+01;
    COFD[1173] = 4.49330851E+00;
    COFD[1174] = -3.68208715E-01;
    COFD[1175] = 1.59565402E-02;
    COFD[1176] = -1.78834935E+01;
    COFD[1177] = 4.29613154E+00;
    COFD[1178] = -3.44012526E-01;
    COFD[1179] = 1.49643715E-02;
    COFD[1180] = -1.89544778E+01;
    COFD[1181] = 4.68595732E+00;
    COFD[1182] = -3.91842840E-01;
    COFD[1183] = 1.69262542E-02;
    COFD[1184] = -1.86424545E+01;
    COFD[1185] = 4.53572533E+00;
    COFD[1186] = -3.73386925E-01;
    COFD[1187] = 1.61678881E-02;
    COFD[1188] = -2.05235731E+01;
    COFD[1189] = 5.18417470E+00;
    COFD[1190] = -4.49491573E-01;
    COFD[1191] = 1.91438508E-02;
    COFD[1192] = -1.91121742E+01;
    COFD[1193] = 4.61801405E+00;
    COFD[1194] = -3.83535652E-01;
    COFD[1195] = 1.65862513E-02;
    COFD[1196] = -1.91174465E+01;
    COFD[1197] = 4.61801405E+00;
    COFD[1198] = -3.83535652E-01;
    COFD[1199] = 1.65862513E-02;
    COFD[1200] = -2.02466616E+01;
    COFD[1201] = 4.97613338E+00;
    COFD[1202] = -4.26175206E-01;
    COFD[1203] = 1.82809270E-02;
    COFD[1204] = -2.02434920E+01;
    COFD[1205] = 4.97613338E+00;
    COFD[1206] = -4.26175206E-01;
    COFD[1207] = 1.82809270E-02;
    COFD[1208] = -2.02468029E+01;
    COFD[1209] = 4.97613338E+00;
    COFD[1210] = -4.26175206E-01;
    COFD[1211] = 1.82809270E-02;
    COFD[1212] = -2.10567785E+01;
    COFD[1213] = 5.22065615E+00;
    COFD[1214] = -4.53751850E-01;
    COFD[1215] = 1.93099506E-02;
    COFD[1216] = -2.10590575E+01;
    COFD[1217] = 5.22065615E+00;
    COFD[1218] = -4.53751850E-01;
    COFD[1219] = 1.93099506E-02;
    COFD[1220] = -2.15664711E+01;
    COFD[1221] = 5.35995365E+00;
    COFD[1222] = -4.69587823E-01;
    COFD[1223] = 1.99080702E-02;
    COFD[1224] = -2.18316041E+01;
    COFD[1225] = 5.40768793E+00;
    COFD[1226] = -4.72942538E-01;
    COFD[1227] = 1.99389038E-02;
    COFD[1228] = -2.21306065E+01;
    COFD[1229] = 5.47842731E+00;
    COFD[1230] = -4.79952894E-01;
    COFD[1231] = 2.01568406E-02;
    COFD[1232] = -2.24304817E+01;
    COFD[1233] = 5.55319328E+00;
    COFD[1234] = -4.87759313E-01;
    COFD[1235] = 2.04205124E-02;
    COFD[1236] = -2.24760458E+01;
    COFD[1237] = 5.55793029E+00;
    COFD[1238] = -4.88170533E-01;
    COFD[1239] = 2.04302911E-02;
    COFD[1240] = -2.26422142E+01;
    COFD[1241] = 5.59528609E+00;
    COFD[1242] = -4.91473190E-01;
    COFD[1243] = 2.05131236E-02;
    COFD[1244] = -2.28794505E+01;
    COFD[1245] = 5.61013561E+00;
    COFD[1246] = -4.89933558E-01;
    COFD[1247] = 2.03098561E-02;
    COFD[1248] = -2.28924756E+01;
    COFD[1249] = 5.59504443E+00;
    COFD[1250] = -4.86415566E-01;
    COFD[1251] = 2.00947399E-02;
    COFD[1252] = -2.29380484E+01;
    COFD[1253] = 5.58219321E+00;
    COFD[1254] = -4.82941269E-01;
    COFD[1255] = 1.98711890E-02;
    COFD[1256] = -1.52414485E+01;
    COFD[1257] = 3.35922578E+00;
    COFD[1258] = -2.25181399E-01;
    COFD[1259] = 9.92132878E-03;
    COFD[1260] = -2.26337136E+01;
    COFD[1261] = 5.50133264E+00;
    COFD[1262] = -4.62484498E-01;
    COFD[1263] = 1.85871217E-02;
    COFD[1264] = -1.57994893E+01;
    COFD[1265] = 4.22225052E+00;
    COFD[1266] = -3.35156428E-01;
    COFD[1267] = 1.46104855E-02;
    COFD[1268] = -1.50766130E+01;
    COFD[1269] = 3.47945612E+00;
    COFD[1270] = -2.40703722E-01;
    COFD[1271] = 1.05907441E-02;
    COFD[1272] = -1.50911794E+01;
    COFD[1273] = 3.47945612E+00;
    COFD[1274] = -2.40703722E-01;
    COFD[1275] = 1.05907441E-02;
    COFD[1276] = -1.63542394E+01;
    COFD[1277] = 3.82388595E+00;
    COFD[1278] = -2.84480724E-01;
    COFD[1279] = 1.24506311E-02;
    COFD[1280] = -1.25141260E+01;
    COFD[1281] = 2.77873601E+00;
    COFD[1282] = -1.50637360E-01;
    COFD[1283] = 6.72684281E-03;
    COFD[1284] = -2.12831323E+01;
    COFD[1285] = 5.61184117E+00;
    COFD[1286] = -4.90532156E-01;
    COFD[1287] = 2.03507922E-02;
    COFD[1288] = -1.63588981E+01;
    COFD[1289] = 3.82388595E+00;
    COFD[1290] = -2.84480724E-01;
    COFD[1291] = 1.24506311E-02;
    COFD[1292] = -1.63493345E+01;
    COFD[1293] = 3.82388595E+00;
    COFD[1294] = -2.84480724E-01;
    COFD[1295] = 1.24506311E-02;
    COFD[1296] = -1.73374529E+01;
    COFD[1297] = 4.21416723E+00;
    COFD[1298] = -3.34163932E-01;
    COFD[1299] = 1.45697432E-02;
    COFD[1300] = -1.72738845E+01;
    COFD[1301] = 4.19029808E+00;
    COFD[1302] = -3.31177076E-01;
    COFD[1303] = 1.44446234E-02;
    COFD[1304] = -2.14449559E+01;
    COFD[1305] = 5.56531152E+00;
    COFD[1306] = -4.88789821E-01;
    COFD[1307] = 2.04437116E-02;
    COFD[1308] = -1.59863030E+01;
    COFD[1309] = 3.67388294E+00;
    COFD[1310] = -2.64990709E-01;
    COFD[1311] = 1.16042706E-02;
    COFD[1312] = -1.93276434E+01;
    COFD[1313] = 4.85015581E+00;
    COFD[1314] = -4.10945109E-01;
    COFD[1315] = 1.76651398E-02;
    COFD[1316] = -1.88463816E+01;
    COFD[1317] = 4.68393046E+00;
    COFD[1318] = -3.91610863E-01;
    COFD[1319] = 1.69174645E-02;
    COFD[1320] = -1.98646734E+01;
    COFD[1321] = 5.04367502E+00;
    COFD[1322] = -4.34153325E-01;
    COFD[1323] = 1.85956055E-02;
    COFD[1324] = -1.95552142E+01;
    COFD[1325] = 4.90255048E+00;
    COFD[1326] = -4.17368501E-01;
    COFD[1327] = 1.79287358E-02;
    COFD[1328] = -2.11637902E+01;
    COFD[1329] = 5.42846112E+00;
    COFD[1330] = -4.74321870E-01;
    COFD[1331] = 1.99459749E-02;
    COFD[1332] = -1.99803490E+01;
    COFD[1333] = 4.97875278E+00;
    COFD[1334] = -4.26485475E-01;
    COFD[1335] = 1.82931933E-02;
    COFD[1336] = -1.99835686E+01;
    COFD[1337] = 4.97875278E+00;
    COFD[1338] = -4.26485475E-01;
    COFD[1339] = 1.82931933E-02;
    COFD[1340] = -2.10571712E+01;
    COFD[1341] = 5.31360223E+00;
    COFD[1342] = -4.64787000E-01;
    COFD[1343] = 1.97483720E-02;
    COFD[1344] = -2.10553242E+01;
    COFD[1345] = 5.31360223E+00;
    COFD[1346] = -4.64787000E-01;
    COFD[1347] = 1.97483720E-02;
    COFD[1348] = -2.10572534E+01;
    COFD[1349] = 5.31360223E+00;
    COFD[1350] = -4.64787000E-01;
    COFD[1351] = 1.97483720E-02;
    COFD[1352] = -2.16471888E+01;
    COFD[1353] = 5.46403569E+00;
    COFD[1354] = -4.78350420E-01;
    COFD[1355] = 2.00977393E-02;
    COFD[1356] = -2.16484742E+01;
    COFD[1357] = 5.46403569E+00;
    COFD[1358] = -4.78350420E-01;
    COFD[1359] = 2.00977393E-02;
    COFD[1360] = -2.20820517E+01;
    COFD[1361] = 5.57296554E+00;
    COFD[1362] = -4.89394407E-01;
    COFD[1363] = 2.04545698E-02;
    COFD[1364] = -2.23196295E+01;
    COFD[1365] = 5.61199715E+00;
    COFD[1366] = -4.91433824E-01;
    COFD[1367] = 2.04245969E-02;
    COFD[1368] = -2.23961406E+01;
    COFD[1369] = 5.58969231E+00;
    COFD[1370] = -4.85146102E-01;
    COFD[1371] = 2.00165025E-02;
    COFD[1372] = -2.25070779E+01;
    COFD[1373] = 5.58488808E+00;
    COFD[1374] = -4.81505670E-01;
    COFD[1375] = 1.97377350E-02;
    COFD[1376] = -2.25413169E+01;
    COFD[1377] = 5.58519141E+00;
    COFD[1378] = -4.81277367E-01;
    COFD[1379] = 1.97172346E-02;
    COFD[1380] = -2.26104730E+01;
    COFD[1381] = 5.58175450E+00;
    COFD[1382] = -4.78660487E-01;
    COFD[1383] = 1.95178220E-02;
    COFD[1384] = -2.26368466E+01;
    COFD[1385] = 5.50714270E+00;
    COFD[1386] = -4.64060018E-01;
    COFD[1387] = 1.86881341E-02;
    COFD[1388] = -2.26543439E+01;
    COFD[1389] = 5.49570925E+00;
    COFD[1390] = -4.60969345E-01;
    COFD[1391] = 1.84906305E-02;
    COFD[1392] = -2.26384265E+01;
    COFD[1393] = 5.45743664E+00;
    COFD[1394] = -4.53877300E-01;
    COFD[1395] = 1.80982969E-02;
    COFD[1396] = -1.59633387E+01;
    COFD[1397] = 3.66853818E+00;
    COFD[1398] = -2.64346221E-01;
    COFD[1399] = 1.15784613E-02;
    COFD[1400] = -2.26491367E+01;
    COFD[1401] = 5.50690143E+00;
    COFD[1402] = -4.64003349E-01;
    COFD[1403] = 1.86846584E-02;
    COFD[1404] = -1.57199037E+01;
    COFD[1405] = 4.19936335E+00;
    COFD[1406] = -3.32311009E-01;
    COFD[1407] = 1.44921003E-02;
    COFD[1408] = -1.50270339E+01;
    COFD[1409] = 3.46140064E+00;
    COFD[1410] = -2.38440092E-01;
    COFD[1411] = 1.04960087E-02;
    COFD[1412] = -1.50420953E+01;
    COFD[1413] = 3.46140064E+00;
    COFD[1414] = -2.38440092E-01;
    COFD[1415] = 1.04960087E-02;
    COFD[1416] = -1.62775714E+01;
    COFD[1417] = 3.79163564E+00;
    COFD[1418] = -2.80257365E-01;
    COFD[1419] = 1.22656902E-02;
    COFD[1420] = -1.24693568E+01;
    COFD[1421] = 2.76686648E+00;
    COFD[1422] = -1.49120141E-01;
    COFD[1423] = 6.66220432E-03;
    COFD[1424] = -2.14087397E+01;
    COFD[1425] = 5.57282008E+00;
    COFD[1426] = -4.76690890E-01;
    COFD[1427] = 1.94000719E-02;
    COFD[1428] = -1.62824412E+01;
    COFD[1429] = 3.79163564E+00;
    COFD[1430] = -2.80257365E-01;
    COFD[1431] = 1.22656902E-02;
    COFD[1432] = -1.62724462E+01;
    COFD[1433] = 3.79163564E+00;
    COFD[1434] = -2.80257365E-01;
    COFD[1435] = 1.22656902E-02;
    COFD[1436] = -1.72738845E+01;
    COFD[1437] = 4.19029808E+00;
    COFD[1438] = -3.31177076E-01;
    COFD[1439] = 1.44446234E-02;
    COFD[1440] = -1.72167708E+01;
    COFD[1441] = 4.16886779E+00;
    COFD[1442] = -3.28518156E-01;
    COFD[1443] = 1.43341626E-02;
    COFD[1444] = -2.14082453E+01;
    COFD[1445] = 5.55346617E+00;
    COFD[1446] = -4.87783156E-01;
    COFD[1447] = 2.04210886E-02;
    COFD[1448] = -1.59525102E+01;
    COFD[1449] = 3.66023858E+00;
    COFD[1450] = -2.63401043E-01;
    COFD[1451] = 1.15432000E-02;
    COFD[1452] = -1.92867554E+01;
    COFD[1453] = 4.83375900E+00;
    COFD[1454] = -4.09146560E-01;
    COFD[1455] = 1.76006599E-02;
    COFD[1456] = -1.87897298E+01;
    COFD[1457] = 4.66162351E+00;
    COFD[1458] = -3.88920477E-01;
    COFD[1459] = 1.68089648E-02;
    COFD[1460] = -1.98075055E+01;
    COFD[1461] = 5.02169524E+00;
    COFD[1462] = -4.31582804E-01;
    COFD[1463] = 1.84953568E-02;
    COFD[1464] = -1.94823660E+01;
    COFD[1465] = 4.87333294E+00;
    COFD[1466] = -4.13769241E-01;
    COFD[1467] = 1.77802244E-02;
    COFD[1468] = -2.11341653E+01;
    COFD[1469] = 5.41773516E+00;
    COFD[1470] = -4.73414338E-01;
    COFD[1471] = 1.99258685E-02;
    COFD[1472] = -1.99235839E+01;
    COFD[1473] = 4.95514826E+00;
    COFD[1474] = -4.23691395E-01;
    COFD[1475] = 1.81828318E-02;
    COFD[1476] = -1.99269592E+01;
    COFD[1477] = 4.95514826E+00;
    COFD[1478] = -4.23691395E-01;
    COFD[1479] = 1.81828318E-02;
    COFD[1480] = -2.09912124E+01;
    COFD[1481] = 5.28557747E+00;
    COFD[1482] = -4.61402384E-01;
    COFD[1483] = 1.96111546E-02;
    COFD[1484] = -2.09892693E+01;
    COFD[1485] = 5.28557747E+00;
    COFD[1486] = -4.61402384E-01;
    COFD[1487] = 1.96111546E-02;
    COFD[1488] = -2.09912990E+01;
    COFD[1489] = 5.28557747E+00;
    COFD[1490] = -4.61402384E-01;
    COFD[1491] = 1.96111546E-02;
    COFD[1492] = -2.15968629E+01;
    COFD[1493] = 5.44280136E+00;
    COFD[1494] = -4.75998855E-01;
    COFD[1495] = 2.00116091E-02;
    COFD[1496] = -2.15982183E+01;
    COFD[1497] = 5.44280136E+00;
    COFD[1498] = -4.75998855E-01;
    COFD[1499] = 2.00116091E-02;
    COFD[1500] = -2.20571986E+01;
    COFD[1501] = 5.56221277E+00;
    COFD[1502] = -4.88535386E-01;
    COFD[1503] = 2.04385478E-02;
    COFD[1504] = -2.23089123E+01;
    COFD[1505] = 5.60657745E+00;
    COFD[1506] = -4.91280842E-01;
    COFD[1507] = 2.04393241E-02;
    COFD[1508] = -2.24147231E+01;
    COFD[1509] = 5.59714923E+00;
    COFD[1510] = -4.86899964E-01;
    COFD[1511] = 2.01242645E-02;
    COFD[1512] = -2.25078393E+01;
    COFD[1513] = 5.58461787E+00;
    COFD[1514] = -4.82178861E-01;
    COFD[1515] = 1.97950972E-02;
    COFD[1516] = -2.25423017E+01;
    COFD[1517] = 5.58490857E+00;
    COFD[1518] = -4.81943660E-01;
    COFD[1519] = 1.97740434E-02;
    COFD[1520] = -2.26201688E+01;
    COFD[1521] = 5.58491601E+00;
    COFD[1522] = -4.79798668E-01;
    COFD[1523] = 1.95960400E-02;
    COFD[1524] = -2.26685946E+01;
    COFD[1525] = 5.51963910E+00;
    COFD[1526] = -4.66555705E-01;
    COFD[1527] = 1.88313890E-02;
    COFD[1528] = -2.26705835E+01;
    COFD[1529] = 5.50119356E+00;
    COFD[1530] = -4.62439469E-01;
    COFD[1531] = 1.85841071E-02;
    COFD[1532] = -2.26836497E+01;
    COFD[1533] = 5.47552771E+00;
    COFD[1534] = -4.57173951E-01;
    COFD[1535] = 1.82791721E-02;
    COFD[1536] = -1.59327297E+01;
    COFD[1537] = 3.65620899E+00;
    COFD[1538] = -2.62933804E-01;
    COFD[1539] = 1.15253223E-02;
    COFD[1540] = -1.64683757E+01;
    COFD[1541] = 2.28170571E+00;
    COFD[1542] = 3.44078407E-02;
    COFD[1543] = -6.01703889E-03;
    COFD[1544] = -1.97550088E+01;
    COFD[1545] = 5.56931926E+00;
    COFD[1546] = -4.89105511E-01;
    COFD[1547] = 2.04493129E-02;
    COFD[1548] = -1.94373127E+01;
    COFD[1549] = 5.02567894E+00;
    COFD[1550] = -4.32045169E-01;
    COFD[1551] = 1.85132214E-02;
    COFD[1552] = -1.94570287E+01;
    COFD[1553] = 5.02567894E+00;
    COFD[1554] = -4.32045169E-01;
    COFD[1555] = 1.85132214E-02;
    COFD[1556] = -2.08367725E+01;
    COFD[1557] = 5.35267674E+00;
    COFD[1558] = -4.69010505E-01;
    COFD[1559] = 1.98979152E-02;
    COFD[1560] = -1.60528285E+01;
    COFD[1561] = 4.11188603E+00;
    COFD[1562] = -3.21540884E-01;
    COFD[1563] = 1.40482564E-02;
    COFD[1564] = -1.77563250E+01;
    COFD[1565] = 3.57475686E+00;
    COFD[1566] = -1.56396297E-01;
    COFD[1567] = 3.12157721E-03;
    COFD[1568] = -2.08438809E+01;
    COFD[1569] = 5.35267674E+00;
    COFD[1570] = -4.69010505E-01;
    COFD[1571] = 1.98979152E-02;
    COFD[1572] = -2.08293255E+01;
    COFD[1573] = 5.35267674E+00;
    COFD[1574] = -4.69010505E-01;
    COFD[1575] = 1.98979152E-02;
    COFD[1576] = -2.14449559E+01;
    COFD[1577] = 5.56531152E+00;
    COFD[1578] = -4.88789821E-01;
    COFD[1579] = 2.04437116E-02;
    COFD[1580] = -2.14082453E+01;
    COFD[1581] = 5.55346617E+00;
    COFD[1582] = -4.87783156E-01;
    COFD[1583] = 2.04210886E-02;
    COFD[1584] = -1.90499441E+01;
    COFD[1585] = 3.99221757E+00;
    COFD[1586] = -2.19854880E-01;
    COFD[1587] = 6.22736279E-03;
    COFD[1588] = -2.05128705E+01;
    COFD[1589] = 5.23843909E+00;
    COFD[1590] = -4.55815614E-01;
    COFD[1591] = 1.93898040E-02;
    COFD[1592] = -2.19317743E+01;
    COFD[1593] = 5.45216133E+00;
    COFD[1594] = -4.52916925E-01;
    COFD[1595] = 1.80456400E-02;
    COFD[1596] = -2.20036369E+01;
    COFD[1597] = 5.55935694E+00;
    COFD[1598] = -4.74154740E-01;
    COFD[1599] = 1.92584304E-02;
    COFD[1600] = -2.16379567E+01;
    COFD[1601] = 5.29019717E+00;
    COFD[1602] = -4.24502606E-01;
    COFD[1603] = 1.65197343E-02;
    COFD[1604] = -2.19399793E+01;
    COFD[1605] = 5.41841631E+00;
    COFD[1606] = -4.46818971E-01;
    COFD[1607] = 1.77127652E-02;
    COFD[1608] = -2.01064363E+01;
    COFD[1609] = 4.41511629E+00;
    COFD[1610] = -2.84086963E-01;
    COFD[1611] = 9.37586971E-03;
    COFD[1612] = -2.20947902E+01;
    COFD[1613] = 5.36053938E+00;
    COFD[1614] = -4.36434519E-01;
    COFD[1615] = 1.71484255E-02;
    COFD[1616] = -2.20998738E+01;
    COFD[1617] = 5.36053938E+00;
    COFD[1618] = -4.36434519E-01;
    COFD[1619] = 1.71484255E-02;
    COFD[1620] = -2.13351281E+01;
    COFD[1621] = 4.87252053E+00;
    COFD[1622] = -3.56127804E-01;
    COFD[1623] = 1.29948788E-02;
    COFD[1624] = -2.13320853E+01;
    COFD[1625] = 4.87252053E+00;
    COFD[1626] = -3.56127804E-01;
    COFD[1627] = 1.29948788E-02;
    COFD[1628] = -2.13352637E+01;
    COFD[1629] = 4.87252053E+00;
    COFD[1630] = -3.56127804E-01;
    COFD[1631] = 1.29948788E-02;
    COFD[1632] = -2.03618484E+01;
    COFD[1633] = 4.33469236E+00;
    COFD[1634] = -2.71728538E-01;
    COFD[1635] = 8.76524142E-03;
    COFD[1636] = -2.03640292E+01;
    COFD[1637] = 4.33469236E+00;
    COFD[1638] = -2.71728538E-01;
    COFD[1639] = 8.76524142E-03;
    COFD[1640] = -1.96505535E+01;
    COFD[1641] = 3.94748284E+00;
    COFD[1642] = -2.13105313E-01;
    COFD[1643] = 5.89820150E-03;
    COFD[1644] = -1.91553507E+01;
    COFD[1645] = 3.66719242E+00;
    COFD[1646] = -1.70260825E-01;
    COFD[1647] = 3.79442136E-03;
    COFD[1648] = -1.85405111E+01;
    COFD[1649] = 3.34746301E+00;
    COFD[1650] = -1.22575165E-01;
    COFD[1651] = 1.49120224E-03;
    COFD[1652] = -1.80138685E+01;
    COFD[1653] = 3.07345302E+00;
    COFD[1654] = -8.20536856E-02;
    COFD[1655] = -4.51137426E-04;
    COFD[1656] = -1.79714342E+01;
    COFD[1657] = 3.04109524E+00;
    COFD[1658] = -7.73097243E-02;
    COFD[1659] = -6.77522484E-04;
    COFD[1660] = -1.74160321E+01;
    COFD[1661] = 2.77201495E+00;
    COFD[1662] = -3.78503295E-02;
    COFD[1663] = -2.56220054E-03;
    COFD[1664] = -1.66390161E+01;
    COFD[1665] = 2.35716270E+00;
    COFD[1666] = 2.33630190E-02;
    COFD[1667] = -5.49227499E-03;
    COFD[1668] = -1.63277528E+01;
    COFD[1669] = 2.20684400E+00;
    COFD[1670] = 4.53338465E-02;
    COFD[1671] = -6.53458896E-03;
    COFD[1672] = -1.59785669E+01;
    COFD[1673] = 2.03061309E+00;
    COFD[1674] = 7.09547643E-02;
    COFD[1675] = -7.74512315E-03;
    COFD[1676] = -2.04833713E+01;
    COFD[1677] = 5.23112374E+00;
    COFD[1678] = -4.54967682E-01;
    COFD[1679] = 1.93570423E-02;
    COFD[1680] = -2.27747816E+01;
    COFD[1681] = 5.60263382E+00;
    COFD[1682] = -4.91348327E-01;
    COFD[1683] = 2.04651120E-02;
    COFD[1684] = -1.43151174E+01;
    COFD[1685] = 3.68038508E+00;
    COFD[1686] = -2.65779346E-01;
    COFD[1687] = 1.16360771E-02;
    COFD[1688] = -1.40999008E+01;
    COFD[1689] = 3.08120012E+00;
    COFD[1690] = -1.89629903E-01;
    COFD[1691] = 8.40361952E-03;
    COFD[1692] = -1.41191261E+01;
    COFD[1693] = 3.08120012E+00;
    COFD[1694] = -1.89629903E-01;
    COFD[1695] = 8.40361952E-03;
    COFD[1696] = -1.52792891E+01;
    COFD[1697] = 3.36790500E+00;
    COFD[1698] = -2.26321740E-01;
    COFD[1699] = 9.97135055E-03;
    COFD[1700] = -1.17159737E+01;
    COFD[1701] = 2.48123210E+00;
    COFD[1702] = -1.11322604E-01;
    COFD[1703] = 4.99282389E-03;
    COFD[1704] = -2.11388331E+01;
    COFD[1705] = 5.55529675E+00;
    COFD[1706] = -4.87942518E-01;
    COFD[1707] = 2.04249054E-02;
    COFD[1708] = -1.52861376E+01;
    COFD[1709] = 3.36790500E+00;
    COFD[1710] = -2.26321740E-01;
    COFD[1711] = 9.97135055E-03;
    COFD[1712] = -1.52721107E+01;
    COFD[1713] = 3.36790500E+00;
    COFD[1714] = -2.26321740E-01;
    COFD[1715] = 9.97135055E-03;
    COFD[1716] = -1.59863030E+01;
    COFD[1717] = 3.67388294E+00;
    COFD[1718] = -2.64990709E-01;
    COFD[1719] = 1.16042706E-02;
    COFD[1720] = -1.59525102E+01;
    COFD[1721] = 3.66023858E+00;
    COFD[1722] = -2.63401043E-01;
    COFD[1723] = 1.15432000E-02;
    COFD[1724] = -2.05128705E+01;
    COFD[1725] = 5.23843909E+00;
    COFD[1726] = -4.55815614E-01;
    COFD[1727] = 1.93898040E-02;
    COFD[1728] = -1.50233475E+01;
    COFD[1729] = 3.26660767E+00;
    COFD[1730] = -2.13287177E-01;
    COFD[1731] = 9.41137857E-03;
    COFD[1732] = -1.81735763E+01;
    COFD[1733] = 4.38391495E+00;
    COFD[1734] = -3.54941287E-01;
    COFD[1735] = 1.54195107E-02;
    COFD[1736] = -1.76285640E+01;
    COFD[1737] = 4.19935698E+00;
    COFD[1738] = -3.32310212E-01;
    COFD[1739] = 1.44920670E-02;
    COFD[1740] = -1.86157761E+01;
    COFD[1741] = 4.55689508E+00;
    COFD[1742] = -3.75937921E-01;
    COFD[1743] = 1.62703488E-02;
    COFD[1744] = -1.83538377E+01;
    COFD[1745] = 4.42828044E+00;
    COFD[1746] = -3.60417833E-01;
    COFD[1747] = 1.56455103E-02;
    COFD[1748] = -2.02969740E+01;
    COFD[1749] = 5.11106992E+00;
    COFD[1750] = -4.42047129E-01;
    COFD[1751] = 1.89042990E-02;
    COFD[1752] = -1.87685041E+01;
    COFD[1753] = 4.49191492E+00;
    COFD[1754] = -3.68041771E-01;
    COFD[1755] = 1.59498676E-02;
    COFD[1756] = -1.87733838E+01;
    COFD[1757] = 4.49191492E+00;
    COFD[1758] = -3.68041771E-01;
    COFD[1759] = 1.59498676E-02;
    COFD[1760] = -1.98830936E+01;
    COFD[1761] = 4.84731557E+00;
    COFD[1762] = -4.10638352E-01;
    COFD[1763] = 1.76543886E-02;
    COFD[1764] = -1.98801866E+01;
    COFD[1765] = 4.84731557E+00;
    COFD[1766] = -4.10638352E-01;
    COFD[1767] = 1.76543886E-02;
    COFD[1768] = -1.98832231E+01;
    COFD[1769] = 4.84731557E+00;
    COFD[1770] = -4.10638352E-01;
    COFD[1771] = 1.76543886E-02;
    COFD[1772] = -2.07935867E+01;
    COFD[1773] = 5.13870075E+00;
    COFD[1774] = -4.45087556E-01;
    COFD[1775] = 1.90141513E-02;
    COFD[1776] = -2.07956631E+01;
    COFD[1777] = 5.13870075E+00;
    COFD[1778] = -4.45087556E-01;
    COFD[1779] = 1.90141513E-02;
    COFD[1780] = -2.12462495E+01;
    COFD[1781] = 5.25612649E+00;
    COFD[1782] = -4.57885944E-01;
    COFD[1783] = 1.94706895E-02;
    COFD[1784] = -2.16479704E+01;
    COFD[1785] = 5.36415648E+00;
    COFD[1786] = -4.69836047E-01;
    COFD[1787] = 1.99067039E-02;
    COFD[1788] = -2.18745018E+01;
    COFD[1789] = 5.40581593E+00;
    COFD[1790] = -4.72804333E-01;
    COFD[1791] = 1.99370903E-02;
    COFD[1792] = -2.21149618E+01;
    COFD[1793] = 5.45837402E+00;
    COFD[1794] = -4.77716491E-01;
    COFD[1795] = 2.00742141E-02;
    COFD[1796] = -2.21693758E+01;
    COFD[1797] = 5.46715955E+00;
    COFD[1798] = -4.78699053E-01;
    COFD[1799] = 2.01106334E-02;
    COFD[1800] = -2.23927663E+01;
    COFD[1801] = 5.52880430E+00;
    COFD[1802] = -4.85323223E-01;
    COFD[1803] = 2.03436366E-02;
    COFD[1804] = -2.27595706E+01;
    COFD[1805] = 5.59981860E+00;
    COFD[1806] = -4.91614072E-01;
    COFD[1807] = 2.05016331E-02;
    COFD[1808] = -2.28218117E+01;
    COFD[1809] = 5.60692017E+00;
    COFD[1810] = -4.91295058E-01;
    COFD[1811] = 2.04387644E-02;
    COFD[1812] = -2.29034521E+01;
    COFD[1813] = 5.61183042E+00;
    COFD[1814] = -4.90566396E-01;
    COFD[1815] = 2.03536798E-02;
    COFD[1816] = -1.50031687E+01;
    COFD[1817] = 3.26223357E+00;
    COFD[1818] = -2.12746642E-01;
    COFD[1819] = 9.38912883E-03;
    COFD[1820] = -2.17131617E+01;
    COFD[1821] = 4.69815223E+00;
    COFD[1822] = -3.28454103E-01;
    COFD[1823] = 1.15970636E-02;
    COFD[1824] = -1.76147026E+01;
    COFD[1825] = 4.86049500E+00;
    COFD[1826] = -4.12200578E-01;
    COFD[1827] = 1.77160971E-02;
    COFD[1828] = -1.70534856E+01;
    COFD[1829] = 4.14240922E+00;
    COFD[1830] = -3.25239774E-01;
    COFD[1831] = 1.41980687E-02;
    COFD[1832] = -1.70757047E+01;
    COFD[1833] = 4.14240922E+00;
    COFD[1834] = -3.25239774E-01;
    COFD[1835] = 1.41980687E-02;
    COFD[1836] = -1.84777607E+01;
    COFD[1837] = 4.49330851E+00;
    COFD[1838] = -3.68208715E-01;
    COFD[1839] = 1.59565402E-02;
    COFD[1840] = -1.37794315E+01;
    COFD[1841] = 3.23973858E+00;
    COFD[1842] = -2.09989036E-01;
    COFD[1843] = 9.27667906E-03;
    COFD[1844] = -2.07653719E+01;
    COFD[1845] = 5.01092022E+00;
    COFD[1846] = -3.77985635E-01;
    COFD[1847] = 1.40968645E-02;
    COFD[1848] = -1.84863000E+01;
    COFD[1849] = 4.49330851E+00;
    COFD[1850] = -3.68208715E-01;
    COFD[1851] = 1.59565402E-02;
    COFD[1852] = -1.84688406E+01;
    COFD[1853] = 4.49330851E+00;
    COFD[1854] = -3.68208715E-01;
    COFD[1855] = 1.59565402E-02;
    COFD[1856] = -1.93276434E+01;
    COFD[1857] = 4.85015581E+00;
    COFD[1858] = -4.10945109E-01;
    COFD[1859] = 1.76651398E-02;
    COFD[1860] = -1.92867554E+01;
    COFD[1861] = 4.83375900E+00;
    COFD[1862] = -4.09146560E-01;
    COFD[1863] = 1.76006599E-02;
    COFD[1864] = -2.19317743E+01;
    COFD[1865] = 5.45216133E+00;
    COFD[1866] = -4.52916925E-01;
    COFD[1867] = 1.80456400E-02;
    COFD[1868] = -1.81735763E+01;
    COFD[1869] = 4.38391495E+00;
    COFD[1870] = -3.54941287E-01;
    COFD[1871] = 1.54195107E-02;
    COFD[1872] = -2.13425698E+01;
    COFD[1873] = 5.40460130E+00;
    COFD[1874] = -4.72718910E-01;
    COFD[1875] = 1.99362717E-02;
    COFD[1876] = -2.09191285E+01;
    COFD[1877] = 5.30153901E+00;
    COFD[1878] = -4.63335119E-01;
    COFD[1879] = 1.96897053E-02;
    COFD[1880] = -2.16802612E+01;
    COFD[1881] = 5.52918296E+00;
    COFD[1882] = -4.85360709E-01;
    COFD[1883] = 2.03448006E-02;
    COFD[1884] = -2.14326461E+01;
    COFD[1885] = 5.41729961E+00;
    COFD[1886] = -4.73400281E-01;
    COFD[1887] = 1.99269567E-02;
    COFD[1888] = -2.22176950E+01;
    COFD[1889] = 5.54251230E+00;
    COFD[1890] = -4.70946314E-01;
    COFD[1891] = 1.90785869E-02;
    COFD[1892] = -2.18590741E+01;
    COFD[1893] = 5.47368915E+00;
    COFD[1894] = -4.79424291E-01;
    COFD[1895] = 2.01372920E-02;
    COFD[1896] = -2.18653077E+01;
    COFD[1897] = 5.47368915E+00;
    COFD[1898] = -4.79424291E-01;
    COFD[1899] = 2.01372920E-02;
    COFD[1900] = -2.25017046E+01;
    COFD[1901] = 5.59178974E+00;
    COFD[1902] = -4.85668031E-01;
    COFD[1903] = 2.00491907E-02;
    COFD[1904] = -2.24978710E+01;
    COFD[1905] = 5.59178974E+00;
    COFD[1906] = -4.85668031E-01;
    COFD[1907] = 2.00491907E-02;
    COFD[1908] = -2.25018756E+01;
    COFD[1909] = 5.59178974E+00;
    COFD[1910] = -4.85668031E-01;
    COFD[1911] = 2.00491907E-02;
    COFD[1912] = -2.26255668E+01;
    COFD[1913] = 5.52088531E+00;
    COFD[1914] = -4.66799303E-01;
    COFD[1915] = 1.88452220E-02;
    COFD[1916] = -2.26283699E+01;
    COFD[1917] = 5.52088531E+00;
    COFD[1918] = -4.66799303E-01;
    COFD[1919] = 1.88452220E-02;
    COFD[1920] = -2.26428179E+01;
    COFD[1921] = 5.43854589E+00;
    COFD[1922] = -4.50460433E-01;
    COFD[1923] = 1.79115741E-02;
    COFD[1924] = -2.25594209E+01;
    COFD[1925] = 5.32923056E+00;
    COFD[1926] = -4.31065031E-01;
    COFD[1927] = 1.68637777E-02;
    COFD[1928] = -2.24455407E+01;
    COFD[1929] = 5.21716989E+00;
    COFD[1930] = -4.11825254E-01;
    COFD[1931] = 1.58436458E-02;
    COFD[1932] = -2.23014132E+01;
    COFD[1933] = 5.09843164E+00;
    COFD[1934] = -3.92118361E-01;
    COFD[1935] = 1.48192579E-02;
    COFD[1936] = -2.23041722E+01;
    COFD[1937] = 5.08480808E+00;
    COFD[1938] = -3.89912767E-01;
    COFD[1939] = 1.47062570E-02;
    COFD[1940] = -2.21269412E+01;
    COFD[1941] = 4.97192306E+00;
    COFD[1942] = -3.71804005E-01;
    COFD[1943] = 1.37844212E-02;
    COFD[1944] = -2.17987077E+01;
    COFD[1945] = 4.73948321E+00;
    COFD[1946] = -3.35004943E-01;
    COFD[1947] = 1.19275825E-02;
    COFD[1948] = -2.16564654E+01;
    COFD[1949] = 4.65505428E+00;
    COFD[1950] = -3.21651467E-01;
    COFD[1951] = 1.12547138E-02;
    COFD[1952] = -2.15260284E+01;
    COFD[1953] = 4.56640766E+00;
    COFD[1954] = -3.07568513E-01;
    COFD[1955] = 1.05437086E-02;
    COFD[1956] = -1.81432461E+01;
    COFD[1957] = 4.37565431E+00;
    COFD[1958] = -3.53906025E-01;
    COFD[1959] = 1.53760786E-02;
    COFD[1960] = -2.22067747E+01;
    COFD[1961] = 5.03874797E+00;
    COFD[1962] = -3.82440213E-01;
    COFD[1963] = 1.43232015E-02;
    COFD[1964] = -1.72232223E+01;
    COFD[1965] = 4.69060745E+00;
    COFD[1966] = -3.92369888E-01;
    COFD[1967] = 1.69459661E-02;
    COFD[1968] = -1.65488358E+01;
    COFD[1969] = 3.95035840E+00;
    COFD[1970] = -3.00959418E-01;
    COFD[1971] = 1.31692593E-02;
    COFD[1972] = -1.65675362E+01;
    COFD[1973] = 3.95035840E+00;
    COFD[1974] = -3.00959418E-01;
    COFD[1975] = 1.31692593E-02;
    COFD[1976] = -1.78903913E+01;
    COFD[1977] = 4.29613154E+00;
    COFD[1978] = -3.44012526E-01;
    COFD[1979] = 1.49643715E-02;
    COFD[1980] = -1.34709807E+01;
    COFD[1981] = 3.09379603E+00;
    COFD[1982] = -1.91268635E-01;
    COFD[1983] = 8.47480224E-03;
    COFD[1984] = -2.15095980E+01;
    COFD[1985] = 5.46737673E+00;
    COFD[1986] = -4.55696085E-01;
    COFD[1987] = 1.81982625E-02;
    COFD[1988] = -1.78969684E+01;
    COFD[1989] = 4.29613154E+00;
    COFD[1990] = -3.44012526E-01;
    COFD[1991] = 1.49643715E-02;
    COFD[1992] = -1.78834935E+01;
    COFD[1993] = 4.29613154E+00;
    COFD[1994] = -3.44012526E-01;
    COFD[1995] = 1.49643715E-02;
    COFD[1996] = -1.88463816E+01;
    COFD[1997] = 4.68393046E+00;
    COFD[1998] = -3.91610863E-01;
    COFD[1999] = 1.69174645E-02;
    COFD[2000] = -1.87897298E+01;
    COFD[2001] = 4.66162351E+00;
    COFD[2002] = -3.88920477E-01;
    COFD[2003] = 1.68089648E-02;
    COFD[2004] = -2.20036369E+01;
    COFD[2005] = 5.55935694E+00;
    COFD[2006] = -4.74154740E-01;
    COFD[2007] = 1.92584304E-02;
    COFD[2008] = -1.76285640E+01;
    COFD[2009] = 4.19935698E+00;
    COFD[2010] = -3.32310212E-01;
    COFD[2011] = 1.44920670E-02;
    COFD[2012] = -2.09191285E+01;
    COFD[2013] = 5.30153901E+00;
    COFD[2014] = -4.63335119E-01;
    COFD[2015] = 1.96897053E-02;
    COFD[2016] = -2.03766950E+01;
    COFD[2017] = 5.13263469E+00;
    COFD[2018] = -4.44457285E-01;
    COFD[2019] = 1.89932102E-02;
    COFD[2020] = -2.12121370E+01;
    COFD[2021] = 5.39823225E+00;
    COFD[2022] = -4.72294645E-01;
    COFD[2023] = 1.99340225E-02;
    COFD[2024] = -2.10944088E+01;
    COFD[2025] = 5.34286099E+00;
    COFD[2026] = -4.68100992E-01;
    COFD[2027] = 1.98731399E-02;
    COFD[2028] = -2.21110290E+01;
    COFD[2029] = 5.58360799E+00;
    COFD[2030] = -4.82701436E-01;
    COFD[2031] = 1.98437922E-02;
    COFD[2032] = -2.14323189E+01;
    COFD[2033] = 5.37331605E+00;
    COFD[2034] = -4.70491203E-01;
    COFD[2035] = 1.99134666E-02;
    COFD[2036] = -2.14369874E+01;
    COFD[2037] = 5.37331605E+00;
    COFD[2038] = -4.70491203E-01;
    COFD[2039] = 1.99134666E-02;
    COFD[2040] = -2.22816111E+01;
    COFD[2041] = 5.59185582E+00;
    COFD[2042] = -4.91155812E-01;
    COFD[2043] = 2.05043018E-02;
    COFD[2044] = -2.22788436E+01;
    COFD[2045] = 5.59185582E+00;
    COFD[2046] = -4.91155812E-01;
    COFD[2047] = 2.05043018E-02;
    COFD[2048] = -2.22817344E+01;
    COFD[2049] = 5.59185582E+00;
    COFD[2050] = -4.91155812E-01;
    COFD[2051] = 2.05043018E-02;
    COFD[2052] = -2.25312008E+01;
    COFD[2053] = 5.58497731E+00;
    COFD[2054] = -4.81782064E-01;
    COFD[2055] = 1.97602508E-02;
    COFD[2056] = -2.25331708E+01;
    COFD[2057] = 5.58497731E+00;
    COFD[2058] = -4.81782064E-01;
    COFD[2059] = 1.97602508E-02;
    COFD[2060] = -2.26391623E+01;
    COFD[2061] = 5.54906697E+00;
    COFD[2062] = -4.72198999E-01;
    COFD[2063] = 1.91488843E-02;
    COFD[2064] = -2.26655271E+01;
    COFD[2065] = 5.49155307E+00;
    COFD[2066] = -4.60156593E-01;
    COFD[2067] = 1.84445999E-02;
    COFD[2068] = -2.26097272E+01;
    COFD[2069] = 5.40751776E+00;
    COFD[2070] = -4.44796499E-01;
    COFD[2071] = 1.76008889E-02;
    COFD[2072] = -2.25351184E+01;
    COFD[2073] = 5.32200412E+00;
    COFD[2074] = -4.29810296E-01;
    COFD[2075] = 1.67968656E-02;
    COFD[2076] = -2.25457424E+01;
    COFD[2077] = 5.31236460E+00;
    COFD[2078] = -4.28177190E-01;
    COFD[2079] = 1.67109235E-02;
    COFD[2080] = -2.24546824E+01;
    COFD[2081] = 5.23807779E+00;
    COFD[2082] = -4.15451615E-01;
    COFD[2083] = 1.60367849E-02;
    COFD[2084] = -2.22774184E+01;
    COFD[2085] = 5.07362417E+00;
    COFD[2086] = -3.88094414E-01;
    COFD[2087] = 1.46129219E-02;
    COFD[2088] = -2.21555553E+01;
    COFD[2089] = 5.00070625E+00;
    COFD[2090] = -3.76355283E-01;
    COFD[2091] = 1.40141622E-02;
    COFD[2092] = -2.20003467E+01;
    COFD[2093] = 4.90271383E+00;
    COFD[2094] = -3.60896715E-01;
    COFD[2095] = 1.32352181E-02;
    COFD[2096] = -1.76002031E+01;
    COFD[2097] = 4.19171952E+00;
    COFD[2098] = -3.31354810E-01;
    COFD[2099] = 1.44520623E-02;
    COFD[2100] = -2.08912827E+01;
    COFD[2101] = 4.35422933E+00;
    COFD[2102] = -2.74727512E-01;
    COFD[2103] = 8.91328380E-03;
    COFD[2104] = -1.82251914E+01;
    COFD[2105] = 5.05237312E+00;
    COFD[2106] = -4.35182396E-01;
    COFD[2107] = 1.86363074E-02;
    COFD[2108] = -1.74792112E+01;
    COFD[2109] = 4.29676909E+00;
    COFD[2110] = -3.44085306E-01;
    COFD[2111] = 1.49671135E-02;
    COFD[2112] = -1.74984476E+01;
    COFD[2113] = 4.29676909E+00;
    COFD[2114] = -3.44085306E-01;
    COFD[2115] = 1.49671135E-02;
    COFD[2116] = -1.89616623E+01;
    COFD[2117] = 4.68595732E+00;
    COFD[2118] = -3.91842840E-01;
    COFD[2119] = 1.69262542E-02;
    COFD[2120] = -1.42229194E+01;
    COFD[2121] = 3.38669384E+00;
    COFD[2122] = -2.28784122E-01;
    COFD[2123] = 1.00790953E-02;
    COFD[2124] = -2.08812333E+01;
    COFD[2125] = 5.08859217E+00;
    COFD[2126] = -3.90525428E-01;
    COFD[2127] = 1.47376395E-02;
    COFD[2128] = -1.89685165E+01;
    COFD[2129] = 4.68595732E+00;
    COFD[2130] = -3.91842840E-01;
    COFD[2131] = 1.69262542E-02;
    COFD[2132] = -1.89544778E+01;
    COFD[2133] = 4.68595732E+00;
    COFD[2134] = -3.91842840E-01;
    COFD[2135] = 1.69262542E-02;
    COFD[2136] = -1.98646734E+01;
    COFD[2137] = 5.04367502E+00;
    COFD[2138] = -4.34153325E-01;
    COFD[2139] = 1.85956055E-02;
    COFD[2140] = -1.98075055E+01;
    COFD[2141] = 5.02169524E+00;
    COFD[2142] = -4.31582804E-01;
    COFD[2143] = 1.84953568E-02;
    COFD[2144] = -2.16379567E+01;
    COFD[2145] = 5.29019717E+00;
    COFD[2146] = -4.24502606E-01;
    COFD[2147] = 1.65197343E-02;
    COFD[2148] = -1.86157761E+01;
    COFD[2149] = 4.55689508E+00;
    COFD[2150] = -3.75937921E-01;
    COFD[2151] = 1.62703488E-02;
    COFD[2152] = -2.16802612E+01;
    COFD[2153] = 5.52918296E+00;
    COFD[2154] = -4.85360709E-01;
    COFD[2155] = 2.03448006E-02;
    COFD[2156] = -2.12121370E+01;
    COFD[2157] = 5.39823225E+00;
    COFD[2158] = -4.72294645E-01;
    COFD[2159] = 1.99340225E-02;
    COFD[2160] = -2.19327397E+01;
    COFD[2161] = 5.60638188E+00;
    COFD[2162] = -4.91272522E-01;
    COFD[2163] = 2.04396264E-02;
    COFD[2164] = -2.18273547E+01;
    COFD[2165] = 5.55753905E+00;
    COFD[2166] = -4.88136714E-01;
    COFD[2167] = 2.04294957E-02;
    COFD[2168] = -2.20500806E+01;
    COFD[2169] = 5.44448440E+00;
    COFD[2170] = -4.51529024E-01;
    COFD[2171] = 1.79698119E-02;
    COFD[2172] = -2.21885140E+01;
    COFD[2173] = 5.59472344E+00;
    COFD[2174] = -4.91421518E-01;
    COFD[2175] = 2.05117088E-02;
    COFD[2176] = -2.21933982E+01;
    COFD[2177] = 5.59472344E+00;
    COFD[2178] = -4.91421518E-01;
    COFD[2179] = 2.05117088E-02;
    COFD[2180] = -2.25170474E+01;
    COFD[2181] = 5.58249828E+00;
    COFD[2182] = -4.78873376E-01;
    COFD[2183] = 1.95316774E-02;
    COFD[2184] = -2.25141375E+01;
    COFD[2185] = 5.58249828E+00;
    COFD[2186] = -4.78873376E-01;
    COFD[2187] = 1.95316774E-02;
    COFD[2188] = -2.25171771E+01;
    COFD[2189] = 5.58249828E+00;
    COFD[2190] = -4.78873376E-01;
    COFD[2191] = 1.95316774E-02;
    COFD[2192] = -2.24096921E+01;
    COFD[2193] = 5.41604244E+00;
    COFD[2194] = -4.46391067E-01;
    COFD[2195] = 1.76894436E-02;
    COFD[2196] = -2.24117709E+01;
    COFD[2197] = 5.41604244E+00;
    COFD[2198] = -4.46391067E-01;
    COFD[2199] = 1.76894436E-02;
    COFD[2200] = -2.22712221E+01;
    COFD[2201] = 5.27184216E+00;
    COFD[2202] = -4.21373345E-01;
    COFD[2203] = 1.63542609E-02;
    COFD[2204] = -2.21166784E+01;
    COFD[2205] = 5.13519978E+00;
    COFD[2206] = -3.98132825E-01;
    COFD[2207] = 1.51293092E-02;
    COFD[2208] = -2.18596594E+01;
    COFD[2209] = 4.96464859E+00;
    COFD[2210] = -3.70657753E-01;
    COFD[2211] = 1.37267014E-02;
    COFD[2212] = -2.15864565E+01;
    COFD[2213] = 4.79387853E+00;
    COFD[2214] = -3.43720861E-01;
    COFD[2215] = 1.23700578E-02;
    COFD[2216] = -2.15876146E+01;
    COFD[2217] = 4.77996453E+00;
    COFD[2218] = -3.41483820E-01;
    COFD[2219] = 1.22561537E-02;
    COFD[2220] = -2.13890574E+01;
    COFD[2221] = 4.65974918E+00;
    COFD[2222] = -3.22390767E-01;
    COFD[2223] = 1.12918650E-02;
    COFD[2224] = -2.10053295E+01;
    COFD[2225] = 4.40732366E+00;
    COFD[2226] = -2.82889322E-01;
    COFD[2227] = 9.31663975E-03;
    COFD[2228] = -2.08019969E+01;
    COFD[2229] = 4.29990973E+00;
    COFD[2230] = -2.66411377E-01;
    COFD[2231] = 8.50361424E-03;
    COFD[2232] = -2.05654443E+01;
    COFD[2233] = 4.16804380E+00;
    COFD[2234] = -2.46372859E-01;
    COFD[2235] = 7.52105030E-03;
    COFD[2236] = -1.85864144E+01;
    COFD[2237] = 4.54915847E+00;
    COFD[2238] = -3.75000738E-01;
    COFD[2239] = 1.62324821E-02;
    COFD[2240] = -2.15101033E+01;
    COFD[2241] = 4.62311744E+00;
    COFD[2242] = -3.16576672E-01;
    COFD[2243] = 1.09982058E-02;
    COFD[2244] = -1.79344949E+01;
    COFD[2245] = 4.91373893E+00;
    COFD[2246] = -4.18747629E-01;
    COFD[2247] = 1.79856610E-02;
    COFD[2248] = -1.72556499E+01;
    COFD[2249] = 4.17889917E+00;
    COFD[2250] = -3.29752510E-01;
    COFD[2251] = 1.43850275E-02;
    COFD[2252] = -1.72753760E+01;
    COFD[2253] = 4.17889917E+00;
    COFD[2254] = -3.29752510E-01;
    COFD[2255] = 1.43850275E-02;
    COFD[2256] = -1.86499071E+01;
    COFD[2257] = 4.53572533E+00;
    COFD[2258] = -3.73386925E-01;
    COFD[2259] = 1.61678881E-02;
    COFD[2260] = -1.39924781E+01;
    COFD[2261] = 3.26384506E+00;
    COFD[2262] = -2.12947087E-01;
    COFD[2263] = 9.39743888E-03;
    COFD[2264] = -2.12661865E+01;
    COFD[2265] = 5.24930667E+00;
    COFD[2266] = -4.17435088E-01;
    COFD[2267] = 1.61434424E-02;
    COFD[2268] = -1.86570209E+01;
    COFD[2269] = 4.53572533E+00;
    COFD[2270] = -3.73386925E-01;
    COFD[2271] = 1.61678881E-02;
    COFD[2272] = -1.86424545E+01;
    COFD[2273] = 4.53572533E+00;
    COFD[2274] = -3.73386925E-01;
    COFD[2275] = 1.61678881E-02;
    COFD[2276] = -1.95552142E+01;
    COFD[2277] = 4.90255048E+00;
    COFD[2278] = -4.17368501E-01;
    COFD[2279] = 1.79287358E-02;
    COFD[2280] = -1.94823660E+01;
    COFD[2281] = 4.87333294E+00;
    COFD[2282] = -4.13769241E-01;
    COFD[2283] = 1.77802244E-02;
    COFD[2284] = -2.19399793E+01;
    COFD[2285] = 5.41841631E+00;
    COFD[2286] = -4.46818971E-01;
    COFD[2287] = 1.77127652E-02;
    COFD[2288] = -1.83538377E+01;
    COFD[2289] = 4.42828044E+00;
    COFD[2290] = -3.60417833E-01;
    COFD[2291] = 1.56455103E-02;
    COFD[2292] = -2.14326461E+01;
    COFD[2293] = 5.41729961E+00;
    COFD[2294] = -4.73400281E-01;
    COFD[2295] = 1.99269567E-02;
    COFD[2296] = -2.10944088E+01;
    COFD[2297] = 5.34286099E+00;
    COFD[2298] = -4.68100992E-01;
    COFD[2299] = 1.98731399E-02;
    COFD[2300] = -2.18273547E+01;
    COFD[2301] = 5.55753905E+00;
    COFD[2302] = -4.88136714E-01;
    COFD[2303] = 2.04294957E-02;
    COFD[2304] = -2.15746136E+01;
    COFD[2305] = 5.44803850E+00;
    COFD[2306] = -4.76610560E-01;
    COFD[2307] = 2.00355294E-02;
    COFD[2308] = -2.22208695E+01;
    COFD[2309] = 5.51722375E+00;
    COFD[2310] = -4.66081431E-01;
    COFD[2311] = 1.88044011E-02;
    COFD[2312] = -2.19982918E+01;
    COFD[2313] = 5.51276597E+00;
    COFD[2314] = -4.83701824E-01;
    COFD[2315] = 2.02915297E-02;
    COFD[2316] = -2.20033797E+01;
    COFD[2317] = 5.51276597E+00;
    COFD[2318] = -4.83701824E-01;
    COFD[2319] = 2.02915297E-02;
    COFD[2320] = -2.25118159E+01;
    COFD[2321] = 5.58206320E+00;
    COFD[2322] = -4.82956809E-01;
    COFD[2323] = 1.98731634E-02;
    COFD[2324] = -2.25087703E+01;
    COFD[2325] = 5.58206320E+00;
    COFD[2326] = -4.82956809E-01;
    COFD[2327] = 1.98731634E-02;
    COFD[2328] = -2.25119517E+01;
    COFD[2329] = 5.58206320E+00;
    COFD[2330] = -4.82956809E-01;
    COFD[2331] = 1.98731634E-02;
    COFD[2332] = -2.26047186E+01;
    COFD[2333] = 5.50251732E+00;
    COFD[2334] = -4.62858785E-01;
    COFD[2335] = 1.86120573E-02;
    COFD[2336] = -2.26069017E+01;
    COFD[2337] = 5.50251732E+00;
    COFD[2338] = -4.62858785E-01;
    COFD[2339] = 1.86120573E-02;
    COFD[2340] = -2.25784319E+01;
    COFD[2341] = 5.40653540E+00;
    COFD[2342] = -4.44612351E-01;
    COFD[2343] = 1.75906515E-02;
    COFD[2344] = -2.24451970E+01;
    COFD[2345] = 5.27966625E+00;
    COFD[2346] = -4.22746172E-01;
    COFD[2347] = 1.64279811E-02;
    COFD[2348] = -2.23212622E+01;
    COFD[2349] = 5.16629594E+00;
    COFD[2350] = -4.03322392E-01;
    COFD[2351] = 1.53998221E-02;
    COFD[2352] = -2.21433231E+01;
    COFD[2353] = 5.03586935E+00;
    COFD[2354] = -3.81976071E-01;
    COFD[2355] = 1.42995069E-02;
    COFD[2356] = -2.21429983E+01;
    COFD[2357] = 5.02172689E+00;
    COFD[2358] = -3.79713172E-01;
    COFD[2359] = 1.41845745E-02;
    COFD[2360] = -2.19256567E+01;
    COFD[2361] = 4.89350049E+00;
    COFD[2362] = -3.59442064E-01;
    COFD[2363] = 1.31619214E-02;
    COFD[2364] = -2.15962984E+01;
    COFD[2365] = 4.66482266E+00;
    COFD[2366] = -3.23189752E-01;
    COFD[2367] = 1.13320181E-02;
    COFD[2368] = -2.14611166E+01;
    COFD[2369] = 4.58545934E+00;
    COFD[2370] = -3.10556391E-01;
    COFD[2371] = 1.06931834E-02;
    COFD[2372] = -2.12918073E+01;
    COFD[2373] = 4.48188359E+00;
    COFD[2374] = -2.94414571E-01;
    COFD[2375] = 9.88838906E-03;
    COFD[2376] = -1.83249299E+01;
    COFD[2377] = 4.42045763E+00;
    COFD[2378] = -3.59451578E-01;
    COFD[2379] = 1.56056164E-02;
    COFD[2380] = -1.78379251E+01;
    COFD[2381] = 2.81667592E+00;
    COFD[2382] = -4.44112417E-02;
    COFD[2383] = -2.24836515E-03;
    COFD[2384] = -1.94691430E+01;
    COFD[2385] = 5.43830787E+00;
    COFD[2386] = -4.75472880E-01;
    COFD[2387] = 1.99909996E-02;
    COFD[2388] = -1.90915649E+01;
    COFD[2389] = 4.84384483E+00;
    COFD[2390] = -4.10265575E-01;
    COFD[2391] = 1.76414287E-02;
    COFD[2392] = -1.91136491E+01;
    COFD[2393] = 4.84384483E+00;
    COFD[2394] = -4.10265575E-01;
    COFD[2395] = 1.76414287E-02;
    COFD[2396] = -2.05324091E+01;
    COFD[2397] = 5.18417470E+00;
    COFD[2398] = -4.49491573E-01;
    COFD[2399] = 1.91438508E-02;
    COFD[2400] = -1.57040212E+01;
    COFD[2401] = 3.93614244E+00;
    COFD[2402] = -2.99111497E-01;
    COFD[2403] = 1.30888229E-02;
    COFD[2404] = -1.87419199E+01;
    COFD[2405] = 3.96926341E+00;
    COFD[2406] = -2.16412264E-01;
    COFD[2407] = 6.06012078E-03;
    COFD[2408] = -2.05408665E+01;
    COFD[2409] = 5.18417470E+00;
    COFD[2410] = -4.49491573E-01;
    COFD[2411] = 1.91438508E-02;
    COFD[2412] = -2.05235731E+01;
    COFD[2413] = 5.18417470E+00;
    COFD[2414] = -4.49491573E-01;
    COFD[2415] = 1.91438508E-02;
    COFD[2416] = -2.11637902E+01;
    COFD[2417] = 5.42846112E+00;
    COFD[2418] = -4.74321870E-01;
    COFD[2419] = 1.99459749E-02;
    COFD[2420] = -2.11341653E+01;
    COFD[2421] = 5.41773516E+00;
    COFD[2422] = -4.73414338E-01;
    COFD[2423] = 1.99258685E-02;
    COFD[2424] = -2.01064363E+01;
    COFD[2425] = 4.41511629E+00;
    COFD[2426] = -2.84086963E-01;
    COFD[2427] = 9.37586971E-03;
    COFD[2428] = -2.02969740E+01;
    COFD[2429] = 5.11106992E+00;
    COFD[2430] = -4.42047129E-01;
    COFD[2431] = 1.89042990E-02;
    COFD[2432] = -2.22176950E+01;
    COFD[2433] = 5.54251230E+00;
    COFD[2434] = -4.70946314E-01;
    COFD[2435] = 1.90785869E-02;
    COFD[2436] = -2.21110290E+01;
    COFD[2437] = 5.58360799E+00;
    COFD[2438] = -4.82701436E-01;
    COFD[2439] = 1.98437922E-02;
    COFD[2440] = -2.20500806E+01;
    COFD[2441] = 5.44448440E+00;
    COFD[2442] = -4.51529024E-01;
    COFD[2443] = 1.79698119E-02;
    COFD[2444] = -2.22208695E+01;
    COFD[2445] = 5.51722375E+00;
    COFD[2446] = -4.66081431E-01;
    COFD[2447] = 1.88044011E-02;
    COFD[2448] = -2.09121217E+01;
    COFD[2449] = 4.72895031E+00;
    COFD[2450] = -3.33332771E-01;
    COFD[2451] = 1.18431478E-02;
    COFD[2452] = -2.24612720E+01;
    COFD[2453] = 5.49330641E+00;
    COFD[2454] = -4.60498247E-01;
    COFD[2455] = 1.84639199E-02;
    COFD[2456] = -2.24674385E+01;
    COFD[2457] = 5.49330641E+00;
    COFD[2458] = -4.60498247E-01;
    COFD[2459] = 1.84639199E-02;
    COFD[2460] = -2.20420845E+01;
    COFD[2461] = 5.14570932E+00;
    COFD[2462] = -3.99877142E-01;
    COFD[2463] = 1.52199557E-02;
    COFD[2464] = -2.20382982E+01;
    COFD[2465] = 5.14570932E+00;
    COFD[2466] = -3.99877142E-01;
    COFD[2467] = 1.52199557E-02;
    COFD[2468] = -2.20422534E+01;
    COFD[2469] = 5.14570932E+00;
    COFD[2470] = -3.99877142E-01;
    COFD[2471] = 1.52199557E-02;
    COFD[2472] = -2.12139017E+01;
    COFD[2473] = 4.66550450E+00;
    COFD[2474] = -3.23297137E-01;
    COFD[2475] = 1.13374150E-02;
    COFD[2476] = -2.12166669E+01;
    COFD[2477] = 4.66550450E+00;
    COFD[2478] = -3.23297137E-01;
    COFD[2479] = 1.13374150E-02;
    COFD[2480] = -2.07454296E+01;
    COFD[2481] = 4.37720767E+00;
    COFD[2482] = -2.78259024E-01;
    COFD[2483] = 9.08774918E-03;
    COFD[2484] = -2.01683880E+01;
    COFD[2485] = 4.06006798E+00;
    COFD[2486] = -2.30066357E-01;
    COFD[2487] = 6.72473245E-03;
    COFD[2488] = -1.97003081E+01;
    COFD[2489] = 3.79996103E+00;
    COFD[2490] = -1.90497694E-01;
    COFD[2491] = 4.78603172E-03;
    COFD[2492] = -1.92768438E+01;
    COFD[2493] = 3.56517881E+00;
    COFD[2494] = -1.54966016E-01;
    COFD[2495] = 3.05232702E-03;
    COFD[2496] = -1.92450150E+01;
    COFD[2497] = 3.53733698E+00;
    COFD[2498] = -1.50812570E-01;
    COFD[2499] = 2.85140727E-03;
    COFD[2500] = -1.88405709E+01;
    COFD[2501] = 3.33125761E+00;
    COFD[2502] = -1.20168824E-01;
    COFD[2503] = 1.37545815E-03;
    COFD[2504] = -1.80427153E+01;
    COFD[2505] = 2.90760528E+00;
    COFD[2506] = -5.77438179E-02;
    COFD[2507] = -1.61157463E-03;
    COFD[2508] = -1.76677062E+01;
    COFD[2509] = 2.72650149E+00;
    COFD[2510] = -3.11601195E-02;
    COFD[2511] = -2.88242823E-03;
    COFD[2512] = -1.73379146E+01;
    COFD[2513] = 2.55561949E+00;
    COFD[2514] = -5.90530207E-03;
    COFD[2515] = -4.09393782E-03;
    COFD[2516] = -2.02693653E+01;
    COFD[2517] = 5.10426133E+00;
    COFD[2518] = -4.41256919E-01;
    COFD[2519] = 1.88737290E-02;
    COFD[2520] = -2.15057026E+01;
    COFD[2521] = 4.49401210E+00;
    COFD[2522] = -2.96300228E-01;
    COFD[2523] = 9.98228772E-03;
    COFD[2524] = -1.83539686E+01;
    COFD[2525] = 4.98756925E+00;
    COFD[2526] = -4.27526072E-01;
    COFD[2527] = 1.83341755E-02;
    COFD[2528] = -1.76775033E+01;
    COFD[2529] = 4.24719726E+00;
    COFD[2530] = -3.38206061E-01;
    COFD[2531] = 1.47350654E-02;
    COFD[2532] = -1.76992976E+01;
    COFD[2533] = 4.24719726E+00;
    COFD[2534] = -3.38206061E-01;
    COFD[2535] = 1.47350654E-02;
    COFD[2536] = -1.91208314E+01;
    COFD[2537] = 4.61801405E+00;
    COFD[2538] = -3.83535652E-01;
    COFD[2539] = 1.65862513E-02;
    COFD[2540] = -1.43129712E+01;
    COFD[2541] = 3.31177824E+00;
    COFD[2542] = -2.18945280E-01;
    COFD[2543] = 9.64764419E-03;
    COFD[2544] = -2.13847439E+01;
    COFD[2545] = 5.17440955E+00;
    COFD[2546] = -4.04678430E-01;
    COFD[2547] = 1.54706350E-02;
    COFD[2548] = -1.91291147E+01;
    COFD[2549] = 4.61801405E+00;
    COFD[2550] = -3.83535652E-01;
    COFD[2551] = 1.65862513E-02;
    COFD[2552] = -1.91121742E+01;
    COFD[2553] = 4.61801405E+00;
    COFD[2554] = -3.83535652E-01;
    COFD[2555] = 1.65862513E-02;
    COFD[2556] = -1.99803490E+01;
    COFD[2557] = 4.97875278E+00;
    COFD[2558] = -4.26485475E-01;
    COFD[2559] = 1.82931933E-02;
    COFD[2560] = -1.99235839E+01;
    COFD[2561] = 4.95514826E+00;
    COFD[2562] = -4.23691395E-01;
    COFD[2563] = 1.81828318E-02;
    COFD[2564] = -2.20947902E+01;
    COFD[2565] = 5.36053938E+00;
    COFD[2566] = -4.36434519E-01;
    COFD[2567] = 1.71484255E-02;
    COFD[2568] = -1.87685041E+01;
    COFD[2569] = 4.49191492E+00;
    COFD[2570] = -3.68041771E-01;
    COFD[2571] = 1.59498676E-02;
    COFD[2572] = -2.18590741E+01;
    COFD[2573] = 5.47368915E+00;
    COFD[2574] = -4.79424291E-01;
    COFD[2575] = 2.01372920E-02;
    COFD[2576] = -2.14323189E+01;
    COFD[2577] = 5.37331605E+00;
    COFD[2578] = -4.70491203E-01;
    COFD[2579] = 1.99134666E-02;
    COFD[2580] = -2.21885140E+01;
    COFD[2581] = 5.59472344E+00;
    COFD[2582] = -4.91421518E-01;
    COFD[2583] = 2.05117088E-02;
    COFD[2584] = -2.19982918E+01;
    COFD[2585] = 5.51276597E+00;
    COFD[2586] = -4.83701824E-01;
    COFD[2587] = 2.02915297E-02;
    COFD[2588] = -2.24612720E+01;
    COFD[2589] = 5.49330641E+00;
    COFD[2590] = -4.60498247E-01;
    COFD[2591] = 1.84639199E-02;
    COFD[2592] = -2.23842815E+01;
    COFD[2593] = 5.56066804E+00;
    COFD[2594] = -4.88405706E-01;
    COFD[2595] = 2.04357330E-02;
    COFD[2596] = -2.23903059E+01;
    COFD[2597] = 5.56066804E+00;
    COFD[2598] = -4.88405706E-01;
    COFD[2599] = 2.04357330E-02;
    COFD[2600] = -2.28055321E+01;
    COFD[2601] = 5.58523510E+00;
    COFD[2602] = -4.81201481E-01;
    COFD[2603] = 1.97107111E-02;
    COFD[2604] = -2.28018456E+01;
    COFD[2605] = 5.58523510E+00;
    COFD[2606] = -4.81201481E-01;
    COFD[2607] = 1.97107111E-02;
    COFD[2608] = -2.28056965E+01;
    COFD[2609] = 5.58523510E+00;
    COFD[2610] = -4.81201481E-01;
    COFD[2611] = 1.97107111E-02;
    COFD[2612] = -2.28142732E+01;
    COFD[2613] = 5.46711245E+00;
    COFD[2614] = -4.55648352E-01;
    COFD[2615] = 1.81956544E-02;
    COFD[2616] = -2.28169587E+01;
    COFD[2617] = 5.46711245E+00;
    COFD[2618] = -4.55648352E-01;
    COFD[2619] = 1.81956544E-02;
    COFD[2620] = -2.27264088E+01;
    COFD[2621] = 5.34358207E+00;
    COFD[2622] = -4.33525531E-01;
    COFD[2623] = 1.69942087E-02;
    COFD[2624] = -2.25883305E+01;
    COFD[2625] = 5.21373152E+00;
    COFD[2626] = -4.11240905E-01;
    COFD[2627] = 1.58129026E-02;
    COFD[2628] = -2.23956532E+01;
    COFD[2629] = 5.07018801E+00;
    COFD[2630] = -3.87535402E-01;
    COFD[2631] = 1.45842225E-02;
    COFD[2632] = -2.21481022E+01;
    COFD[2633] = 4.91019065E+00;
    COFD[2634] = -3.62076012E-01;
    COFD[2635] = 1.32946182E-02;
    COFD[2636] = -2.21417190E+01;
    COFD[2637] = 4.89399517E+00;
    COFD[2638] = -3.59520327E-01;
    COFD[2639] = 1.31658681E-02;
    COFD[2640] = -2.19360376E+01;
    COFD[2641] = 4.77045497E+00;
    COFD[2642] = -3.39955491E-01;
    COFD[2643] = 1.21784372E-02;
    COFD[2644] = -2.16062611E+01;
    COFD[2645] = 4.54192945E+00;
    COFD[2646] = -3.03755204E-01;
    COFD[2647] = 1.03536977E-02;
    COFD[2648] = -2.14350691E+01;
    COFD[2649] = 4.44599921E+00;
    COFD[2650] = -2.88841525E-01;
    COFD[2651] = 9.61119142E-03;
    COFD[2652] = -2.12290071E+01;
    COFD[2653] = 4.32707381E+00;
    COFD[2654] = -2.70563830E-01;
    COFD[2655] = 8.70791456E-03;
    COFD[2656] = -1.87434358E+01;
    COFD[2657] = 4.48550694E+00;
    COFD[2658] = -3.67277454E-01;
    COFD[2659] = 1.59194755E-02;
    COFD[2660] = -2.15154466E+01;
    COFD[2661] = 4.49401210E+00;
    COFD[2662] = -2.96300228E-01;
    COFD[2663] = 9.98228772E-03;
    COFD[2664] = -1.83542556E+01;
    COFD[2665] = 4.98756925E+00;
    COFD[2666] = -4.27526072E-01;
    COFD[2667] = 1.83341755E-02;
    COFD[2668] = -1.76808721E+01;
    COFD[2669] = 4.24719726E+00;
    COFD[2670] = -3.38206061E-01;
    COFD[2671] = 1.47350654E-02;
    COFD[2672] = -1.77028170E+01;
    COFD[2673] = 4.24719726E+00;
    COFD[2674] = -3.38206061E-01;
    COFD[2675] = 1.47350654E-02;
    COFD[2676] = -1.91261963E+01;
    COFD[2677] = 4.61801405E+00;
    COFD[2678] = -3.83535652E-01;
    COFD[2679] = 1.65862513E-02;
    COFD[2680] = -1.43135319E+01;
    COFD[2681] = 3.31177824E+00;
    COFD[2682] = -2.18945280E-01;
    COFD[2683] = 9.64764419E-03;
    COFD[2684] = -2.13884087E+01;
    COFD[2685] = 5.17440955E+00;
    COFD[2686] = -4.04678430E-01;
    COFD[2687] = 1.54706350E-02;
    COFD[2688] = -1.91345696E+01;
    COFD[2689] = 4.61801405E+00;
    COFD[2690] = -3.83535652E-01;
    COFD[2691] = 1.65862513E-02;
    COFD[2692] = -1.91174465E+01;
    COFD[2693] = 4.61801405E+00;
    COFD[2694] = -3.83535652E-01;
    COFD[2695] = 1.65862513E-02;
    COFD[2696] = -1.99835686E+01;
    COFD[2697] = 4.97875278E+00;
    COFD[2698] = -4.26485475E-01;
    COFD[2699] = 1.82931933E-02;
    COFD[2700] = -1.99269592E+01;
    COFD[2701] = 4.95514826E+00;
    COFD[2702] = -4.23691395E-01;
    COFD[2703] = 1.81828318E-02;
    COFD[2704] = -2.20998738E+01;
    COFD[2705] = 5.36053938E+00;
    COFD[2706] = -4.36434519E-01;
    COFD[2707] = 1.71484255E-02;
    COFD[2708] = -1.87733838E+01;
    COFD[2709] = 4.49191492E+00;
    COFD[2710] = -3.68041771E-01;
    COFD[2711] = 1.59498676E-02;
    COFD[2712] = -2.18653077E+01;
    COFD[2713] = 5.47368915E+00;
    COFD[2714] = -4.79424291E-01;
    COFD[2715] = 2.01372920E-02;
    COFD[2716] = -2.14369874E+01;
    COFD[2717] = 5.37331605E+00;
    COFD[2718] = -4.70491203E-01;
    COFD[2719] = 1.99134666E-02;
    COFD[2720] = -2.21933982E+01;
    COFD[2721] = 5.59472344E+00;
    COFD[2722] = -4.91421518E-01;
    COFD[2723] = 2.05117088E-02;
    COFD[2724] = -2.20033797E+01;
    COFD[2725] = 5.51276597E+00;
    COFD[2726] = -4.83701824E-01;
    COFD[2727] = 2.02915297E-02;
    COFD[2728] = -2.24674385E+01;
    COFD[2729] = 5.49330641E+00;
    COFD[2730] = -4.60498247E-01;
    COFD[2731] = 1.84639199E-02;
    COFD[2732] = -2.23903059E+01;
    COFD[2733] = 5.56066804E+00;
    COFD[2734] = -4.88405706E-01;
    COFD[2735] = 2.04357330E-02;
    COFD[2736] = -2.23964038E+01;
    COFD[2737] = 5.56066804E+00;
    COFD[2738] = -4.88405706E-01;
    COFD[2739] = 2.04357330E-02;
    COFD[2740] = -2.28124927E+01;
    COFD[2741] = 5.58523510E+00;
    COFD[2742] = -4.81201481E-01;
    COFD[2743] = 1.97107111E-02;
    COFD[2744] = -2.28087547E+01;
    COFD[2745] = 5.58523510E+00;
    COFD[2746] = -4.81201481E-01;
    COFD[2747] = 1.97107111E-02;
    COFD[2748] = -2.28126594E+01;
    COFD[2749] = 5.58523510E+00;
    COFD[2750] = -4.81201481E-01;
    COFD[2751] = 1.97107111E-02;
    COFD[2752] = -2.28218429E+01;
    COFD[2753] = 5.46711245E+00;
    COFD[2754] = -4.55648352E-01;
    COFD[2755] = 1.81956544E-02;
    COFD[2756] = -2.28245694E+01;
    COFD[2757] = 5.46711245E+00;
    COFD[2758] = -4.55648352E-01;
    COFD[2759] = 1.81956544E-02;
    COFD[2760] = -2.27345228E+01;
    COFD[2761] = 5.34358207E+00;
    COFD[2762] = -4.33525531E-01;
    COFD[2763] = 1.69942087E-02;
    COFD[2764] = -2.25968468E+01;
    COFD[2765] = 5.21373152E+00;
    COFD[2766] = -4.11240905E-01;
    COFD[2767] = 1.58129026E-02;
    COFD[2768] = -2.24044984E+01;
    COFD[2769] = 5.07018801E+00;
    COFD[2770] = -3.87535402E-01;
    COFD[2771] = 1.45842225E-02;
    COFD[2772] = -2.21572213E+01;
    COFD[2773] = 4.91019065E+00;
    COFD[2774] = -3.62076012E-01;
    COFD[2775] = 1.32946182E-02;
    COFD[2776] = -2.21508561E+01;
    COFD[2777] = 4.89399517E+00;
    COFD[2778] = -3.59520327E-01;
    COFD[2779] = 1.31658681E-02;
    COFD[2780] = -2.19453884E+01;
    COFD[2781] = 4.77045497E+00;
    COFD[2782] = -3.39955491E-01;
    COFD[2783] = 1.21784372E-02;
    COFD[2784] = -2.16159823E+01;
    COFD[2785] = 4.54192945E+00;
    COFD[2786] = -3.03755204E-01;
    COFD[2787] = 1.03536977E-02;
    COFD[2788] = -2.14451165E+01;
    COFD[2789] = 4.44599921E+00;
    COFD[2790] = -2.88841525E-01;
    COFD[2791] = 9.61119142E-03;
    COFD[2792] = -2.12391752E+01;
    COFD[2793] = 4.32707381E+00;
    COFD[2794] = -2.70563830E-01;
    COFD[2795] = 8.70791456E-03;
    COFD[2796] = -1.87483158E+01;
    COFD[2797] = 4.48550694E+00;
    COFD[2798] = -3.67277454E-01;
    COFD[2799] = 1.59194755E-02;
    COFD[2800] = -1.98610372E+01;
    COFD[2801] = 3.61510430E+00;
    COFD[2802] = -1.62429647E-01;
    COFD[2803] = 3.41379096E-03;
    COFD[2804] = -1.94091087E+01;
    COFD[2805] = 5.32291505E+00;
    COFD[2806] = -4.65883522E-01;
    COFD[2807] = 1.97916109E-02;
    COFD[2808] = -1.87877985E+01;
    COFD[2809] = 4.61260432E+00;
    COFD[2810] = -3.82854484E-01;
    COFD[2811] = 1.65575163E-02;
    COFD[2812] = -1.88114012E+01;
    COFD[2813] = 4.61260432E+00;
    COFD[2814] = -3.82854484E-01;
    COFD[2815] = 1.65575163E-02;
    COFD[2816] = -2.02564783E+01;
    COFD[2817] = 4.97613338E+00;
    COFD[2818] = -4.26175206E-01;
    COFD[2819] = 1.82809270E-02;
    COFD[2820] = -1.52614053E+01;
    COFD[2821] = 3.64565939E+00;
    COFD[2822] = -2.61726871E-01;
    COFD[2823] = 1.14799244E-02;
    COFD[2824] = -2.03436890E+01;
    COFD[2825] = 4.57152878E+00;
    COFD[2826] = -3.08371263E-01;
    COFD[2827] = 1.05838559E-02;
    COFD[2828] = -2.02658925E+01;
    COFD[2829] = 4.97613338E+00;
    COFD[2830] = -4.26175206E-01;
    COFD[2831] = 1.82809270E-02;
    COFD[2832] = -2.02466616E+01;
    COFD[2833] = 4.97613338E+00;
    COFD[2834] = -4.26175206E-01;
    COFD[2835] = 1.82809270E-02;
    COFD[2836] = -2.10571712E+01;
    COFD[2837] = 5.31360223E+00;
    COFD[2838] = -4.64787000E-01;
    COFD[2839] = 1.97483720E-02;
    COFD[2840] = -2.09912124E+01;
    COFD[2841] = 5.28557747E+00;
    COFD[2842] = -4.61402384E-01;
    COFD[2843] = 1.96111546E-02;
    COFD[2844] = -2.13351281E+01;
    COFD[2845] = 4.87252053E+00;
    COFD[2846] = -3.56127804E-01;
    COFD[2847] = 1.29948788E-02;
    COFD[2848] = -1.98830936E+01;
    COFD[2849] = 4.84731557E+00;
    COFD[2850] = -4.10638352E-01;
    COFD[2851] = 1.76543886E-02;
    COFD[2852] = -2.25017046E+01;
    COFD[2853] = 5.59178974E+00;
    COFD[2854] = -4.85668031E-01;
    COFD[2855] = 2.00491907E-02;
    COFD[2856] = -2.22816111E+01;
    COFD[2857] = 5.59185582E+00;
    COFD[2858] = -4.91155812E-01;
    COFD[2859] = 2.05043018E-02;
    COFD[2860] = -2.25170474E+01;
    COFD[2861] = 5.58249828E+00;
    COFD[2862] = -4.78873376E-01;
    COFD[2863] = 1.95316774E-02;
    COFD[2864] = -2.25118159E+01;
    COFD[2865] = 5.58206320E+00;
    COFD[2866] = -4.82956809E-01;
    COFD[2867] = 1.98731634E-02;
    COFD[2868] = -2.20420845E+01;
    COFD[2869] = 5.14570932E+00;
    COFD[2870] = -3.99877142E-01;
    COFD[2871] = 1.52199557E-02;
    COFD[2872] = -2.28055321E+01;
    COFD[2873] = 5.58523510E+00;
    COFD[2874] = -4.81201481E-01;
    COFD[2875] = 1.97107111E-02;
    COFD[2876] = -2.28124927E+01;
    COFD[2877] = 5.58523510E+00;
    COFD[2878] = -4.81201481E-01;
    COFD[2879] = 1.97107111E-02;
    COFD[2880] = -2.27782156E+01;
    COFD[2881] = 5.40563818E+00;
    COFD[2882] = -4.44444322E-01;
    COFD[2883] = 1.75813146E-02;
    COFD[2884] = -2.27738592E+01;
    COFD[2885] = 5.40563818E+00;
    COFD[2886] = -4.44444322E-01;
    COFD[2887] = 1.75813146E-02;
    COFD[2888] = -2.27784100E+01;
    COFD[2889] = 5.40563818E+00;
    COFD[2890] = -4.44444322E-01;
    COFD[2891] = 1.75813146E-02;
    COFD[2892] = -2.23528188E+01;
    COFD[2893] = 5.09597242E+00;
    COFD[2894] = -3.91720233E-01;
    COFD[2895] = 1.47988554E-02;
    COFD[2896] = -2.23560472E+01;
    COFD[2897] = 5.09597242E+00;
    COFD[2898] = -3.91720233E-01;
    COFD[2899] = 1.47988554E-02;
    COFD[2900] = -2.19668416E+01;
    COFD[2901] = 4.84253971E+00;
    COFD[2902] = -3.51393460E-01;
    COFD[2903] = 1.27562727E-02;
    COFD[2904] = -2.16264963E+01;
    COFD[2905] = 4.62444880E+00;
    COFD[2906] = -3.16789032E-01;
    COFD[2907] = 1.10089656E-02;
    COFD[2908] = -2.12774461E+01;
    COFD[2909] = 4.41372939E+00;
    COFD[2910] = -2.83873783E-01;
    COFD[2911] = 9.36532569E-03;
    COFD[2912] = -2.08875213E+01;
    COFD[2913] = 4.19325533E+00;
    COFD[2914] = -2.50186251E-01;
    COFD[2915] = 7.70758812E-03;
    COFD[2916] = -2.08615666E+01;
    COFD[2917] = 4.16888533E+00;
    COFD[2918] = -2.46500133E-01;
    COFD[2919] = 7.52727544E-03;
    COFD[2920] = -2.04884490E+01;
    COFD[2921] = 3.97419032E+00;
    COFD[2922] = -2.17151244E-01;
    COFD[2923] = 6.09602105E-03;
    COFD[2924] = -2.00028628E+01;
    COFD[2925] = 3.68035914E+00;
    COFD[2926] = -1.72246313E-01;
    COFD[2927] = 3.89112485E-03;
    COFD[2928] = -1.97359681E+01;
    COFD[2929] = 3.54254584E+00;
    COFD[2930] = -1.51588685E-01;
    COFD[2931] = 2.88891088E-03;
    COFD[2932] = -1.94433415E+01;
    COFD[2933] = 3.38805125E+00;
    COFD[2934] = -1.28605694E-01;
    COFD[2935] = 1.78139928E-03;
    COFD[2936] = -1.98609095E+01;
    COFD[2937] = 4.84231878E+00;
    COFD[2938] = -4.10101001E-01;
    COFD[2939] = 1.76356687E-02;
    COFD[2940] = -1.98544963E+01;
    COFD[2941] = 3.61510430E+00;
    COFD[2942] = -1.62429647E-01;
    COFD[2943] = 3.41379096E-03;
    COFD[2944] = -1.94089542E+01;
    COFD[2945] = 5.32291505E+00;
    COFD[2946] = -4.65883522E-01;
    COFD[2947] = 1.97916109E-02;
    COFD[2948] = -1.87858595E+01;
    COFD[2949] = 4.61260432E+00;
    COFD[2950] = -3.82854484E-01;
    COFD[2951] = 1.65575163E-02;
    COFD[2952] = -1.88093686E+01;
    COFD[2953] = 4.61260432E+00;
    COFD[2954] = -3.82854484E-01;
    COFD[2955] = 1.65575163E-02;
    COFD[2956] = -2.02532461E+01;
    COFD[2957] = 4.97613338E+00;
    COFD[2958] = -4.26175206E-01;
    COFD[2959] = 1.82809270E-02;
    COFD[2960] = -1.52611017E+01;
    COFD[2961] = 3.64565939E+00;
    COFD[2962] = -2.61726871E-01;
    COFD[2963] = 1.14799244E-02;
    COFD[2964] = -2.03415655E+01;
    COFD[2965] = 4.57152878E+00;
    COFD[2966] = -3.08371263E-01;
    COFD[2967] = 1.05838559E-02;
    COFD[2968] = -2.02625990E+01;
    COFD[2969] = 4.97613338E+00;
    COFD[2970] = -4.26175206E-01;
    COFD[2971] = 1.82809270E-02;
    COFD[2972] = -2.02434920E+01;
    COFD[2973] = 4.97613338E+00;
    COFD[2974] = -4.26175206E-01;
    COFD[2975] = 1.82809270E-02;
    COFD[2976] = -2.10553242E+01;
    COFD[2977] = 5.31360223E+00;
    COFD[2978] = -4.64787000E-01;
    COFD[2979] = 1.97483720E-02;
    COFD[2980] = -2.09892693E+01;
    COFD[2981] = 5.28557747E+00;
    COFD[2982] = -4.61402384E-01;
    COFD[2983] = 1.96111546E-02;
    COFD[2984] = -2.13320853E+01;
    COFD[2985] = 4.87252053E+00;
    COFD[2986] = -3.56127804E-01;
    COFD[2987] = 1.29948788E-02;
    COFD[2988] = -1.98801866E+01;
    COFD[2989] = 4.84731557E+00;
    COFD[2990] = -4.10638352E-01;
    COFD[2991] = 1.76543886E-02;
    COFD[2992] = -2.24978710E+01;
    COFD[2993] = 5.59178974E+00;
    COFD[2994] = -4.85668031E-01;
    COFD[2995] = 2.00491907E-02;
    COFD[2996] = -2.22788436E+01;
    COFD[2997] = 5.59185582E+00;
    COFD[2998] = -4.91155812E-01;
    COFD[2999] = 2.05043018E-02;
    COFD[3000] = -2.25141375E+01;
    COFD[3001] = 5.58249828E+00;
    COFD[3002] = -4.78873376E-01;
    COFD[3003] = 1.95316774E-02;
    COFD[3004] = -2.25087703E+01;
    COFD[3005] = 5.58206320E+00;
    COFD[3006] = -4.82956809E-01;
    COFD[3007] = 1.98731634E-02;
    COFD[3008] = -2.20382982E+01;
    COFD[3009] = 5.14570932E+00;
    COFD[3010] = -3.99877142E-01;
    COFD[3011] = 1.52199557E-02;
    COFD[3012] = -2.28018456E+01;
    COFD[3013] = 5.58523510E+00;
    COFD[3014] = -4.81201481E-01;
    COFD[3015] = 1.97107111E-02;
    COFD[3016] = -2.28087547E+01;
    COFD[3017] = 5.58523510E+00;
    COFD[3018] = -4.81201481E-01;
    COFD[3019] = 1.97107111E-02;
    COFD[3020] = -2.27738592E+01;
    COFD[3021] = 5.40563818E+00;
    COFD[3022] = -4.44444322E-01;
    COFD[3023] = 1.75813146E-02;
    COFD[3024] = -2.27695405E+01;
    COFD[3025] = 5.40563818E+00;
    COFD[3026] = -4.44444322E-01;
    COFD[3027] = 1.75813146E-02;
    COFD[3028] = -2.27740520E+01;
    COFD[3029] = 5.40563818E+00;
    COFD[3030] = -4.44444322E-01;
    COFD[3031] = 1.75813146E-02;
    COFD[3032] = -2.23480100E+01;
    COFD[3033] = 5.09597242E+00;
    COFD[3034] = -3.91720233E-01;
    COFD[3035] = 1.47988554E-02;
    COFD[3036] = -2.23512075E+01;
    COFD[3037] = 5.09597242E+00;
    COFD[3038] = -3.91720233E-01;
    COFD[3039] = 1.47988554E-02;
    COFD[3040] = -2.19616169E+01;
    COFD[3041] = 4.84253971E+00;
    COFD[3042] = -3.51393460E-01;
    COFD[3043] = 1.27562727E-02;
    COFD[3044] = -2.16209569E+01;
    COFD[3045] = 4.62444880E+00;
    COFD[3046] = -3.16789032E-01;
    COFD[3047] = 1.10089656E-02;
    COFD[3048] = -2.12716446E+01;
    COFD[3049] = 4.41372939E+00;
    COFD[3050] = -2.83873783E-01;
    COFD[3051] = 9.36532569E-03;
    COFD[3052] = -2.08814980E+01;
    COFD[3053] = 4.19325533E+00;
    COFD[3054] = -2.50186251E-01;
    COFD[3055] = 7.70758812E-03;
    COFD[3056] = -2.08555287E+01;
    COFD[3057] = 4.16888533E+00;
    COFD[3058] = -2.46500133E-01;
    COFD[3059] = 7.52727544E-03;
    COFD[3060] = -2.04822359E+01;
    COFD[3061] = 3.97419032E+00;
    COFD[3062] = -2.17151244E-01;
    COFD[3063] = 6.09602105E-03;
    COFD[3064] = -1.99963411E+01;
    COFD[3065] = 3.68035914E+00;
    COFD[3066] = -1.72246313E-01;
    COFD[3067] = 3.89112485E-03;
    COFD[3068] = -1.97291698E+01;
    COFD[3069] = 3.54254584E+00;
    COFD[3070] = -1.51588685E-01;
    COFD[3071] = 2.88891088E-03;
    COFD[3072] = -1.94364398E+01;
    COFD[3073] = 3.38805125E+00;
    COFD[3074] = -1.28605694E-01;
    COFD[3075] = 1.78139928E-03;
    COFD[3076] = -1.98580023E+01;
    COFD[3077] = 4.84231878E+00;
    COFD[3078] = -4.10101001E-01;
    COFD[3079] = 1.76356687E-02;
    COFD[3080] = -1.98613298E+01;
    COFD[3081] = 3.61510430E+00;
    COFD[3082] = -1.62429647E-01;
    COFD[3083] = 3.41379096E-03;
    COFD[3084] = -1.94091156E+01;
    COFD[3085] = 5.32291505E+00;
    COFD[3086] = -4.65883522E-01;
    COFD[3087] = 1.97916109E-02;
    COFD[3088] = -1.87878849E+01;
    COFD[3089] = 4.61260432E+00;
    COFD[3090] = -3.82854484E-01;
    COFD[3091] = 1.65575163E-02;
    COFD[3092] = -1.88114917E+01;
    COFD[3093] = 4.61260432E+00;
    COFD[3094] = -3.82854484E-01;
    COFD[3095] = 1.65575163E-02;
    COFD[3096] = -2.02566224E+01;
    COFD[3097] = 4.97613338E+00;
    COFD[3098] = -4.26175206E-01;
    COFD[3099] = 1.82809270E-02;
    COFD[3100] = -1.52614188E+01;
    COFD[3101] = 3.64565939E+00;
    COFD[3102] = -2.61726871E-01;
    COFD[3103] = 1.14799244E-02;
    COFD[3104] = -2.03437836E+01;
    COFD[3105] = 4.57152878E+00;
    COFD[3106] = -3.08371263E-01;
    COFD[3107] = 1.05838559E-02;
    COFD[3108] = -2.02660394E+01;
    COFD[3109] = 4.97613338E+00;
    COFD[3110] = -4.26175206E-01;
    COFD[3111] = 1.82809270E-02;
    COFD[3112] = -2.02468029E+01;
    COFD[3113] = 4.97613338E+00;
    COFD[3114] = -4.26175206E-01;
    COFD[3115] = 1.82809270E-02;
    COFD[3116] = -2.10572534E+01;
    COFD[3117] = 5.31360223E+00;
    COFD[3118] = -4.64787000E-01;
    COFD[3119] = 1.97483720E-02;
    COFD[3120] = -2.09912990E+01;
    COFD[3121] = 5.28557747E+00;
    COFD[3122] = -4.61402384E-01;
    COFD[3123] = 1.96111546E-02;
    COFD[3124] = -2.13352637E+01;
    COFD[3125] = 4.87252053E+00;
    COFD[3126] = -3.56127804E-01;
    COFD[3127] = 1.29948788E-02;
    COFD[3128] = -1.98832231E+01;
    COFD[3129] = 4.84731557E+00;
    COFD[3130] = -4.10638352E-01;
    COFD[3131] = 1.76543886E-02;
    COFD[3132] = -2.25018756E+01;
    COFD[3133] = 5.59178974E+00;
    COFD[3134] = -4.85668031E-01;
    COFD[3135] = 2.00491907E-02;
    COFD[3136] = -2.22817344E+01;
    COFD[3137] = 5.59185582E+00;
    COFD[3138] = -4.91155812E-01;
    COFD[3139] = 2.05043018E-02;
    COFD[3140] = -2.25171771E+01;
    COFD[3141] = 5.58249828E+00;
    COFD[3142] = -4.78873376E-01;
    COFD[3143] = 1.95316774E-02;
    COFD[3144] = -2.25119517E+01;
    COFD[3145] = 5.58206320E+00;
    COFD[3146] = -4.82956809E-01;
    COFD[3147] = 1.98731634E-02;
    COFD[3148] = -2.20422534E+01;
    COFD[3149] = 5.14570932E+00;
    COFD[3150] = -3.99877142E-01;
    COFD[3151] = 1.52199557E-02;
    COFD[3152] = -2.28056965E+01;
    COFD[3153] = 5.58523510E+00;
    COFD[3154] = -4.81201481E-01;
    COFD[3155] = 1.97107111E-02;
    COFD[3156] = -2.28126594E+01;
    COFD[3157] = 5.58523510E+00;
    COFD[3158] = -4.81201481E-01;
    COFD[3159] = 1.97107111E-02;
    COFD[3160] = -2.27784100E+01;
    COFD[3161] = 5.40563818E+00;
    COFD[3162] = -4.44444322E-01;
    COFD[3163] = 1.75813146E-02;
    COFD[3164] = -2.27740520E+01;
    COFD[3165] = 5.40563818E+00;
    COFD[3166] = -4.44444322E-01;
    COFD[3167] = 1.75813146E-02;
    COFD[3168] = -2.27786045E+01;
    COFD[3169] = 5.40563818E+00;
    COFD[3170] = -4.44444322E-01;
    COFD[3171] = 1.75813146E-02;
    COFD[3172] = -2.23530335E+01;
    COFD[3173] = 5.09597242E+00;
    COFD[3174] = -3.91720233E-01;
    COFD[3175] = 1.47988554E-02;
    COFD[3176] = -2.23562633E+01;
    COFD[3177] = 5.09597242E+00;
    COFD[3178] = -3.91720233E-01;
    COFD[3179] = 1.47988554E-02;
    COFD[3180] = -2.19670750E+01;
    COFD[3181] = 4.84253971E+00;
    COFD[3182] = -3.51393460E-01;
    COFD[3183] = 1.27562727E-02;
    COFD[3184] = -2.16267439E+01;
    COFD[3185] = 4.62444880E+00;
    COFD[3186] = -3.16789032E-01;
    COFD[3187] = 1.10089656E-02;
    COFD[3188] = -2.12777055E+01;
    COFD[3189] = 4.41372939E+00;
    COFD[3190] = -2.83873783E-01;
    COFD[3191] = 9.36532569E-03;
    COFD[3192] = -2.08877906E+01;
    COFD[3193] = 4.19325533E+00;
    COFD[3194] = -2.50186251E-01;
    COFD[3195] = 7.70758812E-03;
    COFD[3196] = -2.08618366E+01;
    COFD[3197] = 4.16888533E+00;
    COFD[3198] = -2.46500133E-01;
    COFD[3199] = 7.52727544E-03;
    COFD[3200] = -2.04887269E+01;
    COFD[3201] = 3.97419032E+00;
    COFD[3202] = -2.17151244E-01;
    COFD[3203] = 6.09602105E-03;
    COFD[3204] = -2.00031545E+01;
    COFD[3205] = 3.68035914E+00;
    COFD[3206] = -1.72246313E-01;
    COFD[3207] = 3.89112485E-03;
    COFD[3208] = -1.97362723E+01;
    COFD[3209] = 3.54254584E+00;
    COFD[3210] = -1.51588685E-01;
    COFD[3211] = 2.88891088E-03;
    COFD[3212] = -1.94436504E+01;
    COFD[3213] = 3.38805125E+00;
    COFD[3214] = -1.28605694E-01;
    COFD[3215] = 1.78139928E-03;
    COFD[3216] = -1.98610390E+01;
    COFD[3217] = 4.84231878E+00;
    COFD[3218] = -4.10101001E-01;
    COFD[3219] = 1.76356687E-02;
    COFD[3220] = -1.79641972E+01;
    COFD[3221] = 2.68406243E+00;
    COFD[3222] = -2.49058117E-02;
    COFD[3223] = -3.18217796E-03;
    COFD[3224] = -1.99878729E+01;
    COFD[3225] = 5.47248098E+00;
    COFD[3226] = -4.79290468E-01;
    COFD[3227] = 2.01323903E-02;
    COFD[3228] = -1.96267288E+01;
    COFD[3229] = 4.88238340E+00;
    COFD[3230] = -4.14880316E-01;
    COFD[3231] = 1.78258928E-02;
    COFD[3232] = -1.96513910E+01;
    COFD[3233] = 4.88238340E+00;
    COFD[3234] = -4.14880316E-01;
    COFD[3235] = 1.78258928E-02;
    COFD[3236] = -2.10673266E+01;
    COFD[3237] = 5.22065615E+00;
    COFD[3238] = -4.53751850E-01;
    COFD[3239] = 1.93099506E-02;
    COFD[3240] = -1.61865953E+01;
    COFD[3241] = 3.97811734E+00;
    COFD[3242] = -3.04543772E-01;
    COFD[3243] = 1.33243270E-02;
    COFD[3244] = -1.90109377E+01;
    COFD[3245] = 3.88639599E+00;
    COFD[3246] = -2.03781856E-01;
    COFD[3247] = 5.44050761E-03;
    COFD[3248] = -2.10774569E+01;
    COFD[3249] = 5.22065615E+00;
    COFD[3250] = -4.53751850E-01;
    COFD[3251] = 1.93099506E-02;
    COFD[3252] = -2.10567785E+01;
    COFD[3253] = 5.22065615E+00;
    COFD[3254] = -4.53751850E-01;
    COFD[3255] = 1.93099506E-02;
    COFD[3256] = -2.16471888E+01;
    COFD[3257] = 5.46403569E+00;
    COFD[3258] = -4.78350420E-01;
    COFD[3259] = 2.00977393E-02;
    COFD[3260] = -2.15968629E+01;
    COFD[3261] = 5.44280136E+00;
    COFD[3262] = -4.75998855E-01;
    COFD[3263] = 2.00116091E-02;
    COFD[3264] = -2.03618484E+01;
    COFD[3265] = 4.33469236E+00;
    COFD[3266] = -2.71728538E-01;
    COFD[3267] = 8.76524142E-03;
    COFD[3268] = -2.07935867E+01;
    COFD[3269] = 5.13870075E+00;
    COFD[3270] = -4.45087556E-01;
    COFD[3271] = 1.90141513E-02;
    COFD[3272] = -2.26255668E+01;
    COFD[3273] = 5.52088531E+00;
    COFD[3274] = -4.66799303E-01;
    COFD[3275] = 1.88452220E-02;
    COFD[3276] = -2.25312008E+01;
    COFD[3277] = 5.58497731E+00;
    COFD[3278] = -4.81782064E-01;
    COFD[3279] = 1.97602508E-02;
    COFD[3280] = -2.24096921E+01;
    COFD[3281] = 5.41604244E+00;
    COFD[3282] = -4.46391067E-01;
    COFD[3283] = 1.76894436E-02;
    COFD[3284] = -2.26047186E+01;
    COFD[3285] = 5.50251732E+00;
    COFD[3286] = -4.62858785E-01;
    COFD[3287] = 1.86120573E-02;
    COFD[3288] = -2.12139017E+01;
    COFD[3289] = 4.66550450E+00;
    COFD[3290] = -3.23297137E-01;
    COFD[3291] = 1.13374150E-02;
    COFD[3292] = -2.28142732E+01;
    COFD[3293] = 5.46711245E+00;
    COFD[3294] = -4.55648352E-01;
    COFD[3295] = 1.81956544E-02;
    COFD[3296] = -2.28218429E+01;
    COFD[3297] = 5.46711245E+00;
    COFD[3298] = -4.55648352E-01;
    COFD[3299] = 1.81956544E-02;
    COFD[3300] = -2.23528188E+01;
    COFD[3301] = 5.09597242E+00;
    COFD[3302] = -3.91720233E-01;
    COFD[3303] = 1.47988554E-02;
    COFD[3304] = -2.23480100E+01;
    COFD[3305] = 5.09597242E+00;
    COFD[3306] = -3.91720233E-01;
    COFD[3307] = 1.47988554E-02;
    COFD[3308] = -2.23530335E+01;
    COFD[3309] = 5.09597242E+00;
    COFD[3310] = -3.91720233E-01;
    COFD[3311] = 1.47988554E-02;
    COFD[3312] = -2.15052219E+01;
    COFD[3313] = 4.60438096E+00;
    COFD[3314] = -3.13580143E-01;
    COFD[3315] = 1.08462412E-02;
    COFD[3316] = -2.15088278E+01;
    COFD[3317] = 4.60438096E+00;
    COFD[3318] = -3.13580143E-01;
    COFD[3319] = 1.08462412E-02;
    COFD[3320] = -2.09926115E+01;
    COFD[3321] = 4.29644275E+00;
    COFD[3322] = -2.65881077E-01;
    COFD[3323] = 8.47752275E-03;
    COFD[3324] = -2.03719033E+01;
    COFD[3325] = 3.96005658E+00;
    COFD[3326] = -2.15023838E-01;
    COFD[3327] = 5.99243668E-03;
    COFD[3328] = -1.99572105E+01;
    COFD[3329] = 3.72188482E+00;
    COFD[3330] = -1.78555760E-01;
    COFD[3331] = 4.19961806E-03;
    COFD[3332] = -1.94608357E+01;
    COFD[3333] = 3.45627503E+00;
    COFD[3334] = -1.38749652E-01;
    COFD[3335] = 2.26961182E-03;
    COFD[3336] = -1.94327481E+01;
    COFD[3337] = 3.43141909E+00;
    COFD[3338] = -1.35054012E-01;
    COFD[3339] = 2.09180741E-03;
    COFD[3340] = -1.90327838E+01;
    COFD[3341] = 3.22670299E+00;
    COFD[3342] = -1.04657166E-01;
    COFD[3343] = 6.29620572E-04;
    COFD[3344] = -1.81581209E+01;
    COFD[3345] = 2.77116239E+00;
    COFD[3346] = -3.77250246E-02;
    COFD[3347] = -2.56819995E-03;
    COFD[3348] = -1.78252926E+01;
    COFD[3349] = 2.60486558E+00;
    COFD[3350] = -1.32252668E-02;
    COFD[3351] = -3.74170980E-03;
    COFD[3352] = -1.75400793E+01;
    COFD[3353] = 2.45372496E+00;
    COFD[3354] = 9.16587873E-03;
    COFD[3355] = -4.81554146E-03;
    COFD[3356] = -2.07714550E+01;
    COFD[3357] = 5.13381106E+00;
    COFD[3358] = -4.44579059E-01;
    COFD[3359] = 1.89972323E-02;
    COFD[3360] = -1.79693351E+01;
    COFD[3361] = 2.68406243E+00;
    COFD[3362] = -2.49058117E-02;
    COFD[3363] = -3.18217796E-03;
    COFD[3364] = -1.99879761E+01;
    COFD[3365] = 5.47248098E+00;
    COFD[3366] = -4.79290468E-01;
    COFD[3367] = 2.01323903E-02;
    COFD[3368] = -1.96280812E+01;
    COFD[3369] = 4.88238340E+00;
    COFD[3370] = -4.14880316E-01;
    COFD[3371] = 1.78258928E-02;
    COFD[3372] = -1.96528119E+01;
    COFD[3373] = 4.88238340E+00;
    COFD[3374] = -4.14880316E-01;
    COFD[3375] = 1.78258928E-02;
    COFD[3376] = -2.10696543E+01;
    COFD[3377] = 5.22065615E+00;
    COFD[3378] = -4.53751850E-01;
    COFD[3379] = 1.93099506E-02;
    COFD[3380] = -1.61867990E+01;
    COFD[3381] = 3.97811734E+00;
    COFD[3382] = -3.04543772E-01;
    COFD[3383] = 1.33243270E-02;
    COFD[3384] = -1.90124255E+01;
    COFD[3385] = 3.88639599E+00;
    COFD[3386] = -2.03781856E-01;
    COFD[3387] = 5.44050761E-03;
    COFD[3388] = -2.10798324E+01;
    COFD[3389] = 5.22065615E+00;
    COFD[3390] = -4.53751850E-01;
    COFD[3391] = 1.93099506E-02;
    COFD[3392] = -2.10590575E+01;
    COFD[3393] = 5.22065615E+00;
    COFD[3394] = -4.53751850E-01;
    COFD[3395] = 1.93099506E-02;
    COFD[3396] = -2.16484742E+01;
    COFD[3397] = 5.46403569E+00;
    COFD[3398] = -4.78350420E-01;
    COFD[3399] = 2.00977393E-02;
    COFD[3400] = -2.15982183E+01;
    COFD[3401] = 5.44280136E+00;
    COFD[3402] = -4.75998855E-01;
    COFD[3403] = 2.00116091E-02;
    COFD[3404] = -2.03640292E+01;
    COFD[3405] = 4.33469236E+00;
    COFD[3406] = -2.71728538E-01;
    COFD[3407] = 8.76524142E-03;
    COFD[3408] = -2.07956631E+01;
    COFD[3409] = 5.13870075E+00;
    COFD[3410] = -4.45087556E-01;
    COFD[3411] = 1.90141513E-02;
    COFD[3412] = -2.26283699E+01;
    COFD[3413] = 5.52088531E+00;
    COFD[3414] = -4.66799303E-01;
    COFD[3415] = 1.88452220E-02;
    COFD[3416] = -2.25331708E+01;
    COFD[3417] = 5.58497731E+00;
    COFD[3418] = -4.81782064E-01;
    COFD[3419] = 1.97602508E-02;
    COFD[3420] = -2.24117709E+01;
    COFD[3421] = 5.41604244E+00;
    COFD[3422] = -4.46391067E-01;
    COFD[3423] = 1.76894436E-02;
    COFD[3424] = -2.26069017E+01;
    COFD[3425] = 5.50251732E+00;
    COFD[3426] = -4.62858785E-01;
    COFD[3427] = 1.86120573E-02;
    COFD[3428] = -2.12166669E+01;
    COFD[3429] = 4.66550450E+00;
    COFD[3430] = -3.23297137E-01;
    COFD[3431] = 1.13374150E-02;
    COFD[3432] = -2.28169587E+01;
    COFD[3433] = 5.46711245E+00;
    COFD[3434] = -4.55648352E-01;
    COFD[3435] = 1.81956544E-02;
    COFD[3436] = -2.28245694E+01;
    COFD[3437] = 5.46711245E+00;
    COFD[3438] = -4.55648352E-01;
    COFD[3439] = 1.81956544E-02;
    COFD[3440] = -2.23560472E+01;
    COFD[3441] = 5.09597242E+00;
    COFD[3442] = -3.91720233E-01;
    COFD[3443] = 1.47988554E-02;
    COFD[3444] = -2.23512075E+01;
    COFD[3445] = 5.09597242E+00;
    COFD[3446] = -3.91720233E-01;
    COFD[3447] = 1.47988554E-02;
    COFD[3448] = -2.23562633E+01;
    COFD[3449] = 5.09597242E+00;
    COFD[3450] = -3.91720233E-01;
    COFD[3451] = 1.47988554E-02;
    COFD[3452] = -2.15088278E+01;
    COFD[3453] = 4.60438096E+00;
    COFD[3454] = -3.13580143E-01;
    COFD[3455] = 1.08462412E-02;
    COFD[3456] = -2.15124599E+01;
    COFD[3457] = 4.60438096E+00;
    COFD[3458] = -3.13580143E-01;
    COFD[3459] = 1.08462412E-02;
    COFD[3460] = -2.09965725E+01;
    COFD[3461] = 4.29644275E+00;
    COFD[3462] = -2.65881077E-01;
    COFD[3463] = 8.47752275E-03;
    COFD[3464] = -2.03761382E+01;
    COFD[3465] = 3.96005658E+00;
    COFD[3466] = -2.15023838E-01;
    COFD[3467] = 5.99243668E-03;
    COFD[3468] = -1.99616770E+01;
    COFD[3469] = 3.72188482E+00;
    COFD[3470] = -1.78555760E-01;
    COFD[3471] = 4.19961806E-03;
    COFD[3472] = -1.94655007E+01;
    COFD[3473] = 3.45627503E+00;
    COFD[3474] = -1.38749652E-01;
    COFD[3475] = 2.26961182E-03;
    COFD[3476] = -1.94374263E+01;
    COFD[3477] = 3.43141909E+00;
    COFD[3478] = -1.35054012E-01;
    COFD[3479] = 2.09180741E-03;
    COFD[3480] = -1.90376207E+01;
    COFD[3481] = 3.22670299E+00;
    COFD[3482] = -1.04657166E-01;
    COFD[3483] = 6.29620572E-04;
    COFD[3484] = -1.81632410E+01;
    COFD[3485] = 2.77116239E+00;
    COFD[3486] = -3.77250246E-02;
    COFD[3487] = -2.56819995E-03;
    COFD[3488] = -1.78306707E+01;
    COFD[3489] = 2.60486558E+00;
    COFD[3490] = -1.32252668E-02;
    COFD[3491] = -3.74170980E-03;
    COFD[3492] = -1.75455548E+01;
    COFD[3493] = 2.45372496E+00;
    COFD[3494] = 9.16587873E-03;
    COFD[3495] = -4.81554146E-03;
    COFD[3496] = -2.07735316E+01;
    COFD[3497] = 5.13381106E+00;
    COFD[3498] = -4.44579059E-01;
    COFD[3499] = 1.89972323E-02;
    COFD[3500] = -1.71023566E+01;
    COFD[3501] = 2.22992608E+00;
    COFD[3502] = 4.19679647E-02;
    COFD[3503] = -6.37519657E-03;
    COFD[3504] = -2.04125610E+01;
    COFD[3505] = 5.57739710E+00;
    COFD[3506] = -4.89778630E-01;
    COFD[3507] = 2.04637924E-02;
    COFD[3508] = -2.01678848E+01;
    COFD[3509] = 5.04165097E+00;
    COFD[3510] = -4.33913744E-01;
    COFD[3511] = 1.85861246E-02;
    COFD[3512] = -2.01934260E+01;
    COFD[3513] = 5.04165097E+00;
    COFD[3514] = -4.33913744E-01;
    COFD[3515] = 1.85861246E-02;
    COFD[3516] = -2.15776583E+01;
    COFD[3517] = 5.35995365E+00;
    COFD[3518] = -4.69587823E-01;
    COFD[3519] = 1.99080702E-02;
    COFD[3520] = -1.66550826E+01;
    COFD[3521] = 4.12489968E+00;
    COFD[3522] = -3.23119338E-01;
    COFD[3523] = 1.41122711E-02;
    COFD[3524] = -1.83523887E+01;
    COFD[3525] = 3.52256138E+00;
    COFD[3526] = -1.48611326E-01;
    COFD[3527] = 2.74505262E-03;
    COFD[3528] = -2.15884160E+01;
    COFD[3529] = 5.35995365E+00;
    COFD[3530] = -4.69587823E-01;
    COFD[3531] = 1.99080702E-02;
    COFD[3532] = -2.15664711E+01;
    COFD[3533] = 5.35995365E+00;
    COFD[3534] = -4.69587823E-01;
    COFD[3535] = 1.99080702E-02;
    COFD[3536] = -2.20820517E+01;
    COFD[3537] = 5.57296554E+00;
    COFD[3538] = -4.89394407E-01;
    COFD[3539] = 2.04545698E-02;
    COFD[3540] = -2.20571986E+01;
    COFD[3541] = 5.56221277E+00;
    COFD[3542] = -4.88535386E-01;
    COFD[3543] = 2.04385478E-02;
    COFD[3544] = -1.96505535E+01;
    COFD[3545] = 3.94748284E+00;
    COFD[3546] = -2.13105313E-01;
    COFD[3547] = 5.89820150E-03;
    COFD[3548] = -2.12462495E+01;
    COFD[3549] = 5.25612649E+00;
    COFD[3550] = -4.57885944E-01;
    COFD[3551] = 1.94706895E-02;
    COFD[3552] = -2.26428179E+01;
    COFD[3553] = 5.43854589E+00;
    COFD[3554] = -4.50460433E-01;
    COFD[3555] = 1.79115741E-02;
    COFD[3556] = -2.26391623E+01;
    COFD[3557] = 5.54906697E+00;
    COFD[3558] = -4.72198999E-01;
    COFD[3559] = 1.91488843E-02;
    COFD[3560] = -2.22712221E+01;
    COFD[3561] = 5.27184216E+00;
    COFD[3562] = -4.21373345E-01;
    COFD[3563] = 1.63542609E-02;
    COFD[3564] = -2.25784319E+01;
    COFD[3565] = 5.40653540E+00;
    COFD[3566] = -4.44612351E-01;
    COFD[3567] = 1.75906515E-02;
    COFD[3568] = -2.07454296E+01;
    COFD[3569] = 4.37720767E+00;
    COFD[3570] = -2.78259024E-01;
    COFD[3571] = 9.08774918E-03;
    COFD[3572] = -2.27264088E+01;
    COFD[3573] = 5.34358207E+00;
    COFD[3574] = -4.33525531E-01;
    COFD[3575] = 1.69942087E-02;
    COFD[3576] = -2.27345228E+01;
    COFD[3577] = 5.34358207E+00;
    COFD[3578] = -4.33525531E-01;
    COFD[3579] = 1.69942087E-02;
    COFD[3580] = -2.19668416E+01;
    COFD[3581] = 4.84253971E+00;
    COFD[3582] = -3.51393460E-01;
    COFD[3583] = 1.27562727E-02;
    COFD[3584] = -2.19616169E+01;
    COFD[3585] = 4.84253971E+00;
    COFD[3586] = -3.51393460E-01;
    COFD[3587] = 1.27562727E-02;
    COFD[3588] = -2.19670750E+01;
    COFD[3589] = 4.84253971E+00;
    COFD[3590] = -3.51393460E-01;
    COFD[3591] = 1.27562727E-02;
    COFD[3592] = -2.09926115E+01;
    COFD[3593] = 4.29644275E+00;
    COFD[3594] = -2.65881077E-01;
    COFD[3595] = 8.47752275E-03;
    COFD[3596] = -2.09965725E+01;
    COFD[3597] = 4.29644275E+00;
    COFD[3598] = -2.65881077E-01;
    COFD[3599] = 8.47752275E-03;
    COFD[3600] = -2.02931555E+01;
    COFD[3601] = 3.90905923E+00;
    COFD[3602] = -2.07240428E-01;
    COFD[3603] = 5.61016042E-03;
    COFD[3604] = -1.97882337E+01;
    COFD[3605] = 3.62109503E+00;
    COFD[3606] = -1.63328103E-01;
    COFD[3607] = 3.45738977E-03;
    COFD[3608] = -1.91738701E+01;
    COFD[3609] = 3.29927367E+00;
    COFD[3610] = -1.15421965E-01;
    COFD[3611] = 1.14722453E-03;
    COFD[3612] = -1.86224125E+01;
    COFD[3613] = 3.01293116E+00;
    COFD[3614] = -7.31811485E-02;
    COFD[3615] = -8.74503577E-04;
    COFD[3616] = -1.85739766E+01;
    COFD[3617] = 2.97960589E+00;
    COFD[3618] = -6.82975765E-02;
    COFD[3619] = -1.10754786E-03;
    COFD[3620] = -1.80238014E+01;
    COFD[3621] = 2.70981781E+00;
    COFD[3622] = -2.87009191E-02;
    COFD[3623] = -3.00031484E-03;
    COFD[3624] = -1.72678915E+01;
    COFD[3625] = 2.30401734E+00;
    COFD[3626] = 3.11443222E-02;
    COFD[3627] = -5.86207495E-03;
    COFD[3628] = -1.69703255E+01;
    COFD[3629] = 2.15337734E+00;
    COFD[3630] = 5.31165264E-02;
    COFD[3631] = -6.90242414E-03;
    COFD[3632] = -1.66106787E+01;
    COFD[3633] = 1.97256518E+00;
    COFD[3634] = 7.93547823E-02;
    COFD[3635] = -8.14046199E-03;
    COFD[3636] = -2.12156995E+01;
    COFD[3637] = 5.24766160E+00;
    COFD[3638] = -4.56882620E-01;
    COFD[3639] = 1.94309393E-02;
    COFD[3640] = -1.62450439E+01;
    COFD[3641] = 1.79898308E+00;
    COFD[3642] = 1.04422970E-01;
    COFD[3643] = -9.31788260E-03;
    COFD[3644] = -2.06420287E+01;
    COFD[3645] = 5.61233640E+00;
    COFD[3646] = -4.91245632E-01;
    COFD[3647] = 2.04073252E-02;
    COFD[3648] = -2.05574296E+01;
    COFD[3649] = 5.14795860E+00;
    COFD[3650] = -4.45935789E-01;
    COFD[3651] = 1.90364341E-02;
    COFD[3652] = -2.05835832E+01;
    COFD[3653] = 5.14795860E+00;
    COFD[3654] = -4.45935789E-01;
    COFD[3655] = 1.90364341E-02;
    COFD[3656] = -2.18432549E+01;
    COFD[3657] = 5.40768793E+00;
    COFD[3658] = -4.72942538E-01;
    COFD[3659] = 1.99389038E-02;
    COFD[3660] = -1.70329091E+01;
    COFD[3661] = 4.23969256E+00;
    COFD[3662] = -3.37288904E-01;
    COFD[3663] = 1.46975974E-02;
    COFD[3664] = -1.76500883E+01;
    COFD[3665] = 3.15831209E+00;
    COFD[3666] = -9.45393054E-02;
    COFD[3667] = 1.45090308E-04;
    COFD[3668] = -2.18544688E+01;
    COFD[3669] = 5.40768793E+00;
    COFD[3670] = -4.72942538E-01;
    COFD[3671] = 1.99389038E-02;
    COFD[3672] = -2.18316041E+01;
    COFD[3673] = 5.40768793E+00;
    COFD[3674] = -4.72942538E-01;
    COFD[3675] = 1.99389038E-02;
    COFD[3676] = -2.23196295E+01;
    COFD[3677] = 5.61199715E+00;
    COFD[3678] = -4.91433824E-01;
    COFD[3679] = 2.04245969E-02;
    COFD[3680] = -2.23089123E+01;
    COFD[3681] = 5.60657745E+00;
    COFD[3682] = -4.91280842E-01;
    COFD[3683] = 2.04393241E-02;
    COFD[3684] = -1.91553507E+01;
    COFD[3685] = 3.66719242E+00;
    COFD[3686] = -1.70260825E-01;
    COFD[3687] = 3.79442136E-03;
    COFD[3688] = -2.16479704E+01;
    COFD[3689] = 5.36415648E+00;
    COFD[3690] = -4.69836047E-01;
    COFD[3691] = 1.99067039E-02;
    COFD[3692] = -2.25594209E+01;
    COFD[3693] = 5.32923056E+00;
    COFD[3694] = -4.31065031E-01;
    COFD[3695] = 1.68637777E-02;
    COFD[3696] = -2.26655271E+01;
    COFD[3697] = 5.49155307E+00;
    COFD[3698] = -4.60156593E-01;
    COFD[3699] = 1.84445999E-02;
    COFD[3700] = -2.21166784E+01;
    COFD[3701] = 5.13519978E+00;
    COFD[3702] = -3.98132825E-01;
    COFD[3703] = 1.51293092E-02;
    COFD[3704] = -2.24451970E+01;
    COFD[3705] = 5.27966625E+00;
    COFD[3706] = -4.22746172E-01;
    COFD[3707] = 1.64279811E-02;
    COFD[3708] = -2.01683880E+01;
    COFD[3709] = 4.06006798E+00;
    COFD[3710] = -2.30066357E-01;
    COFD[3711] = 6.72473245E-03;
    COFD[3712] = -2.25883305E+01;
    COFD[3713] = 5.21373152E+00;
    COFD[3714] = -4.11240905E-01;
    COFD[3715] = 1.58129026E-02;
    COFD[3716] = -2.25968468E+01;
    COFD[3717] = 5.21373152E+00;
    COFD[3718] = -4.11240905E-01;
    COFD[3719] = 1.58129026E-02;
    COFD[3720] = -2.16264963E+01;
    COFD[3721] = 4.62444880E+00;
    COFD[3722] = -3.16789032E-01;
    COFD[3723] = 1.10089656E-02;
    COFD[3724] = -2.16209569E+01;
    COFD[3725] = 4.62444880E+00;
    COFD[3726] = -3.16789032E-01;
    COFD[3727] = 1.10089656E-02;
    COFD[3728] = -2.16267439E+01;
    COFD[3729] = 4.62444880E+00;
    COFD[3730] = -3.16789032E-01;
    COFD[3731] = 1.10089656E-02;
    COFD[3732] = -2.03719033E+01;
    COFD[3733] = 3.96005658E+00;
    COFD[3734] = -2.15023838E-01;
    COFD[3735] = 5.99243668E-03;
    COFD[3736] = -2.03761382E+01;
    COFD[3737] = 3.96005658E+00;
    COFD[3738] = -2.15023838E-01;
    COFD[3739] = 5.99243668E-03;
    COFD[3740] = -1.97882337E+01;
    COFD[3741] = 3.62109503E+00;
    COFD[3742] = -1.63328103E-01;
    COFD[3743] = 3.45738977E-03;
    COFD[3744] = -1.90788980E+01;
    COFD[3745] = 3.24795923E+00;
    COFD[3746] = -1.07810281E-01;
    COFD[3747] = 7.81209780E-04;
    COFD[3748] = -1.83659520E+01;
    COFD[3749] = 2.88571035E+00;
    COFD[3750] = -5.45325785E-02;
    COFD[3751] = -1.76508613E-03;
    COFD[3752] = -1.77237817E+01;
    COFD[3753] = 2.55843008E+00;
    COFD[3754] = -6.32267950E-03;
    COFD[3755] = -4.07387397E-03;
    COFD[3756] = -1.76962248E+01;
    COFD[3757] = 2.53412632E+00;
    COFD[3758] = -2.71561927E-03;
    COFD[3759] = -4.24696836E-03;
    COFD[3760] = -1.72790097E+01;
    COFD[3761] = 2.32181806E+00;
    COFD[3762] = 2.85392164E-02;
    COFD[3763] = -5.73831066E-03;
    COFD[3764] = -1.64316348E+01;
    COFD[3765] = 1.88148066E+00;
    COFD[3766] = 9.25151744E-02;
    COFD[3767] = -8.75875844E-03;
    COFD[3768] = -1.60744425E+01;
    COFD[3769] = 1.70515393E+00;
    COFD[3770] = 1.17964458E-01;
    COFD[3771] = -9.95487344E-03;
    COFD[3772] = -1.56831497E+01;
    COFD[3773] = 1.51210139E+00;
    COFD[3774] = 1.45705555E-01;
    COFD[3775] = -1.12527582E-02;
    COFD[3776] = -2.16312240E+01;
    COFD[3777] = 5.36160575E+00;
    COFD[3778] = -4.69678973E-01;
    COFD[3779] = 1.99069834E-02;
    COFD[3780] = -1.54004394E+01;
    COFD[3781] = 1.38485345E+00;
    COFD[3782] = 1.63989484E-01;
    COFD[3783] = -1.21088577E-02;
    COFD[3784] = -2.07167874E+01;
    COFD[3785] = 5.58797624E+00;
    COFD[3786] = -4.84623564E-01;
    COFD[3787] = 1.99817352E-02;
    COFD[3788] = -2.08840973E+01;
    COFD[3789] = 5.23394577E+00;
    COFD[3790] = -4.55294957E-01;
    COFD[3791] = 1.93696950E-02;
    COFD[3792] = -2.09107297E+01;
    COFD[3793] = 5.23394577E+00;
    COFD[3794] = -4.55294957E-01;
    COFD[3795] = 1.93696950E-02;
    COFD[3796] = -2.21426312E+01;
    COFD[3797] = 5.47842731E+00;
    COFD[3798] = -4.79952894E-01;
    COFD[3799] = 2.01568406E-02;
    COFD[3800] = -1.73560790E+01;
    COFD[3801] = 4.33584314E+00;
    COFD[3802] = -3.48938603E-01;
    COFD[3803] = 1.51686902E-02;
    COFD[3804] = -1.68531596E+01;
    COFD[3805] = 2.76314344E+00;
    COFD[3806] = -3.65464735E-02;
    COFD[3807] = -2.62462500E-03;
    COFD[3808] = -2.21542134E+01;
    COFD[3809] = 5.47842731E+00;
    COFD[3810] = -4.79952894E-01;
    COFD[3811] = 2.01568406E-02;
    COFD[3812] = -2.21306065E+01;
    COFD[3813] = 5.47842731E+00;
    COFD[3814] = -4.79952894E-01;
    COFD[3815] = 2.01568406E-02;
    COFD[3816] = -2.23961406E+01;
    COFD[3817] = 5.58969231E+00;
    COFD[3818] = -4.85146102E-01;
    COFD[3819] = 2.00165025E-02;
    COFD[3820] = -2.24147231E+01;
    COFD[3821] = 5.59714923E+00;
    COFD[3822] = -4.86899964E-01;
    COFD[3823] = 2.01242645E-02;
    COFD[3824] = -1.85405111E+01;
    COFD[3825] = 3.34746301E+00;
    COFD[3826] = -1.22575165E-01;
    COFD[3827] = 1.49120224E-03;
    COFD[3828] = -2.18745018E+01;
    COFD[3829] = 5.40581593E+00;
    COFD[3830] = -4.72804333E-01;
    COFD[3831] = 1.99370903E-02;
    COFD[3832] = -2.24455407E+01;
    COFD[3833] = 5.21716989E+00;
    COFD[3834] = -4.11825254E-01;
    COFD[3835] = 1.58436458E-02;
    COFD[3836] = -2.26097272E+01;
    COFD[3837] = 5.40751776E+00;
    COFD[3838] = -4.44796499E-01;
    COFD[3839] = 1.76008889E-02;
    COFD[3840] = -2.18596594E+01;
    COFD[3841] = 4.96464859E+00;
    COFD[3842] = -3.70657753E-01;
    COFD[3843] = 1.37267014E-02;
    COFD[3844] = -2.23212622E+01;
    COFD[3845] = 5.16629594E+00;
    COFD[3846] = -4.03322392E-01;
    COFD[3847] = 1.53998221E-02;
    COFD[3848] = -1.97003081E+01;
    COFD[3849] = 3.79996103E+00;
    COFD[3850] = -1.90497694E-01;
    COFD[3851] = 4.78603172E-03;
    COFD[3852] = -2.23956532E+01;
    COFD[3853] = 5.07018801E+00;
    COFD[3854] = -3.87535402E-01;
    COFD[3855] = 1.45842225E-02;
    COFD[3856] = -2.24044984E+01;
    COFD[3857] = 5.07018801E+00;
    COFD[3858] = -3.87535402E-01;
    COFD[3859] = 1.45842225E-02;
    COFD[3860] = -2.12774461E+01;
    COFD[3861] = 4.41372939E+00;
    COFD[3862] = -2.83873783E-01;
    COFD[3863] = 9.36532569E-03;
    COFD[3864] = -2.12716446E+01;
    COFD[3865] = 4.41372939E+00;
    COFD[3866] = -2.83873783E-01;
    COFD[3867] = 9.36532569E-03;
    COFD[3868] = -2.12777055E+01;
    COFD[3869] = 4.41372939E+00;
    COFD[3870] = -2.83873783E-01;
    COFD[3871] = 9.36532569E-03;
    COFD[3872] = -1.99572105E+01;
    COFD[3873] = 3.72188482E+00;
    COFD[3874] = -1.78555760E-01;
    COFD[3875] = 4.19961806E-03;
    COFD[3876] = -1.99616770E+01;
    COFD[3877] = 3.72188482E+00;
    COFD[3878] = -1.78555760E-01;
    COFD[3879] = 4.19961806E-03;
    COFD[3880] = -1.91738701E+01;
    COFD[3881] = 3.29927367E+00;
    COFD[3882] = -1.15421965E-01;
    COFD[3883] = 1.14722453E-03;
    COFD[3884] = -1.83659520E+01;
    COFD[3885] = 2.88571035E+00;
    COFD[3886] = -5.45325785E-02;
    COFD[3887] = -1.76508613E-03;
    COFD[3888] = -1.76386339E+01;
    COFD[3889] = 2.51509845E+00;
    COFD[3890] = 1.02670796E-04;
    COFD[3891] = -4.38201691E-03;
    COFD[3892] = -1.70867006E+01;
    COFD[3893] = 2.22837243E+00;
    COFD[3894] = 4.21945850E-02;
    COFD[3895] = -6.38593308E-03;
    COFD[3896] = -1.70480293E+01;
    COFD[3897] = 2.19974886E+00;
    COFD[3898] = 4.63679485E-02;
    COFD[3899] = -6.58350992E-03;
    COFD[3900] = -1.65620371E+01;
    COFD[3901] = 1.95984412E+00;
    COFD[3902] = 8.11931925E-02;
    COFD[3903] = -8.22685123E-03;
    COFD[3904] = -1.56095671E+01;
    COFD[3905] = 1.47685497E+00;
    COFD[3906] = 1.50778364E-01;
    COFD[3907] = -1.14906180E-02;
    COFD[3908] = -1.52407633E+01;
    COFD[3909] = 1.29532808E+00;
    COFD[3910] = 1.76825909E-01;
    COFD[3911] = -1.27084392E-02;
    COFD[3912] = -1.48245688E+01;
    COFD[3913] = 1.09196780E+00;
    COFD[3914] = 2.05935790E-01;
    COFD[3915] = -1.40664564E-02;
    COFD[3916] = -2.18579933E+01;
    COFD[3917] = 5.40322773E+00;
    COFD[3918] = -4.72630192E-01;
    COFD[3919] = 1.99360102E-02;
    COFD[3920] = -1.46746719E+01;
    COFD[3921] = 1.02870676E+00;
    COFD[3922] = 2.14984638E-01;
    COFD[3923] = -1.44879090E-02;
    COFD[3924] = -2.08358871E+01;
    COFD[3925] = 5.58518783E+00;
    COFD[3926] = -4.81282144E-01;
    COFD[3927] = 1.97176495E-02;
    COFD[3928] = -2.12416202E+01;
    COFD[3929] = 5.33751161E+00;
    COFD[3930] = -4.67531962E-01;
    COFD[3931] = 1.98534391E-02;
    COFD[3932] = -2.12686373E+01;
    COFD[3933] = 5.33751161E+00;
    COFD[3934] = -4.67531962E-01;
    COFD[3935] = 1.98534391E-02;
    COFD[3936] = -2.24428140E+01;
    COFD[3937] = 5.55319328E+00;
    COFD[3938] = -4.87759313E-01;
    COFD[3939] = 2.04205124E-02;
    COFD[3940] = -1.76841226E+01;
    COFD[3941] = 4.43623609E+00;
    COFD[3942] = -3.61391880E-01;
    COFD[3943] = 1.56853368E-02;
    COFD[3944] = -1.62777238E+01;
    COFD[3945] = 2.46665777E+00;
    COFD[3946] = 7.25684399E-03;
    COFD[3947] = -4.72424494E-03;
    COFD[3948] = -2.24546999E+01;
    COFD[3949] = 5.55319328E+00;
    COFD[3950] = -4.87759313E-01;
    COFD[3951] = 2.04205124E-02;
    COFD[3952] = -2.24304817E+01;
    COFD[3953] = 5.55319328E+00;
    COFD[3954] = -4.87759313E-01;
    COFD[3955] = 2.04205124E-02;
    COFD[3956] = -2.25070779E+01;
    COFD[3957] = 5.58488808E+00;
    COFD[3958] = -4.81505670E-01;
    COFD[3959] = 1.97377350E-02;
    COFD[3960] = -2.25078393E+01;
    COFD[3961] = 5.58461787E+00;
    COFD[3962] = -4.82178861E-01;
    COFD[3963] = 1.97950972E-02;
    COFD[3964] = -1.80138685E+01;
    COFD[3965] = 3.07345302E+00;
    COFD[3966] = -8.20536856E-02;
    COFD[3967] = -4.51137426E-04;
    COFD[3968] = -2.21149618E+01;
    COFD[3969] = 5.45837402E+00;
    COFD[3970] = -4.77716491E-01;
    COFD[3971] = 2.00742141E-02;
    COFD[3972] = -2.23014132E+01;
    COFD[3973] = 5.09843164E+00;
    COFD[3974] = -3.92118361E-01;
    COFD[3975] = 1.48192579E-02;
    COFD[3976] = -2.25351184E+01;
    COFD[3977] = 5.32200412E+00;
    COFD[3978] = -4.29810296E-01;
    COFD[3979] = 1.67968656E-02;
    COFD[3980] = -2.15864565E+01;
    COFD[3981] = 4.79387853E+00;
    COFD[3982] = -3.43720861E-01;
    COFD[3983] = 1.23700578E-02;
    COFD[3984] = -2.21433231E+01;
    COFD[3985] = 5.03586935E+00;
    COFD[3986] = -3.81976071E-01;
    COFD[3987] = 1.42995069E-02;
    COFD[3988] = -1.92768438E+01;
    COFD[3989] = 3.56517881E+00;
    COFD[3990] = -1.54966016E-01;
    COFD[3991] = 3.05232702E-03;
    COFD[3992] = -2.21481022E+01;
    COFD[3993] = 4.91019065E+00;
    COFD[3994] = -3.62076012E-01;
    COFD[3995] = 1.32946182E-02;
    COFD[3996] = -2.21572213E+01;
    COFD[3997] = 4.91019065E+00;
    COFD[3998] = -3.62076012E-01;
    COFD[3999] = 1.32946182E-02;
    COFD[4000] = -2.08875213E+01;
    COFD[4001] = 4.19325533E+00;
    COFD[4002] = -2.50186251E-01;
    COFD[4003] = 7.70758812E-03;
    COFD[4004] = -2.08814980E+01;
    COFD[4005] = 4.19325533E+00;
    COFD[4006] = -2.50186251E-01;
    COFD[4007] = 7.70758812E-03;
    COFD[4008] = -2.08877906E+01;
    COFD[4009] = 4.19325533E+00;
    COFD[4010] = -2.50186251E-01;
    COFD[4011] = 7.70758812E-03;
    COFD[4012] = -1.94608357E+01;
    COFD[4013] = 3.45627503E+00;
    COFD[4014] = -1.38749652E-01;
    COFD[4015] = 2.26961182E-03;
    COFD[4016] = -1.94655007E+01;
    COFD[4017] = 3.45627503E+00;
    COFD[4018] = -1.38749652E-01;
    COFD[4019] = 2.26961182E-03;
    COFD[4020] = -1.86224125E+01;
    COFD[4021] = 3.01293116E+00;
    COFD[4022] = -7.31811485E-02;
    COFD[4023] = -8.74503577E-04;
    COFD[4024] = -1.77237817E+01;
    COFD[4025] = 2.55843008E+00;
    COFD[4026] = -6.32267950E-03;
    COFD[4027] = -4.07387397E-03;
    COFD[4028] = -1.70867006E+01;
    COFD[4029] = 2.22837243E+00;
    COFD[4030] = 4.21945850E-02;
    COFD[4031] = -6.38593308E-03;
    COFD[4032] = -1.64603517E+01;
    COFD[4033] = 1.91201651E+00;
    COFD[4034] = 8.81041778E-02;
    COFD[4035] = -8.55154551E-03;
    COFD[4036] = -1.64162659E+01;
    COFD[4037] = 1.88142640E+00;
    COFD[4038] = 9.25230102E-02;
    COFD[4039] = -8.75912646E-03;
    COFD[4040] = -1.58768306E+01;
    COFD[4041] = 1.61945878E+00;
    COFD[4042] = 1.30286421E-01;
    COFD[4043] = -1.05317289E-02;
    COFD[4044] = -1.48753985E+01;
    COFD[4045] = 1.11690017E+00;
    COFD[4046] = 2.02362955E-01;
    COFD[4047] = -1.38996262E-02;
    COFD[4048] = -1.45087641E+01;
    COFD[4049] = 9.35690838E-01;
    COFD[4050] = 2.28295914E-01;
    COFD[4051] = -1.51088791E-02;
    COFD[4052] = -1.40894169E+01;
    COFD[4053] = 7.31458802E-01;
    COFD[4054] = 2.57430647E-01;
    COFD[4055] = -1.64638268E-02;
    COFD[4056] = -2.20902647E+01;
    COFD[4057] = 5.45237285E+00;
    COFD[4058] = -4.77075672E-01;
    COFD[4059] = 2.00518642E-02;
    COFD[4060] = -1.46203257E+01;
    COFD[4061] = 9.94330848E-01;
    COFD[4062] = 2.19902430E-01;
    COFD[4063] = -1.47171735E-02;
    COFD[4064] = -2.08746196E+01;
    COFD[4065] = 5.58521622E+00;
    COFD[4066] = -4.81014671E-01;
    COFD[4067] = 1.96952964E-02;
    COFD[4068] = -2.12927812E+01;
    COFD[4069] = 5.34414631E+00;
    COFD[4070] = -4.68238030E-01;
    COFD[4071] = 1.98779007E-02;
    COFD[4072] = -2.13198231E+01;
    COFD[4073] = 5.34414631E+00;
    COFD[4074] = -4.68238030E-01;
    COFD[4075] = 1.98779007E-02;
    COFD[4076] = -2.24883982E+01;
    COFD[4077] = 5.55793029E+00;
    COFD[4078] = -4.88170533E-01;
    COFD[4079] = 2.04302911E-02;
    COFD[4080] = -1.77370270E+01;
    COFD[4081] = 4.44459213E+00;
    COFD[4082] = -3.62399607E-01;
    COFD[4083] = 1.57258496E-02;
    COFD[4084] = -1.62527734E+01;
    COFD[4085] = 2.44062989E+00;
    COFD[4086] = 1.10981222E-02;
    COFD[4087] = -4.90788998E-03;
    COFD[4088] = -2.25003038E+01;
    COFD[4089] = 5.55793029E+00;
    COFD[4090] = -4.88170533E-01;
    COFD[4091] = 2.04302911E-02;
    COFD[4092] = -2.24760458E+01;
    COFD[4093] = 5.55793029E+00;
    COFD[4094] = -4.88170533E-01;
    COFD[4095] = 2.04302911E-02;
    COFD[4096] = -2.25413169E+01;
    COFD[4097] = 5.58519141E+00;
    COFD[4098] = -4.81277367E-01;
    COFD[4099] = 1.97172346E-02;
    COFD[4100] = -2.25423017E+01;
    COFD[4101] = 5.58490857E+00;
    COFD[4102] = -4.81943660E-01;
    COFD[4103] = 1.97740434E-02;
    COFD[4104] = -1.79714342E+01;
    COFD[4105] = 3.04109524E+00;
    COFD[4106] = -7.73097243E-02;
    COFD[4107] = -6.77522484E-04;
    COFD[4108] = -2.21693758E+01;
    COFD[4109] = 5.46715955E+00;
    COFD[4110] = -4.78699053E-01;
    COFD[4111] = 2.01106334E-02;
    COFD[4112] = -2.23041722E+01;
    COFD[4113] = 5.08480808E+00;
    COFD[4114] = -3.89912767E-01;
    COFD[4115] = 1.47062570E-02;
    COFD[4116] = -2.25457424E+01;
    COFD[4117] = 5.31236460E+00;
    COFD[4118] = -4.28177190E-01;
    COFD[4119] = 1.67109235E-02;
    COFD[4120] = -2.15876146E+01;
    COFD[4121] = 4.77996453E+00;
    COFD[4122] = -3.41483820E-01;
    COFD[4123] = 1.22561537E-02;
    COFD[4124] = -2.21429983E+01;
    COFD[4125] = 5.02172689E+00;
    COFD[4126] = -3.79713172E-01;
    COFD[4127] = 1.41845745E-02;
    COFD[4128] = -1.92450150E+01;
    COFD[4129] = 3.53733698E+00;
    COFD[4130] = -1.50812570E-01;
    COFD[4131] = 2.85140727E-03;
    COFD[4132] = -2.21417190E+01;
    COFD[4133] = 4.89399517E+00;
    COFD[4134] = -3.59520327E-01;
    COFD[4135] = 1.31658681E-02;
    COFD[4136] = -2.21508561E+01;
    COFD[4137] = 4.89399517E+00;
    COFD[4138] = -3.59520327E-01;
    COFD[4139] = 1.31658681E-02;
    COFD[4140] = -2.08615666E+01;
    COFD[4141] = 4.16888533E+00;
    COFD[4142] = -2.46500133E-01;
    COFD[4143] = 7.52727544E-03;
    COFD[4144] = -2.08555287E+01;
    COFD[4145] = 4.16888533E+00;
    COFD[4146] = -2.46500133E-01;
    COFD[4147] = 7.52727544E-03;
    COFD[4148] = -2.08618366E+01;
    COFD[4149] = 4.16888533E+00;
    COFD[4150] = -2.46500133E-01;
    COFD[4151] = 7.52727544E-03;
    COFD[4152] = -1.94327481E+01;
    COFD[4153] = 3.43141909E+00;
    COFD[4154] = -1.35054012E-01;
    COFD[4155] = 2.09180741E-03;
    COFD[4156] = -1.94374263E+01;
    COFD[4157] = 3.43141909E+00;
    COFD[4158] = -1.35054012E-01;
    COFD[4159] = 2.09180741E-03;
    COFD[4160] = -1.85739766E+01;
    COFD[4161] = 2.97960589E+00;
    COFD[4162] = -6.82975765E-02;
    COFD[4163] = -1.10754786E-03;
    COFD[4164] = -1.76962248E+01;
    COFD[4165] = 2.53412632E+00;
    COFD[4166] = -2.71561927E-03;
    COFD[4167] = -4.24696836E-03;
    COFD[4168] = -1.70480293E+01;
    COFD[4169] = 2.19974886E+00;
    COFD[4170] = 4.63679485E-02;
    COFD[4171] = -6.58350992E-03;
    COFD[4172] = -1.64162659E+01;
    COFD[4173] = 1.88142640E+00;
    COFD[4174] = 9.25230102E-02;
    COFD[4175] = -8.75912646E-03;
    COFD[4176] = -1.63717750E+01;
    COFD[4177] = 1.85076855E+00;
    COFD[4178] = 9.69498546E-02;
    COFD[4179] = -8.96701578E-03;
    COFD[4180] = -1.58282432E+01;
    COFD[4181] = 1.58723024E+00;
    COFD[4182] = 1.34913922E-01;
    COFD[4183] = -1.07480061E-02;
    COFD[4184] = -1.48216715E+01;
    COFD[4185] = 1.08277276E+00;
    COFD[4186] = 2.07252450E-01;
    COFD[4187] = -1.41278714E-02;
    COFD[4188] = -1.44527134E+01;
    COFD[4189] = 9.00592968E-01;
    COFD[4190] = 2.33311114E-01;
    COFD[4191] = -1.53425385E-02;
    COFD[4192] = -1.40317441E+01;
    COFD[4193] = 6.95865823E-01;
    COFD[4194] = 2.62499652E-01;
    COFD[4195] = -1.66993047E-02;
    COFD[4196] = -2.21414094E+01;
    COFD[4197] = 5.45978982E+00;
    COFD[4198] = -4.77871946E-01;
    COFD[4199] = 2.00798414E-02;
    COFD[4200] = -1.40460932E+01;
    COFD[4201] = 7.18763378E-01;
    COFD[4202] = 2.59237872E-01;
    COFD[4203] = -1.65477278E-02;
    COFD[4204] = -2.09388289E+01;
    COFD[4205] = 5.57974996E+00;
    COFD[4206] = -4.78112864E-01;
    COFD[4207] = 1.94826410E-02;
    COFD[4208] = -2.14367598E+01;
    COFD[4209] = 5.37405372E+00;
    COFD[4210] = -4.70540525E-01;
    COFD[4211] = 1.99137160E-02;
    COFD[4212] = -2.14640929E+01;
    COFD[4213] = 5.37405372E+00;
    COFD[4214] = -4.70540525E-01;
    COFD[4215] = 1.99137160E-02;
    COFD[4216] = -2.26548043E+01;
    COFD[4217] = 5.59528609E+00;
    COFD[4218] = -4.91473190E-01;
    COFD[4219] = 2.05131236E-02;
    COFD[4220] = -1.79421151E+01;
    COFD[4221] = 4.50796148E+00;
    COFD[4222] = -3.69986604E-01;
    COFD[4223] = 1.60286525E-02;
    COFD[4224] = -1.58057876E+01;
    COFD[4225] = 2.22000952E+00;
    COFD[4226] = 4.34143206E-02;
    COFD[4227] = -6.44371681E-03;
    COFD[4228] = -2.26669448E+01;
    COFD[4229] = 5.59528609E+00;
    COFD[4230] = -4.91473190E-01;
    COFD[4231] = 2.05131236E-02;
    COFD[4232] = -2.26422142E+01;
    COFD[4233] = 5.59528609E+00;
    COFD[4234] = -4.91473190E-01;
    COFD[4235] = 2.05131236E-02;
    COFD[4236] = -2.26104730E+01;
    COFD[4237] = 5.58175450E+00;
    COFD[4238] = -4.78660487E-01;
    COFD[4239] = 1.95178220E-02;
    COFD[4240] = -2.26201688E+01;
    COFD[4241] = 5.58491601E+00;
    COFD[4242] = -4.79798668E-01;
    COFD[4243] = 1.95960400E-02;
    COFD[4244] = -1.74160321E+01;
    COFD[4245] = 2.77201495E+00;
    COFD[4246] = -3.78503295E-02;
    COFD[4247] = -2.56220054E-03;
    COFD[4248] = -2.23927663E+01;
    COFD[4249] = 5.52880430E+00;
    COFD[4250] = -4.85323223E-01;
    COFD[4251] = 2.03436366E-02;
    COFD[4252] = -2.21269412E+01;
    COFD[4253] = 4.97192306E+00;
    COFD[4254] = -3.71804005E-01;
    COFD[4255] = 1.37844212E-02;
    COFD[4256] = -2.24546824E+01;
    COFD[4257] = 5.23807779E+00;
    COFD[4258] = -4.15451615E-01;
    COFD[4259] = 1.60367849E-02;
    COFD[4260] = -2.13890574E+01;
    COFD[4261] = 4.65974918E+00;
    COFD[4262] = -3.22390767E-01;
    COFD[4263] = 1.12918650E-02;
    COFD[4264] = -2.19256567E+01;
    COFD[4265] = 4.89350049E+00;
    COFD[4266] = -3.59442064E-01;
    COFD[4267] = 1.31619214E-02;
    COFD[4268] = -1.88405709E+01;
    COFD[4269] = 3.33125761E+00;
    COFD[4270] = -1.20168824E-01;
    COFD[4271] = 1.37545815E-03;
    COFD[4272] = -2.19360376E+01;
    COFD[4273] = 4.77045497E+00;
    COFD[4274] = -3.39955491E-01;
    COFD[4275] = 1.21784372E-02;
    COFD[4276] = -2.19453884E+01;
    COFD[4277] = 4.77045497E+00;
    COFD[4278] = -3.39955491E-01;
    COFD[4279] = 1.21784372E-02;
    COFD[4280] = -2.04884490E+01;
    COFD[4281] = 3.97419032E+00;
    COFD[4282] = -2.17151244E-01;
    COFD[4283] = 6.09602105E-03;
    COFD[4284] = -2.04822359E+01;
    COFD[4285] = 3.97419032E+00;
    COFD[4286] = -2.17151244E-01;
    COFD[4287] = 6.09602105E-03;
    COFD[4288] = -2.04887269E+01;
    COFD[4289] = 3.97419032E+00;
    COFD[4290] = -2.17151244E-01;
    COFD[4291] = 6.09602105E-03;
    COFD[4292] = -1.90327838E+01;
    COFD[4293] = 3.22670299E+00;
    COFD[4294] = -1.04657166E-01;
    COFD[4295] = 6.29620572E-04;
    COFD[4296] = -1.90376207E+01;
    COFD[4297] = 3.22670299E+00;
    COFD[4298] = -1.04657166E-01;
    COFD[4299] = 6.29620572E-04;
    COFD[4300] = -1.80238014E+01;
    COFD[4301] = 2.70981781E+00;
    COFD[4302] = -2.87009191E-02;
    COFD[4303] = -3.00031484E-03;
    COFD[4304] = -1.72790097E+01;
    COFD[4305] = 2.32181806E+00;
    COFD[4306] = 2.85392164E-02;
    COFD[4307] = -5.73831066E-03;
    COFD[4308] = -1.65620371E+01;
    COFD[4309] = 1.95984412E+00;
    COFD[4310] = 8.11931925E-02;
    COFD[4311] = -8.22685123E-03;
    COFD[4312] = -1.58768306E+01;
    COFD[4313] = 1.61945878E+00;
    COFD[4314] = 1.30286421E-01;
    COFD[4315] = -1.05317289E-02;
    COFD[4316] = -1.58282432E+01;
    COFD[4317] = 1.58723024E+00;
    COFD[4318] = 1.34913922E-01;
    COFD[4319] = -1.07480061E-02;
    COFD[4320] = -1.52744247E+01;
    COFD[4321] = 1.31960863E+00;
    COFD[4322] = 1.73344299E-01;
    COFD[4323] = -1.25457703E-02;
    COFD[4324] = -1.42557623E+01;
    COFD[4325] = 8.10499162E-01;
    COFD[4326] = 2.46166208E-01;
    COFD[4327] = -1.59404478E-02;
    COFD[4328] = -1.38696891E+01;
    COFD[4329] = 6.20774670E-01;
    COFD[4330] = 2.73203413E-01;
    COFD[4331] = -1.71972787E-02;
    COFD[4332] = -1.34919831E+01;
    COFD[4333] = 4.34069354E-01;
    COFD[4334] = 2.99858223E-01;
    COFD[4335] = -1.84358359E-02;
    COFD[4336] = -2.23702060E+01;
    COFD[4337] = 5.52375635E+00;
    COFD[4338] = -4.84828265E-01;
    COFD[4339] = 2.03285256E-02;
    COFD[4340] = -1.30635390E+01;
    COFD[4341] = 2.26455242E-01;
    COFD[4342] = 3.29362482E-01;
    COFD[4343] = -1.98017964E-02;
    COFD[4344] = -2.09750828E+01;
    COFD[4345] = 5.50429364E+00;
    COFD[4346] = -4.63372495E-01;
    COFD[4347] = 1.86455814E-02;
    COFD[4348] = -2.17411496E+01;
    COFD[4349] = 5.41958585E+00;
    COFD[4350] = -4.73477571E-01;
    COFD[4351] = 1.99215358E-02;
    COFD[4352] = -2.17689706E+01;
    COFD[4353] = 5.41958585E+00;
    COFD[4354] = -4.73477571E-01;
    COFD[4355] = 1.99215358E-02;
    COFD[4356] = -2.28924481E+01;
    COFD[4357] = 5.61013561E+00;
    COFD[4358] = -4.89933558E-01;
    COFD[4359] = 2.03098561E-02;
    COFD[4360] = -1.84634235E+01;
    COFD[4361] = 4.65797225E+00;
    COFD[4362] = -3.88483952E-01;
    COFD[4363] = 1.67915384E-02;
    COFD[4364] = -1.49248401E+01;
    COFD[4365] = 1.76546946E+00;
    COFD[4366] = 1.09258240E-01;
    COFD[4367] = -9.54522038E-03;
    COFD[4368] = -2.29049917E+01;
    COFD[4369] = 5.61013561E+00;
    COFD[4370] = -4.89933558E-01;
    COFD[4371] = 2.03098561E-02;
    COFD[4372] = -2.28794505E+01;
    COFD[4373] = 5.61013561E+00;
    COFD[4374] = -4.89933558E-01;
    COFD[4375] = 2.03098561E-02;
    COFD[4376] = -2.26368466E+01;
    COFD[4377] = 5.50714270E+00;
    COFD[4378] = -4.64060018E-01;
    COFD[4379] = 1.86881341E-02;
    COFD[4380] = -2.26685946E+01;
    COFD[4381] = 5.51963910E+00;
    COFD[4382] = -4.66555705E-01;
    COFD[4383] = 1.88313890E-02;
    COFD[4384] = -1.66390161E+01;
    COFD[4385] = 2.35716270E+00;
    COFD[4386] = 2.33630190E-02;
    COFD[4387] = -5.49227499E-03;
    COFD[4388] = -2.27595706E+01;
    COFD[4389] = 5.59981860E+00;
    COFD[4390] = -4.91614072E-01;
    COFD[4391] = 2.05016331E-02;
    COFD[4392] = -2.17987077E+01;
    COFD[4393] = 4.73948321E+00;
    COFD[4394] = -3.35004943E-01;
    COFD[4395] = 1.19275825E-02;
    COFD[4396] = -2.22774184E+01;
    COFD[4397] = 5.07362417E+00;
    COFD[4398] = -3.88094414E-01;
    COFD[4399] = 1.46129219E-02;
    COFD[4400] = -2.10053295E+01;
    COFD[4401] = 4.40732366E+00;
    COFD[4402] = -2.82889322E-01;
    COFD[4403] = 9.31663975E-03;
    COFD[4404] = -2.15962984E+01;
    COFD[4405] = 4.66482266E+00;
    COFD[4406] = -3.23189752E-01;
    COFD[4407] = 1.13320181E-02;
    COFD[4408] = -1.80427153E+01;
    COFD[4409] = 2.90760528E+00;
    COFD[4410] = -5.77438179E-02;
    COFD[4411] = -1.61157463E-03;
    COFD[4412] = -2.16062611E+01;
    COFD[4413] = 4.54192945E+00;
    COFD[4414] = -3.03755204E-01;
    COFD[4415] = 1.03536977E-02;
    COFD[4416] = -2.16159823E+01;
    COFD[4417] = 4.54192945E+00;
    COFD[4418] = -3.03755204E-01;
    COFD[4419] = 1.03536977E-02;
    COFD[4420] = -2.00028628E+01;
    COFD[4421] = 3.68035914E+00;
    COFD[4422] = -1.72246313E-01;
    COFD[4423] = 3.89112485E-03;
    COFD[4424] = -1.99963411E+01;
    COFD[4425] = 3.68035914E+00;
    COFD[4426] = -1.72246313E-01;
    COFD[4427] = 3.89112485E-03;
    COFD[4428] = -2.00031545E+01;
    COFD[4429] = 3.68035914E+00;
    COFD[4430] = -1.72246313E-01;
    COFD[4431] = 3.89112485E-03;
    COFD[4432] = -1.81581209E+01;
    COFD[4433] = 2.77116239E+00;
    COFD[4434] = -3.77250246E-02;
    COFD[4435] = -2.56819995E-03;
    COFD[4436] = -1.81632410E+01;
    COFD[4437] = 2.77116239E+00;
    COFD[4438] = -3.77250246E-02;
    COFD[4439] = -2.56819995E-03;
    COFD[4440] = -1.72678915E+01;
    COFD[4441] = 2.30401734E+00;
    COFD[4442] = 3.11443222E-02;
    COFD[4443] = -5.86207495E-03;
    COFD[4444] = -1.64316348E+01;
    COFD[4445] = 1.88148066E+00;
    COFD[4446] = 9.25151744E-02;
    COFD[4447] = -8.75875844E-03;
    COFD[4448] = -1.56095671E+01;
    COFD[4449] = 1.47685497E+00;
    COFD[4450] = 1.50778364E-01;
    COFD[4451] = -1.14906180E-02;
    COFD[4452] = -1.48753985E+01;
    COFD[4453] = 1.11690017E+00;
    COFD[4454] = 2.02362955E-01;
    COFD[4455] = -1.38996262E-02;
    COFD[4456] = -1.48216715E+01;
    COFD[4457] = 1.08277276E+00;
    COFD[4458] = 2.07252450E-01;
    COFD[4459] = -1.41278714E-02;
    COFD[4460] = -1.42557623E+01;
    COFD[4461] = 8.10499162E-01;
    COFD[4462] = 2.46166208E-01;
    COFD[4463] = -1.59404478E-02;
    COFD[4464] = -1.32691288E+01;
    COFD[4465] = 3.16452414E-01;
    COFD[4466] = 3.16572048E-01;
    COFD[4467] = -1.92093720E-02;
    COFD[4468] = -1.29160168E+01;
    COFD[4469] = 1.39889301E-01;
    COFD[4470] = 3.41628534E-01;
    COFD[4471] = -2.03678767E-02;
    COFD[4472] = -1.24841090E+01;
    COFD[4473] = -6.86597662E-02;
    COFD[4474] = 3.71153707E-01;
    COFD[4475] = -2.17314375E-02;
    COFD[4476] = -2.27467739E+01;
    COFD[4477] = 5.59873321E+00;
    COFD[4478] = -4.91665779E-01;
    COFD[4479] = 2.05114831E-02;
    COFD[4480] = -1.27059857E+01;
    COFD[4481] = 4.81478906E-02;
    COFD[4482] = 3.54630676E-01;
    COFD[4483] = -2.09687597E-02;
    COFD[4484] = -2.09816297E+01;
    COFD[4485] = 5.49082104E+00;
    COFD[4486] = -4.60014303E-01;
    COFD[4487] = 1.84365627E-02;
    COFD[4488] = -2.18770418E+01;
    COFD[4489] = 5.46038651E+00;
    COFD[4490] = -4.77939293E-01;
    COFD[4491] = 2.00823652E-02;
    COFD[4492] = -2.19052760E+01;
    COFD[4493] = 5.46038651E+00;
    COFD[4494] = -4.77939293E-01;
    COFD[4495] = 2.00823652E-02;
    COFD[4496] = -2.29058275E+01;
    COFD[4497] = 5.59504443E+00;
    COFD[4498] = -4.86415566E-01;
    COFD[4499] = 2.00947399E-02;
    COFD[4500] = -1.85877596E+01;
    COFD[4501] = 4.69975029E+00;
    COFD[4502] = -3.93412211E-01;
    COFD[4503] = 1.69852534E-02;
    COFD[4504] = -1.45392473E+01;
    COFD[4505] = 1.58706272E+00;
    COFD[4506] = 1.34937972E-01;
    COFD[4507] = -1.07491300E-02;
    COFD[4508] = -2.29187221E+01;
    COFD[4509] = 5.59504443E+00;
    COFD[4510] = -4.86415566E-01;
    COFD[4511] = 2.00947399E-02;
    COFD[4512] = -2.28924756E+01;
    COFD[4513] = 5.59504443E+00;
    COFD[4514] = -4.86415566E-01;
    COFD[4515] = 2.00947399E-02;
    COFD[4516] = -2.26543439E+01;
    COFD[4517] = 5.49570925E+00;
    COFD[4518] = -4.60969345E-01;
    COFD[4519] = 1.84906305E-02;
    COFD[4520] = -2.26705835E+01;
    COFD[4521] = 5.50119356E+00;
    COFD[4522] = -4.62439469E-01;
    COFD[4523] = 1.85841071E-02;
    COFD[4524] = -1.63277528E+01;
    COFD[4525] = 2.20684400E+00;
    COFD[4526] = 4.53338465E-02;
    COFD[4527] = -6.53458896E-03;
    COFD[4528] = -2.28218117E+01;
    COFD[4529] = 5.60692017E+00;
    COFD[4530] = -4.91295058E-01;
    COFD[4531] = 2.04387644E-02;
    COFD[4532] = -2.16564654E+01;
    COFD[4533] = 4.65505428E+00;
    COFD[4534] = -3.21651467E-01;
    COFD[4535] = 1.12547138E-02;
    COFD[4536] = -2.21555553E+01;
    COFD[4537] = 5.00070625E+00;
    COFD[4538] = -3.76355283E-01;
    COFD[4539] = 1.40141622E-02;
    COFD[4540] = -2.08019969E+01;
    COFD[4541] = 4.29990973E+00;
    COFD[4542] = -2.66411377E-01;
    COFD[4543] = 8.50361424E-03;
    COFD[4544] = -2.14611166E+01;
    COFD[4545] = 4.58545934E+00;
    COFD[4546] = -3.10556391E-01;
    COFD[4547] = 1.06931834E-02;
    COFD[4548] = -1.76677062E+01;
    COFD[4549] = 2.72650149E+00;
    COFD[4550] = -3.11601195E-02;
    COFD[4551] = -2.88242823E-03;
    COFD[4552] = -2.14350691E+01;
    COFD[4553] = 4.44599921E+00;
    COFD[4554] = -2.88841525E-01;
    COFD[4555] = 9.61119142E-03;
    COFD[4556] = -2.14451165E+01;
    COFD[4557] = 4.44599921E+00;
    COFD[4558] = -2.88841525E-01;
    COFD[4559] = 9.61119142E-03;
    COFD[4560] = -1.97359681E+01;
    COFD[4561] = 3.54254584E+00;
    COFD[4562] = -1.51588685E-01;
    COFD[4563] = 2.88891088E-03;
    COFD[4564] = -1.97291698E+01;
    COFD[4565] = 3.54254584E+00;
    COFD[4566] = -1.51588685E-01;
    COFD[4567] = 2.88891088E-03;
    COFD[4568] = -1.97362723E+01;
    COFD[4569] = 3.54254584E+00;
    COFD[4570] = -1.51588685E-01;
    COFD[4571] = 2.88891088E-03;
    COFD[4572] = -1.78252926E+01;
    COFD[4573] = 2.60486558E+00;
    COFD[4574] = -1.32252668E-02;
    COFD[4575] = -3.74170980E-03;
    COFD[4576] = -1.78306707E+01;
    COFD[4577] = 2.60486558E+00;
    COFD[4578] = -1.32252668E-02;
    COFD[4579] = -3.74170980E-03;
    COFD[4580] = -1.69703255E+01;
    COFD[4581] = 2.15337734E+00;
    COFD[4582] = 5.31165264E-02;
    COFD[4583] = -6.90242414E-03;
    COFD[4584] = -1.60744425E+01;
    COFD[4585] = 1.70515393E+00;
    COFD[4586] = 1.17964458E-01;
    COFD[4587] = -9.95487344E-03;
    COFD[4588] = -1.52407633E+01;
    COFD[4589] = 1.29532808E+00;
    COFD[4590] = 1.76825909E-01;
    COFD[4591] = -1.27084392E-02;
    COFD[4592] = -1.45087641E+01;
    COFD[4593] = 9.35690838E-01;
    COFD[4594] = 2.28295914E-01;
    COFD[4595] = -1.51088791E-02;
    COFD[4596] = -1.44527134E+01;
    COFD[4597] = 9.00592968E-01;
    COFD[4598] = 2.33311114E-01;
    COFD[4599] = -1.53425385E-02;
    COFD[4600] = -1.38696891E+01;
    COFD[4601] = 6.20774670E-01;
    COFD[4602] = 2.73203413E-01;
    COFD[4603] = -1.71972787E-02;
    COFD[4604] = -1.29160168E+01;
    COFD[4605] = 1.39889301E-01;
    COFD[4606] = 3.41628534E-01;
    COFD[4607] = -2.03678767E-02;
    COFD[4608] = -1.25394882E+01;
    COFD[4609] = -4.80135853E-02;
    COFD[4610] = 3.68241930E-01;
    COFD[4611] = -2.15974735E-02;
    COFD[4612] = -1.21103793E+01;
    COFD[4613] = -2.54815099E-01;
    COFD[4614] = 3.97347047E-01;
    COFD[4615] = -2.29334921E-02;
    COFD[4616] = -2.28072803E+01;
    COFD[4617] = 5.60504512E+00;
    COFD[4618] = -4.91228535E-01;
    COFD[4619] = 2.04427740E-02;
    COFD[4620] = -1.22701336E+01;
    COFD[4621] = -1.61500371E-01;
    COFD[4622] = 3.84217824E-01;
    COFD[4623] = -2.23309656E-02;
    COFD[4624] = -2.09649830E+01;
    COFD[4625] = 5.45055984E+00;
    COFD[4626] = -4.52626477E-01;
    COFD[4627] = 1.80297457E-02;
    COFD[4628] = -2.20665373E+01;
    COFD[4629] = 5.51007247E+00;
    COFD[4630] = -4.83411907E-01;
    COFD[4631] = 2.02812924E-02;
    COFD[4632] = -2.20949205E+01;
    COFD[4633] = 5.51007247E+00;
    COFD[4634] = -4.83411907E-01;
    COFD[4635] = 2.02812924E-02;
    COFD[4636] = -2.29515301E+01;
    COFD[4637] = 5.58219321E+00;
    COFD[4638] = -4.82941269E-01;
    COFD[4639] = 1.98711890E-02;
    COFD[4640] = -1.87542611E+01;
    COFD[4641] = 4.74513688E+00;
    COFD[4642] = -3.98725524E-01;
    COFD[4643] = 1.71922391E-02;
    COFD[4644] = -1.41324994E+01;
    COFD[4645] = 1.38727136E+00;
    COFD[4646] = 1.63642976E-01;
    COFD[4647] = -1.20926895E-02;
    COFD[4648] = -2.29645534E+01;
    COFD[4649] = 5.58219321E+00;
    COFD[4650] = -4.82941269E-01;
    COFD[4651] = 1.98711890E-02;
    COFD[4652] = -2.29380484E+01;
    COFD[4653] = 5.58219321E+00;
    COFD[4654] = -4.82941269E-01;
    COFD[4655] = 1.98711890E-02;
    COFD[4656] = -2.26384265E+01;
    COFD[4657] = 5.45743664E+00;
    COFD[4658] = -4.53877300E-01;
    COFD[4659] = 1.80982969E-02;
    COFD[4660] = -2.26836497E+01;
    COFD[4661] = 5.47552771E+00;
    COFD[4662] = -4.57173951E-01;
    COFD[4663] = 1.82791721E-02;
    COFD[4664] = -1.59785669E+01;
    COFD[4665] = 2.03061309E+00;
    COFD[4666] = 7.09547643E-02;
    COFD[4667] = -7.74512315E-03;
    COFD[4668] = -2.29034521E+01;
    COFD[4669] = 5.61183042E+00;
    COFD[4670] = -4.90566396E-01;
    COFD[4671] = 2.03536798E-02;
    COFD[4672] = -2.15260284E+01;
    COFD[4673] = 4.56640766E+00;
    COFD[4674] = -3.07568513E-01;
    COFD[4675] = 1.05437086E-02;
    COFD[4676] = -2.20003467E+01;
    COFD[4677] = 4.90271383E+00;
    COFD[4678] = -3.60896715E-01;
    COFD[4679] = 1.32352181E-02;
    COFD[4680] = -2.05654443E+01;
    COFD[4681] = 4.16804380E+00;
    COFD[4682] = -2.46372859E-01;
    COFD[4683] = 7.52105030E-03;
    COFD[4684] = -2.12918073E+01;
    COFD[4685] = 4.48188359E+00;
    COFD[4686] = -2.94414571E-01;
    COFD[4687] = 9.88838906E-03;
    COFD[4688] = -1.73379146E+01;
    COFD[4689] = 2.55561949E+00;
    COFD[4690] = -5.90530207E-03;
    COFD[4691] = -4.09393782E-03;
    COFD[4692] = -2.12290071E+01;
    COFD[4693] = 4.32707381E+00;
    COFD[4694] = -2.70563830E-01;
    COFD[4695] = 8.70791456E-03;
    COFD[4696] = -2.12391752E+01;
    COFD[4697] = 4.32707381E+00;
    COFD[4698] = -2.70563830E-01;
    COFD[4699] = 8.70791456E-03;
    COFD[4700] = -1.94433415E+01;
    COFD[4701] = 3.38805125E+00;
    COFD[4702] = -1.28605694E-01;
    COFD[4703] = 1.78139928E-03;
    COFD[4704] = -1.94364398E+01;
    COFD[4705] = 3.38805125E+00;
    COFD[4706] = -1.28605694E-01;
    COFD[4707] = 1.78139928E-03;
    COFD[4708] = -1.94436504E+01;
    COFD[4709] = 3.38805125E+00;
    COFD[4710] = -1.28605694E-01;
    COFD[4711] = 1.78139928E-03;
    COFD[4712] = -1.75400793E+01;
    COFD[4713] = 2.45372496E+00;
    COFD[4714] = 9.16587873E-03;
    COFD[4715] = -4.81554146E-03;
    COFD[4716] = -1.75455548E+01;
    COFD[4717] = 2.45372496E+00;
    COFD[4718] = 9.16587873E-03;
    COFD[4719] = -4.81554146E-03;
    COFD[4720] = -1.66106787E+01;
    COFD[4721] = 1.97256518E+00;
    COFD[4722] = 7.93547823E-02;
    COFD[4723] = -8.14046199E-03;
    COFD[4724] = -1.56831497E+01;
    COFD[4725] = 1.51210139E+00;
    COFD[4726] = 1.45705555E-01;
    COFD[4727] = -1.12527582E-02;
    COFD[4728] = -1.48245688E+01;
    COFD[4729] = 1.09196780E+00;
    COFD[4730] = 2.05935790E-01;
    COFD[4731] = -1.40664564E-02;
    COFD[4732] = -1.40894169E+01;
    COFD[4733] = 7.31458802E-01;
    COFD[4734] = 2.57430647E-01;
    COFD[4735] = -1.64638268E-02;
    COFD[4736] = -1.40317441E+01;
    COFD[4737] = 6.95865823E-01;
    COFD[4738] = 2.62499652E-01;
    COFD[4739] = -1.66993047E-02;
    COFD[4740] = -1.34919831E+01;
    COFD[4741] = 4.34069354E-01;
    COFD[4742] = 2.99858223E-01;
    COFD[4743] = -1.84358359E-02;
    COFD[4744] = -1.24841090E+01;
    COFD[4745] = -6.86597662E-02;
    COFD[4746] = 3.71153707E-01;
    COFD[4747] = -2.17314375E-02;
    COFD[4748] = -1.21103793E+01;
    COFD[4749] = -2.54815099E-01;
    COFD[4750] = 3.97347047E-01;
    COFD[4751] = -2.29334921E-02;
    COFD[4752] = -1.16357551E+01;
    COFD[4753] = -4.81860216E-01;
    COFD[4754] = 4.29406287E-01;
    COFD[4755] = -2.44123168E-02;
    COFD[4756] = -2.28938366E+01;
    COFD[4757] = 5.61204588E+00;
    COFD[4758] = -4.90798632E-01;
    COFD[4759] = 2.03718133E-02;
    COFD[4760] = -2.27628163E+01;
    COFD[4761] = 5.60192583E+00;
    COFD[4762] = -4.91455403E-01;
    COFD[4763] = 2.04776556E-02;
    COFD[4764] = -1.42894441E+01;
    COFD[4765] = 3.67490723E+00;
    COFD[4766] = -2.65114792E-01;
    COFD[4767] = 1.16092671E-02;
    COFD[4768] = -1.40756935E+01;
    COFD[4769] = 3.07549274E+00;
    COFD[4770] = -1.88889344E-01;
    COFD[4771] = 8.37152866E-03;
    COFD[4772] = -1.40949196E+01;
    COFD[4773] = 3.07549274E+00;
    COFD[4774] = -1.88889344E-01;
    COFD[4775] = 8.37152866E-03;
    COFD[4776] = -1.52486273E+01;
    COFD[4777] = 3.35922578E+00;
    COFD[4778] = -2.25181399E-01;
    COFD[4779] = 9.92132878E-03;
    COFD[4780] = -1.16906297E+01;
    COFD[4781] = 2.47469981E+00;
    COFD[4782] = -1.10436257E-01;
    COFD[4783] = 4.95273813E-03;
    COFD[4784] = -2.10643259E+01;
    COFD[4785] = 5.53614847E+00;
    COFD[4786] = -4.86046736E-01;
    COFD[4787] = 2.03659188E-02;
    COFD[4788] = -1.52554761E+01;
    COFD[4789] = 3.35922578E+00;
    COFD[4790] = -2.25181399E-01;
    COFD[4791] = 9.92132878E-03;
    COFD[4792] = -1.52414485E+01;
    COFD[4793] = 3.35922578E+00;
    COFD[4794] = -2.25181399E-01;
    COFD[4795] = 9.92132878E-03;
    COFD[4796] = -1.59633387E+01;
    COFD[4797] = 3.66853818E+00;
    COFD[4798] = -2.64346221E-01;
    COFD[4799] = 1.15784613E-02;
    COFD[4800] = -1.59327297E+01;
    COFD[4801] = 3.65620899E+00;
    COFD[4802] = -2.62933804E-01;
    COFD[4803] = 1.15253223E-02;
    COFD[4804] = -2.04833713E+01;
    COFD[4805] = 5.23112374E+00;
    COFD[4806] = -4.54967682E-01;
    COFD[4807] = 1.93570423E-02;
    COFD[4808] = -1.50031687E+01;
    COFD[4809] = 3.26223357E+00;
    COFD[4810] = -2.12746642E-01;
    COFD[4811] = 9.38912883E-03;
    COFD[4812] = -1.81432461E+01;
    COFD[4813] = 4.37565431E+00;
    COFD[4814] = -3.53906025E-01;
    COFD[4815] = 1.53760786E-02;
    COFD[4816] = -1.76002031E+01;
    COFD[4817] = 4.19171952E+00;
    COFD[4818] = -3.31354810E-01;
    COFD[4819] = 1.44520623E-02;
    COFD[4820] = -1.85864144E+01;
    COFD[4821] = 4.54915847E+00;
    COFD[4822] = -3.75000738E-01;
    COFD[4823] = 1.62324821E-02;
    COFD[4824] = -1.83249299E+01;
    COFD[4825] = 4.42045763E+00;
    COFD[4826] = -3.59451578E-01;
    COFD[4827] = 1.56056164E-02;
    COFD[4828] = -2.02693653E+01;
    COFD[4829] = 5.10426133E+00;
    COFD[4830] = -4.41256919E-01;
    COFD[4831] = 1.88737290E-02;
    COFD[4832] = -1.87434358E+01;
    COFD[4833] = 4.48550694E+00;
    COFD[4834] = -3.67277454E-01;
    COFD[4835] = 1.59194755E-02;
    COFD[4836] = -1.87483158E+01;
    COFD[4837] = 4.48550694E+00;
    COFD[4838] = -3.67277454E-01;
    COFD[4839] = 1.59194755E-02;
    COFD[4840] = -1.98609095E+01;
    COFD[4841] = 4.84231878E+00;
    COFD[4842] = -4.10101001E-01;
    COFD[4843] = 1.76356687E-02;
    COFD[4844] = -1.98580023E+01;
    COFD[4845] = 4.84231878E+00;
    COFD[4846] = -4.10101001E-01;
    COFD[4847] = 1.76356687E-02;
    COFD[4848] = -1.98610390E+01;
    COFD[4849] = 4.84231878E+00;
    COFD[4850] = -4.10101001E-01;
    COFD[4851] = 1.76356687E-02;
    COFD[4852] = -2.07714550E+01;
    COFD[4853] = 5.13381106E+00;
    COFD[4854] = -4.44579059E-01;
    COFD[4855] = 1.89972323E-02;
    COFD[4856] = -2.07735316E+01;
    COFD[4857] = 5.13381106E+00;
    COFD[4858] = -4.44579059E-01;
    COFD[4859] = 1.89972323E-02;
    COFD[4860] = -2.12156995E+01;
    COFD[4861] = 5.24766160E+00;
    COFD[4862] = -4.56882620E-01;
    COFD[4863] = 1.94309393E-02;
    COFD[4864] = -2.16312240E+01;
    COFD[4865] = 5.36160575E+00;
    COFD[4866] = -4.69678973E-01;
    COFD[4867] = 1.99069834E-02;
    COFD[4868] = -2.18579933E+01;
    COFD[4869] = 5.40322773E+00;
    COFD[4870] = -4.72630192E-01;
    COFD[4871] = 1.99360102E-02;
    COFD[4872] = -2.20902647E+01;
    COFD[4873] = 5.45237285E+00;
    COFD[4874] = -4.77075672E-01;
    COFD[4875] = 2.00518642E-02;
    COFD[4876] = -2.21414094E+01;
    COFD[4877] = 5.45978982E+00;
    COFD[4878] = -4.77871946E-01;
    COFD[4879] = 2.00798414E-02;
    COFD[4880] = -2.23702060E+01;
    COFD[4881] = 5.52375635E+00;
    COFD[4882] = -4.84828265E-01;
    COFD[4883] = 2.03285256E-02;
    COFD[4884] = -2.27467739E+01;
    COFD[4885] = 5.59873321E+00;
    COFD[4886] = -4.91665779E-01;
    COFD[4887] = 2.05114831E-02;
    COFD[4888] = -2.28072803E+01;
    COFD[4889] = 5.60504512E+00;
    COFD[4890] = -4.91228535E-01;
    COFD[4891] = 2.04427740E-02;
    COFD[4892] = -2.28938366E+01;
    COFD[4893] = 5.61204588E+00;
    COFD[4894] = -4.90798632E-01;
    COFD[4895] = 2.03718133E-02;
    COFD[4896] = -1.49828430E+01;
    COFD[4897] = 3.25781069E+00;
    COFD[4898] = -2.12199367E-01;
    COFD[4899] = 9.36657283E-03;
}


/*List of specs with small weight, dim NLITE */
void egtransetKTDIF(int* KTDIF) {
    KTDIF[0] = 2;
    KTDIF[1] = 6;
}


/*Poly fits for thermal diff ratios, dim NO*NLITE*KK */
void egtransetCOFTD(double* COFTD) {
    COFTD[0] = -2.32473318E-01;
    COFTD[1] = 9.16130104E-04;
    COFTD[2] = -3.79155650E-07;
    COFTD[3] = 5.37766578E-11;
    COFTD[4] = 0.00000000E+00;
    COFTD[5] = 0.00000000E+00;
    COFTD[6] = 0.00000000E+00;
    COFTD[7] = 0.00000000E+00;
    COFTD[8] = 2.35283119E-01;
    COFTD[9] = 4.65670599E-04;
    COFTD[10] = -2.60939824E-07;
    COFTD[11] = 4.49271822E-11;
    COFTD[12] = 2.37053352E-01;
    COFTD[13] = 4.69174231E-04;
    COFTD[14] = -2.62903094E-07;
    COFTD[15] = 4.52652072E-11;
    COFTD[16] = 1.80186965E-01;
    COFTD[17] = 6.02882805E-04;
    COFTD[18] = -3.27063140E-07;
    COFTD[19] = 5.50170790E-11;
    COFTD[20] = 1.44152190E-01;
    COFTD[21] = 7.99993584E-05;
    COFTD[22] = -4.89707442E-08;
    COFTD[23] = 9.14277269E-12;
    COFTD[24] = -1.74352698E-01;
    COFTD[25] = 8.62246873E-04;
    COFTD[26] = -3.79545489E-07;
    COFTD[27] = 5.60262093E-11;
    COFTD[28] = 1.80513677E-01;
    COFTD[29] = 6.03975942E-04;
    COFTD[30] = -3.27656165E-07;
    COFTD[31] = 5.51168351E-11;
    COFTD[32] = 1.79840299E-01;
    COFTD[33] = 6.01722902E-04;
    COFTD[34] = -3.26433894E-07;
    COFTD[35] = 5.49112302E-11;
    COFTD[36] = 1.00039110E-01;
    COFTD[37] = 6.50468660E-04;
    COFTD[38] = -3.41778999E-07;
    COFTD[39] = 5.62779132E-11;
    COFTD[40] = 1.05124122E-01;
    COFTD[41] = 6.50665957E-04;
    COFTD[42] = -3.42564538E-07;
    COFTD[43] = 5.64804120E-11;
    COFTD[44] = -1.61357564E-01;
    COFTD[45] = 9.05920260E-04;
    COFTD[46] = -4.07879153E-07;
    COFTD[47] = 6.10626290E-11;
    COFTD[48] = 2.00119897E-01;
    COFTD[49] = 5.64793704E-04;
    COFTD[50] = -3.09445484E-07;
    COFTD[51] = 5.24139335E-11;
    COFTD[52] = -2.00309448E-02;
    COFTD[53] = 8.50440115E-04;
    COFTD[54] = -4.21064468E-07;
    COFTD[55] = 6.67959710E-11;
    COFTD[56] = 1.63245097E-02;
    COFTD[57] = 7.90133388E-04;
    COFTD[58] = -3.98292458E-07;
    COFTD[59] = 6.38851432E-11;
    COFTD[60] = -5.08744745E-02;
    COFTD[61] = 8.54342586E-04;
    COFTD[62] = -4.15926453E-07;
    COFTD[63] = 6.53063261E-11;
    COFTD[64] = -2.72323768E-02;
    COFTD[65] = 8.39184413E-04;
    COFTD[66] = -4.13849924E-07;
    COFTD[67] = 6.54928043E-11;
    COFTD[68] = -1.41799739E-01;
    COFTD[69] = 9.22440172E-04;
    COFTD[70] = -4.23685885E-07;
    COFTD[71] = 6.42121388E-11;
    COFTD[72] = -4.05265093E-02;
    COFTD[73] = 8.65036069E-04;
    COFTD[74] = -4.23755394E-07;
    COFTD[75] = 6.67874248E-11;
    COFTD[76] = -4.05742102E-02;
    COFTD[77] = 8.66054242E-04;
    COFTD[78] = -4.24254167E-07;
    COFTD[79] = 6.68660357E-11;
    COFTD[80] = -1.04600800E-01;
    COFTD[81] = 9.18521761E-04;
    COFTD[82] = -4.33727945E-07;
    COFTD[83] = 6.68516248E-11;
    COFTD[84] = -1.04534973E-01;
    COFTD[85] = 9.17943719E-04;
    COFTD[86] = -4.33454992E-07;
    COFTD[87] = 6.68095539E-11;
    COFTD[88] = -1.04603726E-01;
    COFTD[89] = 9.18547452E-04;
    COFTD[90] = -4.33740076E-07;
    COFTD[91] = 6.68534946E-11;
    COFTD[92] = -1.49470177E-01;
    COFTD[93] = 9.39837943E-04;
    COFTD[94] = -4.29898401E-07;
    COFTD[95] = 6.49891366E-11;
    COFTD[96] = -1.49532849E-01;
    COFTD[97] = 9.40232012E-04;
    COFTD[98] = -4.30078655E-07;
    COFTD[99] = 6.50163862E-11;
    COFTD[100] = -1.70681865E-01;
    COFTD[101] = 9.45804956E-04;
    COFTD[102] = -4.24958444E-07;
    COFTD[103] = 6.35382722E-11;
    COFTD[104] = -1.87226019E-01;
    COFTD[105] = 9.46155815E-04;
    COFTD[106] = -4.18274102E-07;
    COFTD[107] = 6.19098884E-11;
    COFTD[108] = -2.00326928E-01;
    COFTD[109] = 9.43207024E-04;
    COFTD[110] = -4.10815351E-07;
    COFTD[111] = 6.02382726E-11;
    COFTD[112] = -2.10628224E-01;
    COFTD[113] = 9.38318972E-04;
    COFTD[114] = -4.03217330E-07;
    COFTD[115] = 5.86175620E-11;
    COFTD[116] = -2.11550189E-01;
    COFTD[117] = 9.37689414E-04;
    COFTD[118] = -4.02415435E-07;
    COFTD[119] = 5.84513451E-11;
    COFTD[120] = -2.18698571E-01;
    COFTD[121] = 9.32462742E-04;
    COFTD[122] = -3.95893189E-07;
    COFTD[123] = 5.71051753E-11;
    COFTD[124] = -2.30604806E-01;
    COFTD[125] = 9.19204394E-04;
    COFTD[126] = -3.81973799E-07;
    COFTD[127] = 5.43234555E-11;
    COFTD[128] = -2.34683831E-01;
    COFTD[129] = 9.14439041E-04;
    COFTD[130] = -3.76888633E-07;
    COFTD[131] = 5.33062785E-11;
    COFTD[132] = -2.38573690E-01;
    COFTD[133] = 9.07159798E-04;
    COFTD[134] = -3.70373773E-07;
    COFTD[135] = 5.20506190E-11;
    COFTD[136] = 2.01521643E-01;
    COFTD[137] = 5.62744089E-04;
    COFTD[138] = -3.08519239E-07;
    COFTD[139] = 5.22805986E-11;
    COFTD[140] = 1.95741386E-02;
    COFTD[141] = 8.31429168E-04;
    COFTD[142] = -4.19562268E-07;
    COFTD[143] = 6.73418358E-11;
    COFTD[144] = -1.44152190E-01;
    COFTD[145] = -7.99993584E-05;
    COFTD[146] = 4.89707442E-08;
    COFTD[147] = -9.14277269E-12;
    COFTD[148] = 4.06682492E-01;
    COFTD[149] = 3.84705248E-05;
    COFTD[150] = -2.54846868E-08;
    COFTD[151] = 5.86302354E-12;
    COFTD[152] = 4.12895615E-01;
    COFTD[153] = 3.90582612E-05;
    COFTD[154] = -2.58740310E-08;
    COFTD[155] = 5.95259633E-12;
    COFTD[156] = 4.28230888E-01;
    COFTD[157] = 1.20873273E-04;
    COFTD[158] = -7.70268349E-08;
    COFTD[159] = 1.52678954E-11;
    COFTD[160] = 0.00000000E+00;
    COFTD[161] = 0.00000000E+00;
    COFTD[162] = 0.00000000E+00;
    COFTD[163] = 0.00000000E+00;
    COFTD[164] = 2.27469146E-02;
    COFTD[165] = 6.73078907E-04;
    COFTD[166] = -3.40935843E-07;
    COFTD[167] = 5.48499211E-11;
    COFTD[168] = 4.29789463E-01;
    COFTD[169] = 1.21313199E-04;
    COFTD[170] = -7.73071792E-08;
    COFTD[171] = 1.53234639E-11;
    COFTD[172] = 4.26579943E-01;
    COFTD[173] = 1.20407274E-04;
    COFTD[174] = -7.67298757E-08;
    COFTD[175] = 1.52090336E-11;
    COFTD[176] = 3.31191185E-01;
    COFTD[177] = 1.81326714E-04;
    COFTD[178] = -1.11096391E-07;
    COFTD[179] = 2.07635959E-11;
    COFTD[180] = 3.39557243E-01;
    COFTD[181] = 1.79335036E-04;
    COFTD[182] = -1.10135705E-07;
    COFTD[183] = 2.06427239E-11;
    COFTD[184] = 1.22693382E-01;
    COFTD[185] = 6.21278143E-04;
    COFTD[186] = -3.29965208E-07;
    COFTD[187] = 5.47161548E-11;
    COFTD[188] = 4.30605547E-01;
    COFTD[189] = 9.35961902E-05;
    COFTD[190] = -6.03983623E-08;
    COFTD[191] = 1.23115170E-11;
    COFTD[192] = 2.93191523E-01;
    COFTD[193] = 4.01430006E-04;
    COFTD[194] = -2.30705763E-07;
    COFTD[195] = 4.05176586E-11;
    COFTD[196] = 3.05613225E-01;
    COFTD[197] = 3.24505886E-04;
    COFTD[198] = -1.89889572E-07;
    COFTD[199] = 3.38663465E-11;
    COFTD[200] = 2.49017478E-01;
    COFTD[201] = 4.29036573E-04;
    COFTD[202] = -2.42668617E-07;
    COFTD[203] = 4.20801371E-11;
    COFTD[204] = 2.74036956E-01;
    COFTD[205] = 3.96249742E-04;
    COFTD[206] = -2.26857964E-07;
    COFTD[207] = 3.97176979E-11;
    COFTD[208] = 1.59647939E-01;
    COFTD[209] = 6.04192274E-04;
    COFTD[210] = -3.25569591E-07;
    COFTD[211] = 5.45134698E-11;
    COFTD[212] = 2.71946054E-01;
    COFTD[213] = 4.30814303E-04;
    COFTD[214] = -2.45084319E-07;
    COFTD[215] = 4.26912987E-11;
    COFTD[216] = 2.72587742E-01;
    COFTD[217] = 4.31830859E-04;
    COFTD[218] = -2.45662623E-07;
    COFTD[219] = 4.27920337E-11;
    COFTD[220] = 2.11489408E-01;
    COFTD[221] = 5.48044743E-04;
    COFTD[222] = -3.01832704E-07;
    COFTD[223] = 5.13135279E-11;
    COFTD[224] = 2.11223041E-01;
    COFTD[225] = 5.47354491E-04;
    COFTD[226] = -3.01452551E-07;
    COFTD[227] = 5.12488995E-11;
    COFTD[228] = 2.11501250E-01;
    COFTD[229] = 5.48075431E-04;
    COFTD[230] = -3.01849605E-07;
    COFTD[231] = 5.13164012E-11;
    COFTD[232] = 1.58396440E-01;
    COFTD[233] = 6.35599035E-04;
    COFTD[234] = -3.41440278E-07;
    COFTD[235] = 5.70517218E-11;
    COFTD[236] = 1.58529381E-01;
    COFTD[237] = 6.36132489E-04;
    COFTD[238] = -3.41726846E-07;
    COFTD[239] = 5.70996048E-11;
    COFTD[240] = 1.30463673E-01;
    COFTD[241] = 6.81797978E-04;
    COFTD[242] = -3.61586231E-07;
    COFTD[243] = 5.99026538E-11;
    COFTD[244] = 1.05942828E-01;
    COFTD[245] = 7.18849623E-04;
    COFTD[246] = -3.77076618E-07;
    COFTD[247] = 6.20226611E-11;
    COFTD[248] = 8.44927425E-02;
    COFTD[249] = 7.49324701E-04;
    COFTD[250] = -3.89336694E-07;
    COFTD[251] = 6.36480646E-11;
    COFTD[252] = 6.60204083E-02;
    COFTD[253] = 7.74270719E-04;
    COFTD[254] = -3.99007948E-07;
    COFTD[255] = 6.48894411E-11;
    COFTD[256] = 6.42552193E-02;
    COFTD[257] = 7.76498859E-04;
    COFTD[258] = -3.99842626E-07;
    COFTD[259] = 6.49929630E-11;
    COFTD[260] = 5.02867268E-02;
    COFTD[261] = 7.94635936E-04;
    COFTD[262] = -4.06630535E-07;
    COFTD[263] = 6.58364739E-11;
    COFTD[264] = 2.42288160E-02;
    COFTD[265] = 8.26440588E-04;
    COFTD[266] = -4.17936656E-07;
    COFTD[267] = 6.71698220E-11;
    COFTD[268] = 1.49563452E-02;
    COFTD[269] = 8.39181536E-04;
    COFTD[270] = -4.22562584E-07;
    COFTD[271] = 6.77329772E-11;
    COFTD[272] = 4.65224072E-03;
    COFTD[273] = 8.50497867E-04;
    COFTD[274] = -4.26212726E-07;
    COFTD[275] = 6.81161741E-11;
    COFTD[276] = 4.31331269E-01;
    COFTD[277] = 9.20536800E-05;
    COFTD[278] = -5.94509616E-08;
    COFTD[279] = 1.21437993E-11;
}

