// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef PERF_HPP
#define PERF_HPP

#include <chrono>
#include <random>
#include <vector>

#include <origin/utility.hpp>

namespace origin
{
  // FIXME: It might be nice to build support for recognizing some command
  // line parameters for e.g., seeding the random number generator.



  // The performance tester wraps a number of resources that are used to support
  // performance testing: timinng facilities, random value generation, and
  // logging.
  //
  // FIXME: Unify this with the Origin test facilities, if possible. It may
  // not be.
  //
  // FIXME: Support better logging.
  //
  // FIXME: Offload some of these functions into a separate translation unit? 
  // They don't really need to be inlined.
  //
  // FIXME: Parameterize over a different random number generator? A different
  // clock?
  struct performance_tester
  {
    using Clock = std::chrono::system_clock;
    using Time = Clock::time_point;
    using Engine = std::minstd_rand;

    struct Measurements 
    {
      Measurements() : size(-1) { }
      Measurements(std::size_t n) : size(n) { }

      std::size_t size;
      std::vector<long> measurements;
    };

    using Results = std::vector<Measurements>;

    // Initialize the performance testing environment.
    performance_tester() : eng() { init(); }

    // Initialize the performance testing environment 
    performance_tester(std::time_t t) : eng(t) { init(); }

    // Return the current time.
    static Time now() { return Clock::now(); }

    // Return the pseudorandom number generator.
    Engine& engine() { return eng; }


    // Execute the given test with test size n and record the results.
    template <typename Test>
      void operator()(Test test, std::size_t n, std::size_t r)
      {
        // Get the list of measurements for the current test size.
        if (res.empty()) {
          res.push_back(Measurements(n));
        } else if (res.back().size != n) {
          res.push_back(Measurements(n));
        }
        auto& m = res.back().measurements;

        // Time the test and record the result time.
        auto start = now();
        test(*this, n, r);
        auto stop = now();
        m.push_back((stop - start).count());
      }

    template <typename C, typename Traits>
      void print(std::basic_ostream<C, Traits>& os)
      {
        for (const auto& r : res) {
          os << r.size << '\t';
          for (long n : r.measurements) {
            os << n << '\t';
          }
          os << '\n';
        }
      }

  private:
    // Sample the random number generator and clock to ensure that they have
    // been properly initialized.
    void init()
    {
      eng();
      now();

      // Exercise the free store also. Note that we can't guarantee that all
      // allocator types will be initialized, but we can guarantee ensure that
      // the defualt is.
      delete [] new int[64];
    }

  private:
    Engine eng;
    Results res;
  };



  // Run test
  // Run the given test over an increasing test size. The test size increases
  // incrementally at each power of 10 (from 0 to some upper limit) by that
  // power of 10. That is [0, 10) by 1, [10, 100) by 10, [100, 1000) by 100, etc.
  // The result of each test is printed to standard out.
  //
  // FIXME: Rename this to reflect the exponential increase of test size?
  template <typename Env, typename Test>
    void run(Env& env, Test test, std::size_t repeat = 1, std::size_t max = 5)
    {
      std::size_t i, j;
      for (std::size_t n = 0; n < max; ++n) {
        std::size_t p = std::pow(10, n);
        for (i = p, j = 10 * i; i != j; i += p) {
          for (std::size_t r = 0; r < repeat; ++r)
            env(test, i, r);
        }
      }
    }

} // namespace origin

#endif
