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

#ifndef UTIL_FILE_OPERATIONS_HELPERS_HPP
#define UTIL_FILE_OPERATIONS_HELPERS_HPP

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>

namespace util {

[[nodiscard]] std::string read_file_content(const std::filesystem::path &path,
                                            std::size_t max_size,
                                            std::string_view error_label);

void write_file_content(const std::filesystem::path &path,
                        std::string_view content, std::string_view error_label);

} // namespace util

#endif // UTIL_FILE_OPERATIONS_HELPERS_HPP
