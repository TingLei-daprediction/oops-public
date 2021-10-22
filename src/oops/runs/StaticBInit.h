/*
 * (C) Copyright 2018-2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef OOPS_RUNS_STATICBINIT_H_
#define OOPS_RUNS_STATICBINIT_H_

#include <memory>
#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "oops/base/Geometry.h"
#include "oops/base/instantiateCovarFactory.h"
#include "oops/base/ModelSpaceCovarianceBase.h"
#include "oops/base/State.h"
#include "oops/base/Variables.h"
#include "oops/mpi/mpi.h"
#include "oops/runs/Application.h"
#include "oops/util/Logger.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"

namespace oops {

/// Options taken by the StaticBInit application.
template <typename MODEL> class StaticBInitParameters : public ApplicationParameters {
  OOPS_CONCRETE_PARAMETERS(StaticBInitParameters, ApplicationParameters);

 public:
  typedef ModelSpaceCovarianceParametersWrapper<MODEL> CovarianceParameters_;
  typedef typename Geometry<MODEL>::Parameters_        GeometryParameters_;
  typedef typename State<MODEL>::Parameters_           StateParameters_;

  /// Geometry parameters.
  RequiredParameter<GeometryParameters_> geometry{"geometry", this};

  /// List of variables for analysis.
  RequiredParameter<Variables> analysisVariables{"analysis variables", this};

  /// Background state parameters.
  RequiredParameter<StateParameters_> background{"background", this};

  /// Background error covariance model.
  RequiredParameter<CovarianceParameters_> backgroundError{"background error", this};
};

// -----------------------------------------------------------------------------

template <typename MODEL> class StaticBInit : public Application {
  typedef ModelSpaceCovarianceBase<MODEL>           CovarianceBase_;
  typedef CovarianceFactory<MODEL>                  CovarianceFactory_;
  typedef ModelSpaceCovarianceParametersBase<MODEL> CovarianceParametersBase_;
  typedef Geometry<MODEL>                           Geometry_;
  typedef Increment<MODEL>                          Increment_;
  typedef State<MODEL>                              State_;

  typedef StaticBInitParameters<MODEL>              StaticBInitParameters_;

 public:
  // -----------------------------------------------------------------------------
  explicit StaticBInit(const eckit::mpi::Comm & comm = oops::mpi::world()) : Application(comm) {
    instantiateCovarFactory<MODEL>();
  }
  // -----------------------------------------------------------------------------
  virtual ~StaticBInit() {}
  // -----------------------------------------------------------------------------
  int execute(const eckit::Configuration & fullConfig) const {
    //  Deserialize parameters
    StaticBInitParameters_ params;
    params.validateAndDeserialize(fullConfig);

    //  Setup resolution
    const Geometry_ resol(params.geometry, this->getComm(), oops::mpi::myself());

    //  Setup variables
    const Variables &vars = params.analysisVariables;

    //  Setup background state
    const State_ xx(resol, params.background);

    //  Initialize static B matrix
    const CovarianceParametersBase_ &covarParams =
        params.backgroundError.value().covarianceParameters;
    const std::unique_ptr<CovarianceBase_> Bmat(CovarianceFactory_::create(
                                                    covarParams, resol, vars, xx, xx));

    //  Randomize B matrix
    Increment_ dx(resol, vars, xx.validTime());
    Bmat->randomize(dx);
    Log::test() << dx << std::endl;

    return 0;
  }
  // -----------------------------------------------------------------------------
 private:
  std::string appname() const {
    return "oops::StaticBInit<" + MODEL::name() + ">";
  }
  // -----------------------------------------------------------------------------
};

}  // namespace oops

#endif  // OOPS_RUNS_STATICBINIT_H_
