#   macro(origin_run_test target src)
#       origin_executable(${target} ${src})
#       add_test(test_${target} ${target} ${ARGN})
#   endmacro()

#TRY_COMPILE(
#    RESULT
#    ${CMAKE_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}
#    binning binning_aggregate_test.cpp
#    CMAKE_FLAGS -DCMAKE_C_FLAGS:STRING="-fopenmp"
#    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
#)
