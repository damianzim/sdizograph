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

#include "graphgenerator.hpp"

#include <algorithm>
#include <numeric>

namespace sdizo {

std::vector<WEdge> GraphGenerator::Generate(const size_t vertex_no, const size_t density, const bool is_directed,
                                            Vertex* vb) {
  const size_t edges_limit = vertex_no * vertex_no - vertex_no;
  const size_t edges_no = (is_directed ? edges_limit : edges_limit / 2) * std::clamp<size_t>(density, 1, 100) / 100;
  const auto col_start = [&](const size_t i) -> size_t { return is_directed ? 0 : i; };
  std::set<WEdge> spanning_tree;
  SpanningTree(spanning_tree, vertex_no, is_directed);
  std::vector<WEdge> edges;
  if (spanning_tree.size() >= edges_no) goto fill_weights;
  for (size_t i = 0; i < vertex_no; ++i)
    for (size_t j = col_start(i); j < vertex_no; ++j) {
      if (i == j) continue;
      WEdge edge(Edge(i, j), 0);
      if (spanning_tree.find(edge) == spanning_tree.end())
        edges.emplace_back(std::move(edge));  // Weight 0 at the moment.
    }
  std::shuffle(edges.begin(), edges.end(), gen_);
  edges.resize(edges_no - spanning_tree.size());
fill_weights:
  edges.reserve(edges_no);
  for (auto it = spanning_tree.begin(); it != spanning_tree.end();)
    edges.push_back(std::move(spanning_tree.extract(it++).value()));
  std::for_each(edges.begin(), edges.end(), [this](WEdge& edge) { edge.second = distr_(gen_); });
  if (vb != nullptr) *vb = edges.front().first.first;
  return edges;
}

void GraphGenerator::SpanningTree(std::set<WEdge>& spanning_tree, const size_t vertex_no, const bool is_directed) {
  const auto reorder = is_directed ? [](Vertex&, Vertex&) {} : [](Vertex& u, Vertex& v) {
    if (u > v) std::swap(u, v);
  };
  std::vector<Vertex> vertices(vertex_no);
  std::iota(vertices.begin(), vertices.end(), 0);
  std::shuffle(vertices.begin(), vertices.end(), gen_);
  auto it = vertices.begin();
  Vertex prev = is_directed ? vertices.back() : *it++;
  while (it != vertices.end()) {
    Vertex v = *it++;
    reorder(prev, v);
    spanning_tree.emplace(Edge(prev, v), 0);
    prev = v;
  }
}

}  // namespace sdizo
