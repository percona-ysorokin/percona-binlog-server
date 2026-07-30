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

#include "util/file_operations_helpers.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <string_view>

#include "util/exception_location_helpers.hpp"

namespace util {

[[nodiscard]] std::string read_file_content(const std::filesystem::path &path,
                                            std::size_t max_size,
                                            std::string_view error_label) {
  // opening in binary mode
  std::ifstream ifs{};
  ifs.rdbuf()->pubsetbuf(nullptr, 0U);
  ifs.open(path, std::ios_base::in | std::ios_base::binary);
  if (!ifs.is_open()) {
    util::exception_location().raise<std::runtime_error>(
        "cannot open " + std::string{error_label});
  }
  if (!ifs.seekg(0, std::ios_base::end)) {
    util::exception_location().raise<std::runtime_error>(
        "cannot seek " + std::string{error_label} + " to the end");
  }
  const std::streampos end_pos{ifs.tellg()};
  const auto end_offset{static_cast<std::streamoff>(end_pos)};
  if (!ifs.seekg(0, std::ios_base::beg)) {
    util::exception_location().raise<std::runtime_error>(
        "cannot seek " + std::string{error_label} + " to the beginning");
  }

  const auto file_size{static_cast<std::size_t>(end_offset)};
  if (file_size > max_size) {
    util::exception_location().raise<std::out_of_range>(
        std::string{error_label} + " is too large to be loaded in memory");
  }

  std::string file_content(file_size, 'x');
  if (!ifs.read(std::data(file_content),
                static_cast<std::streamoff>(file_size))) {
    util::exception_location().raise<std::runtime_error>(
        "cannot read " + std::string{error_label} + " content");
  }
  return file_content;
}

void write_file_content(const std::filesystem::path &path,
                        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
                        std::string_view content,
                        std::string_view error_label) {
  // opening in binary mode with truncating
  std::ofstream ofs{};
  ofs.rdbuf()->pubsetbuf(nullptr, 0U);
  ofs.open(path,
           std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!ofs.is_open()) {
    util::exception_location().raise<std::runtime_error>(
        "cannot open " + std::string{error_label} + " for writing");
  }

  if (!ofs.write(std::data(content),
                 static_cast<std::streamoff>(std::size(content)))) {
    util::exception_location().raise<std::runtime_error>(
        "cannot write data to " + std::string{error_label});
  }

  // explicit close so a failed flush is reported to the caller
  ofs.close();
  if (ofs.fail()) {
    util::exception_location().raise<std::runtime_error>(
        "cannot close " + std::string{error_label});
  }
}

} // namespace util
