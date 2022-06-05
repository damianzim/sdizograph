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

#ifndef SDIZO_GRAPH_HPP_
#define SDIZO_GRAPH_HPP_

#include <cinttypes>
#include <cstdio>
#include <list>
#include <memory>
#include <set>
#include <vector>

namespace sdizo {

using Vertex = size_t;
using Weight = int32_t;
using Edge = std::pair<Vertex, Vertex>;
using WEdge = std::pair<Edge, Weight>;

using Connection = std::pair<Vertex, Weight>;
using Connections = std::list<Connection>;
using Adjacent = std::vector<Connections>;

template <class GRepr>
class Graph {
 public:
  std::shared_ptr<const Adjacent> Adj() const { return static_cast<GRepr const*>(this)->Adj(); }
  std::unique_ptr<std::vector<WEdge>> Edges() const { return static_cast<GRepr const*>(this)->Edges(); }
  void Print() const { static_cast<GRepr const*>(this)->Print(); }
  std::unique_ptr<std::set<Vertex>> Vertices() const { return static_cast<GRepr const*>(this)->Vertices(); }
  size_t VerticesNo() const { return static_cast<GRepr const*>(this)->VerticesNo(); }

  void AddEdge(Vertex vb, Vertex ve, Weight w) { static_cast<GRepr*>(this)->AddEdge(vb, ve, w); }
};

class AdjacencyMatrix : public Graph<AdjacencyMatrix> {
 public:
  AdjacencyMatrix(const bool is_directed) : is_directed_(is_directed) {}
  AdjacencyMatrix(const bool is_directed, const size_t vertices) : is_directed_(is_directed) { Resize(vertices); }

  std::shared_ptr<const Adjacent> Adj() const {
    auto adjacent = std::make_shared<Adjacent>(g_.size());
    for (size_t i = 0; i < g_.size(); ++i)
      for (size_t j = 0; j < g_.size(); ++j)
        if (g_[i][j] != 0) (*adjacent)[i].emplace_back(j, g_[i][j]);
    return std::move(adjacent);
  }
  std::unique_ptr<std::vector<WEdge>> Edges() const {
    auto edges = std::make_unique<std::vector<WEdge>>();
    const auto col_start = [&](const size_t i) -> bool { return is_directed_ ? 0 : i; };
    for (size_t i = 0; i < g_.size(); ++i)
      for (size_t j = col_start(i); j < g_.size(); ++j)
        if (g_[i][j] != 0) edges->emplace_back(Edge(i, j), g_[i][j]);
    return edges;
  }
  void Print() const {
    std::printf("  |");
    for (size_t i = 0; i < g_.size(); ++i) std::printf("  %2zu", i);
    std::putchar('\n');
    std::printf("--+");
    for (size_t i = 0; i < g_.size(); ++i) std::printf("----");
    std::putchar('\n');
    for (size_t i = 0; i < g_.size(); ++i) {
      std::printf("%2zu|", i);
      for (size_t j = 0; j < g_.size(); ++j) std::printf(" %3d", g_[i][j]);
      std::putchar('\n');
    }
    std::fflush(stdout);
  }
  std::unique_ptr<std::set<Vertex>> Vertices() const { return std::make_unique<std::set<Vertex>>(vertices_); }
  size_t VerticesNo() const { return vertices_.size(); }

  void AddEdge(const Vertex vb, const Vertex ve, const Weight w) {
    vertices_.insert({vb, ve});
    const Vertex max_v = std::max(vb, ve);
    if (max_v >= g_.size()) Resize(max_v + 1);
    g_[vb][ve] = w;
    if (!is_directed_) g_[ve][vb] = w;
  }

 private:
  void Resize(const size_t vertices) {
    if (vertices <= g_.size()) return;
    const size_t prev_size = g_.size();
    g_.resize(vertices);
    std::for_each(g_.begin(), g_.end(), [&](std::vector<Weight>& row) {
      row.resize(vertices);
      std::fill(row.begin() + prev_size, row.end(), 0);
    });
  }

  const bool is_directed_;

  std::vector<std::vector<Weight>> g_;
  std::set<Vertex> vertices_;
};

class AdjacencyList : public Graph<AdjacencyList> {
 public:
  AdjacencyList(const bool is_directed) : is_directed_(is_directed), g_(std::make_shared<Adjacent>()){};
  AdjacencyList(const bool is_directed, const size_t vertices)
      : is_directed_(is_directed), g_(std::make_shared<Adjacent>()) {
    g_->resize(vertices);
  }

  std::shared_ptr<const Adjacent> Adj() const { return g_; }
  std::unique_ptr<std::vector<WEdge>> Edges() const {
    auto edges = std::make_unique<std::vector<WEdge>>();
    for (auto it = g_->cbegin(); it != g_->cend(); ++it) {
      const Vertex vb = std::distance(g_->cbegin(), it);
      std::transform(it->cbegin(), it->cend(), std::back_inserter(*edges), [vb](const Connection& connection) -> WEdge {
        return WEdge(Edge(vb, connection.first), connection.second);
      });
    }
    return edges;
  }
  void Print() const {
    for (size_t i = 0; i < g_->size(); ++i) {
      const Connections& connections = (*g_)[i];
      if (connections.size() == 0) continue;
      std::printf("%zu:", i);
      typename Connections::const_iterator it = connections.cbegin();
      if (it != connections.cend())
        while (true) {
          std::printf(" (%zu, %" PRId32 ")", it->first, it->second);
          if (++it == connections.cend()) break;
          std::putchar(',');
        }
      std::putchar('\n');
      std::fflush(stdout);
    }
  }
  std::unique_ptr<std::set<Vertex>> Vertices() const {
    auto vertices = std::make_unique<std::set<Vertex>>();
    for (auto it = g_->cbegin(); it != g_->cend(); ++it) {
      vertices->insert(std::distance(g_->cbegin(), it));
      std::transform(it->cbegin(), it->cend(), std::inserter(*vertices, vertices->cbegin()),
                     [](const Connection& connection) -> Vertex { return connection.first; });
    }
    return vertices;
  }
  size_t VerticesNo() const { return g_->size(); }

  void AddEdge(const Vertex vb, const Vertex ve, const int32_t w) {
    const Vertex max_v = std::max(vb, ve);
    if (max_v >= g_->size()) g_->resize(max_v + 1);
    (*g_)[vb].emplace_back(ve, w);
    if (!is_directed_) (*g_)[ve].emplace_back(vb, w);
  }

 private:
  const bool is_directed_;

  // v -> [(u, weight), ...]
  std::shared_ptr<Adjacent> g_;
};

}  // namespace sdizo

#endif  // SDIZO_GRAPH_HPP_
