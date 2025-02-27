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

#ifndef BINSRV_STORAGE_HPP
#define BINSRV_STORAGE_HPP

#include "binsrv/storage_fwd.hpp" // IWYU pragma: export

#include <string>
#include <string_view>
#include <vector>

#include "binsrv/basic_storage_backend_fwd.hpp"

#include "util/byte_span_fwd.hpp"

namespace binsrv {

class [[nodiscard]] storage {
public:
  static constexpr std::string_view default_binlog_index_name{"binlog.index"};
  static constexpr std::string_view default_binlog_index_entry_path{"."};

  // passing by value as we are going to move from this unique_ptr
  explicit storage(basic_storage_backend_ptr backend);

  storage(const storage &) = delete;
  storage &operator=(const storage &) = delete;
  storage(storage &&) = delete;
  storage &operator=(storage &&) = delete;

  // desctuctor is explicitly defined as default here to complete the rule of 5
  ~storage() = default;

  [[nodiscard]] std::string_view get_binlog_name() const noexcept {
    return binlog_names_.empty() ? std::string_view{} : binlog_names_.back();
  }
  [[nodiscard]] std::uint64_t get_position() const noexcept {
    return position_;
  }

  [[nodiscard]] static bool
  check_binlog_name(std::string_view binlog_name) noexcept;

  void open_binlog(std::string_view binlog_name);
  void write_event(util::const_byte_span event_data);
  void close_binlog();

private:
  basic_storage_backend_ptr backend_;

  using binlog_name_container = std::vector<std::string>;
  binlog_name_container binlog_names_;
  std::uint64_t position_{0ULL};

  void load_binlog_index();
  void validate_binlog_index(const storage_object_name_container &object_names);
  void save_binlog_index();
};

} // namespace binsrv

#endif // BINSRV_STORAGE_HPP
