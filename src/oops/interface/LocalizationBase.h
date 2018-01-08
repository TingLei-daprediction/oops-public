/*
* Copyright 2011 ECMWF
*
* This software was developed at ECMWF for evaluation
* and may be used for academic and research purposes only.
* The software is provided as is without any warranty.
*
* This software can be used, copied and modified but not
* redistributed or sold. This notice must be reproduced
* on each copy made.
*/

#ifndef OOPS_INTERFACE_LOCALIZATIONBASE_H_
#define OOPS_INTERFACE_LOCALIZATIONBASE_H_

#include <boost/noncopyable.hpp>
#include <map>
#include <string>
#include <mpi.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "oops/interface/Geometry.h"
#include "oops/interface/Increment.h"
#include "eckit/config/Configuration.h"
#include "util/abor1_cpp.h"
#include "util/Printable.h"

namespace oops {

// -----------------------------------------------------------------------------
/// Base class for localizations

template<typename MODEL>
class LocalizationBase : public util::Printable,
                         private boost::noncopyable {
  typedef Increment<MODEL>        Increment_;

 public:
  LocalizationBase() {}
  virtual ~LocalizationBase() {}

  void multiply(Increment_ &) const;

 private:
  virtual void multiply(typename MODEL::Increment &) const =0;
  virtual void print(std::ostream &) const =0;
};

// =============================================================================

/// LocalizationFactory Factory
template <typename MODEL>
class LocalizationFactory {
  typedef Geometry<MODEL> Geometry_;
 public:
  static LocalizationBase<MODEL> * create(const Geometry_ &, const eckit::Configuration &);
  virtual ~LocalizationFactory() { getMakers().clear(); }
 protected:
  explicit LocalizationFactory(const std::string &);
 private:
  virtual LocalizationBase<MODEL> * make(const Geometry_ &, const eckit::Configuration &) =0;
  static std::map < std::string, LocalizationFactory<MODEL> * > & getMakers() {
    static std::map < std::string, LocalizationFactory<MODEL> * > makers_;
    return makers_;
  }
};

// -----------------------------------------------------------------------------

template<class MODEL, class T>
class LocalizationMaker : public LocalizationFactory<MODEL> {
  typedef Geometry<MODEL> Geometry_;
  virtual LocalizationBase<MODEL> * make(const Geometry_ & resol, const eckit::Configuration & conf)
    { return new T(resol.geometry(), conf); }
 public:
  explicit LocalizationMaker(const std::string & name) : LocalizationFactory<MODEL>(name) {}
};

// -----------------------------------------------------------------------------

template <typename MODEL>
LocalizationFactory<MODEL>::LocalizationFactory(const std::string & name) {
  if (getMakers().find(name) != getMakers().end()) {
    Log::error() << name << " already registered in localization factory." << std::endl;
    ABORT("Element already registered in LocalizationFactory.");
  }
  getMakers()[name] = this;
}

// -----------------------------------------------------------------------------

template <typename MODEL>
LocalizationBase<MODEL>* LocalizationFactory<MODEL>::create(const Geometry_ & resol,
                                                            const eckit::Configuration & conf) {
  Log::trace() << "LocalizationBase<MODEL>::create starting" << std::endl;
  const std::string id = conf.getString("localization");
  typename std::map<std::string, LocalizationFactory<MODEL>*>::iterator
    jloc = getMakers().find(id);
  if (jloc == getMakers().end()) {
    Log::trace() << id << " does not exist in localization factory." << std::endl;
    ABORT("Element does not exist in LocalizationFactory.");
  }
  LocalizationBase<MODEL> * ptr = jloc->second->make(resol, conf);
  Log::trace() << "LocalizationBase<MODEL>::create done" << std::endl;
  return ptr;
}

// -----------------------------------------------------------------------------

template <typename MODEL>
void LocalizationBase<MODEL>::multiply(Increment_ & dx) const {
  Log::trace() << "LocalizationBase<MODEL>::multiply starting" << std::endl;
  MPI_Barrier(MPI_COMM_WORLD);
  boost::posix_time::ptime ti = boost::posix_time::microsec_clock::local_time();
  this->multiply(dx.increment());
  boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();
  boost::posix_time::time_duration diff = t - ti;
  Log::info() << "Localization time: " << diff.total_nanoseconds()/1000 << " microseconds" << std::endl;
  Log::trace() << "LocalizationBase<MODEL>::multiply done" << std::endl;
}

// -----------------------------------------------------------------------------

}  // namespace oops

#endif  // OOPS_INTERFACE_LOCALIZATIONBASE_H_
