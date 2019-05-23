// Header file for the Oxbridge Stransverse Mass Library -- oxbridgekinetics.
// See http://www.hep.phy.cam.ac.uk/~lester/mt2/index.html
// Authors: Christopher Lester

#ifndef MT2_3322_CALCULATOR_H
#define MT2_3322_CALCULATOR_H
#include "Mt2Vectors.h"
#include "Mt2Calculator.h"

namespace Mt2 {
/**
   Class which knows how to calculate M_T2.
   Please take care when choosing the appropriate function to use for your analysis.
   @author Alan Barr & Chris Lester
   @date 9 Feb 2006 and onwards
*/

  class Mt2_3322_Calculator : public Mt2Calculator {
  public:
    /** 
	mt2_332
	
	Calculate M_T2 knowing ptmiss not pvis-transverse-lorentz vec
	
    */
    virtual double mt2_3322(const LorentzTransverseVector& visibleA, // 3 d.o.f. 
			   const LorentzTransverseVector& visibleB, // 3 d.o.f.
			   const TwoVector& ptmiss,                 // 2 d.o.f.
			   const double mInvisibleA, const double mInvisibleB) = 0; // 2 d.o.f.
    virtual double mt2_3322_Sq(const LorentzTransverseVector& visibleA, // 3 d.o.f. 
			      const LorentzTransverseVector& visibleB, // 3 d.o.f.
			      const TwoVector& ptmiss,                 // 2 d.o.f.
			      const double mInvisibleA, const double mInvisibleB) = 0; // 2 d.o.f.
    Mt2_3322_Calculator(const std::string & algName) : Mt2Calculator(algName) {};
  };
 
}

#endif
