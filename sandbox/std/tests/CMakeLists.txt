# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011-2012 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

origin_executable(test_build build.cpp)
target_link_libraries(test_build origin)

origin_executable(test_allocator allocator.cpp)
target_link_libraries(test_allocator origin)

origin_executable(test_pointer pointer.cpp)
target_link_libraries(test_pointer origin)


origin_executable(test_vector vector.cpp)
target_link_libraries(test_vector origin)


origin_executable(perf_vector perf_vector.cpp)
target_link_libraries(perf_vector origin)
