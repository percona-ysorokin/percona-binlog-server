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

#include <cstdlib>
#include <exception>
#include <iostream>

#include "operations/basic_operation.hpp"
#include "operations/operation_factory.hpp"

#include "util/command_line_helpers.hpp"

int main(int argc, char *argv[]) {
  const auto cmd_args{util::to_command_line_agg_view(argc, argv)};

  operations::basic_operation_ptr operation{};
  try {
    operation = operations::operation_factory::create(cmd_args);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  if (!operation) {
    const auto executable_name{util::extract_executable_name(cmd_args)};
    std::cerr << "usage: " << executable_name
              << " (fetch|pull)) <json_config_file>\n"
              << "       " << executable_name << " list <json_config_file>\n"
              << "       " << executable_name
              << " search_by_timestamp <json_config_file> <timestamp>\n"
              << "       " << executable_name
              << " search_by_gtid_set <json_config_file> <gtid_set>\n"
              << "       " << executable_name
              << " purge_binlogs <json_config_file> <binlog_name>\n"
              << "       " << executable_name << " version\n";
    return EXIT_FAILURE;
  }

  return (operation->execute() ? EXIT_SUCCESS : EXIT_FAILURE);
}
