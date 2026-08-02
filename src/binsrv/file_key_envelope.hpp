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

#ifndef BINSRV_FILE_KEY_ENVELOPE_HPP
#define BINSRV_FILE_KEY_ENVELOPE_HPP

#include "binsrv/file_key_envelope_fwd.hpp" // IWYU pragma: export

#include <string>

#include "util/hex_value.hpp"
#include "util/nv_tuple.hpp"

namespace binsrv {

class [[nodiscard]] file_key_envelope
    : public util::nv_tuple<
          // clang-format off
          util::nv<"kek_id", std::string>,
          util::nv<"data_hex", util::hex_value>,
          util::nv<"iv_hex", util::optional_hex_value>,
          util::nv<"tag_hex", util::optional_hex_value>
          // clang-format on
          > {};

} // namespace binsrv

#endif // BINSRV_FILE_KEY_ENVELOPE_HPP
