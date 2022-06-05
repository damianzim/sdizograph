// This file is part of the sdizograph distribution.
// Copyright (c) 2022 Damian Zimon <damianzim>.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>

#include "args.hpp"
#include "graphreader.hpp"
#include "test.hpp"

using namespace sdizo;

namespace {

[[noreturn]] void ExitHelp(const char* prog, const bool exit_success = true) {
  std::fprintf(stderr,
               "\
Usage: %s {--example {--input <path>} | --perf [--random] | --func {--input <path>}}\n\
\n\
Required arguments:\n\
\t--example\tRun example on all implemented algorithms and graph representations.\n\
\t--perf\t\tPerformance mode.\n\
\t--random\tRandom seed.\n\
\t--func\t\tFunctional mode, test application functionalites.\n\
\n\
Optional arguments:\n\
\t--input PATH\tFile with data uses to initialize a graph.\n",
               prog);
  std::exit(exit_success ? 0 : 1);
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc < 2) ExitHelp(argv[0], false);
  util::Args args;
  args.ResolveArgs(argv);
  if (args.IsFlag("help")) ExitHelp(argv[0]);

  bool result = true;
  if (args.IsFlag("example"))
    result = test::Example(args);
  else if (args.IsFlag("perf"))
    result = test::Performance(args);
  else if (args.IsFlag("func"))
    result = test::Functional(args);
  else
    ExitHelp(argv[0], false);
  return result ? 0 : 1;
}
