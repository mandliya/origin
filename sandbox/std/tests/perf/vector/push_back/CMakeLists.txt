# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011-2012 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

origin_executable(perf_vector_push_back_origin origin.cpp)
target_link_libraries(perf_vector_push_back_origin origin)

origin_executable(perf_vector_push_back_std std.cpp)
target_link_libraries(perf_vector_push_back_std origin)
