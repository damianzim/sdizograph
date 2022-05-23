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

#include "graphreader.hpp"

#include <cinttypes>
#include <cstdio>


namespace sdizo {

GraphReader::~GraphReader() {
  if (fp_ != nullptr) std::fclose(fp_);
}

size_t GraphReader::Size() const { return size_; }

bool GraphReader::Open(const char* path, size_t& v, size_t& e, size_t* vb, size_t* ve) {
  fp_ = std::fopen(path, "r");
  if (fp_ == nullptr) {
    std::fprintf(stderr, "Error: Open file path=[%s], errno=%d\n", path, errno);
    return false;
  }

  int64_t size, e_read, vb_read, ve_read;
  if (std::fscanf(fp_, "%" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 " \n", &size, &e_read, &vb_read, &ve_read) != 4 || size < 1) {
    std::fclose(fp_);
    fp_ = nullptr;
    return false;
  }
  v = size;
  e = e_read;
  if (vb != nullptr) *vb = vb_read;
  if (ve != nullptr) *ve = ve_read;
  offset_ = 0;
  size_ = size;
  return true;
}

bool GraphReader::ReadEdge(size_t& vb, size_t& ve, int32_t* weight) {
  if (offset_ >= Size()) return false;
  ++offset_;
  int64_t vb_read, ve_read;
  int32_t weight_read;
  if (std::fscanf(fp_, "%" PRId64 " %" PRId64 " %" PRId32 " \n", &vb_read, &ve_read, &weight_read) != 3) return false;
  vb = vb_read;
  ve = ve_read;
  if (weight != nullptr) *weight = weight_read;
  return true;
}

} // namespace sdizo
