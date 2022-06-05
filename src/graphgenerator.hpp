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

#ifndef SDIZO_GRAPHGENERATOR_HPP_
#define SDIZO_GRAPHGENERATOR_HPP_

#include <random>
#include <set>
#include <vector>

#include "graphtype.hpp"

namespace sdizo {

class GraphGenerator {
  static constexpr size_t kWeightLimit = 128;

 public:
  GraphGenerator() = default;
  GraphGenerator(bool random_seed) : gen_(random_seed ? std::random_device{}() : std::mt19937::default_seed){};

  /// @param density - A number from the (0, 100] interval.
  std::vector<WEdge> Generate(size_t vertex_no, size_t density, bool is_directed, Vertex* vb = nullptr);

 private:
  void SpanningTree(std::set<WEdge>& edges, size_t vertex_no, bool is_directed);

  std::uniform_int_distribution<> distr_{1, kWeightLimit};
  std::mt19937 gen_{};
};

}  // namespace sdizo

#endif  // SDIZO_GRAPHGENERATOR_HPP_
