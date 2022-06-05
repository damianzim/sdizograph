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

#include <algorithm>
#include <array>
#include <chrono>
#include <map>
#include <memory>

#include "args.hpp"
#include "graph.hpp"
#include "graphgenerator.hpp"
#include "graphtype.hpp"
#include "mst.hpp"
#include "shortestpath.hpp"
#include "test.hpp"

namespace sdizo::test {
namespace {
namespace config {

constexpr size_t kRepetitions = 100;
static constexpr std::array<size_t, 4> kDensities = {{25, 50, 75, 99}};
static constexpr std::array<size_t, 5> kVertices = {{50, 150, 200, 250, 300}};

}  // namespace config

enum class TestObj {
  kKruskalList,
  kKruskalMatrix,
  kPrimList,
  kPrimMatrix,
  kDijkstraList,
  kDijkstraMatrix,
  kBellmanFordList,
  kBellmanFordMatrix,
};

const char* Label(const TestObj test_obj) {
  switch (test_obj) {
    case TestObj::kKruskalList:
      return "Kruskal List";
    case TestObj::kKruskalMatrix:
      return "Kruskal Matrix";
    case TestObj::kPrimList:
      return "Prim List";
    case TestObj::kPrimMatrix:
      return "Prim Matrix";
    case TestObj::kDijkstraList:
      return "Dijkstra List";
    case TestObj::kDijkstraMatrix:
      return "Dijkstra Matrix";
    case TestObj::kBellmanFordList:
      return "BellmanFord List";
    case TestObj::kBellmanFordMatrix:
      return "BellmanFord Matrix";
    default:
      return nullptr;
  }
}

class MeasureObjs {
 public:
  MeasureObjs() { Reset(); }

  int64_t& operator[](const TestObj test_obj) { return measures_[test_obj]; }
  MeasureObjs& operator++() {
    ++cnt_;
    return *this;
  }

  auto Avg() const {
    std::map<TestObj, double> avg;
    for (const auto& [test_obj, total_time] : measures_) avg[test_obj] = total_time / static_cast<double>(cnt_);
    return avg;
  }

  void Reset() {
    cnt_ = 0;
    measures_[TestObj::kKruskalList] = 0;
    measures_[TestObj::kKruskalMatrix] = 0;
    measures_[TestObj::kPrimList] = 0;
    measures_[TestObj::kPrimMatrix] = 0;
    measures_[TestObj::kDijkstraList] = 0;
    measures_[TestObj::kDijkstraMatrix] = 0;
    measures_[TestObj::kBellmanFordList] = 0;
    measures_[TestObj::kBellmanFordMatrix] = 0;
  }

 private:
  size_t cnt_{0};
  std::map<TestObj, int64_t> measures_;
};

template <typename Fn>
int64_t MeasureNs(Fn op) {
  auto t1 = std::chrono::high_resolution_clock::now();
  op();
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
}

void MeasureMst(GraphGenerator& graph_gen, const size_t vertices, const size_t density, MeasureObjs& measure) {
  auto edges = graph_gen.Generate(vertices, density, false);
  auto g_matrix = std::make_shared<AdjacencyMatrix>(false, vertices);
  auto g_list = std::make_shared<AdjacencyList>(false, vertices);
  std::for_each(edges.cbegin(), edges.cend(), [&g_list, &g_matrix](const WEdge& edge) {
    g_list->AddEdge(edge);
    g_matrix->AddEdge(edge);
  });
  measure[TestObj::kKruskalList] += MeasureNs([&g_list] { mst::Kruskal<AdjacencyList>(g_list); });
  measure[TestObj::kKruskalMatrix] += MeasureNs([&g_matrix] { mst::Kruskal<AdjacencyMatrix>(g_matrix); });
  measure[TestObj::kPrimList] += MeasureNs([&g_list] { mst::Prim<AdjacencyList>(g_list); });
  measure[TestObj::kPrimMatrix] += MeasureNs([&g_matrix] { mst::Prim<AdjacencyMatrix>(g_matrix); });
}

void MeasureShortestPath(GraphGenerator& graph_gen, const size_t vertices, const size_t density, MeasureObjs& measure) {
  Vertex vb;
  auto edges_d = graph_gen.Generate(vertices, density, true, &vb);
  auto g_matrix_d = std::make_shared<AdjacencyMatrix>(true);
  auto g_list_d = std::make_shared<AdjacencyList>(true);
  std::for_each(edges_d.cbegin(), edges_d.cend(), [&g_list_d, &g_matrix_d](const WEdge& edge) {
    g_list_d->AddEdge(edge);
    g_matrix_d->AddEdge(edge);
  });
  measure[TestObj::kDijkstraList] +=
      MeasureNs([&g_list_d, &vb] { shortestpath::Dijkstra<AdjacencyList>(g_list_d, vb); });
  measure[TestObj::kDijkstraMatrix] +=
      MeasureNs([&g_matrix_d, &vb] { shortestpath::Dijkstra<AdjacencyMatrix>(g_matrix_d, vb); });
  measure[TestObj::kBellmanFordList] +=
      MeasureNs([&g_list_d, &vb] { shortestpath::BellmanFord<AdjacencyList>(g_list_d, vb); });
  measure[TestObj::kBellmanFordMatrix] +=
      MeasureNs([&g_matrix_d, &vb] { shortestpath::BellmanFord<AdjacencyMatrix>(g_matrix_d, vb); });
}

}  // namespace

bool Performance(const util::Args& args) {
  GraphGenerator graph_gen(args.IsFlag("random"));
  MeasureObjs measure;
  for (const size_t vertices : config::kVertices) {
    for (const size_t density : config::kDensities) {
      measure.Reset();
      for (size_t rep = 0; rep < config::kRepetitions; ++rep) {
        MeasureMst(graph_gen, vertices, density, measure);
        MeasureShortestPath(graph_gen, vertices, density, measure);
        ++measure;
      }
      std::printf("vertices= %3zu density= %2zu", vertices, density);
      auto avg = measure.Avg();
      for (const auto& [test_obj, value] : avg) std::printf(" | %-18s= %11.2f", Label(test_obj), value);
      std::putchar('\n');
    }
  }
  return true;
}

}  // namespace sdizo::test
