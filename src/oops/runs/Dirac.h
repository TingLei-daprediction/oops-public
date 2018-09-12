/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_RUNS_DIRAC_H_
#define OOPS_RUNS_DIRAC_H_

#include <sstream>
#include <string>

#include <boost/scoped_ptr.hpp>

#include "eckit/config/Configuration.h"
#include "oops/base/instantiateCovarFactory.h"
#include "oops/base/ModelSpaceCovarianceBase.h"
#include "oops/base/PostProcessor.h"
#include "oops/base/StateWriter.h"
#include "oops/base/Variables.h"
#include "oops/generic/UnstructuredGrid.h"
#include "oops/interface/Geometry.h"
#include "oops/interface/Increment.h"
#include "oops/interface/Model.h"
#include "oops/interface/ModelAuxControl.h"
#include "oops/interface/State.h"
#include "oops/runs/Application.h"
#include "oops/util/DateTime.h"
#include "oops/util/Duration.h"
#include "oops/util/Logger.h"

namespace oops {

template <typename MODEL> class Dirac : public Application {
  typedef Geometry<MODEL>            Geometry_;
  typedef Model<MODEL>               Model_;
  typedef ModelAuxControl<MODEL>      ModelAux_;
  typedef Increment<MODEL>           Increment_;
  typedef State<MODEL>               State_;
  typedef Localization<MODEL>        Localization_;

 public:
// -----------------------------------------------------------------------------
  Dirac() {
    instantiateCovarFactory<MODEL>();
  }
// -----------------------------------------------------------------------------
  virtual ~Dirac() {}
// -----------------------------------------------------------------------------
  int execute(const eckit::Configuration & fullConfig) const {
//  Setup resolution
    const eckit::LocalConfiguration resolConfig(fullConfig, "resolution");
    const Geometry_ resol(resolConfig);
    Log::info() << "Setup resolution OK" << std::endl;

//  Setup variables
    const eckit::LocalConfiguration varConfig(fullConfig, "variables");
    const Variables vars(varConfig);
    Log::info() << "Setup variables OK" << std::endl;

//  Setup initial state
    const eckit::LocalConfiguration initialConfig(fullConfig, "initial");
    const State_ xx(resol, vars, initialConfig);
    Log::info() << "Setup initial state OK" << std::endl;

//  Setup time
    const util::DateTime bgndate(xx.validTime());
    Log::info() << "Setup times OK" << std::endl;

//  Setup Dirac and random increments
    Increment_ dxdir(resol, vars, bgndate);
    const eckit::LocalConfiguration diracConfig(fullConfig, "dirac");
    dxdir.dirac(diracConfig);
    Increment_ dxrnd(resol, vars, bgndate);
    dxrnd.random();
    Increment_ dxdirout(dxdir);
    Increment_ dxrndout(dxrnd);
    Increment_ x1(dxdir);
    Increment_ x2(dxdir);
    Increment_ x1save(dxdir);
    Increment_ x2save(dxdir);
    Log::info() << "Setup increments OK" << std::endl;

//  Setup B matrix
    const eckit::LocalConfiguration covarConfig(fullConfig, "Covariance");
    boost::scoped_ptr< ModelSpaceCovarianceBase<MODEL> > B(CovarianceFactory<MODEL>::create(
                                                           covarConfig, resol, vars, xx, xx));
    Log::info() << "Setup full ensemble B matrix OK" << std::endl;

    if (covarConfig.has("localization")) {
        //  Setup localization
        const eckit::LocalConfiguration locConfig(covarConfig, "localization");
        boost::scoped_ptr<Localization_> loc_;
        loc_.reset(new Localization_(resol, locConfig));
        Log::trace() << "Setup localization OK" << std::endl;

        //  Apply localization
        loc_->multiply(dxdirout);
        loc_->multiply(dxrndout);
        Log::trace() << "Apply localization OK" << std::endl;

        //  Write increment
        const eckit::LocalConfiguration output_localization(fullConfig, "output_localization");
        dxdirout.write(output_localization);
        Log::trace() << "Write increment OK" << std::endl;
        Log::test() << "Increment: " << dxrndout << std::endl;

        //  Test adjoint
        x1.random();
        x2.random();
        x1save = x1;
        x2save = x2;
        loc_->multiply(x1);
        loc_->multiply(x2);
        double p1 = x1.dot_product_with(x2save);
        double p2 = x2.dot_product_with(x1save);
        Log::test() << "Adjoint test: " << p1 << " / " << p2 << std::endl;
    }

//  Apply B matrix to Dirac increment
    B->multiply(dxdir, dxdirout);
    B->multiply(dxrnd, dxrndout);
    Log::info() << "Apply B matrix OK" << std::endl;

//  Write increment
    const eckit::LocalConfiguration output_B(fullConfig, "output_B");
    dxdirout.write(output_B);
    Log::trace() << "Write increment OK" << std::endl;
    Log::test() << "Increment: " << dxrndout << std::endl;

//  Test adjoint
    x1.random();
    x2.random();
    x1save = x1;
    x2save = x2;
    B->multiply(x1save, x1);
    B->multiply(x2save, x2);
    double p1 = x1.dot_product_with(x2save);
    double p2 = x2.dot_product_with(x1save);
    Log::test() << "Adjoint test: " << p1 << " / " << p2 << std::endl;

    return 0;
  }
// -----------------------------------------------------------------------------
 private:
  std::string appname() const {
    return "oops::Dirac<" + MODEL::name() + ">";
  }
// -----------------------------------------------------------------------------
};

}  // namespace oops
#endif  // OOPS_RUNS_DIRAC_H_
