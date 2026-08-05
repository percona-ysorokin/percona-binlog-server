// Copyright (c) 2023-2024 Percona and/or its affiliates.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, version 2.0,
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License, version 2.0, for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

#define BOOST_TEST_MODULE CryptoRndTests
// this include is needed as it provides the 'main()' function
// NOLINTNEXTLINE(misc-include-cleaner)
#include <boost/test/unit_test.hpp>

#include <array>
#include <cstddef>

#include <boost/test/unit_test_suite.hpp>

#include <boost/test/tools/old/interface.hpp>

#include "opensslpp/crypto_rng.hpp"

BOOST_AUTO_TEST_CASE(CryptoRndGenerateEmptyBuffer) {
  std::array<std::byte, 0U> output{};
  BOOST_CHECK_NO_THROW(opensslpp::crypto_rng::generate(output));
}

BOOST_AUTO_TEST_CASE(CryptoRndGenerateNonEmptyBuffer) {
  constexpr std::size_t output_size{64U};

  using buffer_type = std::array<std::byte, output_size>;
  buffer_type first_output;
  BOOST_CHECK_NO_THROW(opensslpp::crypto_rng::generate(first_output));

  buffer_type second_output;
  BOOST_CHECK_NO_THROW(opensslpp::crypto_rng::generate(second_output));

  BOOST_CHECK(first_output != second_output);
}
