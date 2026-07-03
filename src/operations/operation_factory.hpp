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

#ifndef OPERATIONS_OPERATION_FACTORY_HPP
#define OPERATIONS_OPERATION_FACTORY_HPP

#include "operations/basic_operation_fwd.hpp"

#include "util/command_line_helpers_fwd.hpp"

namespace operations {

class operation_factory {
public:
  [[nodiscard]] static basic_operation_ptr
  create(util::command_line_arg_view cmd_args);
};

} // namespace operations

#endif // OPERATIONS_OPERATION_FACTORY_HPP
