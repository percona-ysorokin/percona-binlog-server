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

#ifndef OPERATIONS_BASIC_OPERATION_HPP
#define OPERATIONS_BASIC_OPERATION_HPP

#include "operations/basic_operation_fwd.hpp" // IWYU pragma: export

#include <cstdint>

#include "operations/mode_type_fwd.hpp"

#include "util/command_line_helpers_fwd.hpp"

namespace operations {

class basic_operation {
public:
  basic_operation(const basic_operation &) = delete;
  basic_operation &operator=(const basic_operation &) = delete;
  basic_operation(basic_operation &&) = delete;
  basic_operation &operator=(basic_operation &&) = delete;

  virtual ~basic_operation() = default;

  [[nodiscard]] virtual bool execute() const = 0;

protected:
  basic_operation(mode_type mode, util::command_line_arg_view cmd_args,
                  std::size_t expected_number_of_arguments);

  [[nodiscard]] util::command_line_arg_view get_cmd_args() const noexcept {
    return cmd_args_;
  }
  [[nodiscard]] mode_type get_mode() const noexcept { return mode_; }

private:
  mode_type mode_;
  util::command_line_arg_view cmd_args_;
};

} // namespace operations

#endif // OPERATIONS_BASIC_OPERATION_HPP
