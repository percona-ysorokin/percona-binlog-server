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
#include "operations/basic_operation.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>

#include "util/command_line_helpers_fwd.hpp"
#include "util/exception_location_helpers.hpp"

namespace operations {

basic_operation::basic_operation(util::command_line_arg_view cmd_args,
                                 std::size_t expected_number_of_arguments)
    : cmd_args_{cmd_args} {
  // this assertion is ensured by the 'operation_factory::create()' method
  assert(std::size(cmd_args) >= 2UZ);
  // +2 is needed because 'cmd_args' always includes the name of the executable
  // and it should also include the name of the operation
  if (std::size(cmd_args) != expected_number_of_arguments + 2UZ) {
    util::exception_location().raise<std::invalid_argument>(
        "unexpected number of command line arguments for the '" +
        std::string{cmd_args_[1UZ]} + "' operation");
  }
}

} // namespace operations
