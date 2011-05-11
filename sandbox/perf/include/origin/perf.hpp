// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_PERF_HPP
#define ORIGIN_PERF_HPP

#include <functional>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <iosfwd>

namespace origin
{
  /**
   * The perf namespace contains facilities used to instrument program code
   * with performance measurement equipment.
   */
  namespace perf
  {
    template<typename Clock> class timing;
    template<typename Clock> class timer;
    
    /**
     * The timing class is a singleton that is responsible for storing the 
     * results of timers.
     */
    template<typename Clock>
      class timing
      {
      public:
        typedef Clock clock_type;
        typedef typename Clock::duration duration_type;
        typedef typename Clock::time_point time_type;
      private:
        typedef std::vector<duration_type> duration_vector;
      public:
        typedef std::unordered_map<char const*, duration_vector> timer_map;

        // FIXME: Should only be used by a timer?
        /**
         * Record a timing result for the specified timer.
         */
        void record(timer<Clock> const& t, duration_type d)
        {
          timers_[t.id()].push_back(d);
        }
        
        /**
         * Return the results of the timing.
         */
        timer_map const& results() const
        {
          return results();
        }

        // FIXME: I thoroughly dislike this output system. I should decouple
        // the printing from the timing via a separate class.
        /**
         * Print the timing results to the given output stream.
         */
        template<typename Char, typename Traits>
          void print(std::basic_ostream<Char, Traits>& os) const;
        
      private:
        timer_map timers_;
      };
      
    // The following types and functions are used to generate type-correct
    // labels for the durations. This could (should?) be made more public
    // since we're probably going to want to generalize the notion of correct
    // scientific reporting.
    /** @internal */
    //@{
    template<typename Duration>
      struct labeled_duration
      {
        labeled_duration(Duration d)
          : dur{d}
        { }

        Duration dur;
      };

    template<typename Duration>
      labeled_duration<Duration> label_duration(Duration d)
      {
        return {d};
      }

    template<typename Char, typename Traits>
      inline std::basic_ostream<Char, Traits>&
      operator<<(std::basic_ostream<Char, Traits>& os,  
                 labeled_duration<std::chrono::nanoseconds> t)
      {
        return os << t.dur.count() << " ns";
      }
      
    template<typename Char, typename Traits>
      inline std::basic_ostream<Char, Traits>&
      operator<<(std::basic_ostream<Char, Traits>& os,  
                 labeled_duration<std::chrono::microseconds> t)
      {
        return os << t.dur.count() << " us";
      }

    template<typename Char, typename Traits>
      inline std::basic_ostream<Char, Traits>&
      operator<<(std::basic_ostream<Char, Traits>& os,  
                 labeled_duration<std::chrono::milliseconds> t)
      {
        return os << t.dur.count() << " ms";
      }
    //@}

    template<typename Clock>
      template<typename Char, typename Traits>
        void timing<Clock>::print(std::basic_ostream<Char, Traits>& os) const
        {
          typedef typename duration_vector::size_type Size;

          for(auto const& x : timers_) {
            os << x.first << "\n";
            auto const& v = x.second;
            for(Size i = 0; i < v.size(); ++i) {
              os << i << "\t" << label_duration(v[i]) << "\n";
            }
          }
          os.flush();
        }

      
    /**
     * A performance timer encapsulates the timing of a scoped event. The timer 
     * uses a resource (i.e., RAII) object that is started when initialized.
     * The result is reported to the timing register when destroyed.
     * 
     * Timers are initialized over an id, a string literal, that uniquely
     * identifies the timer.
     * 
     * @tparam Clock  A clock type (e.g., system_clock).
     */
    template<typename Clock>
      class timer
      {
      public:
        typedef Clock clock_type;
        typedef typename Clock::duration duration_type;
        typedef typename Clock::time_point time_type;
        typedef timing<Clock> timing_type;

        /** @name Initialization */
        //@{
        /**
         * @brief Default constructor
         * Initialize the timer using the given timing system and specified
         * id. Results of the timer will be stored in timing system when this
         * timer is destroyed.
         */
        timer(timing_type& ts, char const* str)
          : timing_(ts), id_{str}, start_{clock_type::now()}
        { }

        ~timer()
        {
          timing_.record(*this, duration());
        }
        //@}
        
        /**
         * Return the timer's id.
         */
        char const* id() const
        {
          return id_;
        }

      private:
        // Return the time elapsed since the start of the timer.
        duration_type duration() const
        {
          return clock_type::now() - start_;
        }
        
      private:
        timing_type& timing_;
        char const* id_;
        time_type start_;
      };

    /**
     * The system_timing type stores timing results for timers that use the
     * system clock.
     */
    typedef timing<std::chrono::system_clock> system_timing;
      
    /**
     * The system_timer type defines a timer based on the system clock.
     */
    typedef timer<std::chrono::system_clock> system_timer;
 
  } // namespace perf

} // namespace origin

/**
 * @def ORIGIN_SYS_TIMER
 * @ingroup perf
 * 
 * This macro is used to declare a scoped system timer. Results are reported
 * to the specified timing system. The timer is initialized with the given id.
 * 
 * Note that the macro is only enabled if the configuration macro 
 * ORIGIN_ENABLE_PERF_TIMERS is set.
 */

#if ORIGIN_ENABLE_PERF_TIMERS
# define ORIGIN_SYS_TIMER(timing, id) \
  origin::perf::system_timer timer_ ## id ## __{timing, # id};
#else
# define ORIGIN_SYS_TIMER(timing, id)
# define ORIGIN_SYS_FUNCTION_TIMER(timing)
#endif

#endif