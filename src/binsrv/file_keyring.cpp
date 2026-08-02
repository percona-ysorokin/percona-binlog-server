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

#include "binsrv/file_keyring.hpp"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/url/host_type.hpp>
#include <boost/url/parse.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/url_view.hpp>

#include "binsrv/keyring_record.hpp"
#include "binsrv/keyring_record_collection.hpp"

#include "util/exception_location_helpers.hpp"

namespace binsrv {

file_keyring::file_keyring(std::string_view keyring_uri) : key_file_path_{} {
  const auto uri_parse_result{boost::urls::parse_absolute_uri(keyring_uri)};
  if (!uri_parse_result) {
    util::exception_location().raise<std::invalid_argument>(
        "invalid keyring URI");
  }
  const auto &uri{*uri_parse_result};

  if (uri.scheme_id() != boost::urls::scheme::file ||
      uri.scheme() != uri_schema) {
    util::exception_location().raise<std::invalid_argument>(
        "URI of invalid scheme provided");
  }
  if (uri.host_type() != boost::urls::host_type::name || !uri.host().empty()) {
    util::exception_location().raise<std::invalid_argument>(
        "file URI must not have host");
  }
  if (uri.has_port()) {
    util::exception_location().raise<std::invalid_argument>(
        "file URI must not have port");
  }
  if (uri.has_userinfo()) {
    util::exception_location().raise<std::invalid_argument>(
        "file URI must not have userinfo");
  }
  if (uri.has_query()) {
    util::exception_location().raise<std::invalid_argument>(
        "file URI must not have query");
  }
  if (uri.has_fragment()) {
    util::exception_location().raise<std::invalid_argument>(
        "file URI must not have fragment");
  }

  key_file_path_ = uri.path();

  if (!std::filesystem::exists(key_file_path_)) {
    util::exception_location().raise<std::invalid_argument>(
        "key file path does not exist");
  }
  if (!std::filesystem::is_regular_file(key_file_path_)) {
    util::exception_location().raise<std::invalid_argument>(
        "key file path is not a regular file");
  }

  keyring_records_ =
      std::make_unique<keyring_record_collection>(key_file_path_.string());
}

file_keyring::~file_keyring() = default;

[[nodiscard]] bool file_keyring::do_contains(std::string_view key_id) const {
  return keyring_records_->contains_key(key_id);
}
[[nodiscard]] const keyring_record &
file_keyring::do_get_key(std::string_view key_id) const {
  return keyring_records_->get_key(key_id);
}
[[nodiscard]] std::string file_keyring::do_get_description() const {
  return keyring_records_->get_description();
}
} // namespace binsrv
