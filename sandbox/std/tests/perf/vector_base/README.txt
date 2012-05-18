
The purpose of this test is to evaluate the performace cost of allocating
memory to a vector base. Note that this class uses GCC's libstdc++ vector
implementation as a control.

The test creates a vector base: the minimal representation of a vector and
allocates memory equal to the test size. The vector base is destroyed and the
memory is deallocated when the test returns.

We might expect some overhead due to the use of virtual functions in the
Origin allocator model. However, the additional overhead has not been
statistically significant.

