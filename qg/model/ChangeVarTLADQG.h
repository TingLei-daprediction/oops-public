/*
 * (C) Copyright 2017-2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef QG_MODEL_CHANGEVARTLADQG_H_
#define QG_MODEL_CHANGEVARTLADQG_H_

#include <ostream>
#include <string>

#include "oops/util/Printable.h"
#include "ChangeVarTLADQGParams.h"

// Forward declarations
namespace eckit {
  class Configuration;
}

namespace oops {
  class Variables;
}

namespace qg {
  class GeometryQG;
  class StateQG;
  class IncrementQG;

// -----------------------------------------------------------------------------
/// QG linear change of variable

class ChangeVarTLADQG: public util::Printable {
 public:
  typedef ChangeVarTLADQGParams Parameters_;
  static const std::string classname() {return "qg::ChangeVarTLADQG";}

  ChangeVarTLADQG(const GeometryQG &, const Parameters_ &);
  ~ChangeVarTLADQG();

/// Perform linear transforms
  void multiply(IncrementQG &, const oops::Variables &) const;
  void multiplyInverse(IncrementQG &, const oops::Variables &) const;
  void multiplyAD(IncrementQG &, const oops::Variables &) const;
  void multiplyInverseAD(IncrementQG &, const oops::Variables &) const;

  void setTrajectory(const StateQG &, const StateQG &);

 private:
  void print(std::ostream &) const override;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_CHANGEVARTLADQG_H_
