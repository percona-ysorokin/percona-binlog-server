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

#include "binsrv/basic_keyring.hpp"

#include <string>
#include <string_view>

#include "binsrv/keyring_record_fwd.hpp"

namespace binsrv {

basic_keyring::~basic_keyring() = default;

[[nodiscard]] const keyring_record &
basic_keyring::get_key(std::string_view key_id) {
  return do_get_key(key_id);
}
[[nodiscard]] std::string basic_keyring::get_description() const {
  return do_get_description();
}

} // namespace binsrv
