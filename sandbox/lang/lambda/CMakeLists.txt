# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011-2012 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 2.8)

project(lambda)

set(CMAKE_CXX_FLAGS "-std=c++0x")

add_executable(lambda 
               main.cpp 
               symbol.cpp 
               symbol_table.cpp
               lexer.cpp
               parser.cpp
               syntax.cpp
               context.cpp
               sexpr.cpp)
