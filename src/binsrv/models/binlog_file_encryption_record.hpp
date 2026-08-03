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

#ifndef BINSRV_MODELS_BINLOG_FILE_ENCRYPTION_RECORD_HPP
#define BINSRV_MODELS_BINLOG_FILE_ENCRYPTION_RECORD_HPP

#include "binsrv/models/binlog_file_encryption_record_fwd.hpp" // IWYU pragma: export

#include "binsrv/models/file_data_envelope_record.hpp" // IWYU pragma: export
#include "binsrv/models/file_key_envelope_record.hpp"  // IWYU pragma: export

#include "util/nv_tuple.hpp"

namespace binsrv::models {

class [[nodiscard]] binlog_file_encryption_record
    : public util::nv_tuple<
          // clang-format off
          util::nv<"file_key_envelope", file_key_envelope_record>,
          util::nv<"file_data_envelope", file_data_envelope_record>
          // clang-format on
          > {};

} // namespace binsrv::models

#endif // BINSRV_MODELS_BINLOG_FILE_ENCRYPTION_RECORD_HPP
