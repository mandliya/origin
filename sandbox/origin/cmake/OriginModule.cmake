# Copyright (c) 2008-2010 Kent State University
# Copyright (c) 2011-2012 Texas A&M University
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.


## Define an Origin module in the current directory.
##
##    origin_module(
##      VERSION version 
##      AUTHORS authors
##      EXPORTS files
##      IMPORTS modules
##
## A module is...
##
## The list of EXPORTS is a list of files in the module directory without
## their corresponding extensions (e.g. foo bar baz). Each export defines
## three associated components: foo.hpp (the interface), foo.cpp (the
## implementation), and foo.test.cpp (a unit test). These files are required
## for every export.
##
## The list of IMPORTS gives the set of targets on which the module depends.
## Each imported module results in a physical dependency for the constructed
## binary.
##
## This defines a number of macros that are used to configure the build of
## the module, its test suite, and its documentation.
##
##    ORIGIN_CURRENT_MODULE         -- The current module name (all lower case)
##    ORIGIN_CURRENT_LIBRARY_TARGET -- The binary built by the current mdoule
## 
macro(origin_module)

  parse_arguments(
    parsed
    "VERSION;AUTHORS;IMPORT;EXPORT"
    ""
    ${ARGN})

  # Compute the module name by finding the relative path to the current
  # module directory and pre-pending "origin". 
  file(RELATIVE_PATH path ${ORIGIN_ROOT} ${CMAKE_CURRENT_SOURCE_DIR})
  string(REPLACE "/" "." path ${path})
  set(module origin.${path})


  # Define some variables that are useful locally...
  set(ORIGIN_CURRENT_MODULE ${module})

  # Parse out the version string from the component.
  parse_version(${parsed_VERSION})

  # FIXME: There's probably some other stuff I could do here. For example,
  # I might generate version.hpp/version.cpp for the component that includes
  # the version numbers and the author/maintainer data.

  # Set the current library target. This is the binary component that
  # corresponds to this module.
  set(ORIGIN_CURRENT_LIBRARY_TARGET ${module})


  # Make sure that each export has its required components.
  check_exports(${parsed_EXPORT})

  # Generate a target for the for the module.
  #
  # FIXME: For nested modules, the name can't simply be origin_xxx it has
  # to be fully scoped to the origin root (e.g., origin_math_matrix).
  add_library(${ORIGIN_CURRENT_LIBRARY_TARGET} STATIC ${parsed_EXPORT})

  # Build dependencies for the imported modules.
  import_modules(${parsed_IMPORT})


  # Build unit tests for each of the exports.
  test_exports(${parsed_EXPORT})

endmacro()



# Verify that each export has the required components!
macro(check_exports)
  # Save the exports!
  set(ORIGIN_CURRENT_EXPORTS ${ARGV})

  foreach(i ${ARGV})
    # FIXME: Actually verify these things. Issue build warnings if the required
    # components do not exist. We could even refuse to build if we're creating
    # a distro!
  endforeach()
endmacro()


# Save the list of imports and create a dependency of the current library
# to the dependent target.
macro(import_modules)
  # Save the imports!
  set(ORIGIN_CURRENT_IMPORTS ${ARGV})

  # And link them to the current library
  link_imports(${ORIGIN_CURRENT_LIBRARY_TARGET})
endmacro()

# Link the imported modules to the specied target. Additional dependencies
# may be specified after the target.
macro(link_imports target)
  # Link the current library against each imported requirement.
  foreach (i ${ORIGIN_CURRENT_IMPORTS} ${ARGN})
    target_link_libraries(${target} ${i})
  endforeach()
endmacro()


# Generate a unit test for each file in ${export}.test
macro(test_exports)
  foreach(i ${ARGV})
    test_export(${i})
  endforeach()
endmacro()


# For each exported test, generate a unit test for each file in the
# corresponding test directory. Note that the corresponding test directory
# must exist!
macro(test_export exp)
  # Find the set of test files
  file(GLOB files ${exp}.test/*.cpp)
  foreach(i ${files})
    test_file(${exp} ${i})
  endforeach()

endmacro()


# Build a test for the file
macro(test_file exp file)
  
  # Get the name of the test and generate target names for the exe and te
  # test
  get_filename_component(name ${file} NAME_WE)
  set(exe ${ORIGIN_CURRENT_MODULE}.${exp}.${name})
  set(test ${exe}.test)

  # Generate the executable in a export-specific build directory.
  # Note that we don't have to do this, but it keeps the binary dirs from
  # becoming polluted with lots of test programs.
  set(out ${CMAKE_CURRENT_BINARY_DIR}/${exp})

  # Generate the executable target.
  add_executable(${exe} ${file})
  set_target_properties(${exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${out})
  link_imports(${exe} ${ORIGIN_CURRENT_MODULE})
  
  # Generate the test. Be sure to point to the correct executable!
  add_test(${test} ${out}/${exe})

endmacro()
