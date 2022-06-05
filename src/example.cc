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

#include <memory>

#include "graph.hpp"
#include "graphreader.hpp"
#include "mst.hpp"
#include "shortestpath.hpp"
#include "test.hpp"

namespace sdizo::test {

bool Example(const util::Args& args) {
  if (!args.IsOption("input")) {
    std::fprintf(stderr, "Error: Missing option --input.\n");
    return false;
  }

  GraphReader reader;
  size_t v, e, vb, ve;
  if (!reader.Open(args.GetValue("input"), v, e, &vb, &ve)) return false;
  const Vertex avb(vb);
  auto g_matrix = std::make_shared<AdjacencyMatrix>(false, v);
  auto g_list = std::make_shared<AdjacencyList>(false, v);
  auto g_matrix_d = std::make_shared<AdjacencyMatrix>(true, v);
  auto g_list_d = std::make_shared<AdjacencyList>(true, v);
  int32_t w;
  while (reader.ReadEdge(vb, ve, &w)) {
    g_matrix->AddEdge(vb, ve, w);
    g_list->AddEdge(vb, ve, w);
    g_matrix_d->AddEdge(vb, ve, w);
    g_list_d->AddEdge(vb, ve, w);
  }
  std::printf("Matrix\n");
  g_matrix->Print();
  std::printf("Matrix: PRIM\n");
  detail::Print(*mst::Prim<AdjacencyMatrix>(g_matrix));
  std::printf("Matrix: KRUSKAL\n");
  detail::Print(*mst::Kruskal<AdjacencyMatrix>(g_matrix));
  std::printf("Matrix: DIJKSTRA\n");
  detail::Print(avb, *shortestpath::Dijkstra<AdjacencyMatrix>(g_matrix_d, avb));
  {
    auto path_cost = shortestpath::BellmanFord<AdjacencyMatrix>(g_matrix_d, avb);
    if (path_cost) {
      std::printf("Matrix: BELLMAN-FORD\n");
      detail::Print(avb, *path_cost);
    }
  }

  std::printf("List:\n");
  g_list->Print();
  std::printf("List: PRIM\n");
  detail::Print(*mst::Prim<AdjacencyList>(g_list));
  std::printf("List: KRUSKAL\n");
  detail::Print(*mst::Kruskal<AdjacencyList>(g_list));
  std::printf("List: DIJKSTRA\n");
  detail::Print(avb, *shortestpath::Dijkstra<AdjacencyList>(g_list_d, avb));
  {
    auto path_cost = shortestpath::BellmanFord<AdjacencyList>(g_list_d, avb);
    if (path_cost) {
      std::printf("List: BELLMAN-FORD\n");
      detail::Print(avb, *path_cost);
    }
  }
  return true;
}

}  // namespace sdizo::test
