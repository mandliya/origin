// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_MOVE_HPP
#define ORIGIN_ITERATOR_MOVE_HPP

#include <origin/iterator/facade.hpp>

namespace origin
{
  template <typename I>
    class move_iterator;


  // An alias to the iterator facade used by move iterators. Note that the
  // reference type of a move iterator is an rvalue reference to the underlying
  // iterator's value type.
  template <typename I>
    using Move_iterator_facade = 
      iterator_facade<move_iterator<I>, I, Value_type<I>&&>;


  // A move iterator applies a unary move (function) to the
  // referenced elements of an underlying iterator.  Note that moveed 
  // references are assumed to be temporaries. As a result, pointer syntax is 
  // not supported for move iterators.
  //
  // Transform iterators have the same traversal properties as their underlying
  // iterators. 
  template <typename I>
    class move_iterator : public Move_iterator_facade<I>
    {
      static_assert(Input_iterator<I>(), "");
      using Base = Move_iterator_facade<I>;
    public:
      using referece = Value_type<I>&&;

      move_iterator(I i)
        : iter{i}
      { }
      
      // Return the underlying iterator.
      I base() const { return iter; }
      

      // Readable/writable
      reference operator*() const;
      using Base::operator[];

      // Increment
      move_iterator& operator++();
      using Base::operator++;
      
      // Decrement
      move_iterator& operator--();
      using Base::operator--;

      // Advance
      move_iterator& operator+=(Difference_type<I> n);
      move_iterator& operator-=(Difference_type<I> n);
      
    private:
      I iter;
    };

  template <typename I>
    inline auto
    move_iterator<I>::operator*() const -> reference
    { 
      return static_cast<reference>(*iter); 
    }

  template <typename I>
    inline auto move_iterator<I>::operator++() -> move_iterator&
    { 
      ++iter;
      return *this; 
    }

  template <typename I>
    inline auto move_iterator<I>::operator--() -> move_iterator&
    { 
      --iter;
      return *this; 
    }

  template <typename I>
    inline auto move_iterator<I>::operator+=(Difference_type<I> n) 
      -> move_iterator&
    { 
      iter += n; 
      return *this; 
    }
  
  template <typename I>  
    inline auto move_iterator<I>::operator-=(Difference_type<I> n) 
      -> move_iterator&
    { 
      iter -= n; 
      return *this; 
    }

  
  // Equality_comparable
  // Two move iterators compare equal when they refer to the same element
  // and their move functions compare equal
  //
  // FIXME: For stateless move functions, we can omit the computation, but
  // we can't really do that for stateful functions unless they are equality
  // comparable.
  template <typename I>
    inline bool 
    operator==(const move_iterator<I>& a, const move_iterator<I>& b)
    {
      return a.base() == b.base(); 
    }
    
  template <typename I>
    inline bool 
    operator!=(const move_iterator<I>& a, const move_iterator<I>& b)
    {
      return !(a == b);
    }
    
  
  // Totally_ordered
  // A move iterator i compares less than a move iterator j when
  // i.base() compares less than j.base.
  //
  // FIXME: Is this class totally ordered or is it weakly ordered because there
  // can be no ordering of functions?
  template <typename I>
    inline bool 
    operator<(const move_iterator<I>& a, const move_iterator<I>& b) 
    { 
      return a.base() < b.base(); 
    }
    
  template <typename I>
    inline bool 
    operator>(const move_iterator<I>& a, const move_iterator<I>& b) 
    { 
      return a.base() > b.base(); 
    }

  template <typename I>
    inline bool 
    operator<=(const move_iterator<I>& a, const move_iterator<I>& b) 
    { 
      return a.base() <= b.base(); 
    }

  template <typename I>
    inline bool 
    operator>=(const move_iterator<I>& a, const move_iterator<I>& b) 
    { 
      return a.base() >= b.base(); 
    }


  // Difference
  template <typename I>
    inline Difference_type<I> 
    operator-(const move_iterator<I>& a, const move_iterator<I>& b)
    {
      return a.base() - b.base();
    }



  // Make move iterator
  //
  // Return a move iterator wrapping i.
  template <typename I>
    inline move_iterator<I> make_move_iterator(I i)
    {
      return i;
    }
    
} // namespace origin

#endif
