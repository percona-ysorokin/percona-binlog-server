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

#include "opensslpp/crypto_rng.hpp"

#include <iterator>
#include <utility>

#include <openssl/rand.h>

#include "opensslpp/core_error.hpp"

#include "util/byte_span_fwd.hpp"
#include "util/exception_location_helpers.hpp"

namespace opensslpp {

void crypto_rng::generate(util::byte_span output) {
  if (output.empty()) {
    return;
  }

  if (!std::in_range<int>(std::size(output))) {
    util::exception_location().raise<core_error>(
        "crypto rng output size is out of range");
  }
  const auto native_output_size{static_cast<int>(std::size(output))};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (RAND_bytes(reinterpret_cast<unsigned char *>(std::data(output)),
                 native_output_size) != 1) {
    util::exception_location().raise<core_error>(
        "cannot generate random bytes using crypto rng");
  }
}

} // namespace opensslpp
