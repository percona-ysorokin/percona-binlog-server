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

#include "operations/logger_helpers.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iterator>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/lexical_cast.hpp>

#include "binsrv/basic_logger.hpp"
#include "binsrv/log_severity.hpp"
#include "binsrv/replication_config.hpp"
#include "binsrv/replication_mode_type.hpp"
#include "binsrv/rewrite_config.hpp"
#include "binsrv/size_unit.hpp"
#include "binsrv/storage.hpp"
#include "binsrv/storage_backend_type.hpp" // IWYU pragma: keep
#include "binsrv/storage_config.hpp"
#include "binsrv/time_unit.hpp"

#include "easymysql/connection.hpp"
#include "easymysql/connection_config.hpp"
#include "easymysql/library.hpp"
#include "easymysql/ssl_config.hpp"
#include "easymysql/ssl_mode_type.hpp" // IWYU pragma: keep
#include "easymysql/tls_config.hpp"

#include "util/byte_span_fwd.hpp"
#include "util/common_optional_types.hpp"
#include "util/ct_string.hpp"
#include "util/nv_tuple_fwd.hpp"

namespace operations {

namespace {

template <typename T> util::optional_string to_log_string(const T &value) {
  return boost::lexical_cast<std::string>(value);
}

util::optional_string to_log_string(const binsrv::size_unit &value) {
  return value.get_description();
}

util::optional_string to_log_string(const binsrv::time_unit &value) {
  return value.get_description();
}

util::optional_string to_log_string(bool value) {
  return {value ? "true" : "false"};
}

template <typename T>
util::optional_string to_log_string(const std::optional<T> &value) {
  if (!value.has_value()) {
    return {};
  }
  return to_log_string(*value);
}

template <util::ct_string CTS, util::derived_from_named_value_tuple Config>
void log_config_param(binsrv::basic_logger &logger, const Config &config,
                      std::string_view label) {
  const auto opt_log_string{to_log_string(config.template get<CTS>())};
  if (opt_log_string.has_value()) {
    std::string msg{label};
    msg += ": ";
    msg += *opt_log_string;
    logger.log(binsrv::log_severity::info, msg);
  }
}

} // anonymous namespace

void log_ssl_config_info(binsrv::basic_logger &logger,
                         const easymysql::ssl_config &ssl_config) {
  log_config_param<"mode">(logger, ssl_config, "SSL mode");
  log_config_param<"ca">(logger, ssl_config, "SSL ca");
  log_config_param<"capath">(logger, ssl_config, "SSL capath");
  log_config_param<"crl">(logger, ssl_config, "SSL crl");
  log_config_param<"crlpath">(logger, ssl_config, "SSL crlpath");
  log_config_param<"cert">(logger, ssl_config, "SSL cert");
  log_config_param<"key">(logger, ssl_config, "SSL key");
  log_config_param<"cipher">(logger, ssl_config, "SSL cipher");
}

void log_tls_config_info(binsrv::basic_logger &logger,
                         const easymysql::tls_config &tls_config) {
  log_config_param<"ciphersuites">(logger, tls_config, "TLS ciphersuites");
  log_config_param<"version">(logger, tls_config, "TLS version");
}

void log_connection_config_info(
    binsrv::basic_logger &logger,
    const easymysql::connection_config &connection_config) {
  std::string msg;
  msg = "mysql connection string: ";
  msg += connection_config.get_connection_string();
  logger.log(binsrv::log_severity::info, msg);

  log_config_param<"connect_timeout">(logger, connection_config,
                                      "mysql connect timeout (seconds)");
  log_config_param<"read_timeout">(logger, connection_config,
                                   "mysql read timeout (seconds)");
  log_config_param<"write_timeout">(logger, connection_config,
                                    "mysql write timeout (seconds)");

  const auto &optional_ssl_config{connection_config.get<"ssl">()};
  if (optional_ssl_config.has_value()) {
    log_ssl_config_info(logger, *optional_ssl_config);
  }
  const auto &optional_tls_config{connection_config.get<"tls">()};
  if (optional_tls_config.has_value()) {
    log_tls_config_info(logger, *optional_tls_config);
  }
}

void log_rewrite_config_info(binsrv::basic_logger &logger,
                             const binsrv::rewrite_config &rewrite_config) {
  log_config_param<"base_file_name">(logger, rewrite_config,
                                     "rewrite base binlog file name");
  log_config_param<"file_size">(logger, rewrite_config,
                                "rewrite binlog file size");
}
void log_replication_config_info(
    binsrv::basic_logger &logger,
    const binsrv::replication_config &replication_config) {

  log_config_param<"server_id">(logger, replication_config,
                                "mysql replication server id");
  log_config_param<"idle_time">(logger, replication_config,
                                "mysql replication idle time (seconds)");
  log_config_param<"verify_checksum">(
      logger, replication_config, "mysql replication checksum verification");
  log_config_param<"mode">(logger, replication_config,
                           "mysql replication mode");
  const auto &optional_rewrite_config{replication_config.get<"rewrite">()};
  if (optional_rewrite_config.has_value()) {
    log_rewrite_config_info(logger, *optional_rewrite_config);
  }
}

void log_storage_config_info(binsrv::basic_logger &logger,
                             const binsrv::storage_config &storage_config) {

  log_config_param<"backend">(logger, storage_config,
                              "binlog storage backend type");
  logger.log(binsrv::log_severity::info,
             "binlog storage backend URI (masked): " +
                 storage_config.get_masked_uri());
  log_config_param<"fs_buffer_directory">(
      logger, storage_config,
      "binlog storage backend filesystem buffer directory");
  log_config_param<"checkpoint_size">(
      logger, storage_config, "binlog storage backend checkpointing size");
  log_config_param<"checkpoint_interval">(
      logger, storage_config, "binlog storage backend checkpointing interval");
}

void log_storage_info(binsrv::basic_logger &logger,
                      const binsrv::storage &storage) {
  std::string msg{"created binlog storage with the following backend: "};
  msg += storage.get_backend_description();
  logger.log(binsrv::log_severity::info, msg);

  msg.clear();
  msg = "binlog storage initialized in ";
  msg += boost::lexical_cast<std::string>(storage.get_replication_mode());
  msg += " mode";
  logger.log(binsrv::log_severity::info, msg);

  msg.clear();
  if (storage.is_empty()) {
    msg = "binlog storage initialized on an empty directory";
  } else {
    msg = "binlog storage initialized at \"";
    msg += storage.get_current_binlog_name().str();
    msg += "\":";
    msg += std::to_string(storage.get_current_position());
  }
  logger.log(binsrv::log_severity::info, msg);
}

void log_library_info(binsrv::basic_logger &logger,
                      const easymysql::library &mysql_lib) {
  std::string msg{};
  msg = "mysql client version: ";
  msg += mysql_lib.get_readable_client_version();
  logger.log(binsrv::log_severity::info, msg);
}

void log_connection_info(binsrv::basic_logger &logger,
                         const easymysql::connection &connection) {
  std::string msg{};
  msg = "mysql server version: ";
  msg += connection.get_readable_server_version();
  logger.log(binsrv::log_severity::info, msg);

  logger.log(binsrv::log_severity::info,
             "mysql protocol version: " +
                 std::to_string(connection.get_protocol_version()));

  msg = "mysql server connection info: ";
  msg += connection.get_server_connection_info();
  logger.log(binsrv::log_severity::info, msg);

  msg = "mysql connection character set: ";
  msg += connection.get_character_set_name();
  logger.log(binsrv::log_severity::info, msg);
}

void log_replication_info(
    binsrv::basic_logger &logger, std::uint32_t server_id,
    const binsrv::storage &storage, bool verify_checksum,
    easymysql::connection_replication_mode_type blocking_mode) {
  const auto replication_mode{storage.get_replication_mode()};

  std::string msg{"switched to replication (checksum "};
  msg += (verify_checksum ? "enabled" : "disabled");
  msg += ", ";
  msg += boost::lexical_cast<std::string>(replication_mode);
  msg += +" mode)";
  logger.log(binsrv::log_severity::info, msg);

  msg = "replication info (server id ";
  msg += std::to_string(server_id);
  msg += ", ";
  msg += (blocking_mode == easymysql::connection_replication_mode_type::blocking
              ? "blocking"
              : "non-blocking");
  msg += ", starting from ";
  if (replication_mode == binsrv::replication_mode_type::position) {
    if (storage.is_empty()) {
      msg += "the very beginning";
    } else {
      msg += storage.get_current_binlog_name().str();
      msg += ":";
      msg += std::to_string(storage.get_current_position());
    }
  } else {
    const auto &gtids{storage.get_gtids()};
    if (gtids.is_empty()) {
      msg += "an empty";
    } else {
      msg += "the ";
      msg += boost::lexical_cast<std::string>(gtids);
    }
    msg += " GTID set";
  }
  msg += ")";
  logger.log(binsrv::log_severity::info, msg);
}

void log_span_dump(binsrv::basic_logger &logger,
                   util::const_byte_span portion) {
  logger.log(binsrv::log_severity::debug,
             "fetched " + std::to_string(std::size(portion)) +
                 "-byte(s) event from binlog");
  static constexpr auto bytes_per_dump_line{16UZ};
  auto offset{0UZ};
  while (offset < std::size(portion)) {
    std::ostringstream oss;
    oss << '[';
    oss << std::setfill('0') << std::hex;
    auto sub = portion.subspan(
        offset, std::min(bytes_per_dump_line, std::size(portion) - offset));
    for (auto current_byte : sub) {
      oss << ' ' << std::setw(2)
          << std::to_integer<std::uint16_t>(current_byte);
    }
    const std::size_t filler_length =
        (bytes_per_dump_line - std::size(sub)) * 3U;
    oss << std::setfill(' ') << std::setw(static_cast<int>(filler_length))
        << "";
    oss << " ] ";
    const auto &ctype_facet{
        std::use_facet<std::ctype<char>>(std::locale::classic())};

    for (auto current_byte : sub) {
      auto current_char{std::to_integer<char>(current_byte)};
      if (!ctype_facet.is(std::ctype_base::print, current_char)) {
        current_char = '.';
      }
      oss.put(current_char);
    }
    logger.log(binsrv::log_severity::trace, oss.str());
    offset += bytes_per_dump_line;
  }
}

} // namespace operations
