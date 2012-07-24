// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_SEQUENCE_TESTING_HPP
#define ORIGIN_SEQUENCE_TESTING_HPP

#include "concepts.hpp"

#include <origin/type/testing.hpp>

namespace origin
{

  //////////////////////////////////////////////////////////////////////////////
  // Iterator Properties
  //
  // Note that the iterator properties have two function call overloads: one
  // that takes a pair of arguments representing a bounded range, and one that
  // takes two iterators first and last. Additional arguments may also be
  // specified.


  // Regularity of Readable Iterators
  //
  // A readable iterator can be dereferenced multiple times, and without any
  // interleaving traversal operators. 
  //
  // Note that it is not universally required that the resulting of
  // dereferencing the same iterator always returns the same value, although
  // that is a general expectation. It could be the case, for example, that
  // a readable iterator refers to a sequence of volatile objects.
  struct regularity_of_reading
  {
    template <typename P>
      bool
      operator(const P& pair)
      {
        return operator()(p.first, p.second);
      }

    template <typename I>
      bool 
      operator()(I first, I last) const
      {
        if (first != last) {
          // This could be a one-liner, but I want to sequence the dereference
          // operations (i.e., make the order deterministic).
          const Value_type<I>& a = *first;
          const Value_type<I>& b = *first;
          return ((void)a, (void)b, true);  // Supporess warnings...
        } else {
          return true;
        }
      }
  };


  // Iterator Move Assignment
  //
  // Move-assigning an object, a, through an iterator moves the state of a
  // into the object referenced by the iterator.
  struct iterator_move_assignment
  {
    template <typename P>
      bool
      operator(const P& pair)
      {
        return operator()(p.first, p.second);
      }

    template <typename I>
      bool 
      operator()(I first, I last, const T& a, const T& b) const
      {
        if (first != last) {
          // Note that we can't just write (*first = x) == x because the result
          // type of assignment through an iterator is unspecified. Also, we
          // can only verify the assignment if I us known to be readable.
          // Finally, we can only verify the result when the two inputs are
          // known to be equal.
          *first = std::move(a);
          if (a == b)
            return Readable<I>() ? check_equal(*first b) : true;
          else
            return true;
        } else {
          return true;
        }
      }
  };


  // Iterator copy Assignment
  //
  // Copy assigning an object, x, through an iterator copies the value of x
  // into the object referenced by the iterator.
  struct iterator_copy_assignment
  {
    template <typename P>
      bool
      operator(const P& pair)
      {
        return operator()(p.first, p.second);
      }

    template <typename I>
      bool operator()(I first, I last, const T& x) const
      {
        if (first != last) {
          // Note that we can't just write (*first = x) == x because the result
          // type of assignment through an iterator is unspecified. Also, we
          // can only verify the assignment if I us known to be readable.
          *first = x;
          return Readable<I>() ? check_equal(*first, x) : true;
        } else {
          return true;
        }
      }
  };


  namespace testing
  {
    ////////////////////////////////////////////////////////////////////////////
    // Readable
    //
    template <typename I>
      struct readable : testable
      {
        readable(context& cxt) 
          : testable(cxt), read(cxt)
        { }

        template <typename Iv>
          void
          operator()(Iv&& ivar)
          { 
            quick_check(this->cxt, read, ivar, ivar);
          }

        regularity_of_reading read;
      };


    ////////////////////////////////////////////////////////////////////////////
    // Move Writable
    //
    template <typename I>
      struct move_writable : testable
      {
        move_writable(context& cxt) 
          : testable(cxt), move(cxt)
        { }

        template <typename Iv>
          void
          operator()(Iv&& ivar)
          {
            quick_check(this->cxt, move, pvar);
          }

        iterator_move_assignment move;
      };




    // Check that I satisfies the requirements of the Input_iterator concept.
    template <typename I>
      inline void
      check_input_iterator(context& cxt)
      {

      }

    template <typename I>
      inline void
      check_output_iterator(context& cxt)
      {

      }

    template <typename I>
      check_forward_iteratro(context& cxt)
      {

      }

    template <typename I>
      check_bidirectional_iterator(context& cxt)
      {

      }

    template <typename I>
      check_random_access_iterator(context& cxt)
      {

      }

  } // namespace testing

} // namespace origin

#endif
