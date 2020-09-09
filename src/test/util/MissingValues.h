/*
 * (C) Copyright 2020 Met Office UK
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef TEST_UTIL_MISSINGVALUES_H_
#define TEST_UTIL_MISSINGVALUES_H_

#include <string>

#include "eckit/config/LocalConfiguration.h"
#include "eckit/testing/Test.h"
#include "oops/../test/TestEnvironment.h"
#include "oops/mpi/mpi.h"
#include "oops/runs/Test.h"
#include "oops/util/DateTime.h"
#include "oops/util/missingValues.h"

namespace test {

template <typename T>
void testMissingValues()
{
  T missing = util::missingValue(missing);
}

CASE("util/MissingValues/float") {
  testMissingValues<float>();
}

CASE("util/MissingValues/double") {
  testMissingValues<double>();
}

CASE("util/MissingValues/int16_t") {
  testMissingValues<int16_t>();
}

CASE("util/MissingValues/int32_t") {
  testMissingValues<int32_t>();
}

CASE("util/MissingValues/int64_t") {
  testMissingValues<int64_t>();
}

CASE("util/MissingValues/DateTime") {
  testMissingValues<util::DateTime>();
}

CASE("util/MissingValues/std::string") {
  testMissingValues<std::string>();
}

class MissingValues : public oops::Test {
 private:
  std::string testid() const override {return "test::MissingValues";}

  void register_tests() const override {}
};

}  // namespace test

#endif  // TEST_UTIL_MISSINGVALUES_H_
