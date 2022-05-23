#!/bin/bash

find -E . -regex '.\/src\/.*\.(cc|hpp)' | xargs clang-format -verbose -style=file -i
