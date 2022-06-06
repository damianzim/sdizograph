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

#include "test.hpp"

namespace sdizo::test {
namespace {
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

class Main : public Ctx {
 public:
  Main(CtxPtr* ctx, const char* input) : input_(input), ctx_ref_(ctx) { using namespace std::placeholders; }

  const char* Name() const override { return ""; }

 private:
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
