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

#ifndef BINSRV_BINLOG_FILE_ENCRYPTION_METADATA_HPP
#define BINSRV_BINLOG_FILE_ENCRYPTION_METADATA_HPP

#include "binsrv/binlog_file_encryption_metadata_fwd.hpp" // IWYU pragma: export

#include "binsrv/file_data_envelope.hpp" // IWYU pragma: export
#include "binsrv/file_key_envelope.hpp"  // IWYU pragma: export

#include "util/nv_tuple.hpp"

namespace binsrv {

class [[nodiscard]] binlog_file_encryption_metadata
    : public util::nv_tuple<
          // clang-format off
          util::nv<"file_key_envelope", file_key_envelope>,
          util::nv<"file_data_envelope", file_data_envelope>
          // clang-format on
          > {};

} // namespace binsrv

#endif // BINSRV_BINLOG_FILE_ENCRYPTION_METADATA_HPP
