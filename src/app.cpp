#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "binsrv/basic_logger.hpp"
#include "binsrv/exception_handling_helpers.hpp"
#include "binsrv/log_severity.hpp"
#include "binsrv/logger_factory.hpp"
#include "binsrv/master_config.hpp"

#include "binsrv/event/code_type.hpp"
#include "binsrv/event/event.hpp"
#include "binsrv/event/protocol_traits_fwd.hpp"
#include "binsrv/event/reader_context.hpp"

#include "easymysql/binlog.hpp"
#include "easymysql/connection.hpp"
#include "easymysql/connection_config.hpp"
#include "easymysql/library.hpp"

#include "util/byte_span_fwd.hpp"
#include "util/command_line_helpers.hpp"
#include "util/exception_location_helpers.hpp"
#include "util/nv_tuple.hpp"

void log_span_dump(binsrv::basic_logger &logger,
                   util::const_byte_span portion) {
  static constexpr std::size_t bytes_per_dump_line{16U};
  std::size_t offset{0U};
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

void log_event_common_header(
    binsrv::basic_logger &logger,
    const binsrv::event::common_header &common_header) {
  std::ostringstream oss;
  oss << "ts: " << common_header.get_readable_timestamp()
      << ", type:" << common_header.get_readable_type_code()
      << ", server id:" << common_header.get_server_id_raw()
      << ", event size:" << common_header.get_event_size_raw()
      << ", next event position:" << common_header.get_next_event_position_raw()
      << ", flags: (" << common_header.get_readable_flags() << ')';

  logger.log(binsrv::log_severity::debug, oss.str());
}

void log_format_description_event(
    binsrv::basic_logger &logger,
    const binsrv::event::generic_post_header<
        binsrv::event::code_type::format_description> &post_header,
    const binsrv::event::generic_body<
        binsrv::event::code_type::format_description> &body) {
  std::ostringstream oss;
  oss << '\n'
      << "  binlog version    : " << post_header.get_binlog_version_raw()
      << '\n'
      << "  server version    : " << post_header.get_server_version() << '\n'
      << "  create timestamp  : " << post_header.get_readable_create_timestamp()
      << '\n'
      << "  header length     : " << post_header.get_common_header_length()
      << '\n'
      << "  checksum algorithm: " << body.get_readable_checksum_algorithm()
      << '\n'
      << "  post-header length for ROTATE: "
      << post_header.get_post_header_length(binsrv::event::code_type::rotate)
      << '\n'
      << "  post-header length for FDE   : "
      << post_header.get_post_header_length(
             binsrv::event::code_type::format_description);

  logger.log(binsrv::log_severity::debug, oss.str());
}

int main(int argc, char *argv[]) {
  using namespace std::string_literals;

  int exit_code = EXIT_FAILURE;

  const auto cmd_args = util::to_command_line_agg_view(argc, argv);
  const auto number_of_cmd_args = std::size(cmd_args);
  const auto executable_name = util::extract_executable_name(cmd_args);

  if (number_of_cmd_args != binsrv::master_config::flattened_size + 1 &&
      number_of_cmd_args != 2) {
    std::cerr
        << "usage: " << executable_name
        << " <logger_level> <logger_file> <host> <port> <user> <password>\n"
        << "       " << executable_name << " <json_config_file>\n";
    return exit_code;
  }
  binsrv::basic_logger_ptr logger;

  try {
    const auto default_log_level = binsrv::log_severity::trace;

    const binsrv::logger_config initial_logger_config{
        {{default_log_level}, {""}}};

    logger = binsrv::logger_factory::create(initial_logger_config);
    // logging with "delimiter" level has the highest priority and empty label
    logger->log(binsrv::log_severity::delimiter,
                '"' + executable_name + '"' +
                    " started with the following command line arguments:");
    logger->log(binsrv::log_severity::delimiter,
                util::get_readable_command_line_arguments(cmd_args));

    binsrv::master_config_ptr config;
    if (number_of_cmd_args == 2U) {
      logger->log(binsrv::log_severity::delimiter,
                  "Reading connection configuration from the JSON file.");
      config = std::make_shared<binsrv::master_config>(cmd_args[1]);
    } else if (number_of_cmd_args ==
               binsrv::master_config::flattened_size + 1) {
      logger->log(binsrv::log_severity::delimiter,
                  "Reading connection configuration from the command line "
                  "arguments.");
      config = std::make_shared<binsrv::master_config>(cmd_args.subspan(1U));
    } else {
      assert(false);
    }
    assert(config);

    const auto &logger_config = config->root().get<"logger">();
    if (!logger_config.has_file()) {
      logger->set_min_level(logger_config.get<"level">());
    } else {
      logger->log(binsrv::log_severity::delimiter,
                  "Redirecting logging to \"" + logger_config.get<"file">() +
                      "\"");
      auto new_logger = binsrv::logger_factory::create(logger_config);
      std::swap(logger, new_logger);
    }

    const auto log_level_label =
        binsrv::to_string_view(logger->get_min_level());
    logger->log(binsrv::log_severity::delimiter,
                "logging level set to \""s + std::string{log_level_label} +
                    '"');

    const auto &connection_config = config->root().get<"connection">();
    logger->log(binsrv::log_severity::info,
                "mysql connection string: " +
                    connection_config.get_connection_string());

    const easymysql::library mysql_lib;
    logger->log(binsrv::log_severity::info, "initialized mysql client library");

    std::string msg = "mysql client version: ";
    msg += mysql_lib.get_readable_client_version();
    logger->log(binsrv::log_severity::info, msg);

    auto connection = mysql_lib.create_connection(connection_config);
    logger->log(binsrv::log_severity::info,
                "established connection to mysql server");
    msg = "mysql server version: ";
    msg += connection.get_readable_server_version();
    logger->log(binsrv::log_severity::info, msg);

    logger->log(binsrv::log_severity::info,
                "mysql protocol version: " +
                    std::to_string(connection.get_protocol_version()));

    msg = "mysql server connection info: ";
    msg += connection.get_server_connection_info();
    logger->log(binsrv::log_severity::info, msg);

    msg = "mysql connection character set: ";
    msg += connection.get_character_set_name();
    logger->log(binsrv::log_severity::info, msg);

    static constexpr std::uint32_t default_server_id{0U};
    auto binlog =
        connection.create_binlog(default_server_id, std::string_view{},
                                 binsrv::event::magic_binlog_offset);
    logger->log(binsrv::log_severity::info, "opened binary log connection");

    // TODO: make sure we write 'Binlog File Header' [ 0xFE 'bin’]` to the
    //       beginning of the binlog file
    // TODO: The first event is either a START_EVENT_V3 or a
    //       FORMAT_DESCRIPTION_EVENT while the last event is either a
    //       STOP_EVENT or ROTATE_EVENT. For Binlog Version 4 (current one)
    //       only FORMAT_DESCRIPTION_EVENT / ROTATE_EVENT pair should be
    //       acceptable.

    // Network streams are requested with COM_BINLOG_DUMP and
    // each Binlog Event response is prepended with 00 OK-byte.
    static constexpr std::byte expected_event_packet_prefix{'\0'};

    util::const_byte_span portion;

    binsrv::event::reader_context context{};

    while (!(portion = binlog.fetch()).empty()) {
      if (portion[0] != expected_event_packet_prefix) {
        util::exception_location().raise<std::invalid_argument>(
            "unexpected event prefix");
      }
      portion = portion.subspan(1U);
      logger->log(binsrv::log_severity::info,
                  "fetched " + std::to_string(std::size(portion)) +
                      "-byte(s) event from binlog");

      const binsrv::event::event current_event{context, portion};

      log_event_common_header(*logger, current_event.get_common_header());
      if (current_event.get_common_header().get_type_code() ==
          binsrv::event::code_type::format_description) {
        const auto &local_fde_post_header = current_event.get_post_header<
            binsrv::event::code_type::format_description>();
        const auto &local_fde_body =
            current_event
                .get_body<binsrv::event::code_type::format_description>();

        log_format_description_event(*logger, local_fde_post_header,
                                     local_fde_body);
      }
      log_span_dump(*logger, portion);
    }

    exit_code = EXIT_SUCCESS;
  } catch (...) {
    handle_std_exception(logger);
  }

  return exit_code;
}
