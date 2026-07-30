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

#include "binsrv/keyring_factory.hpp"

#include <memory>
#include <stdexcept>
#include <string_view>

#include <boost/url/parse.hpp>

#include "binsrv/basic_keyring_fwd.hpp"
#include "binsrv/file_keyring.hpp"

#include "util/exception_location_helpers.hpp"

namespace binsrv {

basic_keyring_ptr keyring_factory::create(std::string_view keyring_uri) {
  const auto uri_parse_result{boost::urls::parse_absolute_uri(keyring_uri)};
  if (!uri_parse_result) {
    util::exception_location().raise<std::invalid_argument>(
        "invalid keyring URI");
  }

  const auto &uri{*uri_parse_result};
  if (uri.scheme() == file_keyring::uri_schema) {
    return std::make_shared<file_keyring>(keyring_uri);
  }

  util::exception_location().raise<std::invalid_argument>(
      "unsupported keyring URI scheme");
}

} // namespace binsrv
