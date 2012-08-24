// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ALGORITHM_QUANTIFIER_HPP
#define ORIGIN_ALGORITHM_QUANTIFIER_HPP

#include <algorithm>

namespace origin
{
  // Quantifiers
  // The quantifier algorithms evaluate properties of a range of elements.
  // There are five quantifiers: all, none, some, non_all, and one. Note that
  // some is called "any" in the C++ standard library.
  //
  // Note that:
  //    !all == not_all (obviously)
  //    !some == none
  //    !one == some || none
  //
  // These algorithms in this module evaluate a unary predicate over a range
  // of elements.
  //
  //    all_of(first, last, pred)
  //    not_all_of(first, last, pred)
  //    some_of(first, last, pred)
  //    none_of(first, last, pred)
  //    one_of(first, last, pred)
  //
  // TODO: Consider providing overloads like all_of(first, last) where the
  // predicate simply converts *i to bool.
  //
  // These algorithms evaluate the equality of elements to some value. 
  //
  //    all_equal(first, last, value)
  //    not_all_equal(first, last, value)
  //    some_equal(first, last, value)
  //    none_equal(first, last, value)
  //    one_equal(first, last, value)
  //
  // And all of those are generalized over the relation used to compare the
  // the values.
  //
  //    all_equal(first, last, value, comp)
  //    not_all_equal(first, last, value, comp)
  //    some_equal(first, last, value, comp)
  //    none_equal(first, last, value, comp)
  //    one_equal(first, last, value, comp)
  //
  // We also support range interfaces for these algorithsm.
  //
  //    all_of(range, pred)
  //    not_all_of(range, pred)
  //    some_of(range, pred)
  //    none_of(range, pred)
  //    one_of(range, pred)
  //
  //    all_equal(range, value)
  //    not_all_equal(range, value)
  //    some_equal(range, value)
  //    none_equal(range, value)
  //    one_equal(range, value)
  //
  //    all_equal(range, value, comp)
  //    not_all_equal(range, value, comp)
  //    some_equal(range, value, comp)
  //    none_equal(range, value, comp)
  //    one_equal(range, value, comp)
  //
  // Some overloads are also adapted for initializer lists.
  //
  //    all_of(list, pred)
  //    not_all(list, pred)
  //    some_of(list, pred)
  //    none_of(list, pred)
  //    one_of(list, pred)


  // Note: I have resisted the urge to name the not_all algorithms with the
  // nonce word "nall" as in nall_of, nall_equal, etc. Despite the fact that
  // nall is an appropriate substitute for "not all", it isn't very common.
  // In fact it's downright obscure (but I still like it).

  

  template <typename I, typename T> I find(I, I, const T&);
  template <typename I, typename P> I find_if(I, I, P);
  template <typename I, typename P> I find_if_not(I, I, P);


  // All of 
  //
  // Returns true if pred(*i) is true for all iterators i in a range, or if that
  // range is empty. There are six overloads of this function:
  //
  //    all_of(first, last, pred) ~> b
  //    all_of(first, last) ~> b
  //    all_of(range, pred) ~> b
  //    all_of(range) ~> b
  //    all_of(list, pred) ~> b
  //    all_of(list)
  //
  // The first twp evaluate pred(*i) for each element in [first, last). The 
  // remaining overloads are equivalent to calling the former using the begin 
  // and end of the given range or initializer list.
  //
  // When no predicate is given, the elements of the range must be convertible
  // to bool.

  template <typename I, typename P>
    inline bool 
    all_of(I first, I last, P pred)
    {
      static_assert(Query<I, P>(), "");
      assert(is_readable_range(first, last));
      return find_if_not(first, last, pred) == last;
    }

  template <typename I>
    inline bool 
    all_of(I first, I last)
    {
      static_assert(Input_iterator<I>(), "");
      static_assert(Convertible<Value_type<I>, bool>(), "");
      assert(is_readable_range(first, last));
      return all_of(first, last, To_bool{});
    }

  template <typename R, typename P>
    inline bool 
    all_of(const R& range, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      return all_of(begin(range), end(range), pred);
    }
    
  template <typename R>
    inline bool 
    all_of(const R& range)
    {
      static_assert(Input_range<R>(), "");
      static_assert(Convertible<Value_type<R>, bool>(), "");
      return all_of(begin(range), end(range), To_bool{});
    }

  template <typename T, typename P>
    inline bool 
    all_of(std::initializer_list<T> list, P pred)
    {
      static_assert(Predicate<P, T>(), "");
      return all_of(list.begin(), list.end(), pred);
    }
    
  template<typename T>
    inline bool 
    all_of(std::initializer_list<T> list)
    {
      static_assert(Convertible<T, bool>(), "");
      return all_of(list.begin(), list.end(), make_to_bool());
    }
    


  // Not all of
  //
  // Returns true when if pred(*i) is not true for all iterators i in some
  // range or if that range is empty. There are six overloads of this functon:
  //
  //    not_all_of(first, last, pred) ~> b
  //    not_all_of(first, last) ~> b
  //    not_all_of(range, pred) ~> b
  //    not_all_of(range) ~> b
  //    not_all_of(list, pred) ~> b
  //    not_all_of(list) ~> b
  //
  // FIXME: Finish documentation.

  template <typename I, typename P>
    inline bool 
    not_all_of(I first, I last, P pred)
    {
      static_assert(Query<I, P>(), "");
      assert(is_readable_range(first, last));
      return find_if_not(first, last, pred) != last;
    }
  
  template <typename I>
    inline bool 
    not_all_of(I first, I last)
    {
      static_assert(Input_iterator<I>(), "");
      static_assert(Convertible<Value_type<I>, bool>(), "");
      assert(is_readable_range(first, last));
      return not_all_of(first, last, To_bool{});
    }
  
  template <typename R, typename P>
    inline bool 
    not_all_of(const R& range, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      return not_all_of(begin(range), end(range), pred);
    }
    
  template<typename R>
    inline bool 
    not_all_of(const R& range)
    {
      static_assert(Input_range<R>(), "");
      static_assert(Convertible<Value_type<R>, bool>(), "");
      return not_all_of(begin(range), end(range), To_bool{});
    }
    
  template <typename T, typename P>
    inline bool 
    not_all_of(std::initializer_list<T> list, P pred)
    {
      static_assert(Predicate<P, T>(), "");
      return not_all_of(list.begin(), list.end(), pred);
    }
    
  template <typename T>
    inline bool 
    not_all_of(std::initializer_list<T> list)
    {
      static_assert(Convertible<T, bool>(), "");
      return not_all_of(list.begin(), list.end(), To_bool{});
    }
    
    

  // Some of
  //
  // Returns true if pred(*i) is true for at least one iterator in some
  // non-empty range. Returns false otherwise. There are six overloads of this
  // function:
  //
  //    some_of(first, last, pred) ~> b
  //    some_of(first, last) ~> b
  //    some_of(range, pred) ~> b
  //    some_of(range) ~> b
  //    some_of(list, pred) ~> b
  //    some_of(list, pred) ~> b
  //
  // FIXME: Finish documentation.
  
  template <typename I, typename P>
    inline bool 
    some_of(I first, I last, P pred)
    {
      static_assert(Query<I, P>(), "");
      assert(is_readable_range(first, last));
      return find_if(first, last, pred) != last;
    }

  template <typename I>
    inline bool 
    some_of(I first, I last)
    {
      static_assert(Input_iterator<I>(), "");
      static_assert(Convertible<Value_type<I>, bool>(), "");
      assert(is_readable_range(first, last));
      return some_of(first, last, To_bool{});
    }

  template <typename R, typename P>
    inline bool
    some_of(const R& range, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      return some_of(begin(range), end(range), pred);
    }

  template <typename R>
    inline bool 
    some_of(const R& range)
    {
      static_assert(Input_range<R>(), "");
      static_assert(Convertible<Value_type<R>, bool>(), "");
      return some_of(begin(range), end(range), To_bool{});
    }
    
  template <typename T, typename P>
    inline bool 
    some_of(std::initializer_list<T> list, P pred)
    {
      static_assert(Predicate<P, T>(), "");
      return some_of(list.begin(), list.end(), pred);
    }

  template <typename T>
    inline bool 
    some_of(std::initializer_list<T> list)
    {
      return some_of(list.begin(), list.end(), To_bool{});
    }
    


  // None of
  //
  // Returns true if pred(*i) is false for all iterators in a range or if that
  // range is empty. There are six overloads of that function:
  //
  //    none_of(first, last, pred) ~> b
  //    none_of(first, last) ~> b
  //    none_of(range, pred) ~> b
  //    none_of(range) ~> b
  //    none_of(list, pred) ~> b
  //    none_of(list) ~> b
  //
  // FIXME: Finish documentation

  template <typename I, typename P>
    inline bool 
    none_of(I first, I last, P pred)
    {
      static_assert(Query<I, P>(), "");
      assert(is_readable_range(first, last));
      return find_if(first, last, pred) == last;
    }

  template <typename I>
    inline bool 
    none_of(I first, I last)
    {
      static_assert(Input_iterator<I>(), "");
      static_assert(Convertible<Value_type<I>, bool>(), "");
      assert(is_readable_range(first, last));
      return none_of(first, last, To_bool{});
    }

  template <typename R, typename P>
    inline bool 
    none_of(const R& range, P pred)
    {
      static_assert(Range_query<R, P>(), "");
      return none_of(begin(range), end(range), pred);
    }
 
  template <typename R>
    inline bool 
    none_of(const R& range)
    {
      static_assert(Input_range<R>(), "");
      static_assert(Convertible<Value_type<R>, bool>(), "");
      return none_of(begin(range), end(range), To_bool{});
    }

  template <typename T, typename P>
    inline bool 
    none_of(std::initializer_list<T> list, P pred)
    {
      static_assert(Predicate<P, T>(), "");
      return none_of(list.begin(), list.end(), pred);
    }

  template <typename T>
    inline bool 
    none_of(std::initializer_list<T> list)
    {
      return none_of(list.begin(), list.end(), To_bool{});
    }

  

  // One of
  //
  // Returns true when there exists exactly one iterator on a range where
  // pred(*i) is true. There are six overloads of this function:
  //
  //    one_of(first, last, pred) ~> b
  //    one_of(first, last) ~> b
  //    one_of(range, pred) ~> b
  //    one_of(range) ~> b
  //    one_of(list, pred) ~> b
  //    one_of(list) ~> b
  //
  // Complexity:
  //    At most distance(first, last) applications of pred, exactly that many
  //    if the result is true.
  //
  // FIXME: Finish documentation.

  template <typename I, typename P>
    inline bool 
    one_of(I first, I last, P pred)
    {
      static_assert(Query<I, P>(), "");
      assert(is_readable_range(first, last));
      
      first = find_if(first, last, pred);
      if (first != last)
        return none_of(++first, last, pred);
      else
        return false;
    }

  template <typename I>
    inline bool 
    one_of(I first, I last)
    {
      static_assert(Input_iterator<I>(), "");
      static_assert(Convertible<Value_type<I>, bool>(), "");
      assert(is_readable_range(first, last));
      return one_of(first, last, To_bool{});
    }
  
  template <typename R, typename P>
    inline bool 
    one_of(const R& range, P pred)
    {
      static_assert(Range_query<R, P>(), ""); 
      return one_of(begin(range), end(range), pred);
    }
 
  template <typename R>
    inline bool 
    one_of(const R& range)
    {
      static_assert(Input_range<R>(), "");
      static_assert(Convertible<Value_type<R>, bool>(), "");
      return one_of(begin(range), end(range), To_bool{});
    }

  template <typename T, typename P>
    inline bool 
    one_of(std::initializer_list<T> list, P pred)
    {
      static_assert(Predicate<P, T>(), "");
      return one_of(list.begin(), list.end(), pred);
    }

  template <typename T>
    inline bool 
    one_of(std::initializer_list<T> list)
    {
      return one_of(list.begin(), list.end(), To_bool{});
    }
 
 

  // All related
  //
  // Returns true if comp(*i, value) is true for each iterator i in a range. 
  // There are two overloads of this function.
  //
  //    all_related(first, last, value, comp)
  //    all_related(range, value, comp)
  //    all_related(list, value, comp)
  //

  template <typename I, typename T, typename R>
    inline bool 
    all_related(I first, I last, const T& value, R comp)
    {
      static_assert(Search<I, T, R>(), "");
      return find_not(first, last, value, comp) == last;
    }

  template <typename R, typename T, typename Comp>
    inline bool 
    all_related(const R& range, const T& value, Comp comp)
    {
      return all_related(begin(range), end(range), value, comp);
    }
    
  template <typename T, typename U, typename R>
    inline bool 
    all_related(std::initializer_list<T> list, const U& value, R comp)
    {
      static_assert(Relation<R, T, U>(), "");
      return all_related(list.begin(), list.end(), value, comp);
    }
  

   
  // All equal
  //
  // Returns true if x == value for all elements x in [first, last).
  //
  //    all_equaL(first, last, value)
  //    all_equal(range, value)
  //    all_equal(list, value)
  //

  template <typename I, typename T>
    inline bool 
    all_equal(I first, I last, const T& value)
    {
      return all_related(first, last, value, eq());
    }

  template <typename R, typename T>
    inline bool 
    all_equal(const R& range, const T& value)
    {
      static_assert(Range_search<R, T>(), "");
      return all_related(begin(range), end(range), value, eq());
    }

  template<typename T, typename U>
    inline bool 
    all_equal(std::initializer_list<T> list, const U& value)
    {
      static_assert(Equality_comparable<T, U>(), "");

      return all_equal(list.begin(), list.end(), value);
    }
     
 
    
  // Not all related
  //
  // Returns true if comp(*i, value) is false for all iterator in a range.
  //
  //    not_all_related(first, last, value, comp)
  //    not_all_related(range, value, comp)
  //    not_all_related(list, value, comp)
  //

  template <typename I, typename T, typename R>
    inline bool 
    not_all_related(I first, I last, const T& value, R comp)
    {
      return find_not(first, last, value, comp) != last;
    }

  template <typename R, typename T, typename Rel>
    inline bool 
    not_all_related(const R& range, const T& value, Rel comp)
    {
      return not_all_related(begin(range), end(range), value, comp);
    }

  template <typename T, typename U, typename R>
    inline bool 
    not_all_related(std::initializer_list<T> list, const U& value, R comp)
    {
      return not_all_related(list.begin(), list.end(), value, comp);
    }



  // Not all equal
  //
  //    not_all_equal(first, last, value)
  //    not_all_equal(range, value)
  //    not_all_equal(list, value)
  //

  template <typename I, typename T>
    inline bool 
    not_all_equal(I first, I last, const T& value)
    {
      return find_not(first, last, value, eq()) != last;
    }

  template <typename R, typename T>
    inline bool 
    not_all_equal(const R& range, const T& value)
    {
      static_assert(Range_search<R, T>(), "");
      return not_all_equal(begin(range), end(range), value);
    }

  template <typename T, typename U>
    inline bool 
    not_all_equal(std::initializer_list<T> list, const U& value)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return not_all_equal(list.begin(), list.end(), value);
    }




  // Some related
  //
  // Returns true if comp(x, value) is true for some element x in [first, last).
  //
  //    some_related(first, last, value, comp)
  //    some_related(range, value, comp)
  //    some_related(list, value, comp)
  //

  template <typename I, typename T, typename R>
    inline bool
    some_related(I first, I last, const T& value, R comp)
    {
      return find(first, last, value, comp) != last;
    }


  template <typename R, typename T, typename C>
    inline bool 
    some_equal(const R& range, const T& value, C comp)
    {
      static_assert(Range_search<R, T, C>(), "");
      return some_equal(begin(range), end(range), value, comp);
    }

  template <typename T, typename U, typename R>
    inline bool 
    some_equal(std::initializer_list<T> list, const U& value, R comp)
    {
      return some_equal(list.begin(), list.end(), value, comp);
    }



  // Some equal
  //
  //    some_equal(first, last, value)
  //    some_equal(range, value)
  //    some_equal(list, value)
  //

  template <typename I, typename T>
    inline bool 
    some_equal(I first, I last, const T& value)
    {
      return find(first, last, value) != last;
    }

  template <typename R, typename T>
    inline bool 
    some_equal(const R& range, const T& value)
    {
      static_assert(Range_search<R, T, Equal_to>(), "");
      return some_equal(begin(range), end(range), value);
    }

  template <typename T, typename U>
    inline bool 
    some_equal(std::initializer_list<T> list, const U& value)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return some_equal(list.begin(), list.end(), value);
    }

    
    
  // None related
  // Returns true if !comp(x, value) for all elements x in [first, last).
  //
  //    none_related(first, last, value, comp)
  //    none_related(range, value, comp)
  //    none_related(list, value, comp)
  //

  template <typename I, typename T, typename R>
    inline bool 
    none_related(I first, I last, const T& value, R comp)
    {
      return find(first, last, value, comp) == last;
    }

    
  template <typename R, typename T, typename Comp>
    inline bool 
    none_related(const R& range, const T& value, Comp comp)
    {
      return none_related(begin(range), end(range), value, comp);
    }


  template <typename T, typename U, typename R>
    inline bool 
    none_equal(std::initializer_list<T> list, const U& value, R comp)
    {
      return none_equal(list.begin(), list.end(), value, comp);
    }

    

  // None equal
  //
  //    none_equal(first, last, value)
  //    none_equal(range, value)
  //    none_equal(list, value)
  //

  template <typename I, typename T>
    inline bool 
    none_equal(I first, I last, const T& value)
    {
      static_assert(Search<I, T>(), "");
      assert(is_readable_range(first, last));
      return find(first, last, value) == last;
    }

  template <typename R, typename T>
    inline bool 
    none_equal(const R& range, const T& value)
    {
      static_assert(Range_search<R, T>(), "");
      return none_equal(begin(range), end(range), value);
    }

  template <typename T, typename U>
    inline bool 
    none_equal(std::initializer_list<T> list, const U& value)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return none_equal(list.begin(), list.end(), value);
    }


    
  // One related
  // Returns true if comp(x, value) for exactly one element x in [first, last).
  //
  //    one_related(first, last, value, comp)
  //    one_related(range, value, comp)
  //    one_related(list, value, comp)
  //

  template <typename I, typename T, typename R>
    inline bool 
    one_related(I first, I last, const T& value, R comp)
    {
      static_assert(Search<I, T, R>(), "");
      first = find(first, last, value, comp);
      if (first != last)
        return none_related(++first, last, value, comp);
      else
        return false;
    }

  template <typename R, typename T, typename Rel>
    inline bool 
    one_related(const R& range, const T& value, Rel comp)
    {
      static_assert(Range_search<R, T>(), "");
      return one_related(begin(range), end(range), value, comp);
    }

  template <typename T, typename U, typename R>
    inline bool 
    one_related(std::initializer_list<T> list, const U& value, R comp)
    {
      static_assert(Relation<R, T, U>(), "");
      return one_related(list.begin(), list.end(), value, comp);
    }

    

  // One equal
  // Returns true if x == value for exactly one element x in [first, last).
  //
  //    one_equal(first, last, value)
  //    one_equal(range, value)
  //    one_equal(list, value)
  //

  template <typename I, typename T>
    inline bool 
    one_equal(I first, I last, const T& value)
    {
      static_assert(Search<I, T>(), "");
      assert(is_readable_range(first, last));
      first = find(first, last, value);
      if (first != last)
        return none_equal(++first, last, value);
      else
        return false;
    }
    
  template <typename R, typename T>
    inline bool 
    one_equal(const R& range, const T& value)
    {
      static_assert(Range_search<R, T>(), "");
      return one_equal(begin(range), end(range), value);
    }

  template <typename T, typename U>
    inline bool 
    one_equal(std::initializer_list<T> list, const U& value)
    {
      static_assert(Equality_comparable<T, U>(), "");
      return one_equal(list.begin(), list.end(), value);
    }

} // namespace origin

#endif
