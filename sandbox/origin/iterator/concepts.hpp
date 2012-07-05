// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_CONCEPTS_HPP
#define ORIGIN_ITERATOR_CONCEPTS_HPP

#include <iterator>

#include <origin/type/concepts.hpp>

namespace origin
{

  //////////////////////////////////////////////////////////////////////////////
  // Reading and Writing                                                 iter.rw
  //
  // There are three concepts describing reading from and writing through
  // iterators using the unary * operator.
  //////////////////////////////////////////////////////////////////////////////



  //////////////////////////////////////////////////////////////////////////////
  // Readable                                                          iter.read
  //
  // A type I is readable if it defines an associated value type and can be
  // dereferenced. The associated value type (accessed through Value_type<I>)
  // is the type of the object being dereference (excluding references or
  // cv-qualifiers). The result of dereferencing a readable type can be bound
  // to a constant reference to the value type. That is:
  //
  //    const Value_type<I>& x = *i;
  //
  // Must be a valid declaration for all Readable types.
  //
  // A Readable object can be dereferenced multiple times, having the same
  // result each time, assuming there are no intervening changes to the
  // iterator.
  //
  // Template Parameters:
  //    I -- The type being tested
  //
  // Returns:
  //    True if and only if I satisfies the syntactic requirements of the 
  //    Readable concept.
  template <typename I>
    constexpr bool Readable()
    {
      return Has_value_type<I>()
          && Has_dereference<I>() 
          && Convertible<Dereference_result<I>, const Value_type<I>&>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Move Writable                                                     iter.move
  //
  // A type I is copy writable to a value of type T, if values of T can be
  // copy assigned through a dereferenced value of type I. That is, the 
  // following expression must be valid:
  //
  //    *i = move(t);
  // 
  // where i has type I and t has type T. The operation has the result of
  // moving the representation of t into the object referenced by *i.
  //
  // Template Parameters:
  //    I -- The type being tested
  //    T -- A type being move assigned through an iterator of type I
  //
  // Returns:
  //    True if and only if I satisfies the syntactic requirements of the 
  //    Move_writable concept.
  template <typename I, typename T>
    constexpr bool Move_writable()
    {
      return Assignable<Dereference_result<I>, T&&>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Copy Writable                                                     iter.copy
  //
  // A type I is copy writable to a value of type T, if values of T can be
  // copy assigned through a dereferenced value of type I. That is, the 
  // following expression must be valid:
  //
  //    *i = t;
  // 
  // where i has type I and t has type T. If I is also Readable and the 
  // assigned value type is Equality comparable, then *i == t after the
  // assignment.
  //
  // Template Parameters:
  //    I -- The type being tested
  //    T -- A type being copy assigned through an iterator of type I
  //
  // Returns:
  //    True if and only if I satisfies the syntactic requirements of the 
  //    Copy_writable concept.
  template <typename I, typename T>
    constexpr bool Copy_writable()
    {
      return Move_writable<I, T>() 
          && Assignable<Dereference_result<I>, const T&>();
    };


    
  //////////////////////////////////////////////////////////////////////////////
  // Permutable                                                  iter.permutable
  //
  // An iterator type is permutable if its referenced values can be moved
  // between iterators (of the same type). For example.
  //
  //    *i = move(*j);
  //
  // Permutable iterators are required by many of the STL algorithms, including
  // the sort and heap algorithms.
  //
  // Template Parameters:
  //    I -- The type being tested.
  //
  // Retruns:
  //    True if and only if I satisfies the requirements of the Permutable 
  //    concept.
  template <typename I>
    constexpr bool Permutable()
    {
      return Readable<I>() 
          && Movable<Value_type<I>>() 
          && Move_writable<I, Value_type<I>&&>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Mutable                                                        iter.mutable
  //
  // An iterator type is mutable if its referenced values can be copied between
  // iterators (of the same type). For example:
  //
  //    *i = *j;
  //
  // Note that all mutable iterator types are also permutable. Mutable iterators
  // are not commoly required by STL algorithms. Most of algorithms that
  // exchange values do so through move operations (i.e,. permutation).
  //
  // Template Parameters:
  //    I -- The type being tested.
  //
  // Retruns:
  //    True if and only if I satisfies the requirements of the Mutable concept.
  template <typename I>
    constexpr bool Mutable()
    {
      return Permutable<I>() 
          && Copyable<Value_type<I>>() 
          && Copy_writable<I, Value_type<I>>();
    }
    
    

  //////////////////////////////////////////////////////////////////////////////
  // Incrementable Types                                               iter.incr
  //
  // An incrementable type is one whose value can be incremented or advanced.
  // There are three concepts describing incrementable types: weakly
  // incrementable, incrementable, and decrementable. These concepts provide
  // a basis for writing algorithms that only operate on the traversal aspects
  // of an iterator (e.g., advance and distance).
  //
  // Note that all incrementable types should be equality comparable since the
  // values of these types generally represent an offset, index, or state.
  // However, the C++ standard does not require equality comparison for
  // output iterators, so we cannot enforce this requirement at this level.
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  // Weakly Incrementable
  //
  // A weakly incrementable type is a semiregular type that can be pre- and
  // post-incremented. Both operations advance the value, but neither operation
  // is required to be equality preserving. The result type of post-incrementing 
  // is unspecified.
  //
  // Template Parameters:
  //    I -- The type being tested
  //
  // Returns:
  //    True if and only if I satisfies the requirements of the 
  //    Weakly_incrementable concept.
  template <typename I>
    constexpr bool Weakly_incrementable()
    {
      return Copyable<I>()
          && Equality_comparable<I>()
          && Has_difference_type<I>()
          && Has_pre_increment<I>()
          && Same<Pre_increment_result<I>, I&>()
          && Has_post_increment<I>();
    }
    

  //////////////////////////////////////////////////////////////////////////////
  // Incrementable
  //
  // Like a weakly incremetable types, an incrementable type is also pre- and 
  // post-incremetable, but the requirements on those operations are stronger.
  // In particular, both operations are required to be equality preserving, and
  // the result of post-incrementing an incrementable object returns a copy
  // of the previous value or state.
  //
  // Template Parameters:
  //    I -- The type being tested
  //
  // Returns:
  //    True if I is incrementable.
  template <typename I>
    constexpr bool Incrementable()
    {
      return Weakly_incrementable<I>()
          && Same<Post_increment_result<I>, I>();
    }


  //////////////////////////////////////////////////////////////////////////////
  // Decrementable
  //
  // A decrementable type is an inrementable type that can also be pre- and
  // post-decremented. Like the incrementable concept, the pre- and post-
  // decrement operators are required to be equality preserving, and the
  // post-decrement operator returns a copy of the previous state.
  //
  // Note that there is no weakly decrementable. A type cannot support 
  // decrementing without also supportng incrementing, and the ability to
  // decrement requires that the incremnt operation to be regular.
  template <typename I>
    constexpr bool Decrementable()
    {
      return Incrementable<I>()
          && Has_pre_decrement<I>()
          && Same<Pre_decrement_result<I>, I&>()
          && Has_post_decrement<I>()
          && Same<Post_decrement_result<I>, I>();
    }


  //////////////////////////////////////////////////////////////////////////////
  // Strict Weakly Incrementable
  //
  // It is sometimes necessary to overload an algorithm for differently for
  // weakly incrementable types than for incrementable types. This type
  // predicate provides that capability.
  //
  // Template Parameters:
  //    I -- The type being tested
  //
  // Returns:
  //    True if I is weakly incrementable but not incrementable.
  template <typename I>
    constexpr bool Strict_weakly_incrementable()
    {
      return Weakly_incrementable<I>() && !Incrementable<I>();
    }


  // TODO: Create another concept that mirrors random access iterators, but
  // does not include readability or writability. Not sure what the name should
  // be...



  //////////////////////////////////////////////////////////////////////////////
  // Iterator Types                                                   iter.types
  //
  // An Iterator is a type that is incrementable and (inclusively) readable or
  // writable.
  //////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////
  // Iterator Types                                                iter.iterator
  //
  // An iterator, in the most general sense is a type that can be incremented
  // and decremented, although no claims are made on the behavior or result of
  // dereferencing. In that sense, the concept is incomplete but nonetheless
  // useful to describe very general requirements on iterator types.
  //
  // An Iterator is minimally a weakly incrementable and dereferenceable type.
  // Note that no constraints are placed on the result of the dereference
  // operator.
  //
  // Template Parameters:
  //    I -- The type being tested
  //
  // Returns:
  //    True if and only if I satisfies the syntactic requirements of the
  //    Iterator concept.
  template <typename I>
    constexpr bool Iterator()
    {
    return Weakly_incrementable<I>() 
        && Has_dereference<I>();
    }


  // An alias for the associated reference type of the iterator. This supports 
  // writing  backwards compatible iterators where the reference type is 
  // actually named even though it should be deduced as decltype(*i).
  template <typename I>
    using Iterator_reference = decltype(*std::declval<I>());


  //////////////////////////////////////////////////////////////////////////////
  // Input Iterator                                              iter.input
  //
  // An input iterator is...
  //
  // There are two kinds of input iterators...
  
  // Returns true if I is a weak input iterator.
  template <typename I>
    constexpr bool Weak_input_iterator()
    {
      return Weakly_incrementable<I>() && Readable<I>();
    }
    
  // Returns true if I is an input iterator.
  template <typename I>
    constexpr bool Input_iterator()
    {
      return Weak_input_iterator<I>() && Equality_comparable<I>();
    }




  //////////////////////////////////////////////////////////////////////////////
  // Output Iterator                                                 iter.output
  //
  // An output itertor is...
  //
  // There are two kinds of output iterators...
  //
  // Note that, for either output iterator, if T is an rvalue reference, this
  // concept requires T to implement move semantics. If T is not an rvalue
  // reference, the iterator is required to implement copy semantics.
  
  // Returns true if I is a weak output iterator, accepting values of type T.
  template <typename I, typename T>
    constexpr bool Weak_output_iterator()
    {
      return Weakly_incrementable<I>() && Copy_writable<I, T>();
    }
    
  // Returns true if I is an output iterator accepting values of type T.
  template <typename I, typename T>
    constexpr bool Output_iterator()
    {
      return Weak_output_iterator<I, T>() && Equality_comparable<I>();
    }
 
 
  //////////////////////////////////////////////////////////////////////////////
  // Forward Iterator                                                   iter.fwd
  //
  // A forward iterator is an input iterator with a regular post-increment
  // operation. This guarantees that multiple passes of a range may be made
  // and that multiple iterators into the range may be used.
  template <typename I>
    constexpr bool Forward_iterator()
    {
      return Input_iterator<I>()
          && Incrementable<I>()
          && Readable<I>();
    }
    

  //////////////////////////////////////////////////////////////////////////////
  // Bidirectional Iterator                                            iter.bidi
  //
  // A bidirectional iterator is a forward iterator that can also move 
  // backwards using decrement operators.
  template <typename Iter>
    constexpr bool Bidirectional_iterator()
    {
      return Forward_iterator<Iter>()
          && Has_pre_decrement<Iter>()
          && Same<Pre_decrement_result<Iter>, Iter&>()
          && Has_post_decrement<Iter>()
          && Same<Post_decrement_result<Iter>, Iter>();
    };

    
  //////////////////////////////////////////////////////////////////////////////
  // Random Access Iterator                                            iter.rand
  //
  // A random access iterator is a bidirectional iterator that can advance 
  // any number of steps in constant time.
  template <typename I>
    constexpr bool Random_access_iterator()
    {
      return Bidirectional_iterator<I>()
            && Signed<Difference_type<I>>()
            
            && Has_plus_assign<I, Difference_type<I>>()
            && Same<Plus_assign_result<I, Difference_type<I>>, I&>()
            
            && Has_minus_assign<I, Difference_type<I>>()
            && Same<Minus_assign_result<I, Difference_type<I>>, I&>()
            
            && Has_plus<I, Difference_type<I>>()
            && Same<Plus_result<I, Difference_type<I>>, I>()
            
            && Has_plus<Difference_type<I>, I>()
            && Same<Plus_result<Difference_type<I>, I>, I>()
            
            && Has_minus<I, Difference_type<I>>()
            && Same<Minus_result<I, Difference_type<I>>, I>()
            
            && Has_minus<I>()
            && Same<Minus_result<I>, Difference_type<I>>()
            
            && Has_subscript<I, Difference_type<I>>()
            && Same<Subscript_result<I, Difference_type<I>>, Dereference_result<I>>();
    };
    
    

  //////////////////////////////////////////////////////////////////////////////
  // Iterator Bounds                                                 iter.bounds
  //
  // The following concepts differentiate different kinds of iterators up to
  // but not including other kinds (e.g., input but not forward, output but not
  // forward, non-random access). 
  //
  // These are not part of the core iterator concepts, and should not be 
  // necessary if concepts can be partially ordered.
  //////////////////////////////////////////////////////////////////////////////


  // Returns true if I is strictly an input iterator, an input iterator but not 
  // a forward iterator.
  template <typename I>
    constexpr bool Strict_input_iterator()
    {
      return Weak_input_iterator<I>() && !Forward_iterator<I>();
    }
    
    
  // Returns true if I is strictly an output iterator, an output iterator but
  // not a forward iterator.
  template <typename I>
    constexpr bool Strict_output_iterator()
    {
      return Weak_output_iterator<I> && !Readable<I>();
    }
  

  // Returns true if I is any kind of iterator except random access.
  template <typename I>
    constexpr bool Non_random_access_iterator()
    {
      return Weakly_incrementable<I>() && !Random_access_iterator<I>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Iterator Range Properties                                   iter.properties
  //
  // The following predicates describe properties of iterator ranges (pairs of
  // iterators).
  //
  // Note that none of these functions actually check the stated property. For
  // the most part, these properties are uncheckable. These are primarily
  // provided for the purpose of annotation.
  //////////////////////////////////////////////////////////////////////////////


  // Returns true if [first, n) is a weak range.
  template <typename I>
    inline bool 
    is_weak_range(I first, Difference_type<I> n) 
    { 
      static_assert(Weakly_incrementable<I>(), "");
      return n >= 0;
    }
  
  // Returns true if [first, n) is a counted range.
  template <typename I>
    inline bool 
    is_counted_range(I first, Difference_type<I> n) 
    { 
      static_assert(Weakly_incrementable<I>(), "");
      return n >= 0;
    }
    
  // Returns true if [first, last) is a bounded range.
  template <typename I>
    inline auto 
    is_bounded_range(I first, I last) 
      -> Requires<Non_random_access_iterator<I>(), bool>
    {
      static_assert(Equality_comparable<I>(), "");
      return true; 
    }

  // Overload for random access iterators. Minimally, we can guarantee that
  // first <= last, even though some or all elements of [first, last) may not
  // be valid iterators.
  template <typename I>
    inline auto 
    is_bounded_range(I first, I last) 
      -> Requires<Random_access_iterator<I>(), bool>
    {
      return first <= last; 
    }
    
  // Returns true if the weak range [first, n) is readable everywhere except
  // its limit.
  template <typename I>
    inline bool 
    is_readable_range(I first, Difference_type<I> n) 
    {
      static_assert(Readable<I>(), "");
      return is_weak_range(first, n); 
    }
  
  // Returns true if the bounded range [first, last) is readable everywhere
  // except its limit.
  template <typename I>
    inline bool 
    is_readable_range(I first, I last) 
    { 
      static_assert(Readable<I>(), "");
      return is_bounded_range(first, last); 
    }
    
  // Returns true if the weak range [first, n) is writable everywhere except
  // its limit.
  template <typename I, typename T>
    inline bool 
    is_writable_range(I first, Difference_type<I> n, T const& value)
    {
      static_assert(Copy_writable<I, T>(), "");
      return is_weak_range(first, n);
    }
   
  // Returns true if the bounded range [first, last) is writable everywhere 
  // except its limit.
  template <typename I, typename T>
    inline bool 
    is_writable_range(I first, I last, T const& value) 
    { 
      static_assert(Copy_writable<I, T>(), "");
      return is_bounded_range(first, last);
    }
    
  // Returns true if the weak range [first, n) is movable everywhere except its
  // limit.
  template <typename I, typename T>
    inline bool 
    is_movable_range(I first, Difference_type<I> n, T const& value)
    {
      static_assert(Move_writable<I, T&&>(), "");
      return is_weak_range(first, n);
    }
  
  // Returns true if the bounded range [first, last) is movable everywhere except
  // its limit.
  template <typename I, typename T>
    inline bool 
    is_movable_range(I first, I last, T const& value) 
    {
      static_assert(Move_writable<I, T&&>(), "");
      return is_bounded_range(first, last); 
    }
    
  // Returns true if the weak range [first, n) is mutable everywhere except its
  // limit.
  template <typename I>
    inline bool 
    is_mutable_range(I first, Difference_type<I> n)
    {
      return n > 0 ? is_writable_range(first, n, *first) : true;
    }
    
  // Returns true if the bounded range [first, last) is mutable everywhere
  // except its limit.
  template <typename I>
    inline bool 
    is_mutable_range(I first, I last)
    {
      return first != last ? is_writable_range(first, last, *first) : true;
    }

  // Return true if the weak range [first, n) is permutable everywhere except
  // its limit.
  template <typename I>
    inline bool 
    is_permutable_range(I first, Difference_type<I> n)
    {
      return n > 0 ? is_movable_range(first, n, *first) : true;
    }
    
  // Return true if the bounded range [first, n) is permutable everywhere
  // except its limit.
  template <typename I>
    inline bool 
    is_permutable_range(I first, I last)
    {
      return first != last ? is_movable_range(first, last, *first) : true;
    }

} // namespace origin

#endif
