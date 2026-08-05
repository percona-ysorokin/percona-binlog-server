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

#include "opensslpp/core_error.hpp"

#include <cassert>
#include <cstddef>
#include <string>
#include <string_view>

#include <openssl/err.h>

namespace opensslpp {

[[nodiscard]] std::string
core_error::generate_error_message(std::string_view prefix) {
  static constexpr std::string_view prefix_separator{": "};
  static constexpr std::string_view lib_reason_separator{"::"};

  const auto native_err{ERR_get_error()};
  if (native_err == 0L) {
    return (prefix.empty() ? std::string{} : std::string{prefix});
  }

  // ERR_get_error() pops an element from the error queue, so we need to ensure
  // that it is empty after the first call.
  assert(ERR_get_error() == 0L);

  const char *const lib_error_string{ERR_lib_error_string(native_err)};
  const std::string_view lib_error_string_sv{
      lib_error_string != nullptr ? lib_error_string : "<unknown library>"};

  const char *reason_error_string{ERR_reason_error_string(native_err)};
  const std::string_view reason_error_string_sv{reason_error_string != nullptr
                                                    ? reason_error_string
                                                    : "<unknown reason>"};

  const std::size_t max_message_length{
      (prefix.empty() ? 0U : prefix.size() + prefix_separator.size()) +
      std::size(lib_error_string_sv) + std::size(lib_reason_separator) +
      std::size(reason_error_string_sv)};
  std::string message{};
  message.reserve(max_message_length);
  if (!prefix.empty()) {
    message += prefix;
    message += prefix_separator;
  }
  message += lib_error_string_sv;
  message += lib_reason_separator;
  message += reason_error_string_sv;

  return message;
}

} // namespace opensslpp
