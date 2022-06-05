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

#ifndef SDIZO_SHORTESTPATH_HPP_
#define SDIZO_SHORTESTPATH_HPP_

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

#include "graph.hpp"

namespace sdizo::shortestpath {

constexpr auto kDistanceInf = std::numeric_limits<Weight>::max();

template <typename GRepr>
std::unique_ptr<PathCost> Dijkstra(std::shared_ptr<const Graph<GRepr>> g, const Vertex vb) {
  struct Distance {
    Distance(const Vertex v, const Weight d) : d_(d), v_(v){};

    Weight d_;
    Vertex v_;
  };
  struct distance_sort {
    bool operator()(Distance const& lhs, Distance const& rhs) const { return lhs.d_ > rhs.d_; }
  };
  const size_t vertex_no = g->VerticesNo();
  std::vector<Vertex> predecessors(vertex_no);
  std::vector<Weight> distances(vertex_no, kDistanceInf);
  std::vector<Distance> Q{{vb, 0}};
  auto adjacents = g->Adj();
  while (!Q.empty()) {
    std::pop_heap(Q.begin(), Q.end(), distance_sort{});
    const Distance distance = Q.back();
    Q.pop_back();
    const Vertex& u = distance.v_;
    if (distance.d_ > distances[u]) continue;
    distances[u] = distance.d_;
    for (const auto& [v, weight] : (*adjacents)[u]) {
      const Weight new_distance = distances[u] + weight;
      if (new_distance < distances[v]) {
        Q.push_back({v, new_distance});
        std::push_heap(Q.begin(), Q.end(), distance_sort{});
        distances[v] = new_distance;
        predecessors[v] = u;
      }
    }
  }
  return std::make_unique<PathCost>(std::move(predecessors), std::move(distances));
}

template <typename GRepr>
std::unique_ptr<PathCost> BellmanFord(std::shared_ptr<const Graph<GRepr>> g, const Vertex vb) {
  const size_t vertex_no = g->VerticesNo();
  auto edges = g->Edges();
  std::vector<Vertex> predecessors(vertex_no);
  std::vector<Weight> distances(vertex_no);
  std::fill(distances.begin(), distances.end(), kDistanceInf);
  distances[vb] = 0;
  for (size_t i = 0; i < vertex_no - 1; ++i) {
    bool change = false;
    for (const auto& [edge, weight] : *edges) {
      const auto& [u, v] = edge;
      if (distances[u] != kDistanceInf && distances[u] + weight < distances[v]) {
        change = true;
        distances[v] = distances[u] + weight;
        predecessors[v] = u;
      }
    }
    if (!change) goto no_negative_cycle;
  }
  for (const auto& [edge, weight] : *edges)
    if (distances[edge.first] + weight < distances[edge.second])
      return std::unique_ptr<PathCost>(nullptr);  // Negative cycle
no_negative_cycle:
  return std::make_unique<PathCost>(std::move(predecessors), std::move(distances));
}

}  // namespace sdizo::shortestpath

#endif  // SDIZO_SHORTESTPATH_HPP_
