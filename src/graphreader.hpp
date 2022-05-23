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

#ifndef SDIZO_GRAPHREADER_HPP_
#define SDIZO_GRAPHREADER_HPP_

#include <string>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <sys/types.h>

namespace sdizo {

class GraphReader {
 public:
  GraphReader() = default;
  ~GraphReader();

  size_t Size() const;

  bool Open(const char* path, size_t& v, size_t& e, size_t* vb, size_t* ve);
  bool ReadEdge(size_t& vb, size_t& ve, int32_t* w);

 private:
  FILE* fp_{nullptr};
  size_t offset_{0};
  size_t size_{0};
};

} // namespace sdizo

#endif // SDIZO_GRAPHREADER_HPP_
