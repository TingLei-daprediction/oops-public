/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <memory>


#include "./TestConfig.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/testing/Test.h"
#include "lorenz95/ModelBiasCorrection.h"
#include "lorenz95/ModelBiasCovariance.h"
#include "lorenz95/Resolution.h"
#include "oops/mpi/mpi.h"
#include "oops/runs/Test.h"
#include "test/TestFixture.h"

namespace test {

// -----------------------------------------------------------------------------
class ModBiasCovTestFixture : TestFixture {
 public:
  ModBiasCovTestFixture() {
    eckit::LocalConfiguration res(TestConfig::config(), "geometry");
    resol_.reset(new lorenz95::Resolution(res, oops::mpi::world()));
    covconf_.reset(new eckit::LocalConfiguration(TestConfig::config(), "model aux error"));
    nobias_.reset(new eckit::LocalConfiguration());
  }
  ~ModBiasCovTestFixture() {}
  std::unique_ptr<lorenz95::Resolution> resol_;
  std::unique_ptr<const eckit::LocalConfiguration> covconf_;
  std::unique_ptr<const eckit::LocalConfiguration> nobias_;
};
// -----------------------------------------------------------------------------
CASE("test_modelBiasCovariance") {
  ModBiasCovTestFixture f;
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_constructor_conf") {
    lorenz95::ModelBiasCovariance bcovar(*f.covconf_, *f.resol_);
    EXPECT(bcovar.active() == true);
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_constructor_no_conf") {
    lorenz95::ModelBiasCovariance bcovar(*f.nobias_, *f.resol_);
    EXPECT(bcovar.active() == false);
}
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_linearize") {
    // not yet implemented
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_multiply_activei") {
    // construct the ModelBiasCorrection object
    lorenz95::ModelBiasCovariance bcovar(*f.covconf_, *f.resol_);
    lorenz95::ModelBiasCorrection dbias1(*f.resol_, *f.covconf_);
    dbias1.bias() = 2.0;
    lorenz95::ModelBiasCorrection dbias2(dbias1, true);

    bcovar.multiply(dbias1, dbias2);

    double stdev = f.covconf_->getDouble("standard_deviation");
    EXPECT(dbias2.bias() == dbias1.bias() * stdev * stdev);
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_multiply_inactive") {
    // construct the ModelBiasCorrection object
    lorenz95::ModelBiasCovariance bcovar(*f.nobias_, *f.resol_);
    lorenz95::ModelBiasCorrection dbias1(*f.resol_, *f.covconf_);
    dbias1.bias() = 2.0;
    lorenz95::ModelBiasCorrection dbias2(dbias1, true);

    bcovar.multiply(dbias1, dbias2);

    // because the covconf_ is empty, the bias is set to 0
    EXPECT(dbias2.bias() == 0.0);
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_invMult_active") {
    // construct the ModelBiasCorrection object
    lorenz95::ModelBiasCovariance bcovar(*f.covconf_, *f.resol_);
    lorenz95::ModelBiasCorrection dbias1(*f.resol_, *f.covconf_);
    dbias1.bias() = 2.0;
    lorenz95::ModelBiasCorrection dbias2(dbias1, true);

    bcovar.inverseMultiply(dbias1, dbias2);

    double stdev = f.covconf_->getDouble("standard_deviation");
    EXPECT(dbias2.bias() == dbias1.bias() *1.0 / (stdev * stdev));
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_invMult_inactive") {
    // construct the ModelBiasCorrection object
    lorenz95::ModelBiasCovariance bcovar(*f.nobias_, *f.resol_);
    lorenz95::ModelBiasCorrection dbias1(*f.resol_, *f.covconf_);
    dbias1.bias() = 2.0;
    lorenz95::ModelBiasCorrection dbias2(dbias1, true);

    bcovar.inverseMultiply(dbias1, dbias2);

    // because the covconf_ is empty, the bias is set to 0
    EXPECT(dbias2.bias() == 0.0);
  }
// -----------------------------------------------------------------------------
  SECTION("test_modelBiasCovariance_active") {
    lorenz95::ModelBiasCovariance bcovar(*f.covconf_, *f.resol_);
    EXPECT(bcovar.active() == true);
  }
// -----------------------------------------------------------------------------
}  //  CASE
// -----------------------------------------------------------------------------
}  // namespace test
int main(int argc, char **argv)
{
    return eckit::testing::run_tests ( argc, argv );
}
