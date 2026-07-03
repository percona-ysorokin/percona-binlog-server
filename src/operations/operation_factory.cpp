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
#include "operations/operation_factory.hpp"

#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

#include <boost/lexical_cast/try_lexical_convert.hpp>

#include "operations/basic_operation_fwd.hpp"
#include "operations/fetch_operation.hpp" // IWYU pragma: export
#include "operations/generic_operation_fwd.hpp"
#include "operations/list_operation.hpp" // IWYU pragma: export
#include "operations/mode_type.hpp"
#include "operations/pull_operation.hpp"                // IWYU pragma: export
#include "operations/purge_binlogs_operation.hpp"       // IWYU pragma: export
#include "operations/search_by_gtid_set_operation.hpp"  // IWYU pragma: export
#include "operations/search_by_timestamp_operation.hpp" // IWYU pragma: export
#include "operations/version_operation.hpp"             // IWYU pragma: export

#include "util/command_line_helpers_fwd.hpp"
#include "util/conversion_helpers.hpp"
#include "util/exception_location_helpers.hpp"

namespace operations {

[[nodiscard]] basic_operation_ptr
operation_factory::create(util::command_line_arg_view cmd_args) {
  // 'cmd_args' always includes the name of the executable
  // and it should also include at least the name of the operation
  static constexpr auto min_number_of_cmd_args{2UZ};

  if (std::size(cmd_args) < min_number_of_cmd_args) {
    util::exception_location().raise<std::invalid_argument>(
        "insufficient number of command line arguments");
  }

  mode_type operation_mode{mode_type::delimiter};

  if (!boost::conversion::try_lexical_convert(cmd_args[1UZ], operation_mode)) {
    util::exception_location().raise<std::invalid_argument>(
        "invalid operation mode specified");
  }

  if (operation_mode == mode_type::delimiter) {
    util::exception_location().raise<std::invalid_argument>(
        "invalid operation mode specified");
  }

  static constexpr auto number_of_operations{
      util::enum_to_index(mode_type::delimiter)};

  static constexpr auto make_functions{
      []<std::size_t... IndexPack>(
          std::index_sequence<IndexPack...>) constexpr {
        return std::array{+[](util::command_line_arg_view cmd_args_value)
                              -> basic_operation_ptr {
          return std::make_unique<
              generic_operation<util::index_to_enum<mode_type>(IndexPack)>>(
              cmd_args_value);
        }...};
      }(std::make_index_sequence<number_of_operations>{})};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  return make_functions[util::enum_to_index(operation_mode)](cmd_args);
}

} // namespace operations
