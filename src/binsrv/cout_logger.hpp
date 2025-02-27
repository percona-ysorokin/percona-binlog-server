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

#ifndef BINSRV_OSTREAM_LOGGER_HPP
#define BINSRV_OSTREAM_LOGGER_HPP

#include <string_view>

#include "binsrv/basic_logger.hpp" // IWYU pragma: export

namespace binsrv {

class [[nodiscard]] cout_logger : public basic_logger {
public:
  explicit cout_logger(log_severity min_level) : basic_logger{min_level} {}

private:
  void do_log(std::string_view message) override;
};

} // namespace binsrv

#endif // BINSRV_OSTREAM_LOGGER_HPP
