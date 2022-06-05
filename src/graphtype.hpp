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

#ifndef SDIZO_GRAPHTYPE_HPP_
#define SDIZO_GRAPHTYPE_HPP_

#include <cinttypes>
#include <cstddef>
#include <list>
#include <utility>
#include <vector>

namespace sdizo {

using Vertex = size_t;
using Weight = int32_t;
using Edge = std::pair<Vertex, Vertex>;
using WEdge = std::pair<Edge, Weight>;

}  // namespace sdizo

#endif  // SDIZO_GRAPHTYPE_HPP_
