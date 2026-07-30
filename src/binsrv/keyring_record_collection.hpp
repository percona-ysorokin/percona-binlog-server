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

#ifndef BINSRV_KEYRING_RECORD_COLLECTION_HPP
#define BINSRV_KEYRING_RECORD_COLLECTION_HPP

#include "binsrv/keyring_record_collection_fwd.hpp" // IWYU pragma: export

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "binsrv/keyring_record_fwd.hpp"

#include "util/nv_tuple.hpp"

namespace binsrv {

class [[nodiscard]] keyring_record_collection {
private:
  using key_collection = std::vector<keyring_record>;

  using impl_type = util::nv_tuple<
      // clang-format off
      util::nv<"version", std::uint32_t>,
      util::nv<"keys", key_collection>
      // clang-format on
      >;

public:
  explicit keyring_record_collection(std::string_view file_name);

  [[nodiscard]] const auto &root() const noexcept { return impl_; }
  [[nodiscard]] const keyring_record &get_key(std::string_view key_id) const;
  [[nodiscard]] std::string get_description() const;

private:
  impl_type impl_;

  void validate() const;
};

} // namespace binsrv

#endif // BINSRV_KEYRING_RECORD_COLLECTION_HPP
