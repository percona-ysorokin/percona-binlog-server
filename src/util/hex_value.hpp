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

#ifndef UTIL_HEX_VALUE_HPP
#define UTIL_HEX_VALUE_HPP

#include "util/hex_value_fwd.hpp" // IWYU pragma: export

#include <cstddef>
#include <string>
#include <string_view>

#include <boost/container/small_vector.hpp>

#include "util/byte_span_fwd.hpp"

namespace util {

class [[nodiscard]] hex_value {
public:
  // small_vector is expected to have 24 bytes overhead
  static constexpr std::size_t inline_capacity{40U};
  // TODO: in c++26 change to std::inplace_vector
  using buffer_type =
      boost::container::small_vector<std::byte, inline_capacity>;

  hex_value() = default;
  explicit hex_value(const_byte_span data);
  explicit hex_value(std::string_view value_hex);

  [[nodiscard]] static bool try_parse(std::string_view value_sv,
                                      hex_value &value) noexcept;

  [[nodiscard]] std::string to_hex_string() const;

  [[nodiscard]] const_byte_span get_data() const noexcept {
    return {std::data(data_), std::size(data_)};
  }

  [[nodiscard]] bool is_empty() const noexcept { return data_.empty(); }
  [[nodiscard]] std::size_t get_size() const noexcept {
    return std::size(data_);
  }

  friend bool operator==(const hex_value &, const hex_value &) = default;

private:
  buffer_type data_;
};

} // namespace util

#endif // UTIL_HEX_VALUE_HPP
