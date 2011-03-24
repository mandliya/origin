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
  template
  VERSION 0.1.0
  DEPENDS core 
  AUTHORS
    Author 1 <author1 -at- address.foo>
    Author 2 <author2 -at- address.foo>
)

# add_subdirectory(tests)
