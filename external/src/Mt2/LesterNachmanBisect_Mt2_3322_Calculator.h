// Header file for the Oxbridge Stransverse Mass Library -- oxbridgekinetics.
// See http://www.hep.phy.cam.ac.uk/~lester/mt2/index.html
// Authors: Christopher Lester and Alan Barr

#ifndef LesterNachmanBisect_Mt2_3322_Calculator_H
#define LesterNachmanBisect_Mt2_3322_Calculator_H

#include "Mt2_3322_Calculator.h"
#include <vector>

/** 
 * This is a wrapper for the algorithm of https://arxiv.org/abs/1411.4312 found here: https://arxiv.org/src/1411.4312v5/anc/lester_mt2_bisect.h
*/

namespace ZhenyuHan {
  class mt2;
}

namespace Mt2 {

  class LesterNachmanBisect_Mt2_3322_Calculator : public Mt2_3322_Calculator {
  public:
    double mt2_3322(const LorentzTransverseVector& visibleA, // 3 d.o.f. 
		   const LorentzTransverseVector& visibleB, // 3 d.o.f.
		   const TwoVector& ptmiss,                 // 2 d.o.f.
		   const double mInvisibleA, const double mInvisibleB); // 2 d.o.f.
    double mt2_3322_Sq(const LorentzTransverseVector& visibleA, // 3 d.o.f. 
		      const LorentzTransverseVector& visibleB, // 3 d.o.f.
		      const TwoVector& ptmiss,                 // 2 d.o.f.
		      const double mInvisibleA, const double mInvisibleB); // 2 d.o.f.
    LesterNachmanBisect_Mt2_3322_Calculator();
    
  };

} //end namespace Mt2

#endif
