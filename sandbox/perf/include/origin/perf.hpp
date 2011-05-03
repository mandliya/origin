// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_PERF_HPP
#define ORIGIN_PERF_HPP

#include <string>
#include <functional>
#include <chrono>

namespace origin
{
  namespace perf
  {
    /**
     * A performance timer encapsulates the timing of a scoped event. The timer 
     * uses a resource (i.e., RAII) object that is started when initialized.
     * The result is reported to the timing register when destroyed.
     * 
     * Timers are initialized over an id. A string may also be used as an id.
     * 
     * @tparam Clock  A clock type (e.g., system_clock).
     */
    template<typename Clock>
      class timer
      {
        typedef std::size_t size_type;
        typedef Clock clock_type;
        typedef typename Clock::duration duration_type;
        typedef typename Clock::time_point time_type;
      public:
        timer(std::size_t id = 0)
          : id_{id}, start_{clock_type::now()}
        { }
          
        timer(char const* str)
          : id_{hash(str)}, start_{clock_type::now()}
        { }

        timer(std::string const& str)
          : id_{hash(str)}, start_{clock_type::now()}
        { }

        ~timer()
        {
          std::cout << "timer(" << id_ << ") " << duration().count() << "\n";
        }

      private:
        // Compute the hash value of the given string.
        static std::size_t hash(char const* str)
        {
          return std::hash<char const*>{}(str);
        }
        
        // Return the time elapsed since the start of the timer.
        duration_type duration() const
        {
          return clock_type::now() - start_;
        }
        
      private:
        size_type id_;
        time_type start_;
      };
    };
  
  
} // namespace origin


#endif
