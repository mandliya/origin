# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 2.8)

set(ORIGIN_ROOT $ENV{ORIGIN_ROOT})
list(APPEND CMAKE_MODULE_PATH ${ORIGIN_ROOT}/cmake)
include(Origin)

origin_component(
  binning
  VERSION 0.1.0
  DEPENDS core
  AUTHORS
    Christopher Tuttle <ctuttle -at- kent.edu>
    Brian Bartman <bbartmanboost -at- gmail.com>
)

# variables which are visible through out CMake.
set(BINNING_HEADER_FILES "")
subdirs(include tests)

