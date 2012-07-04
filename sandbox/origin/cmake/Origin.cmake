# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Note that this is only ever included once. It prevents multiple inclusions
# and checks when building from the top-level.
if(NOT ORIGIN_INCLUDED)
  set(ORIGIN_INCLUDED TRUE)

  # Set the root of the source tree.
  set(ORIGIN_ROOT ${CMAKE_SOURCE_DIR})

  # Get the parent of the Origin module tree. This will be put on the include
  # path.
  #
  # FIXME: If the module tree becomes a nested directory within a larger
  # project then the "parent" will be the top-level directory and we'll need
  # to specify module paths relative to the module root. 
  get_filename_component(ORIGIN_PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)


  # ORIGIN_CMAKE_DIR -- Directory containing Origin's CMake modules
  get_filename_component(ORIGIN_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

  # There are some useful CMake utilities in Boost.
  include(BoostUtils)


  # Be sure to compile in C++11 mode!
  # FIXME: Move the C++ configuration stuff into a separate config module.
  set(CMAKE_CXX_FLAGS "-std=c++11")

  # Make sure that we can include files as <origin/xxx>.
  include_directories(${ORIGIN_PARENT_DIR})  


  # Include Origin-specific macros
  include(OriginVersion)
  include(OriginModule)
  include(OriginTest)

endif()

