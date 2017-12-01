/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_ERRORCOVARIANCEQG_H_
#define QG_MODEL_ERRORCOVARIANCEQG_H_

#include <ostream>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "model/QgFortran.h"
#include "model/GeometryQG.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"
#include "util/ObjectCounter.h"
#include "util/Printable.h"

// Forward declarations
namespace oops {
  class Variables;
}

namespace qg {
  class IncrementQG;
  class StateQG;

// -----------------------------------------------------------------------------
/// Background error covariance matrix for QG model.

class ErrorCovarianceQG : public util::Printable,
                          private boost::noncopyable,
                          private util::ObjectCounter<ErrorCovarianceQG> {
 public:
  static const std::string classname() {return "qg::ErrorCovarianceQG";}

  ErrorCovarianceQG(const GeometryQG &, const oops::Variables &, const eckit::Configuration &, const StateQG &);
  ~ErrorCovarianceQG();

  void linearize(const StateQG &, const GeometryQG &);
  void multiply(const IncrementQG &, IncrementQG &) const;
  void inverseMultiply(const IncrementQG &, IncrementQG &) const;
  void randomize(IncrementQG &) const;

 private:
  void print(std::ostream &) const;
  F90bmat keyFtnConfig_;
  boost::scoped_ptr<const GeometryQG> geom_;
  util::DateTime time_;
};
// -----------------------------------------------------------------------------

}  // namespace qg
#endif  // QG_MODEL_ERRORCOVARIANCEQG_H_
