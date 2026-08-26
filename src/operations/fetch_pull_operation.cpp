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
#include "operations/fetch_pull_operation.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/lexical_cast.hpp>

#include "binsrv/basic_logger.hpp"
#include "binsrv/exception_handling_helpers.hpp"
#include "binsrv/log_severity.hpp"
#include "binsrv/main_config.hpp"

#include "operations/basic_operation.hpp"
#include "operations/collector_context.hpp"
#include "operations/mode_type.hpp"

#include "util/command_line_helpers_fwd.hpp"
#include "util/exception_location_helpers.hpp"

namespace operations {

namespace {

bool wait_for_interruptable(std::uint32_t idle_time_seconds,
                            const volatile std::atomic_flag &termination_flag) {
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
       sleep_iteration < idle_time_seconds && !termination_flag.test();
       ++sleep_iteration) {
    std::this_thread::sleep_for(std::chrono::seconds(1U));
  }
  return !termination_flag.test();
}

// since c++20 it is no longer needed to initialize std::atomic_flag with
// ATOMIC_FLAG_INIT as this flag is modified from a signal handler it is marked
// as volatile to make sure optimizer do optimizations which will be unsafe for
// this scenario
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile std::atomic_flag global_termination_flag{};

} // anonymous namespace

extern "C" void custom_signal_handler(int /*signo*/) {
  global_termination_flag.test_and_set();
}

fetch_pull_operation::fetch_pull_operation(mode_type mode,
                                           util::command_line_arg_view cmd_args)
    : basic_operation{mode, cmd_args, expected_number_of_arguments} {}

[[nodiscard]] bool fetch_pull_operation::execute() const {
  bool result{false};

  binsrv::basic_logger_ptr logger;

  try {
    logger = collector_context::initialize_console_logger(get_cmd_args());
    const binsrv::main_config_ptr config{
        std::make_shared<binsrv::main_config>(get_config_file_path())};
    collector_context::reinitialize_logger_from_config(logger, *config);

    assert(get_mode() == operations::mode_type::fetch ||
           get_mode() == operations::mode_type::pull);
    std::string msg;
    msg = '\'';
    msg += boost::lexical_cast<std::string>(get_mode());
    msg += "' operation mode specified";
    logger->log(binsrv::log_severity::delimiter, msg);

    // setting custom SIGINT and SIGTERM signal handlers
    if (std::signal(SIGTERM, &custom_signal_handler) == SIG_ERR) {
      util::exception_location().raise<std::logic_error>(
          "cannot set custom signal handler for SIGTERM");
    }
    if (std::signal(SIGINT, &custom_signal_handler) == SIG_ERR) {
      util::exception_location().raise<std::logic_error>(
          "cannot set custom signal handler for SIGINT");
    }

    logger->log(binsrv::log_severity::info,
                "set custom handlers for SIGINT and SIGTERM signals");
    const volatile std::atomic_flag &termination_flag{global_termination_flag};

    operations::collector_context collector_ctx{get_mode(), config, logger,
                                                termination_flag};
    collector_ctx.receive_binlog_events();

    const auto idle_time_seconds{
        config->root().get<"replication">().get<"idle_time">()};
    if (get_mode() == operations::mode_type::pull) {
      auto iteration_number{1UZ};
      while (!termination_flag.test()) {
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
    }

    if (termination_flag.test()) {
      logger->log(
          binsrv::log_severity::info,
          "successfully shut down after receiving a termination signal");
    } else {
      logger->log(
          binsrv::log_severity::info,
          "successfully shut down after finishing the requested operation");
    }

    result = true;
  } catch (...) {
    handle_std_exception(logger);
  }
  return result;
}

} // namespace operations
