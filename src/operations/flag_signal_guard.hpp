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

#ifndef OPERATIONS_FLAG_SIGNAL_GUARD_HPP
#define OPERATIONS_FLAG_SIGNAL_GUARD_HPP

namespace operations {

class flag_signal_guard {
public:
  struct signal_helper;

  static const flag_signal_guard &instance();

  flag_signal_guard(const flag_signal_guard &) = delete;
  flag_signal_guard &operator=(const flag_signal_guard &) = delete;
  flag_signal_guard(flag_signal_guard &&) = delete;
  flag_signal_guard &operator=(flag_signal_guard &&) = delete;

  ~flag_signal_guard() = default;

  [[nodiscard]] bool is_flag_set() const volatile noexcept;

private:
  flag_signal_guard();
};

} // namespace operations

#endif // OPERATIONS_FLAG_SIGNAL_GUARD_HPP
