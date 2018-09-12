/*
 * (C) Copyright 2009-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <fstream>
#include <iostream>

#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include "./TestConfig.h"
#include "eckit/config/LocalConfiguration.h"
#include "lorenz95/GomL95.h"
#include "lorenz95/IncrementL95.h"
#include "lorenz95/LocsL95.h"
#include "lorenz95/Resolution.h"
#include "lorenz95/StateL95.h"
#include "oops/base/Variables.h"
#include "oops/util/DateTime.h"
#include "test/TestFixture.h"

namespace test {

// -----------------------------------------------------------------------------
class IncrementTestFixture : TestFixture {
 public:
  IncrementTestFixture() {
    file_.reset(new eckit::LocalConfiguration(TestConfig::config(), "state"));
    eckit::LocalConfiguration res(TestConfig::config(), "resolution");
    resol_.reset(new lorenz95::Resolution(res));
    date_str_ = file_->getString("date");
    time_.reset(new util::DateTime(date_str_));
    vars_.reset(new oops::Variables(TestConfig::config()));
  }
  ~IncrementTestFixture() {}
  boost::scoped_ptr<const eckit::LocalConfiguration> file_;
  boost::scoped_ptr<lorenz95::Resolution> resol_;
  std::string date_str_;
  boost::scoped_ptr<util::DateTime> time_;
  boost::scoped_ptr<oops::Variables> vars_;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_SUITE(test_IncrementL95, IncrementTestFixture)

// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_constructor) {
    boost::scoped_ptr<lorenz95::IncrementL95>
      dx(new lorenz95::IncrementL95(*resol_, *vars_, *time_));
    BOOST_CHECK(dx.get() != NULL);
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_interpolation_constructor) {
    boost::scoped_ptr<lorenz95::IncrementL95>
      dx1(new lorenz95::IncrementL95(*resol_, *vars_, *time_));
    boost::scoped_ptr<lorenz95::IncrementL95>
      dx2(new lorenz95::IncrementL95(*resol_, *dx1));
    BOOST_CHECK(dx2.get() != NULL);
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_copy_constructor) {
    boost::scoped_ptr<lorenz95::IncrementL95>
      dx1(new lorenz95::IncrementL95(*resol_, *vars_, *time_));
    boost::scoped_ptr<lorenz95::IncrementL95> dx2(new lorenz95::IncrementL95(*dx1));
    BOOST_CHECK(dx2.get() != NULL);
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_diff) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    // construct the first stateL95 object
    lorenz95::StateL95 xx1(*resol_, *vars_, tt);

    // read in the state config info
    xx1.read(*file_);
    lorenz95::StateL95 xx2(xx1);

    // to vary the results a little, change the second StateL95 field values
    double fact = 0.75;
    dx *= fact;
    xx2 += dx;

    dx.diff(xx1, xx2);

    // to verify the diff method has worked correctly, we need
    // to open and read the file containing the FieldL95
    std::string filename(file_->getString("filename"));
    std::ifstream inStream(filename.c_str());
    if (!inStream.is_open()) {
      BOOST_ERROR("diff functionality cannot be determined");
    }

    // we read in these two values but do not use them
    int resolInt;
    inStream >> resolInt;
    std::string time;
    inStream >> time;

    std::vector<double> doubleVec(resolInt);
    for (int i = 0; i < resolInt; ++i) {
      inStream >> doubleVec[i];
    }
    inStream.close();

    for (int i = 0; i < resol_->npoints(); ++i) {
      BOOST_CHECK_CLOSE((dx.getField())[i],
                         doubleVec[i] - (doubleVec[i] + (doubleVec[i] * fact)),
                         1.0e-6);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_zero) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    // first check that we have good data, ie, at least one element is non-zero
    bool goodData = false;
    for (int i = 0; i < resol_->npoints() && goodData == false; ++i) {
      if ((dx.getField())[i] != 0) {
        goodData = true;
      }
    }

    if (!goodData) {
      BOOST_ERROR("unable to test zero method, since test data is already all zero");
    } else {
      dx.zero();

      for (int i = 0; i < resol_->npoints(); ++i) {
        BOOST_CHECK_EQUAL(dx.getField()[i], 0);
      }
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_zero_set_datetime) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    // first check that we have good data, ie, at least one element is non-zero
    bool goodData = false;
    for (int i = 0; i < resol_->npoints() && goodData == false; ++i) {
      if (dx.getField()[i] != 0) {
        goodData = true;
      }
    }

    if (!goodData) {
      BOOST_ERROR("unable to test zero method, since test data is already all zero");
    } else {
      const std::string modified_date_string("2010-01-01T10:35:00Z");
      const util::DateTime dtModified(modified_date_string);
      dx.zero(dtModified);

      for (int i = 0; i < resol_->npoints(); ++i) {
        BOOST_CHECK_EQUAL(dx.getField()[i], 0);
      }

      BOOST_CHECK(dx.validTime().toString() != date_str_);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_assignment) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // construct the second dx object
    lorenz95::IncrementL95 dx2(*resol_, *vars_, tt);
    double fact = 0.75;
    dx2.read(*file_);
    dx2 *= fact;

    dx1 = dx2;

    for (int i = 0; i < dx1.getField().resol(); ++i) {
      BOOST_CHECK_EQUAL(dx1.getField()[i], dx2.getField()[i]);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_compound_assignment_add) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // copy construct the second stateL95 object
    lorenz95::IncrementL95 dx2(dx1);

    dx1 += dx2;

    // since the two IncrementL95 objects started off with the same data,
    // once they've been added together incL591 will be double what incL952 is
    for (int i = 0; i < dx1.getField().resol(); ++i) {
      BOOST_CHECK_EQUAL(dx1.getField()[i], 2.0 * dx2.getField()[i]);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_compound_assignment_subtract) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // copy construct the second stateL95 object
    lorenz95::IncrementL95 dx2(dx1);

    dx1 -= dx2;

    // since the two IncrementL95 objects started off with the same data,
    // once incL952 has been subtracted from incL951, the result is zero
    for (int i = 0; i < dx1.getField().resol(); ++i) {
      BOOST_CHECK_EQUAL(dx1.getField()[i], 0.0);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_compound_assignment_multiply) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    // create a copy of the original data for testing against
    std::vector<double> testData(dx.getField().resol());
    for (unsigned int ii = 0; ii < testData.size(); ++ii) {
      testData.at(ii) = dx.getField()[ii];
    }

    double fact = 0.75;
    dx *= fact;

    for (int ii = 0; ii < dx.getField().resol(); ++ii) {
      BOOST_CHECK_EQUAL(dx.getField()[ii], testData.at(ii) * fact);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_axpy) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // copy construct the second stateL95 object
    lorenz95::IncrementL95 dx2(dx1);

    double fact = 0.75;
    dx1.axpy(fact, dx2);

    for (int i = 0; i < dx1.getField().resol(); ++i) {
      BOOST_CHECK_EQUAL(dx1.getField()[i], dx2.getField()[i] + fact * dx2.getField()[i]);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_dot_product_with) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // copy construct the second stateL95 object
    lorenz95::IncrementL95 dx2(dx1);

    double dpwResult = dx1.dot_product_with(dx2);

    // prepare a value to test against
    double testResult = 0.0;
    for (int i = 0; i < dx1.getField().resol(); ++i) {
      testResult += (dx1.getField()[i] * dx2.getField()[i]);
    }

    BOOST_CHECK_EQUAL(dpwResult, testResult);
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_schur_product_with) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx1(*resol_, *vars_, tt);
    dx1.read(*file_);

    // copy construct the second stateL95 object
    lorenz95::IncrementL95 dx2(dx1);

    dx1.schur_product_with(dx2);

    // both incL951 and incL952 started off with the same data in x_,
    // so to test incL951 against incL952xincL952 is a valid test
    for (int i = 0; i < dx1.getField().resol(); ++i) {
      BOOST_CHECK_EQUAL(dx1.getField()[i], dx2.getField()[i] * dx2.getField()[i]);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_read) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    // to verify the information has been read correctly, we need to open
    // and read the file using ifstream functionality
    const std::string filename(file_->getString("filename"));
    std::ifstream inStream(filename.c_str());
    if (!inStream.is_open()) {
      BOOST_ERROR("read functionality cannot be determined");
    }

    int resolInt;
    inStream >> resolInt;

    std::string time;
    inStream >> time;

    std::vector<double> doubleVec(resolInt);
    for (int i = 0; i < resolInt; ++i) {
      inStream >> doubleVec[i];
    }
    inStream.close();

    for (int i = 0; i < resol_->npoints(); ++i) {
      BOOST_CHECK_EQUAL(dx.getField()[i], doubleVec[i]);
    }
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_write) {
    util::DateTime tt(file_->getString("date"));
    lorenz95::IncrementL95 dx(*resol_, *vars_, tt);
    dx.read(*file_);

    eckit::LocalConfiguration opFileCfg(TestConfig::config(), "outputFile");
    dx.write(opFileCfg);

    // Should read back in and compare values
  }
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_validTime) {
    lorenz95::IncrementL95 dx(*resol_, *vars_, *time_);
    BOOST_CHECK_EQUAL(dx.validTime().toString(), date_str_);
  }
// -----------------------------------------------------------------------------
/*
  BOOST_AUTO_TEST_CASE(test_incrementL95_stream_output) {
    lorenz95::IncrementL95 dx(*resol_, *vars_, *time_);

    // use the operator<< method to write the value to a file
    std::filebuf fb;
    std::string filename("IncrementL95Test.txt");
    fb.open(filename.c_str(), std::ios::out);
    std::ostream os(&fb);
    os << dx;
    fb.close();

    // then read the value that was written to the file
    std::string input;
    std::string inputTest(" Valid time: " + date_str_);
    std::ifstream inputFile(filename.c_str());
    if (inputFile.is_open()) {
      getline(inputFile, input);  // ignore the first (blank) line
      getline(inputFile, input);

      BOOST_CHECK_EQUAL(input, inputTest);
    } else {
      // if we can't open the file then we can't
      // verify that the value was correctly written
      BOOST_ERROR("operator<< functionality cannot be determined");
    }
    inputFile.close();
  }
*/
// -----------------------------------------------------------------------------
  BOOST_AUTO_TEST_CASE(test_incrementL95_getField) {
    lorenz95::IncrementL95 dx(*resol_, *vars_, *time_);

    // there are 2 values in FieldL95: the 1st is the *resol_ value,
    // the 2nd is a vector of doubles initialised to 0.0, the size of the
    // vector is the *resol_ value (we're just checking the final one)
    BOOST_CHECK_EQUAL(dx.getField().resol(), resol_->npoints());
  }
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
}  // namespace test
