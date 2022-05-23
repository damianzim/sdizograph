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

#include "graphreader.hpp"
#include "test.hpp"

namespace sdizo::test {

bool Functional(const util::Args& args) {
  if (!args.IsOption("input")) {
    std::fprintf(stderr, "Error: Missing option --input.\n");
    return false;
  }

  GraphReader reader;
  size_t v, e, vb, ve;
  if (!reader.Open(args.GetValue("input"), v, e, &vb, &ve)) return false;
  std::printf("v=%zu e=%zu vb=%zu ve=%zu\n", v, e, vb, ve);
  int32_t w;
  while (reader.ReadEdge(vb, ve, &w)) std::printf("vb=%zu ve=%zu w=%d\n", vb, ve, w);
  return true;
}

} // namespace sdizo::test
