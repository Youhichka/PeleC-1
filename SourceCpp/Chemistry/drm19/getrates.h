#ifndef __BASELINE_CPU_GET_RATES__
#define __BASELINE_CPU_GET_RATES__

/*#ifndef __SINGE_MOLE_MASSES__
#define __SINGE_MOLE_MASSES__
const double molecular_masses[21] = {2.01594, 1.00797, 15.9994, 31.9988, 
  17.00737, 18.01534, 33.00677, 14.02709, 14.02709, 15.03506, 16.04303, 
  28.01055, 44.00995, 29.01852, 30.02649, 31.03446, 28.05418, 29.06215, 
  30.07012, 28.0134, 39.948}; 
#endif


#ifndef __SINGE_RECIP_MOLE_MASSES__
#define __SINGE_RECIP_MOLE_MASSES__
const double recip_molecular_masses[21] = {0.4960465093207139, 
  0.9920930186414277, 0.06250234383789392, 0.03125117191894696, 
  0.05879803873262004, 0.05550825019122593, 0.03029681486555637, 
  0.07129062407099405, 0.07129062407099405, 0.06651120780362699, 
  0.06233236489615739, 0.03570083414998991, 0.02272213442641948, 
  0.03446075127194632, 0.03330392596670473, 0.03222224585186918, 
  0.03564531203549703, 0.0344090165386938, 0.03325560390181349, 
  0.03569720205330306, 0.02503254230499649}; 
#endif*/
#include "chemistry_file.H"

#ifdef __cplusplus
extern "C" {
#endif
AMREX_GPU_HOST_DEVICE
void getrates(const double pressure, const double temperature, const double 
  avmolwt, const double *mass_frac, double *wdot); 
#ifdef __cplusplus
}
#endif
#endif // __BASELINE_CPU_GET_RATES__