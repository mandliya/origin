// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ALGORITHM_COPY_HPP
#define ORIGIN_ALGORITHM_COPY_HPP

#include <algorithm>

namespace origin
{
  template <typename T> T&       min(T& a, T& b);
  template <typename T> const T& min(const T& a, const T& b);


  // TODO: Write origin_memmove and origin_memcopy too.

  // Origin memset
  //
  // The Origin memset algorithm wraps standard memset and returns a pointer
  // to the end of the filled range. The syntax of the algorithm is:
  //
  //    origin_memset(ptr, n, value)
  //
  template <typename T>
    T* origin_memset(T* ptr, std::size_t n, const T& value)
    {
      static_assert(Byte<T>(), "");
      assume(is_writable_range(ptr, ptr + n, value));

      std::memset(ptr, static_cast<unsigned char>(value), n);
      return ptr + n;
    }



  // Copy algorithms
  // The copy module concerns the transfer of values form one range of eleemnts
  // into (or over) another range. It has the following algorithms:
  //
  //    copy(first, last, result)
  //    copy_n(first, last, result)
  //
  //    copy_if(first, last, pred)
  //    copy_if_not(first, last, pred)
  //    ...
  //
  // The range-based algorithms support the following interfaces.
  //
  //    copy(in, out)
  //    copy_n(in, out)
  //    copy_if(in, out, pred)
  //    copy_if_not(in, out, pred)
  //
  // Here, out can either be an output iterator or an output range.
  //
  // Move algorithms also transfer values, but invalidate the original object.
  //
  //    move(first, last, result)
  //    ...
  //    move_if(first, last, result, pred)
  //    move_if(in, out, pred)
  //    move_if_not(first, last, result, pred)
  //    move_if_not(in, out, pred)
  //
  // For good measure, we include swap algorithms:
  //
  //    iter_swap(i, j)
  //
  //    exchange(first1, last1, first2)
  //    exchange_if(first1, last1, first2, pred)
  //
  // FIXME: Finish documenting all of these algorithms.
  //
  // FIXME: Add overlap requirements for all of these algorithms.

  
   
  // Copy step
  // Copy the value of *i to the object pointed at by o and increment both
  // iterators.
  template <typename I, typename O>
    inline void copy_step(I& i, O& o)
    {
      *o = *i;
      ++i;
      ++o;
    }


  // Copy implementation
  //
  // The copy algorithm can be heavily optimized for contiguous ranges of
  // trivially copyable types.
  //
  // TODO:

  template <typename I, typename O>
    inline O 
    copy_impl(I first, I last, O result)
    {
      while (first != last)
        copy_step(first, result);
      return result;
    }
  
  template <typename T>
    inline auto 
    copy_impl(const T* first, const T* last, T* result) 
      -> Requires<Trivial<T>(), T*>
    {
      std::size_t n = last - first;
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  

  template <typename T>
    inline auto 
    copy_impl(T* first, T* last, T* result) 
      -> Requires<Trivial<T>(), T*> 
    {
      std::size_t n = last - first;
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }


  // Copy
  //
  // Copy the values from an input range into an output range. There are two
  // overloads of this function:
  //
  //    copy(first, last, result) ~> i
  //    copy(range, result) ~> i
  //    copy(in, out) ~> i
  //
  // The first overload copies the elements in [first, last) into the output
  // range indicated by result. The last two overloads are equivalent to:
  //
  //    copy(range, result) <=> copy(begin(range), end(range), result)
  //    copy(in, out) <=> copy(begin(in), end(in), begin(out))
  //
  // The algorithm returns an iterator past the last assigned output iterator,
  // indicating the end of that range.



  template <typename I, typename O>
    inline O
    copy(I first, I last, O result)
    {
      static_assert(Copy<I, O>(), "");
      assert(is_readable_range(first, last));
      assume(is_writable_range(result, distance(first, last), *first));
      assume(not_overlapped(first, last, result));
      return copy_impl(unwrap(first), unwrap(last), unwrap(result));
    }
  
  template <typename R, typename O>
    inline auto
    copy(const R& range, O result)
      -> Requires<Output_iterator<O>(), O>
    {
      return copy(begin(range), end(range), result);
    }

  template <typename R, typename O>
    inline auto
    copy(const R& in, O&& out)
      -> Requires<Output_range<O>(), decltype(begin(out))>
    {
      assume(size(out) >= size(in));
      return copy(begin(in), end(in), begin(out));
    }



  // Bounded copy implementation
  //
  // The bounded copy algorithm can be optimized for contiguous ranges of
  // trivially copyable types.
  //
  // FIXME: The optimization constraint is too strong. It should be trivially
  // copyable not trivial.

  template <typename I, typename O>
    inline std::pair<I, O>
    bounded_copy_impl(I in_first, I in_last, O out_first, O out_last)
    {
      while (in_first != in_last && out_first != out_last)
        copy_step(in_first, out_first);
      return {in_first, out_first};
    }

  template <typename T>
    inline auto
    bounded_copy_impl(const T* in_first, const T* in_last, T* out_first, T* out_last) 
      -> Requires<Trivial<T>(), std::pair<const T*, T*>>
    {
      std::size_t n = min(in_last - in_first, out_last - out_first);
      std::memcpy(out_first, in_first, n * sizeof(T));
      return {in_first + n, out_first + n};
    }

  template <typename T>
    inline auto
    bounded_copy_impl(T* in_first, T* in_last, T* out_first, T* out_last) 
      -> Requires<Trivial<T>(), std::pair<T*, T*>>
    {
      std::size_t n = min(in_last - in_first, out_last - out_first);
      std::memcpy(out_first, in_first, n * sizeof(T));
      return {in_first + n, out_first + n};
    }



  // Bounded copy
  //
  // Copy as many elements from an input range into an output range.If the size
  // of the input range is less than that of the output range, then all elements
  // will be copied. Otherwise, only some elements will be copied. There are
  // two overloads of this algorithm:
  //
  //    bounded_copy(in_first, in_last, out_first, out_last)
  //    bounded_copy(in, out)
  //
  // The algorithm returns a pair of  iterators past the last copied element in
  // the input range and the last assigned element in the output range.

  template <typename I, typename O>
    inline std::pair<I, O>
    bounded_copy(I in_first, I in_last, O out_first, O out_last)
    {
      static_assert(Copy<I, O>(), "");
      assert(is_readable_range(in_first, in_last));
      assert(is_writable_range(out_first, out_last, *in_first));
      assume(not_overlapped(in_first, in_last, out_first));
      
      return bounded_copy_impl(unwrap(in_first), unwrap(in_last),
                               unwrap(out_first), unwrap(out_last));
    }

  template <typename R, typename O>
    inline auto
    bounded_copy(R&& in, O&& out)
      -> std::pair<decltype(begin(in)), decltype(begin(out))>
    {
      return bounded_copy(begin(in), end(in), begin(out), end(out));
    }



  // Copy n implementation
  //
  // FIXME: The optimizations should require trivially copy assignable, but
  // GCC does not support those traits yet.
  template <typename I, typename O>
    inline O copy_n_impl(I first, Difference_type<I> n, O result)
    {
      while(n != 0) {
        copy_step(first, result);
        --n;
      }
      return result;
    }

  template <typename T>
    inline auto copy_n_impl(const T* first, std::ptrdiff_t n, T* result) 
      -> Requires<Trivial<T>(), T*>
    {
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  

  template <typename T>
    inline auto copy_n_impl(T* first, std::ptrdiff_t n, T* result) 
      -> Requires<Trivial<T>(), T*> 
    {
      std::memcpy(result, first, n * sizeof(T));
      return result + n;
    }  


  // Copy n
  // Copy the values in the range [first, first + n) into the output range
  // pointed to be result.
  template <typename I, typename O>
    inline O copy_n(I first, Difference_type<I> n, O result)
    {
      static_assert(Copy<I, O>(), "");
      return copy_n_impl(unwrap(first), n, unwrap(result));
    }




  // Copy if
  template <typename I, typename O, typename P>
    inline O copy_if(I first, I last, O result, P pred)
    {
      static_assert(Query<I, P>(), "");
      static_assert(Copy<I, O>(), "");
      assert(is_readable_range(first, last));
      assume(is_writable_range(result, count_if(first, last, pred), *first));
      
      while(first != last) {
        if(pred(*first))
          copy_step(first, result);
        else
          ++first;
      }
      return result;
    }
    
  

    
  // Copy if not
  template <typename I, typename O, typename P>
    inline O copy_if_not(I first, I last, O result, P pred)
    {
      while(first != last) {
        if(!pred(*first))
          copy_step(first, result);
        else
          ++first;
      }
      return result;
    }




  // Copy backward step
  // Decrement both iterators and copy the value of *i to the object pointed
  // at by o.
  template <typename I, typename O>
    inline void copy_backward_step(I& i, O& o)
    {
      --i;
      --o;
      *o = *i;
    }
    

  // Copy backward
  //
  // TODO: Optimize for the the case where the value can be trivially moved
  // from the input range to the output range.
  template <typename I, typename O>
    inline O copy_backward(I first, I last, O result)
    {
      static_assert(Bidirectional_iterator<I>(), "");
      static_assert(Bidirectional_iterator<O>(), "");
      static_assert(Copy<I, O>(), "");

      while(first != last)
        copy_backward_step(first, result);
      return result;
    }
    
    
  
  // Copy backward (range)
  template <typename R, typename O>
    inline void copy_backward(const R& in, O&& out)
    {
      static_assert(Bidirectional_range<O>(), "");
      static_assert(Bidirectional_range<Auto<O>>(), "");
      static_assert(Range_copy<R, Auto<O>>(), "");
      return copy_backward(begin(in), end(in), begin(out));
    }


    
  // Move step
  // Move the value of *i into the object pointed at by *o and increment both
  // iterators.
  template <typename I, typename O>
    void move_step(I& i, O& o)
    {
      *o = std::move(*i);
      ++i;
      ++o;
    }



  // Move implementation
  //
  // FIXME: 

  template <typename I, typename O>
    inline O move_impl(I first, I last, O result)
    {
      while(first != last)
        move_step(first, result);
      return result;
    }

  template <typename T>
    inline auto move_impl(const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      std::memmove(result, first, n * sizeof(T));
      return result + n;
    }

  template <typename T>
    inline auto move_impl(T* first, T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      std::memmove(result, first, n * sizeof(T));
      return result + n;
    }


    
  // Move
  // Move the elements in the range [first, last) into the range 
  // [result, result + (last - first)).
  template <typename I, typename O>
    inline O move(I first, I last, O result)
    {
      static_assert(Move<I, O>(), "");
      assert(is_readable_range(first, last));
      assume(is_movable_range(result, distance(first, last), *first));
      return move_impl(unwrap(first), unwrap(last), unwrap(result));
    }
  
  template <typename R, typename O>
    inline void move(const R& i, O o)
    {
      static_assert(Range_move<R, O>(), "");
      assume(size(i) <= size(o));
      move(begin(i), end(i), begin(o));
    }



  // Bounded move implementation
  //
  // FIXME: The optimization constraint is too tight. This should just be
  // trivially move constructible.

  template <typename I, typename O>
    inline std::pair<I, O>
    bounded_move_impl(I in_first, I in_last, O out_first, O out_last)
    {
      while (in_first != in_last && out_first != out_last)
        move_step(in_first, out_first);
      return {in_first, out_first};
    }

  template <typename T>
    inline auto
    bounded_move_impl(const T* in_first, const T* in_last, T* out_first, T* out_last)
      -> Requires<Trivial<T>(), std::pair<const T*, T*>>
    {
      std::size_t n = min(in_last - in_first, out_last - out_first);
      return origin_memcopy(out_first, in_first, n);
    }

  template <typename T>
    inline auto
    bounded_move_impl(T* in_first, T* in_last, T* out_first, T* out_last)
      -> Requires<Trivial<T>(), std::pair<T*, T*>>
    {
      std::size_t n = min(in_last - in_first, out_last - out_first);
      return origin_memcopy(out_first, in_first, n);
    }



  // Bounded move
  //
  // The bounded move algorithm moves the maximum number of elements from an
  // input range to an output range. There are two overloads of this function:
  //
  //    bounded_move(in_first, in_last, out_first, out_last) ~> p
  //    bounded_move(in, out) ~> p
  //
  // The algorithm returns a pair iterators p such that p.first is past the
  // last copied input iterator and p.second is past the last move-assigned
  // output iterator.

  template <typename I, typename O>
    inline std::pair<I, O>
    bounded_move(I in_first, I in_last, O out_first, O out_last)
    {
      return bounded_move_impl(unwrap(in_first), unwrap(in_last), 
                               unwrap(out_first), unwrap(out_last));
    }

  template <typename R, typename O>
    inline auto
    bounded_move(R&& in, O&& out) 
      -> std::pair<decltype(begin(in)), decltype(begin(out))>
    {
      return bounded_move(begin(in), end(in), begin(out), end(out));
    }



  // Move if
  //
  // FIXME: Write documentation.

  template <typename I, typename O, typename P>
    inline O move_if(I first, I last, O result)
    {
      static_assert(Query<I, P>(), "");
      static_assert(Move<I, O>(), "");
      assert(is_readable_range(first, last));
      assume(is_movable_range(result, distance(first, last), *first));

      while(first != last) {
        if(pred(*first))
          move_step(first, result);
        else
          ++first;
      }
      return result;
    }
    
  template <typename R, typename O, typename P>
    inline void move_if(R&& in, O&& out, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      static_assert(Range_move<R, O>(), "");
      return move_if(begin(in), end(in), begin(out), pred);
    }

    
    
  // Move if not
  template <typename I, typename O, typename P>
    inline O move_if_not(I first, I last, O result)
    {
      static_assert(Query<I, P>(), "");
      static_assert(Move<I, O>(), "");
      assert(is_readable_range(first, last));
      assume(is_movable_range(result, distance(first, last), *first));

      while(first != last) {
        if(!pred(*first))
          move_step(first, result);
        else
          ++first;
      }
      return result;
    }

    
    
  // Move if not (range)
  template <typename R, typename O, typename P>
    inline void move_if_not(R&& in, O&& out, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      static_assert(Range_move<R, O>(), "");
      return move_if_not(begin(in), end(in), begin(out), pred);
    }

    
    
  // Move backward step
  // Decrement both iterators and move the representation of *i to the object 
  // pointed at by o.
  template <typename I, typename O>
    inline void move_backward_step(I& i, O& o)
    {
      --i;
      --o;
      *o = std::move(*i);
    }


  // Move backward implementation
  template <typename I, typename O>
    inline O move_backward_impl(I first, I last, O result)
    {
      while (first != last)
        move_backward_step(first, result);
      return result;
    }

  template <typename T>
    inline auto move_backward_impl(const T* first, const T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      result -= n;
      std::memmove(result, first, n * sizeof(T));
      return result;
    }

  template <typename T>
    inline auto move_backward_impl(T* first, T* last, T* result)
      -> Requires<Trivial<T>(), T*>
    {
      std::ptrdiff_t n = last - first;
      result -= n;
      std::memmove(result, first, n * sizeof(T));
      return result;
    }


  // Move backward
  //
  // FIXME: Write type type requirements and preconditions of this algorithm.
  template <typename I, typename O>
    inline O move_backward(I first, I last, O result)
    {
      return move_backward_impl(unwrap(first), unwrap(last), unwrap(result));
    }


  // Move backward (range)
  template <typename R, typename O>
    inline O move_backward(const R& range, O result)
    {
      return move_backward(begin(range), end(range), begin(result));
    }




  // Iterator swap
  //
  // FIXME: I am just about 100% sure that Value_type<I1> and Value_type<I2>
  // must be the same type to guarantee correct behavior. 
  //
  // FIXME: Do not call std::swap! Call origin's swap.
  template <typename I1, typename I2>
    inline void iter_swap(I1 i, I2 j)
    {
      static_assert(Move_writable<I1, Value_type<I2>>(), "");
      static_assert(Move_writable<I2, Value_type<I1>>(), "");

      std::swap(*i, *j);
    }


    
  // Exchange step
  template <typename I1, typename I2>
    inline void exchange_step(I1& i, I2& j)
    {
      iter_swap(i, j);
      ++i;
      ++j;
    }


  // Exchange
  //
  // This is the same as swap_ranges in the standard library and exchange_values
  // in Elements of Programming.
  template <typename I1, typename I2>
    inline I2 exchange(I1 first1, I1 last1, I2 first2)
    {
      static_assert(Move<I1, I2>(), "");
      static_assert(Move<I2, I1>(), "");

      while(first1 != last1)
        swap_step(first1, first2);
      return first2;
    }

    
  
  // Exchange if
  // Selectively exchange the values of [first1, last1) and 
  // [first2, first2 + n).
  template <typename I1, typename I2, typename P>
    inline I2 exchange_if(I1 first1, I1 last1, I2 first2, P pred)
    {
      while(first1 != last1) {
        if(pred(*first1))
          exchange_step(first1, first2);
        else
          ++first1;
      }
      return first2;
    }
    
  



  // Fill algorithms
  //
  // There are a number of algorithms in the fill module. They are:
  //
  //    fill_step(out, value)
  //    fill(first, last, value)
  //    fill(range, value)
  //    fill_n(first, n, value)
  //    bounded_fill_n(first, last, n, value)
  //    bounded_fill_n(range, n, value)



  // Fill step
  //
  // Assign value to the output iterator out and increment it.
  template <typename O, typename T>
    inline void 
    fill_step(O& out, const T& value)
    {
      *out = value;
      ++out;
    }



  // Fill implemenetation
  //
  // The fill implementation is optimized for cases where memset is an
  // appropriate operation: contiguous regions of trivially  constructible
  // objects whose size is 1 byte.
  //
  // FIXME: The optimization constraint is too tight.

  template <typename O, typename T>
    inline void 
    fill_impl(O first, O last, const T& value)
    {
      while (first != last)
        fill_step(first, value);
    }

  template <typename T>
    inline auto
    fill_impl(T* first, T* last, const T& value)
      -> Requires<Byte<T>(), void>
    {
      origin_memset(first, last - first, value);
    }



  // Fill
  //
  // Copy value into each element of an output range. There are two overloads
  // of this function:
  //
  //    fill(first, last, value)
  //    fill(range, value)

  template <typename O, typename T>
    void fill(O first, O last, const T& value)
    {
      static_assert(Fill<O, T>(), "");
      assert(is_writable_range(first, last, value));

      fill_impl(unwrap(first), unwrap(last), value);
    }
    
  template <typename R, typename T>
    inline void 
    fill(R& range, const T& value)
    {
      static_assert(Copyable<T>(), "");
      static_assert(Output_range<T>(), "");
      
      return fill(begin(range), end(range), value);
    }

    

  // Fill n implementation

  template <typename O, typename T>
    inline O 
    fill_n_impl(O first, Difference_type<O> n, const T& value)
    {
      while(n != 0) {
        fill_step(first, value);
        --n;
      }
      return first;
    }

  template <typename T>
    inline auto
    fill_n_impl(T* first, std::ptrdiff_t n, const T& value)
      -> Requires<Byte<T>(), void>
    {
      return origin_memset(first, value, n);
    }

    
  // Fill n
  //
  // Copy value into each element of the range [first, first + n). There are
  // no other overloads of this algorithm.

  template <typename O, typename T>
    inline O 
    fill_n(O first, Difference_type<O> n, const T& value)
    {
      static_assert(Copyable<T>(), "");
      static_assert(Weak_output_iterator<O, T>(), "");
      assert(is_writable_range(first, n, value));

      return fill_n_impl(unwrap(first), n, value);
    }
    


  // Bounded fill n implementation
  //
  // The bounded fill n implementation can be optimized for the case where
  // memset is used to fill a region of memory.

  template <typename O, typename T>
    inline std::pair<O, Difference_type<O>>
    bounded_fill_n_impl(O first, O last, Difference_type<O> n, const T& value)
    {
      while (first != last && n != 0) {
        fill_step(first, value);
        --n;
      }
      return {first, n};
    }

  template <typename T>
    inline auto
    bounded_fill_n_impl(T* first, T* last, std::ptrdiff_t n, const T& value)
      -> Requires<Byte<T>(), std::pair<T*, std::ptrdiff_t>>
    {
      return origin_memset(first, min(last - first, n), value);
    }



  // Bounded fill n
  //
  // Copy as many of n values as possible into the range the output range. The
  // algorithm returns a pair containing an iterator past the last assigned
  // iterator and the number of values that were not copied (if any).
  // 

  template <typename O, typename T>
    inline std::pair<O, Difference_type<O>>
    bounded_fill_n(O first, O last, Difference_type<O> n, const T& value)
    {
      while (first != last && n != 0) {
        fill_step(first, value);
        --n;
      }
      return {first, n};
    }


} // namespace origin

#endif
