# This file is part of the sdizograph distribution.
# Copyright (c) 2022 Damian Zimon <damianzim>.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME sdizograph)
project(${PROJECT_NAME} CXX)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options("-Wno-unknown-warning-option")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build")
endif()

set(SDIZOGRAPH_SOURCE
  src/args.cc
  src/functional.cc
  src/graph.cc
  src/graphreader.cc
)

add_library(sdizographlib STATIC ${SDIZOGRAPH_SOURCE})

add_executable(${PROJECT_NAME} src/main.cc)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE sdizographlib)
