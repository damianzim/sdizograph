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

#include "graph.hpp"

#include <cstdio>
#include <list>
#include <numeric>

namespace sdizo::detail {

void Print(const SpanningTree& st) {
  std::printf("Spanning tree cost: %d\n", SpanningTreeCost(st));
  for (const auto& [edge, weight] : st) std::printf("[%2zu]--(%3d)--[%2zu]\n", edge.first, weight, edge.second);
}

Weight SpanningTreeCost(const SpanningTree& st) {
  return std::accumulate(st.begin(), st.end(), 0,
                         [](const Weight cost, const WEdge& wedge) -> Weight { return cost + wedge.second; });
}

void Print(const Vertex vb, const PathCost& path_cost) {
  const auto& predecessors = path_cost.first;
  const auto& distances = path_cost.second;
  for (size_t i = 0; i < predecessors.size(); ++i) {
    if (i == vb) {
      std::printf("[%2zu]-(%3d)->[%2zu]\n", i, distances[i], i);
      continue;
    }
    std::list<Vertex> path;
    for (Vertex v = i; v != vb; v = predecessors[v]) path.push_front(v);
    std::printf("[%2zu]-(%3d)->[%2zu]: [%2zu]", vb, distances[i], i, vb);
    for (const Vertex& w : path) std::printf("->[%2zu]", w);
    std::putchar('\n');
  }
}

}  // namespace sdizo::detail
