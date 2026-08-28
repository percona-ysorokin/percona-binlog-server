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
#include "operations/fetch_operation.hpp"

#include <cassert>
#include <csignal>
#include <cstddef>
#include <memory>

#include "binsrv/basic_logger.hpp"
#include "binsrv/exception_handling_helpers.hpp"
#include "binsrv/log_severity.hpp"
#include "binsrv/main_config.hpp"

#include "easymysql/connection_fwd.hpp"

#include "operations/basic_operation.hpp"
#include "operations/collector_context.hpp"
#include "operations/flag_signal_guard.hpp"
#include "operations/mode_type.hpp"

#include "util/command_line_helpers_fwd.hpp"

namespace operations {

generic_operation<mode_type::fetch>::generic_operation(
    util::command_line_arg_view cmd_args)
    : basic_operation{cmd_args, expected_number_of_arguments} {}

[[nodiscard]] bool generic_operation<mode_type::fetch>::execute() const {
  bool result{false};

  binsrv::basic_logger_ptr logger;

  try {
    logger = collector_context::initialize_console_logger(get_cmd_args());
    const binsrv::main_config_ptr config{
        std::make_shared<binsrv::main_config>(get_config_file_path())};
    collector_context::reinitialize_logger_from_config(logger, *config);

    logger->log(binsrv::log_severity::delimiter,
                "'fetch' operation mode specified");

    const auto &termination_flag{flag_signal_guard::instance()};
    logger->log(binsrv::log_severity::info,
                "set custom handlers for SIGINT and SIGTERM signals");

    operations::collector_context collector_ctx{
        easymysql::connection_replication_mode_type::non_blocking, config,
        logger, termination_flag};
    const auto receive_result{collector_ctx.receive_binlog_events()};

    if (receive_result) {
      logger->log(binsrv::log_severity::info,
                  "successfully fetched everything and disconnected");
      result = true;
    } else {
      logger->log(binsrv::log_severity::error,
                  "fetching binlog events loop did not reach EOF");
    }
  } catch (...) {
    handle_std_exception(logger);
  }
  return result;
}

} // namespace operations
