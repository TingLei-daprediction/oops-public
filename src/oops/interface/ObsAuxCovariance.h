/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef OOPS_INTERFACE_OBSAUXCOVARIANCE_H_
#define OOPS_INTERFACE_OBSAUXCOVARIANCE_H_

#include <iostream>
#include <memory>
#include <string>

#include <boost/noncopyable.hpp>

#include "eckit/config/Configuration.h"
#include "oops/interface/ObsAuxControl.h"
#include "oops/interface/ObsAuxIncrement.h"
#include "oops/interface/ObsSpace.h"
#include "oops/util/Logger.h"
#include "oops/util/ObjectCounter.h"
#include "oops/util/Printable.h"
#include "oops/util/Timer.h"

namespace oops {

// -----------------------------------------------------------------------------

template <typename OBS>
class ObsAuxCovariance : public util::Printable,
                         private boost::noncopyable,
                         private util::ObjectCounter<ObsAuxCovariance<OBS> > {
  typedef typename OBS::ObsAuxCovariance          ObsAuxCovariance_;
  typedef ObsAuxControl<OBS>                      ObsAuxControl_;
  typedef ObsAuxIncrement<OBS>                    ObsAuxIncrement_;

 public:
  typedef typename ObsAuxCovariance_::Parameters_ Parameters_;

  static const std::string classname() {return "oops::ObsAuxCovariance";}

  ObsAuxCovariance(const ObsSpace<OBS> &, const Parameters_ &);
  ~ObsAuxCovariance();

/// Operators
  void linearize(const ObsAuxControl_ &, const eckit::Configuration &);
  void multiply(const ObsAuxIncrement_ &, ObsAuxIncrement_ &) const;
  void inverseMultiply(const ObsAuxIncrement_ &, ObsAuxIncrement_ &) const;
  void randomize(ObsAuxIncrement_ &) const;

 private:
  void print(std::ostream &) const;
  std::unique_ptr<ObsAuxCovariance_> cov_;
};

// =============================================================================

template<typename OBS>
ObsAuxCovariance<OBS>::ObsAuxCovariance(const ObsSpace<OBS> & os,
                                          const Parameters_ & params) : cov_()
{
  Log::trace() << "ObsAuxCovariance<OBS>::ObsAuxCovariance starting" << std::endl;
  util::Timer timer(classname(), "ObsAuxCovariance");
  cov_.reset(new ObsAuxCovariance_(os.obsspace(), params));
  Log::trace() << "ObsAuxCovariance<OBS>::ObsAuxCovariance done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
ObsAuxCovariance<OBS>::~ObsAuxCovariance() {
  Log::trace() << "ObsAuxCovariance<OBS>::~ObsAuxCovariance starting" << std::endl;
  util::Timer timer(classname(), "~ObsAuxCovariance");
  cov_.reset();
  Log::trace() << "ObsAuxCovariance<OBS>::~ObsAuxCovariance done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
void ObsAuxCovariance<OBS>::linearize(const ObsAuxControl_ & xx,
                                      const eckit::Configuration & innerConf) {
  Log::trace() << "ObsAuxCovariance<OBS>::linearize starting" << std::endl;
  util::Timer timer(classname(), "linearize");
  cov_->linearize(xx.obsauxcontrol(), innerConf);
  Log::trace() << "ObsAuxCovariance<OBS>::linearize done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
void ObsAuxCovariance<OBS>::multiply(const ObsAuxIncrement_ & dx1, ObsAuxIncrement_ & dx2) const {
  Log::trace() << "ObsAuxCovariance<OBS>::multiply starting" << std::endl;
  util::Timer timer(classname(), "multiply");
  cov_->multiply(dx1.obsauxincrement(), dx2.obsauxincrement());
  Log::trace() << "ObsAuxCovariance<OBS>::multiply done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
void ObsAuxCovariance<OBS>::inverseMultiply(const ObsAuxIncrement_ & dx1,
                                              ObsAuxIncrement_ & dx2) const {
  Log::trace() << "ObsAuxCovariance<OBS>::inverseMultiply starting" << std::endl;
  util::Timer timer(classname(), "inverseMultiply");
  cov_->inverseMultiply(dx1.obsauxincrement(), dx2.obsauxincrement());
  Log::trace() << "ObsAuxCovariance<OBS>::inverseMultiply done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
void ObsAuxCovariance<OBS>::randomize(ObsAuxIncrement_ & dx) const {
  Log::trace() << "ObsAuxCovariance<OBS>::randomize starting" << std::endl;
  util::Timer timer(classname(), "randomize");
  cov_->randomize(dx.obsauxincrement());
  Log::trace() << "ObsAuxCovariance<OBS>::randomize done" << std::endl;
}

// -----------------------------------------------------------------------------

template<typename OBS>
void ObsAuxCovariance<OBS>::print(std::ostream & os) const {
  Log::trace() << "ObsAuxCovariance<OBS>::print starting" << std::endl;
  util::Timer timer(classname(), "print");
  os << *cov_;
  Log::trace() << "ObsAuxCovariance<OBS>::print done" << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_INTERFACE_OBSAUXCOVARIANCE_H_
