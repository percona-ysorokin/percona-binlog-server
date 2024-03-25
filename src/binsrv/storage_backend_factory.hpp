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

#ifndef BINSRV_STORAGE_BACKEND_FACTORY_HPP
#define BINSRV_STORAGE_BACKEND_FACTORY_HPP

#include "binsrv/basic_storage_backend_fwd.hpp"
#include "binsrv/storage_config_fwd.hpp"

namespace binsrv {

struct [[nodiscard]] storage_backend_factory {
  [[nodiscard]] static basic_storage_backend_ptr
  create(const storage_config &config);
};

} // namespace binsrv

#endif // BINSRV_STORAGE_BACKEND_FACTORY_HPP
