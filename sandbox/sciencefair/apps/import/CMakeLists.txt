# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 2.8)

# Apparenly, this is going to depend on Boost.FS.
add_executable(sf-import import.cpp)
target_link_libraries(sf-import ${Boost_LIBRARIES})