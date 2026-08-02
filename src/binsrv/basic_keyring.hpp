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

#ifndef BINSRV_BASIC_KEYRING_HPP
#define BINSRV_BASIC_KEYRING_HPP

#include "binsrv/basic_keyring_fwd.hpp" // IWYU pragma: export

#include "binsrv/keyring_record_fwd.hpp"

#include <string>

namespace binsrv {

class [[nodiscard]] basic_keyring {
public:
  basic_keyring() = default;
  basic_keyring(const basic_keyring &) = delete;
  basic_keyring(basic_keyring &&) noexcept = delete;
  basic_keyring &operator=(const basic_keyring &) = delete;
  basic_keyring &operator=(basic_keyring &&) = delete;

  virtual ~basic_keyring();

  [[nodiscard]] bool contains(std::string_view key_id) const;
  [[nodiscard]] const keyring_record &get_key(std::string_view key_id) const;
  [[nodiscard]] std::string get_description() const;

private:
  [[nodiscard]] virtual bool do_contains(std::string_view key_id) const = 0;
  [[nodiscard]] virtual const keyring_record &
  do_get_key(std::string_view key_id) const = 0;
  [[nodiscard]] virtual std::string do_get_description() const = 0;
};

} // namespace binsrv

#endif // BINSRV_BASIC_KEYRING_HPP
