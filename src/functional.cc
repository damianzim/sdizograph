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

#include <charconv>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "graph.hpp"
#include "graphgenerator.hpp"
#include "graphreader.hpp"
#include "mst.hpp"
#include "shortestpath.hpp"
#include "test.hpp"

namespace sdizo::test {
namespace {

bool LoadGraph(std::vector<WEdge>& edges, size_t& vertices, const char* input, Vertex* vb = nullptr) {
  size_t v, e;
  Vertex ub, ue;
  Weight weight;
  [[maybe_unused]] Vertex ve;
  GraphReader reader;
  if (!reader.Open(input, v, e, vb, &ve)) return false;
  vertices = v;
  while (reader.ReadEdge(ub, ue, &weight)) edges.emplace_back(Edge(ub, ue), weight);
  return true;
}

namespace menu {

using std::operator""sv;

class Ctx;
using CtxPtr = std::shared_ptr<Ctx>;

using CmdArgs = std::string;
using CmdFn = std::function<void(std::string_view)>;
using CmdName = std::string;
using Cmd = std::pair<CmdArgs, CmdFn>;
using CmdMap = std::map<CmdName, Cmd, std::less<>>;

using namespace std::placeholders;

bool GetToken(std::string_view& line, std::string_view& token, const char* label = nullptr) {
  size_t space = line.find_first_of(' ');
  if (space == line.npos) {
    if (label != nullptr) std::printf("Error: Missing %s\n", label);
    return false;
  }
  token = line.substr(0, space);
  line = line.substr(space + 1);
  return true;
}
template <typename T>
bool ParseNum(std::string_view token, T& val, const char* label = nullptr) {
  auto parsed = std::from_chars(token.data(), token.end(), val);
  if (parsed.ec != std::errc::invalid_argument) return true;
  if (label != nullptr) std::printf("Error: Invalid %s value\n", label);
  return false;
}

class Ctx {
 public:
  Ctx() { cmds_["help"] = std::make_pair("", std::bind(&Ctx::Help, this, _1)); }
  virtual ~Ctx() = default;

  virtual const char* Name() const = 0;

  bool Handle(std::string_view line) {
    std::string_view token;
    if (!GetToken(line, token, "command")) return false;
    const CmdMap::const_iterator it = cmds_.find(token);
    if (it == cmds_.end()) {
      std::printf("Error: Command not found\n");
      return true;
    }
    it->second.second(line);
    return true;
  }

 protected:
  CmdMap cmds_;

 private:
  void Help(std::string_view) const {
    for (const CmdMap::value_type& pair : cmds_)
      if (pair.second.first.size())
        std::printf("?%s %s\n", pair.first.c_str(), pair.second.first.c_str());
      else
        std::printf("?%s\n", pair.first.c_str());
  }
};

class Directed : public Ctx {
 public:
  Directed() {
    cmds_["generate"] = std::make_pair("<vertices> <density>", std::bind(&Directed::GenerateGraph, this, _1));
    cmds_["list"] = std::make_pair("", std::bind(&Directed::PrintList, this, _1));
    cmds_["matrix"] = std::make_pair("", std::bind(&Directed::PrintMatrix, this, _1));
    cmds_["dijkstra"] = std::make_pair("{list | matrix}", std::bind(&Directed::Dijkstra, this, _1));
    cmds_["bellmanford"] = std::make_pair("{list | matrix}", std::bind(&Directed::BellmanFord, this, _1));
  }

  const char* Name() const { return "directed"; }

  void Load(const std::vector<WEdge>& edges, const size_t vertices, const Vertex vb) {
    vb_ = vb;
    g_matrix_ = std::make_shared<AdjacencyMatrix>(true, vertices);
    g_list_ = std::make_shared<AdjacencyList>(true, vertices);
    std::for_each(edges.cbegin(), edges.cend(), [this](const WEdge& edge) {
      g_list_->AddEdge(edge);
      g_matrix_->AddEdge(edge);
    });
  }

 private:
  void PrintList(std::string_view) const {
    if (g_list_ != nullptr)
      g_list_->Print();
    else
      std::printf("Error: Adjacency list is empty\n");
  }
  void PrintMatrix(std::string_view) const {
    if (g_matrix_ != nullptr)
      g_matrix_->Print();
    else
      std::printf("Error: Adjacency matrix is empty\n");
  }
  void Dijkstra(std::string_view line) const {
    if (g_list_ == nullptr || g_matrix_ == nullptr) {
      std::printf("Error: Graph does not exist\n");
      return;
    }
    std::string_view token;
    if (!GetToken(line, token)) {
      std::printf("Error: Missing argument\n");
      return;
    }
    if (token.compare("list"sv) == 0)
      detail::Print(vb_, *shortestpath::Dijkstra<AdjacencyList>(g_list_, vb_));
    else if (token.compare("matrix"sv) == 0)
      detail::Print(vb_, *shortestpath::Dijkstra<AdjacencyMatrix>(g_matrix_, vb_));
    else
      std::printf("Error: Invalid graph representaton\n");
  }
  void BellmanFord(std::string_view line) const {
    if (g_list_ == nullptr || g_matrix_ == nullptr) {
      std::printf("Error: Graph does not exist\n");
      return;
    }
    std::string_view token;
    if (!GetToken(line, token)) {
      std::printf("Error: Missing argument\n");
      return;
    }
    std::unique_ptr<PathCost> path_cost = std::make_unique<PathCost>();
    if (token.compare("list"sv) == 0)
      path_cost = shortestpath::BellmanFord<AdjacencyList>(g_list_, vb_);
    else if (token.compare("matrix"sv) == 0)
      path_cost = shortestpath::BellmanFord<AdjacencyMatrix>(g_matrix_, vb_);
    else {
      std::printf("Error: Invalid graph representaton\n");
      return;
    }
    if (path_cost)
      detail::Print(vb_, *path_cost);
    else
      std::printf("Warning: Detected negative cycle\n");
  }

  void GenerateGraph(std::string_view line) {
    std::string_view token;
    if (!GetToken(line, token, "vertices")) return;
    long vertices;
    if (!ParseNum(token, vertices, "vertices")) return;
    if (vertices < 1) {
      std::printf("Error: Invalid number of vertices, should be greater than 0\n");
      return;
    }
    if (!GetToken(line, token, "density")) return;
    int density;
    if (!ParseNum(token, density, "density")) return;
    if (density < 1 || density > 100) {
      std::printf("Error: Invalid density, should be 0 < density <= 100\n");
      return;
    }
    Vertex vb;
    std::vector<WEdge> edges = graph_gen_.Generate(vertices, density, true, &vb);
    Load(edges, vertices, vb);
  }

  std::shared_ptr<AdjacencyList> g_list_{nullptr};
  std::shared_ptr<AdjacencyMatrix> g_matrix_{nullptr};
  GraphGenerator graph_gen_{true};
  Vertex vb_;
};

class Undirected : public Ctx {
 public:
  Undirected() {
    cmds_["generate"] = std::make_pair("<vertices> <density>", std::bind(&Undirected::GenerateGraph, this, _1));
    cmds_["list"] = std::make_pair("", std::bind(&Undirected::PrintList, this, _1));
    cmds_["matrix"] = std::make_pair("", std::bind(&Undirected::PrintMatrix, this, _1));
    cmds_["kruskal"] = std::make_pair("{list | matrix}", std::bind(&Undirected::Kruskal, this, _1));
    cmds_["prim"] = std::make_pair("{list | matrix}", std::bind(&Undirected::Prim, this, _1));
  }

  const char* Name() const { return "undirected"; }

  void Load(const std::vector<WEdge>& edges, const size_t vertices) {
    g_matrix_ = std::make_shared<AdjacencyMatrix>(false, vertices);
    g_list_ = std::make_shared<AdjacencyList>(false, vertices);
    std::for_each(edges.cbegin(), edges.cend(), [this](const WEdge& edge) {
      g_list_->AddEdge(edge);
      g_matrix_->AddEdge(edge);
    });
  }

 private:
  void PrintList(std::string_view) const {
    if (g_list_ != nullptr)
      g_list_->Print();
    else
      std::printf("Error: Adjacency list is empty\n");
  }
  void PrintMatrix(std::string_view) const {
    if (g_matrix_ != nullptr)
      g_matrix_->Print();
    else
      std::printf("Error: Adjacency matrix is empty\n");
  }
  void Kruskal(std::string_view line) const {
    if (g_list_ == nullptr || g_matrix_ == nullptr) {
      std::printf("Error: Graph does not exist\n");
      return;
    }
    std::string_view token;
    if (!GetToken(line, token)) {
      std::printf("Error: Missing argument\n");
      return;
    }
    if (token.compare("list"sv) == 0)
      detail::Print(*mst::Kruskal<AdjacencyList>(g_list_));
    else if (token.compare("matrix"sv) == 0)
      detail::Print(*mst::Kruskal<AdjacencyMatrix>(g_matrix_));
    else
      std::printf("Error: Invalid graph representaton\n");
  }
  void Prim(std::string_view line) const {
    if (g_list_ == nullptr || g_matrix_ == nullptr) {
      std::printf("Error: Graph does not exist\n");
      return;
    }
    std::string_view token;
    if (!GetToken(line, token)) {
      std::printf("Error: Missing argument\n");
      return;
    }
    if (token.compare("list"sv) == 0)
      detail::Print(*mst::Prim<AdjacencyList>(g_list_));
    else if (token.compare("matrix"sv) == 0)
      detail::Print(*mst::Prim<AdjacencyMatrix>(g_matrix_));
    else
      std::printf("Error: Invalid graph representaton\n");
  }

  void GenerateGraph(std::string_view line) {
    std::string_view token;
    if (!GetToken(line, token, "vertices")) return;
    long vertices;
    if (!ParseNum(token, vertices, "vertices")) return;
    if (vertices < 1) {
      std::printf("Error: Invalid number of vertices, should be greater than 0\n");
      return;
    }
    if (!GetToken(line, token, "density")) return;
    int density;
    if (!ParseNum(token, density, "density")) return;
    if (density < 1 || density > 100) {
      std::printf("Error: Invalid density, should be 0 < density <= 100\n");
      return;
    }
    std::vector<WEdge> edges = graph_gen_.Generate(vertices, density, false);
    Load(edges, vertices);
  }

  std::shared_ptr<AdjacencyList> g_list_{nullptr};
  std::shared_ptr<AdjacencyMatrix> g_matrix_{nullptr};
  GraphGenerator graph_gen_{true};
};

class Main : public Ctx {
 public:
  Main(CtxPtr* ctx, const char* input) : input_(input), ctx_ref_(ctx) {
    cmds_["directed"] = std::make_pair("[init]", std::bind(&Main::EnterDirected, this, _1));
    cmds_["undirected"] = std::make_pair("[init]", std::bind(&Main::EnterUndirected, this, _1));
  }

  const char* Name() const override { return ""; }

 private:
  void EnterDirected(std::string_view line) {
    std::shared_ptr<Directed> ctx_undirected = std::make_shared<Directed>();
    *ctx_ref_ = ctx_undirected;
    std::string_view token;
    if (!GetToken(line, token) || token.compare("init"sv) != 0) return;
    if (input_ == nullptr) {
      std::printf("Error: No --input provided\n");
      return;
    }
    std::vector<WEdge> edges;
    size_t vertices;
    Vertex vb;
    if (!LoadGraph(edges, vertices, input_, &vb)) {
      std::printf("Error: Loading graph\n");
      return;
    }
    ctx_undirected->Load(edges, vertices, vb);
  }
  void EnterUndirected(std::string_view line) {
    std::shared_ptr<Undirected> ctx_undirected = std::make_shared<Undirected>();
    *ctx_ref_ = ctx_undirected;
    std::string_view token;
    if (!GetToken(line, token) || token.compare("init"sv) != 0) return;
    if (input_ == nullptr) {
      std::printf("Error: No --input provided\n");
      return;
    }
    std::vector<WEdge> edges;
    size_t vertices;
    if (!LoadGraph(edges, vertices, input_)) {
      std::printf("Error: Loading graph\n");
      return;
    }
    ctx_undirected->Load(edges, vertices);
  }

  const char* input_{nullptr};

  CtxPtr* ctx_ref_{nullptr};
};

}  // namespace menu

}  // namespace

bool Functional(const util::Args& args) {
  std::shared_ptr<menu::Ctx> ctx{nullptr};
  std::shared_ptr<menu::Ctx> ctx_main = std::make_shared<menu::Main>(&ctx, args.GetValue("input"));
  ctx = ctx_main;

  size_t linecap = 1024;
  char* line = static_cast<char*>(std::malloc(1024));
  if (line == nullptr) return false;
  ssize_t linelen;

  while (true) {
    std::printf("%s> ", ctx->Name());
    if ((linelen = ::getline(&line, &linecap, stdin)) <= 0) break;
    line[linelen - 1] = '\0';
    if (std::strcmp("exit", line) == 0) {
      if (ctx == ctx_main) break;
      ctx = ctx_main;
      continue;
    }
    line[linelen - 1] = ' ';
    if (!ctx->Handle(std::string_view(line, linelen))) break;
  }

  free(line);
  return true;
}

}  // namespace sdizo::test
