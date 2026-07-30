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

#ifndef BINSRV_ENCRYPTION_CONFIG_HPP
#define BINSRV_ENCRYPTION_CONFIG_HPP

#include "binsrv/encryption_config_fwd.hpp" // IWYU pragma: export

#include <string>

#include "binsrv/encryption_format_type_fwd.hpp"

#include "util/nv_tuple.hpp"

namespace binsrv {

struct [[nodiscard]] encryption_config
    : util::nv_tuple<
          // clang-format off
          util::nv<"format", encryption_format_type>,
          util::nv<"keyring_uri", std::string>
          // clang-format on
          > {

  void validate() const;
};

} // namespace binsrv

#endif // BINSRV_ENCRYPTION_CONFIG_HPP
