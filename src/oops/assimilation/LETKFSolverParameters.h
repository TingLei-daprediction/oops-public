/*
 * (C) Copyright 2020 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef OOPS_ASSIMILATION_LETKFSOLVERPARAMETERS_H_
#define OOPS_ASSIMILATION_LETKFSOLVERPARAMETERS_H_

#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"

namespace oops {

/// Parameters for LETKF inflation
class LETKFInflationParameters : public Parameters {
  OOPS_CONCRETE_PARAMETERS(LETKFInflationParameters, Parameters)

 public:
  // multiplicative prior inflation Pf'=mult*Pf
  Parameter<double> mult{"mult", 1.0, this};

  // RTPP: Relaxation to prior perturbation.
  // delta_xa'(iens)=rtppCoeff*delta_xb'(iens)+(1-rtppCoeff)*delta_xa'(iens)
  //
  // Zhang, F., C. Snyder, and J. Sun, 2004: Tests of an ensemble
  // Kalman Filter for convective-scale data assim-imilation:
  // Impact of initial estimate and observations.
  // Mon. Wea. Rev., 132, 1238-1253.
  Parameter<double> rtpp{"rtpp", 0.0, this};

  // Relaxation to prior spread
  // delta_xa'(i_grid)=delta_xa(i_grid)*
  //      *{ rtps*( std(Xb(i_grid,:))-std(Xa(i_grid,:)) )/std(Xa(i_grid,:)) + 1 }
  // where i_grid is the index of grid point
  // std is ensemble standard deviation for prior (Xb) and posterior (Xa)
  Parameter<double> rtps{"rtps", 0.0, this};

  // rtpsInflMin and rtpsInflMax are min and max perturbation inflation
  // allowed with rtps inflation.
  double rtpsInflMin() const { return 1.0; }
  double rtpsInflMax() const { return 1e30; }

  bool dortpp() const { return rtpp > 0.0 && rtpp <= 1.0; }
  bool dortps() const { return rtps > 0.0 && rtps <= 1.0; }
};

/// LETKF parameters
class LETKFSolverParameters : public Parameters {
  OOPS_CONCRETE_PARAMETERS(LETKFSolverParameters, Parameters)
 public:
  Parameter<LETKFInflationParameters> infl{"local ensemble DA.inflation", {}, this};
};

// -----------------------------------------------------------------------------

}  // namespace oops
#endif  // OOPS_ASSIMILATION_LETKFSOLVERPARAMETERS_H_
