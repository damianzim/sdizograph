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

#ifndef SDIZO_MST_HPP_
#define SDIZO_MST_HPP_

#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "graph.hpp"

namespace sdizo::mst {

constexpr auto kWeightInf = std::numeric_limits<Weight>::max();

template <typename GRepr>
std::unique_ptr<SpanningTree> Kruskal(std::shared_ptr<const Graph<GRepr>> g) {
  auto edges = g->Edges();
  std::sort(edges->begin(), edges->end(),
            [](const auto& lhs, const auto& rhs) -> bool { return lhs.second < rhs.second; });
  auto spanning_tree = std::make_unique<SpanningTree>();
  std::map<Vertex, std::shared_ptr<std::set<Vertex>>> disjoint_sets;
  {
    auto vertices = g->Vertices();
    std::transform(vertices->cbegin(), vertices->cend(), std::inserter(disjoint_sets, disjoint_sets.end()),
                   [](const Vertex v) -> std::pair<Vertex, std::shared_ptr<std::set<Vertex>>> {
                     return std::make_pair(v, std::make_shared<std::set<Vertex>>(std::set<Vertex>({v})));
                   });
  }
  for (const WEdge& wedge : *edges) {
    auto& u_set = disjoint_sets[wedge.first.first];
    auto& v_set = disjoint_sets[wedge.first.second];
    if (u_set == v_set) continue;
    spanning_tree->insert(wedge);
    auto union_set = std::make_shared<std::set<Vertex>>();
    std::set_union(u_set->cbegin(), u_set->cend(), v_set->cbegin(), v_set->cend(),
                   std::inserter(*union_set, union_set->end()));
    for (const Vertex& w : *union_set) disjoint_sets[w] = union_set;
  }
  return spanning_tree;
}

template <typename GRepr>
std::unique_ptr<SpanningTree> Prim(std::shared_ptr<const Graph<GRepr>> g) {
  struct Distance {
    Distance(const Vertex v, const Weight weight) : v_(v), weight_(weight){};

    Vertex v_;
    Weight weight_;
  };
  struct distance_sort {
    bool operator()(Distance const& lhs, Distance const& rhs) const { return lhs.weight_ > rhs.weight_; }
  };
  const size_t vertex_no = g->VerticesNo();
  std::vector<Vertex> predecessors(vertex_no);
  std::vector<Weight> weights(vertex_no, kWeightInf);
  const Vertex vb{0};
  std::vector<bool> visited(vertex_no, false);
  visited[vb] = true;
  std::vector<Distance> Q{{vb, 0}};
  auto adjacents = g->Adj();
  while (!Q.empty()) {
    std::pop_heap(Q.begin(), Q.end(), distance_sort{});
    const Distance distance = Q.back();
    Q.pop_back();
    const Vertex& u = distance.v_;
    visited[u] = true;
    if (distance.weight_ > weights[u]) continue;
    weights[u] = distance.weight_;
    for (const auto& [v, weight] : (*adjacents)[u]) {
      if (!visited[v] && weight < weights[v]) {
        Q.push_back({v, weight});
        std::push_heap(Q.begin(), Q.end(), distance_sort{});
        weights[v] = weight;
        predecessors[v] = u;
      }
    }
  }
  auto spanning_tree = std::make_unique<std::set<WEdge>>();
  for (Vertex v = 0; v < predecessors.size(); ++v)
    if (v != vb) spanning_tree->emplace(Edge(predecessors[v], v), weights[v]);
  return spanning_tree;
}

}  // namespace sdizo::mst

#endif  // SDIZO_MST_HPP_
