/*
 * (C) Copyright 2019-2021 UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "model/QgTraits.h"
#include "oops/runs/LocalEnsembleDA.h"
#include "oops/runs/Run.h"

int main(int argc,  char ** argv) {
  oops::Run run(argc, argv);
  oops::LocalEnsembleDA<qg::QgTraits, qg::QgObsTraits> letkf;
  return run.execute(letkf);
}
