/*
 * (C) Copyright 2018 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef OOPS_BASE_VARIABLECHANGEBASE_H_
#define OOPS_BASE_VARIABLECHANGEBASE_H_

#include <boost/noncopyable.hpp>

#include "oops/base/Variables.h"
#include "oops/interface/Geometry.h"
#include "oops/interface/Increment.h"
#include "oops/interface/State.h"
#include "oops/util/Printable.h"

namespace eckit {
  class Configuration;
}

namespace oops {

// -----------------------------------------------------------------------------

/// Base class for generic variable transform

template <typename MODEL>
class VariableChangeBase : public util::Printable,
                           private boost::noncopyable {
  typedef Increment<MODEL>           Increment_;
  typedef State<MODEL>               State_;

 public:
  explicit VariableChangeBase(const eckit::Configuration &);
  virtual ~VariableChangeBase() {}

  void setInputVariables(Variables);
  void setOutputVariables(Variables);
  virtual void linearize(const State_ &, const Geometry_ &) =0;

  virtual void transform(const Increment_ &, Increment_ &) const =0;
  virtual void transformInverse(const Increment_ &, Increment_ &) const =0;
  virtual void transformAdjoint(const Increment_ &, Increment_ &) const =0;
  virtual void transformInverseAdjoint(const Increment_ &, Increment_ &) const =0;

  Increment_ transform(const Increment_ &) const;
  Increment_ transformInverse(const Increment_ &) const;
  Increment_ transformAdjoint(const Increment_ &) const;
  Increment_ transformInverseAdjoint(const Increment_ &) const;

 private:
  virtual void print(std::ostream &) const =0;
  Variables varin_;
  Variables varout_;
};

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_BASE_VARIABLECHANGEBASE_H_
