/*
 * (C) Copyright 2009-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef QG_MODEL_INSTANTIATEQGCHANGEVARFACTORY_H_
#define QG_MODEL_INSTANTIATEQGCHANGEVARFACTORY_H_

#include "model/ChangeVar.h"
#include "model/QgTraits.h"
#include "oops/interface/VariableChange.h"

namespace qg {

void instantiateQgChangeVarFactory() {
  static oops::VariableChangeMaker<qg::QgTraits,
                                   oops::VariableChange<qg::QgTraits, qg::ChangeVar> >
               makerChangeVarQG_("ChVarQG");
}

}  // namespace qg

#endif  // QG_MODEL_INSTANTIATEQGCHANGEVARFACTORY_H_
