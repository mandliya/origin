# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011-2012 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 2.8)

# Include Origin as if we're in the source tree
set(ORIGIN_ROOT $ENV{ORIGIN_ROOT})
list(APPEND CMAKE_MODULE_PATH ${ORIGIN_ROOT}/cmake)
include(Origin)

origin_module(
  VERSION 0.1.0
  AUTHORS Andrew Sutton <andrew.n.sutton -at- gmail.com>
          Michael Lopez <michael.lopez.332@gmail.com>

  EXPORT stream
)
