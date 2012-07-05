// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_CONCEPTS_HPP
#define ORIGIN_ITERATOR_CONCEPTS_HPP

#include <origin/type/concepts.hpp>

namespace origin
{

  //////////////////////////////////////////////////////////////////////////////
  // Reading and Writing                                                 iter.rw
  //
  // There are three concepts describing reading from and writing through
  // iterators: Readable, Move_writable, and Copy_writable.
  //
  // TODO: Document me...
  //////////////////////////////////////////////////////////////////////////////



  //////////////////////////////////////////////////////////////////////////////
  // Readable                                                          iter.read
  //
  // A type is readable if it has an associated value type, is dereferenceable,
  // and a constant refernce to the value type can be bound to the result of
  // dereferencing.
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
  template <typename I, typename T>
    constexpr bool Copy_writable()
    {
      return Move_writable<I, T>() 
          && Assignable<Dereference_result<I>, const T&>();
    };


    
  //////////////////////////////////////////////////////////////////////////////
  // Permutable                                                  iter.permutable
  //
  // Returns true if I is permutable. A permutable iterator is readable,
  // move-writable, and has a movable value type.
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
  // Returns true if I mutable. A mutable iterator is readable and writable,
  // and has a copyable value type.
  //
  // Note that all Mutable iterator types are also Permutable.
  template <typename I>
    constexpr bool Mutable()
    {
      return Readable<I>() 
          && Copyable<Value_type<I>>() 
          && Copy_writable<I, Value_type<I>>();
    }
    
    

  //////////////////////////////////////////////////////////////////////////////
  // Incrementable Types                                               iter.incr
  //
  // TODO: Document me...
  //////////////////////////////////////////////////////////////////////////////



  //////////////////////////////////////////////////////////////////////////////
  // Incrementable   
  //
  // A weakly incrementable type is a semiregular type that can be pre- and
  // post-incremented. Neither operation is requireed to be equality
  // preserving, and the result of post-increment is unspecified.
  //
  // An incrementable type is an equality comparable, weakly incrementable 
  // type with a post-increment operator.
  //
  // Note that the weakly incrementable type does not include the
  // post-increment operator.


  // Returns true if I is weakly incrementable.
  template <typename I>
    constexpr bool Weakly_incrementable()
    {
      return Copyable<I>()
          && Equality_comparable<I>()
          && Has_difference_type<I>()
          && Has_pre_increment<I>()
          && Same<Pre_increment_result<I>, I&>();
    }
    
  
  // Returns true if I is incremetnable.
  template <typename I>
    constexpr bool Incrementable()
    {
      return Weakly_incrementable<I>()
          && Same<Post_increment_result<I>, I>();
    }


  //////////////////////////////////////////////////////////////////////////////
  // Strict Weakly Incrementable
  //
  // A type that is weakly incrementable but not (strongly) incrementable.
  template <typename I>
    constexpr bool Strict_weakly_incrementable()
    {
      return Weakly_incrementable<I>() && !Incrementable<I>();
    }




  //////////////////////////////////////////////////////////////////////////////
  // Decrementable
  //
  // A decrementable type is an inrementable type that can also be pre- and
  // post-decremented.
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
