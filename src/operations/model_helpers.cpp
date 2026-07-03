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

#include "operations/model_helpers.hpp"

#include "binsrv/storage.hpp"

#include "binsrv/models/search_response.hpp"

namespace operations {

void append_record_to_search_response(
    binsrv::models::search_response &response, const binsrv::storage &storage,
    const binsrv::storage::binlog_record &record) {
  response.add_record(record.name.str(), record.size,
                      storage.get_binlog_uri(record.name),
                      record.previous_gtids, record.added_gtids,
                      record.timestamps.get_min_timestamp().get_value(),
                      record.timestamps.get_max_timestamp().get_value());
}

} // namespace operations
