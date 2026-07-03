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
#include "operations/version_operation.hpp"

#include <iostream>

#include "operations/basic_operation.hpp"
#include "operations/mode_type.hpp"

#include "app_version.hpp"

#include "util/command_line_helpers_fwd.hpp"

namespace operations {

generic_operation<mode_type::version>::generic_operation(
    util::command_line_arg_view cmd_args)
    : basic_operation{mode_type::version, cmd_args,
                      expected_number_of_arguments} {}

[[nodiscard]] bool generic_operation<mode_type::version>::execute() const {
  std::cout << app_version.get_string() << '\n';
  return true;
}

} // namespace operations
