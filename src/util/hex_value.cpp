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

#include "util/hex_value.hpp"

#include <cstddef>
#include <exception>
#include <ios>
#include <istream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <boost/algorithm/hex.hpp>

#include "util/byte_span.hpp"
#include "util/exception_location_helpers.hpp"

namespace util {

hex_value::hex_value(const_byte_span data)
    : data_{std::begin(data), std::end(data)} {}

hex_value::hex_value(std::string_view value_hex) {
  if ((std::size(value_hex) % 2U) != 0U) {
    exception_location().raise<std::invalid_argument>(
        "invalid hex_value length");
  }

  data_.resize(std::size(value_hex) / 2U);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  char *const out_it{reinterpret_cast<char *>(std::data(data_))};
  try {
    boost::algorithm::unhex(value_hex, out_it);
  } catch (const std::exception &) {
    exception_location().raise<std::invalid_argument>(
        "invalid hex_value characters");
  }
}

[[nodiscard]] bool hex_value::try_parse(std::string_view value_sv,
                                        hex_value &value) noexcept {
  try {
    value = hex_value(value_sv);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

[[nodiscard]] std::string hex_value::to_hex_string() const {
  std::string result(std::size(data_) * 2U, '\0');
  const auto data_sv{util::as_string_view(get_data())};
  boost::algorithm::hex(data_sv, std::begin(result));
  return result;
}

std::ostream &operator<<(std::ostream &output, const hex_value &value) {
  return output << value.to_hex_string();
}

std::istream &operator>>(std::istream &input, hex_value &value) {
  std::string value_str;
  input >> value_str;
  if (!input) {
    return input;
  }

  hex_value parsed_value;
  if (!hex_value::try_parse(value_str, parsed_value)) {
    input.setstate(std::ios_base::failbit);
    return input;
  }

  value = std::move(parsed_value);
  return input;
}

} // namespace util
