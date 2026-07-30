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

#ifndef BINSRV_FILE_KEYRING_HPP
#define BINSRV_FILE_KEYRING_HPP

#include <filesystem>
#include <string>
#include <string_view>

#include "binsrv/basic_keyring.hpp" // IWYU pragma: export

#include "binsrv/keyring_record_collection_fwd.hpp"
#include "binsrv/keyring_record_fwd.hpp"

namespace binsrv {

class [[nodiscard]] file_keyring final : public basic_keyring {
public:
  static constexpr std::string_view uri_schema{"file"};

  explicit file_keyring(std::string_view keyring_uri);

  file_keyring(const file_keyring &) = delete;
  file_keyring(file_keyring &&) noexcept = delete;
  file_keyring &operator=(const file_keyring &) = delete;
  file_keyring &operator=(file_keyring &&) = delete;

  ~file_keyring() override;

  [[nodiscard]] const std::filesystem::path &
  get_key_file_path() const noexcept {
    return key_file_path_;
  }

private:
  std::filesystem::path key_file_path_;
  keyring_record_collection_ptr keyring_records_;

  [[nodiscard]] const keyring_record &
  do_get_key(std::string_view key_id) override;
  [[nodiscard]] std::string do_get_description() const override;
};

} // namespace binsrv

#endif // BINSRV_FILE_KEYRING_HPP
