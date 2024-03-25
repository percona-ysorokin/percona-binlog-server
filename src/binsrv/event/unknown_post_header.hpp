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

#ifndef BINSRV_EVENT_UNKNOWN_POST_HEADER_HPP
#define BINSRV_EVENT_UNKNOWN_POST_HEADER_HPP

#include "binsrv/event/unknown_post_header_fwd.hpp" // IWYU pragma: export

#include <cstddef>

#include "binsrv/event/protocol_traits_fwd.hpp"

#include "util/byte_span_fwd.hpp"

namespace binsrv::event {

class [[nodiscard]] unknown_post_header {
public:
  static constexpr std::size_t size_in_bytes{unspecified_post_header_length};

  // this class will be used as the first type of the event post header
  // variant, so it needs to be default constructible
  unknown_post_header() noexcept = default;
  explicit unknown_post_header(util::const_byte_span /*unused*/) noexcept {}
};

} // namespace binsrv::event

#endif // BINSRV_EVENT_UNKNOWN_POST_HEADER_HPP
