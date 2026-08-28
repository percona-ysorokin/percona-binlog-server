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

#include "operations/flag_signal_guard.hpp"

#include <atomic>
#include <csignal>
#include <stdexcept>

#include "util/exception_location_helpers.hpp"

namespace operations {

struct flag_signal_guard::signal_helper {
  // as this flag is modified from a signal handler, it is marked
  // as volatile to make sure that the optimizer will be aware that
  // this value can be changed at any time
  static volatile std::atomic_flag termination_flag;
};

// since c++20 it is no longer needed to initialize std::atomic_flag with
// ATOMIC_FLAG_INIT
volatile std::atomic_flag flag_signal_guard::signal_helper::termination_flag{};

// signal handlers must have C linkage, so we need to declare this function
// as 'extern "C"'

// it is also OK to declare this extern "C" function inside the 'operations'
// namespace, as it will still have C linkage (the name of the function will
// not be mangled and will be just "flag_signal_handler" in the object file,
// not "operations::flag_signal_handler")
extern "C" void flag_signal_handler([[maybe_unused]] int signo) {
  operations::flag_signal_guard::signal_helper::termination_flag.test_and_set();
}

const flag_signal_guard &flag_signal_guard::instance() {
  static const flag_signal_guard instance;
  return instance;
}

// we do not want to make this method static and we require it to be called
// on an initialized instance of flag_signal_guard to make sure that the signal
// handlers are set before this method is called
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool flag_signal_guard::is_flag_set() const volatile noexcept {
  return flag_signal_guard::signal_helper::termination_flag.test();
}

flag_signal_guard::flag_signal_guard() {
  // setting custom SIGINT and SIGTERM signal handlers
  if (std::signal(SIGTERM, &flag_signal_handler) == SIG_ERR) {
    util::exception_location().raise<std::logic_error>(
        "cannot set custom signal handler for SIGTERM");
  }
  if (std::signal(SIGINT, &flag_signal_handler) == SIG_ERR) {
    util::exception_location().raise<std::logic_error>(
        "cannot set custom signal handler for SIGINT");
  }
}

} // namespace operations
