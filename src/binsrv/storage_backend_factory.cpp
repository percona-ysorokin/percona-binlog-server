#include "binsrv/storage_backend_factory.hpp"

#include <memory>
#include <stdexcept>

#include "binsrv/basic_storage_backend_fwd.hpp"
#include "binsrv/filesystem_storage_backend.hpp"
#include "binsrv/s3_storage_backend.hpp"
#include "binsrv/storage_config.hpp"

#include "util/exception_location_helpers.hpp"

namespace binsrv {

basic_storage_backend_ptr
storage_backend_factory::create(const storage_config &config) {
  const auto &storage_backend_type = config.get<"type">();
  if (storage_backend_type == "fs") {
    return std::make_unique<filesystem_storage_backend>(config.get<"path">());
  }
  if (storage_backend_type == "s3") {
    return std::make_unique<s3_storage_backend>(config.get<"path">());
  }
  util::exception_location().raise<std::invalid_argument>(
      "unknown storage backend type");
}

} // namespace binsrv
