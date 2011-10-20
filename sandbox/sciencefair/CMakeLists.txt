# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 2.8)

# We're actually using Boost
find_package(Boost COMPONENTS system filesystem)

set(ORIGIN_ROOT $ENV{ORIGIN_ROOT})
list(APPEND CMAKE_MODULE_PATH ${ORIGIN_ROOT}/cmake)
include(Origin)

origin_component(
  sciencefair
  VERSION 0.1.0
  DEPENDS core data
  AUTHORS
    Andrew Sutton <andrew.n.sutton -at- gmail.com>
)

# FIXME: Generate the complete set of include and library paths from other
# dependencies here. Also, generate the complete set of common link libraries
# for the ScienceFair applications.
include_directories(include ~/Code/boost/boost-1.47)

add_subdirectory(apps)
add_subdirectory(tests)

