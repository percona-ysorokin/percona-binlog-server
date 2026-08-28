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
#include "operations/pull_operation.hpp"

#include <cassert>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

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

namespace {

bool wait_for_interruptable(std::uint32_t idle_time_seconds,
                            const flag_signal_guard &termination_flag) {
  // instead of
  // 'std::this_thread::sleep_for(std::chrono::seconds(idle_time_seconds))'
  // we do 'std::this_thread::sleep_for(1s)' '<idle_time_seconds>' times
  // in a loop also checking for termination condition

  // standard pattern with declaring an instance of
  // std::conditional_variable and waiting for it (for
  // '<idle_time_seconds>' seconds) to be notified from the signal handler
  // can be dangerous as the chances of signal handler being called on the
  // same thread as this one ('main()') are pretty big.
  for (std::uint32_t sleep_iteration{0U};
       sleep_iteration < idle_time_seconds && !termination_flag.is_flag_set();
       ++sleep_iteration) {
    std::this_thread::sleep_for(std::chrono::seconds(1U));
  }
  return !termination_flag.is_flag_set();
}

} // anonymous namespace

generic_operation<mode_type::pull>::generic_operation(
    util::command_line_arg_view cmd_args)
    : basic_operation{cmd_args, expected_number_of_arguments} {}

[[nodiscard]] bool generic_operation<mode_type::pull>::execute() const {
  bool result{false};

  binsrv::basic_logger_ptr logger;

  try {
    logger = collector_context::initialize_console_logger(get_cmd_args());
    const binsrv::main_config_ptr config{
        std::make_shared<binsrv::main_config>(get_config_file_path())};
    collector_context::reinitialize_logger_from_config(logger, *config);

    logger->log(binsrv::log_severity::delimiter,
                "'pull' operation mode specified");

    const auto &termination_flag{flag_signal_guard::instance()};

    logger->log(binsrv::log_severity::info,
                "set custom handlers for SIGINT and SIGTERM signals");

    operations::collector_context collector_ctx{
        easymysql::connection_replication_mode_type::blocking, config, logger,
        termination_flag};
    collector_ctx.receive_binlog_events();

    const auto idle_time_seconds{
        config->root().get<"replication">().get<"idle_time">()};

    std::string msg;
    auto iteration_number{1UZ};
    while (!termination_flag.is_flag_set()) {
      msg = "entering idle mode for ";
      msg += std::to_string(idle_time_seconds);
      msg += " seconds";
      logger->log(binsrv::log_severity::info, msg);

      if (!wait_for_interruptable(idle_time_seconds, termination_flag)) {
        break;
      }

      msg = "awoke after sleeping and trying to reconnect (iteration ";
      msg += std::to_string(iteration_number);
      msg += ')';
      logger->log(binsrv::log_severity::info, msg);

      collector_ctx.receive_binlog_events();
      ++iteration_number;
    }

    logger->log(binsrv::log_severity::info,
                "successfully shut down after receiving a termination signal");

    result = true;
  } catch (...) {
    handle_std_exception(logger);
  }
  return result;
}

} // namespace operations
