#!/bin/sh

# Date: Mon, 03 Aug 2020
# Author:  R. Goffe
#
# Description: this script will ensure consistent formatting of source files
#
#              It relies on:
#              -  clang-format for C++ files (https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
#              -  cmake-format for CMakeFiles (https://github.com/cheshirekow/cmake_format)
#
# Requirements:
#
#              $ apt install clang-format
#              $ pip install cmakelang
#
# Usage example: ./misc/style.sh

print_step () {
    tput bold    # bold text
    tput setaf 2 # foreground green
    echo "$1"
    tput sgr 0   # reset to terminal defaults
}

print_error () {
    tput bold    # bold text
    tput setaf 1 # foreground red
    echo $1
    tput sgr 0   # reset to terminal defaults
}

print_info () {
    echo $1
}

if ! type clang-format > /dev/null 2>&1
then
    print_error "Program 'clang-format' could not be found"
    print_info  "Consider the following install command:"
    print_info  "  sudo apt install clang-format"
    exit
fi

if ! type cmake-format > /dev/null 2>&1
then
    print_error "Program 'cmake-format' could not be found"
    print_info  "Consider the following install command:"
    print_info  "  pip install cmakelang"
    exit
fi

# Go to git project root dir
cd $(git rev-parse --show-toplevel) || exit

print_step "Running clang-format on C++ source files..."
find src -name *.hh -o -name *.cc | xargs clang-format -i -style=file

print_step "Running cmake-format on CMake source files..."
find . -name CMakeLists.txt | xargs cmake-format -c .cmake-format --in-place

print_step "Success"
