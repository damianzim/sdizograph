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

#ifndef SDIZO_TEST_HPP_
#define SDIZO_TEST_HPP_

#include "args.hpp"

namespace sdizo::test {

bool Example(const util::Args& args);
bool Functional(const util::Args& args);
inline bool Performance() { return true; }

}  // namespace sdizo::test

#endif  // SDIZO_TEST_HPP_
