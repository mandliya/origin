// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONTAINER_TESTING_HPP
#define ORIGIN_CONTAINER_TESTING_HPP

#include <cassert>
#include <iostream>

namespace origin
{
  template <typename R>
    void print(const R& range)
    {
      for (const auto& x : range)
        std::cout << x << ' ';
      std::cout << '\n';
    }


  namespace testing
  {
    // Testing facilities for container classes.
    template <typename C>
      struct container
      {
        using init_list = std::initializer_list<int>;

        static void check_default_ctor();

        static void check_copy_ctor(const C& x);
        static void check_copy_assign(const C& a, const C& b);
        
        static void check_move_ctor(const C& x);
        static void check_move_assign(const C& a, const C& b);

        static void check_init_list_ctor(init_list l);
        static void check_init_list_assign(init_list l, const C& x);

        static void check_swap(const C& a, const C& b);
        static void check_clear(const C& x);

      };


    // Testing facilities for sequence classes. By default, we assume that
    // sequences all tested types are sequences of integers unless stated
    // otherwise.
    template <typename S>
      struct sequence
      {
        static void check_insert(const S& x);
        static void check_erase(const S& x);

        static void check_push_back();
        static void check_pop_back();
      };


    // Container tests

    template <typename C>
      void
      container<C>::check_default_ctor()
      {
        C c;
        assert(c.empty());

        // Check the instantition of container properties.
        assert(empty(c) == true);
        assert(size(c) == 0);

        // Check iterator invariants.
        assert(c.begin() == c.end());
        assert(begin(c) == c.begin());
        assert(end(c) == c.end());
        // assert(distance(begin(c), end(c)) == 0);
        // distance(c.begin(), c.end());
      }


    template <typename C>
      void 
      container<C>::check_copy_ctor(const C& x)
      {
        C c = x;
        assert(c == x);
      }

    template <typename C>
      void 
      container<C>::check_copy_assign(const C& a, const C& b)
      {
        C c = b;
        c = a;
        assert(c == a);
      }    

    template <typename C>
      void 
      container<C>::check_move_ctor(const C& x)
      {
        C c1 = x;
        C c2 = std::move(c1);
        assert(c2 == x);
        assert(c1.empty());
      }

    template <typename C>
      void 
      container<C>::check_move_assign(const C& a, const C& b)
      {
        C c1 = a;
        C c2 = b;
        c2 = std::move(a);
        assert(c2 == a);
      }


    template <typename C>
      void 
      container<C>::check_init_list_ctor(init_list l)
      {
        C c = l;
        assert(lexicographical_equal(c, l));
      }

    template <typename C>
      void 
      container<C>::check_init_list_assign(init_list l, const C& x)
      {
        C c = x;
        c = l;
        assert(lexicographical_equal(c, l));
      }

    template <typename C>
      void
      container<C>::check_swap(const C& a, const C& b)
      {
        C c1 = a;
        C c2 = b;
        c1.swap(c2);
        assert(c1 == b);
        assert(c2 == a);
      }

    template <typename C>
      void
      container<C>::check_clear(const C& x)
      {
        C c = x;
        c.clear();
        assert(c.empty());
      }

    // Sequence tests

    template <typename S>
      void 
      sequence<S>::check_insert(const S& x)
      {
        S s = x;

        // Insert at the front.
        auto i1 = s.insert(s.begin(), -1);
        assert(s.size() == x.size() + 1);
        assert(s.front() == -1);
        assert(i1 == s.begin());
        
        // Insert at the end.
        auto i2 = s.insert(s.end(), -1);
        assert(s.size() == x.size() + 2);
        assert(s.back() == -1);
        assert(next(i2) == s.end());

        // Insert in the middle.
        auto i3 = s.insert(next(s.begin()), -2);
        assert(s.size() == x.size() + 3);
        assert(*i3 == -2);
        assert(i3 == next(s.begin()));

        // Make sure that original range is unchanged.
        auto r = range(next(i3), prev(s.end()));
        assert(lexicographical_equal(r, x));
      };

    template <typename S>
      void
      sequence<S>::check_erase(const S& x)
      {
        assert(x.size() > 3);
        S s = x;

        // Erase from the front.
        auto x1 = *next(s.begin());
        auto i1 = s.erase(s.begin());
        assert(s.size() == x.size() - 1);
        assert(s.front() == x1);
        assert(i1 == s.begin());

        // Erase from the back.
        auto x2 = *prev(s.end(), 2);
        auto i2 = s.erase(prev(s.end()));
        assert(s.size() == x.size() - 2);
        assert(*prev(s.end()) == x2);
        assert(i2 == s.end());

        // Erase from the middle
        auto i3 = next(s.begin());
        auto x3 = *next(i3);
        auto i4 = s.erase(i3);
        assert(s.size() == x.size() - 3);
        assert(*i4 == x3);
      }
  
  } // namespace container

} // namespace origin


#endif

