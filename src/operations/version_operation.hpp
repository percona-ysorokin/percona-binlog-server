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

#ifndef OPERATIONS_VERSION_OPERATION_HPP
#define OPERATIONS_VERSION_OPERATION_HPP

#include "operations/generic_operation_fwd.hpp"

#include "operations/basic_operation.hpp"
#include "operations/mode_type.hpp"

#include "util/command_line_helpers_fwd.hpp"

namespace operations {

template <>
class generic_operation<mode_type::version> : public basic_operation {
public:
  static constexpr auto expected_number_of_arguments{0UZ};

  explicit generic_operation(util::command_line_arg_view cmd_args);
  [[nodiscard]] bool execute() const override;
};

} // namespace operations

#endif // OPERATIONS_VERSION_OPERATION_HPP
