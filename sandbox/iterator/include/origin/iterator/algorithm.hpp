// // Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_ALGORITHM_HPP
#define ORIGIN_ITERATOR_ALGORITHM_HPP

#include <cassert>
#include <memory>
#include <random>

#include <origin/functional.hpp>
#include <origin/iterator/concepts.hpp>
#include <origin/iterator/iterator.hpp>

namespace origin {

  // NOTE: With the exception of for_each (which is just a little weird), all
  // of the non-modifying algorithms are concerned with notions of predicate
  // satisfaction or equality. Note that equality can be translated into
  // predicate satisfaction by binding an argument. Many of internals of 
  // equality algorithms reuse predicate algorithms.
  
  // NOTE: It might be worth classifying algorithms as equal and disjoint range 
  // algorithms. An equal range algorithm operations on ranges of equal length.
  // and takes 3 iterator parameters [f1, l1) and [f2, ...). Note that the
  // implied l2 must be reachable from f2 in exactly distance(f1, l1) steps.
  // Examples include equal, mismatch, and copy.
  
  // A disjoint range algorithm operates on two independently sized ranges with 
  // one usually indicating a subrange of another. Examples include search and
  // find_end (which is not very well named, in my opinion).

  // Swap the order of arguments for the given predicate. This require
  // FIXME: This is should be moved into Origin functional.
  // FIXME: How can we generally conceptually constrain a function over its
  // sequence of arguments?
  template<typename Func>
    struct __reorder_arguments
    {
      __reorder_arguments(Func f)
        : fn(f)
      { }
      
      template<typename T, typename U>
        auto operator()(T const& a, U const& b) const -> decltype(fn(b, a))
        {
          return fn(b, a);
        }
        
        Func fn;
    };
    
    template<typename Func>
      inline __reorder_arguments<Func> reorder_arguments(Func fn)
      {
        return __reorder_arguments<Func>{fn};
      }


  // Iter is required to be an Move_iterator. Reading is allowed to be 
  // destructive.
  //
  // Func is required to be a procedure taking an argument of the given value
  // type. Func is not required to be a Function because its return value is 
  // ignored, it can modify its arguments, etc.
  template<typename Iter, typename Func>
    requires(Move_iterator<Iter> && 
             Procedure<Func, decltype(get(std::declval<Iter>()))>)
      Func for_each(Iter f, Iter l, Func fn)
      {
        while(f != l) {
          fn(get(f));
          f = next(f);
        }
      }

  // Predicate algorithms

  // Iter is required to be an Input_iterator, implying a non-destructive read.
  //
  // Pred is required to be a Predicate, taking an argument of Iter's value 
  // type.
  //
  // NOTE: If we allow Move_iterators, then some unknown portion of the 
  // iterated range be invalidated (devalued?) if the algorithm returns false.
  // The same appleis for any_of and none_of.
  //
  // NOTE: It's unclear if this algorithm is useful for non-forward iterators
  // since they would consume the sequence being evaluated. The result of such
  // an operation is that the original arguments to [f, l) will define a new
  // range [f', l) where f' == l if the function returned true and f' != l if
  // otherwise.
  template<typename Iter, typename Pred>
    requires(Input_iterator<Iter> && Predicate<Pred, value_type<Iter>>)
      bool all_of(Iter f, Iter l, Pred p)
      {
        while(f != l) {
          if(!p(get(f)))
            return false;
          f = next(f);
        }
        return true;
      }
    
  // Iter is required to be an Input_iterator, implying a non-destructive read.
  //
  // Pred is required to be a Predicate, taking an argument of the Iter's
  // value type. 
  //
  // SEE: Notes on all_of
  template<typename Iter, typename Pred>
    requires(Input_iterator<Iter> && Predicate<Pred, value_type<Iter>>)
      bool any_of(Iter f, Iter l, Pred p)
      {
        while(f != l) {
          if(p(get(f)))
            return true;
          f = next(f);
        }
        return false;
      }
    
  // Iter is required to be an Input_iterator, implying a non-destructive read.
  //
  // Pred is required to be a Predicate, taking an argument of Iter's value 
  // type. 
  //
  // SEE: Notes on all_of
  template<typename Iter, typename Pred>
    bool none_of(Iter f, Iter l, Pred p)
    {
      while(f != l) {
        if(p(get(f)))
          return false;
        f = next(f);
      }
      return true;
    }

  // FIXME: We have to make a decision about the degree of variability on
  // algorithms that use "natural comparisons": the provided operators. There
  // are generally two options.
  //
  //  1. Write constraints on the syntax of comparing two, possibly unrelated
  //     types (e.g., string x int). This is the status quo, and in the extreme
  //     leads to very general, but gawdawful template requirements.
  //
  //  2. Be more specific about the relationship between the types being
  //     compared. Are we really parameterizing the algorithm over unrelated
  //     types, or do we require them to be related in such a way that we
  //     can require a semantically meaningful natural comparison on only a
  //     singlee type.
  //
  // My preference in this design is the second option, and that relationship
  // will generally be expressed by the Common requirement and its associated
  // Common_type. Most comparability requiremnts on two types will probably
  // involve something like this:
  //
  //    Common<T, U> && Equality_comparable<Common_type<T, U>>
  //
  // The first requirement asserts that T and U share a common type. The second
  // requirement says that we can use operators == and != with argument types
  // of either T or U.
  //
  // A possible shorthand might be Equality_comparable<Common<T, U>::type>.
  // In other words, the requirement Common<T, U> is evaluated before the 
  // lookup of the nested type member. It's also probable that Common_type
  // requires Common, so Equality_comparable<Common_type<T, U>> may also be a
  // valid spelling of the requirements.
  
  // NOTE: Writing requirements in terms of Common/Common_type may require a
  // change to the formulation of the concept and its associated type. It is
  // possible that Common_type<T, U> is neither T nor U. 
  //
  // This may induce a language issue if we perform lookup in terms of 
  // associated requirements (i.e., via so-called archetype concept maps)
  // instead of the current ADL-based solution. For example, consider the
  // following:
  //
  //    using foo = duration<int, ratio<4, 5>>; // foo is 4/5 of a second
  //    using bar = duration<int, ratio<6, 7>>; // bar is 6/7 of a second.
  //    using c = Common_type<foo, bar>;  // == duration<int, ratio<2, 35>>.
  //
  // Basically, we have to ensure that the Equality_comparable<C> finds an
  // overload that accepts either argument type. This means that either the
  // operation is defined generically over any durations (as it is), or that
  // there are overloads for c that take argument types foo and bar. The latter
  // approach is far less tenable (except for very small type systems).

  // NOTE: There's a subtle problem with signed and unsigned integral types.
  // The common type of signed and unsigned integral types is their unsigned
  // value, but we generally don't want to compare signed and unsigned ints
  // for equality or order. Their different encodings can lead to program 
  // errors. Note that such comparisons result in warnings, but not errors.
  // This means that it's possible to pass a concept check but fail type
  // checking (especially if warnings are treated as errors).
  //
  // To me, this would imply that signed and unsigned integral types should not
  // actuaally have a common type because they can't satisfy the common type
  // principle.

  // Iter is required to be an Input_iterator whose value type is 
  // Equality_comparable.
  //
  // Iter's value type is required to be equality comparable with T.
  //
  // NOTE: The algorithm must not accept Move_iterators since it must be the 
  // case that get(find(f, l, x)) == x. That is, get is applied twice to the
  // the same element.
  template<typename Iter, typename T>
    requires(Input_Iterator<Iter> && 
             Equality_comparable<Common_type<value_type<Iter>, T>>)
      Iter find(Iter f, Iter l, T const& x)
      {
        while(f != l) {
          if(get(f) == x)
            return f;
          f = next(f);
        }
        return l;
      }
    
  // Iter is required to be an Input_iterator.
  //
  // Pred is required to be a Predicate taking an argument of Iter's value
  // type.
  //
  // SEE: Notes on find(). The same principle applies since it must be the case
  // that p(get(find_if(f, l, p))) == true. 
  template<typename Iter, typename Pred>
    requires(Input_Iterator<Iter> && Predicate<Pred, value_type<Iter>>)
      Iter find_if(Iter f, Iter l, Pred p)
      {
        while(f != l) {
          if(p(get(f)))
            return f;
          f = next(f);
        }
        return l;
      }

  // Iter is required to be an Input_iterator.
  //
  // Pred is required to be a Predicate taking an argument of Iter's value
  // type.
  //
  // SEE: Notes on find(). The same principle applies since it must be the case
  // that p(get(find_if(f, l, p))) == true. 
  template<typename Iter, typename Pred>
    requires(Input_Iterator<Iter> && Predicate<Pred, value_type<Iter>>)
      Iter find_if_not(Iter f, Iter l, Pred p)
      {
        while(f != l) {
          if(!p(get(f)))
            return f;
          f = next(f);
        }
      }

  // Iter is required to be an Input_iterator whose value type is 
  // Equality_comparable.
  //
  // Iter's value type is required to be equality comparable with T.
  //
  // NOTE: It's possible that we could permit Move_iterators for the count
  // algorithms since the algorithm traverses the entire sequence. It should
  // not be possible to leave the sequence in a partially devalued state.
  template<typename Iter, typename T>
    requires(Input_Iterator<Iter> && 
             Equality_comparable<Common_type<value_type<Iter>, T>>)
      typename difference_type<Iter>::type count(Iter f, Iter l, T const& x)
      {
        typedef typename difference_type<Iter>::type Distance;
        Distance n = 0;
        while(f != l) {
          if(get(f) == x)
            ++n;
          f = next(f);
        }
        return n;
      }
    
  // Iter is required to be an Input_iterator.
  //
  // Pred is required to be a Predicate taking an argument of Iter's value
  // type.
  //
  // SEE: Notes on count().
  template<typename Iter, typename Pred>
    requires(Input_Iterator<Iter> && Predicate<Pred, value_type<Iter>>)
      typename difference_type<Iter>::type count_if(Iter f, Iter l, Pred p)
      {
        typedef typename difference_type<Iter>::type Distance;
        Distance n = 0;
        while(f != l) {
          if(p(get(f)))
            ++n;
          f = next(f);
        }
        return n;
      }

  // Equality algorithms
  // Most if not all of the parameterized over an Eq function require that the
  // algorithm model a binary Predicate instead of a more precise Relation or
  // Equivalence_relation. The generalization allows arbitrary comparisons
  // between different kinds of objects, and does so without affecting the
  // postcondition or invariants of the algorithm. In this way, these 
  // algorithms are not unlike the predicate algorithms above.


  // Mismatch returns the positions where two sequences are first not equal.
  //
  // Iter1 and Iter2 are required to be an Input_iterators.
  //
  // Eq is an binary Predicate taking the value types of Iter1 and Iter2 as
  // arguments, in that order.
  //
  // NOTE: Move_iterators are not permitted since it would make it difficult
  // to adequately express the postconditions of the algorithm; that is, for
  // the result r, r.first != r.second => !eq(get(r.first), get(r.second)).
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Predicate<Eq, Value_type<Iter1>, Value_type<Iter2>)
      std::pair<Iter1, Iter2> mismatch(Iter1 f1, Iter1 l1, Iter2 f2, Eq eq)
      {
        while(f1 != l1) {
          if(!eq(f1, f2))
            return {f1, f2};
          f1 = next(f1);
          f2 = next(f2);
        }
        return {l1, l1};
      }
    
  // Iter1 and Iter2 are required to be an Input_iterators whose value types
  // share a Common type and are Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Equality_comparable<Common_type<Value_type<Iter1>, Value_type<Iter2>>)
      std::pair<Iter1, Iter2> mismatch(Iter1 f1, Iter1 l1, Iter2 f2)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return mismatch(f1, l1, f2, std::equal_to<Value>{});
      }
    
  // Iter1 and Iter2 are required to be an Input_iterators.
  //
  // Eq is an binary Predicate taking the value types of Iter1 and Iter2 as
  // arguments, in that order.
  //
  // SEE: Notes on mismatch().
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Predicate<Eq, Value_type<Iter1>, Value_type<Iter2>)
      bool equal(Iter1 f1, Iter1 l1, Iter2 f2, Eq eq)
      {
        // NOTE: This could be implemented in terms of mismatch, but there are
        // fewer comparisons writing it as a distinct algorithm.
        while(f1 != l1) {
          if(!eq(f1, f2))
            return false;
          f1 = next(f1);
          f2 = next(f2);
        }
      }

  // Iter1 and Iter2 are required to be an Input_iterators whose value types
  // share a Common type and are Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Equality_comparable<Common_type<Value_type<Iter1>, Value_type<Iter2>>)
      bool equal(Iter1 f1, Iter1 l1, Iter2 f2)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return equal(f1, l1, f2, std::equal_to<Value>{});
      }

  // NOTE: find_end is basically search, but returning the last occurrence of
  // the subrange [f2, l2) and not the first. This should actually be called
  // search_last rather than find.
  
  // Iter1 and Iter2 are required to be an Forward_iterators.
  //
  // Eq is an binary Predicate taking the value types of Iter1 and Iter2 as
  // arguments, in that order.
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Forward_iterator<Iter1> && 
             Forward_iterator<Iter2> &&
             Predicate<Eq, Value_type<Iter1>, Value_type<Iter2>)
    Iter1 find_end(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Eq eq)
    {
      typedef typename value_type<Iter1>::type Value1;
      typedef typename value_type<Iter2>::type Value2;

      Iter1 ret = l1;
      Iter1 limit = next(f1, 1 + distance(f1, l1) - distance(f2, l2));
      while(f1 != limit) {
        // NOTE: We have to invert the order of comparisons because a) we're
        // choosing to use equal to implement the algorithm and b) we don't
        // require the value types to have a common type. That is eq(a, b) may
        // not be valid for eq(b, a). This could be eliminated if Predicate
        // was actually Relation.
        if(equal(f2, l2, f1, reorder_arguments(eq)))
          ret = f1;
        f1 = next(f1);
      }
      return ret;
    }
  
  // Iter1 and Iter2 are required to be an Forward_iterators whose value types
  // share a Common type and are Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Forward_iterator<Iter1> && 
             Forward_iterator<Iter2> &&
             Equality_comparable<Common_type<Value_type<Iter1>, Value_type<Iter2>>)
      Iter1 find_end(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return find_end(f1, l1, f2, l2, std::equal_to<Value>{});
      }

  // NOTE: This doesn't find a subsequence (like find_end or search), it
  // finds the first position in [f1, l1) that matches any of the elements in 
  // the subrange [f2, l2), which must be multipass out of necessity.
  //
  // Iter1 is required to be an Input_iterator.
  //
  // Iter2 is required to be a Forward_iterator.
  //
  // Eq is required to be binary Predicate taking the value types of Iter1 and 
  // Iter2 as arguments, in that order.
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Intput_iterator<Iter1> && 
             Forward_iterator<Iter2> &&
             Predicate<Eq, Value_type<Iter1>, Value_type<Iter2>>)
      Iter1 find_first_of(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Eq eq)
      {
        using namespace std::placeholders;

        while(f1 != l1) {
          if(any_of(f2, l2, std::bind(eq, get(f1), _1)))
            return f1;
          f1 = next(f1);
        }
        return l1;
      }

  // Iter1 is required to be an Input_iterator.
  //
  // Iter2 is required to be a Forward_iterator.
  //
  // The value types of Iter1 and Iter2 are required to share a common type,
  // and that type must be Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Input_iterator<Iter1> &&
             Forward_iterator<Iter2> &&
             Equality_comparable<Common_type<Value_type<Iter1>, Value_type<Iter2>>)
      Iter1 find_first_of(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return find_first_of(f1, l1, f2, l2, std::equal_to<Value>{});
      }

  // Iter is required to be a Forward_iterator.
  //
  // Eq is required to be a Relation, but not necessarily an equivalence 
  // relation.
  //
  // NOTE: The stronger requirement (Relation vs. Predicate) is simply due to
  // the fact that the argument types will be the same.
  template<typename Iter, typename Eq>
    requires(Forward_iterator<Iter> && Relation<Eq, Value_type<Iter>>)
      Iter adjacent_find(Iter f, Iter l, Eq eq)
      {
        // NOTE: Must have at least one element!
        if(f != l) {
          Iter i = next(f);
          while(i != l) {
            if(eq(get(f), get(i)))
              return f;
            f = next(f);
            i = next(i);
          }
            
        }
        return l;
      }

  // Iter is required to be a Forward_iterator whose value type is
  // Equality_comparable.
  template<typename Iter>
    requires(Forward_iterator<Iter> && Equality_comparable<Value_type<Iter>>)
      Iter adjacent_find(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return adjacent_find(f, l, std::equal_to<Value>{});
      }
    
  // NOTE: Sometimes bind can be less verbose than lambda expressions. Are
  // there any serious performance considerations? Good question. Need to
  // run some performance tests.

  // Iter1 and Iter2 are required to be Forward_iterators whose value types
  // are the same.
  //
  // Eq is a binary Relation taking the shared value type as an argument.
  //
  // NOTE: The value types are required to be the same. I suppose it would not
  // be reasonable to say that ranges of different types are permutations of
  // each other. Could we legitimately weaken this to Common_type, however?
  //
  // Also note that we require a Relation rather than a predicate. This is
  // simply an artifact of the Same_type requirement. That is, writing
  // Same_type and Predicate actually implies Relation.
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Forward_iterator<Iter1> &&
             Forward_iterator<Iter2> &&
             Relation<Eq, Same_type<Value_type<Iter1>, Value_type<Iter2>>>)
      bool is_permutation(Iter1 f1, Iter1 l1, Iter2 f2, Eq eq)
      {
        using namespace std::placeholders;
        typedef typename value_type<Iter1>::type Value;
        typedef typename difference_type<Iter1>::type Distance;

        // Skip past equal parts of the sequences by searching for the first
        // mismatch. If we reach the end of the sequence, then the sequences
        // are equal, and that's still a permutation.
        auto p = mismatch(f1, l1, f2, eq);
        if(p.first == l1)
          return true;
        
        // For each element in [f1, l1), count the number of equal elements
        // in [next(f1], l1). Count the number of equal elements in [f2, l2).
        Iter2 l2 = next(f2, distance(f1, l1));
        for(Iter1 i = f1; i != l1; i = next(i)) {
          // If there are any objects in [f1, i) that are equal to i, then we 
          // can skip the count because we've algety counted them.
          if(find_if(f1, i, std::bind(eq, _1, get(i))) != i)
            continue;
          
          // Count the occurrences of the ith value in the first and second
          // range. If the counts are not equal, return false.
          Distance n1 = count_if(i, l1, std::bind(eq, _1, get(i)));
          Distance n2 = count_if(f2, l2, std::bind(eq, _1, get(i)));
          if(n1 != n2)
            return false;
        }
        return true;
      }
    
  // Iter1 and Iter2 are required to be Forward_iterators whose value types
  // are the same, and whose shared type is Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Forward_iterator<Iter1> &&
             Forward_iterator<Iter2> &&
             Equality_comparable<Same_type<Value_type<Iter1>, Value_type<Iter2>>>)
      inline bool is_permutation(Iter1 f1, Iter1 l1, Iter2 f2)
      {
        typedef typename same_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return is_permutation(f1, l1, f2, std::equal_to<Value>{});
      }
    
  // Iter1 and Iter2 are required to be Forward_iterators.
  //
  // Eq is required to be a Predicate taking the values types of Iter1 and
  // Iter2 as arguments, in that order.
  template<typename Iter1, typename Iter2, typename Eq>
    requires(Forward_iterator<Iter1> && 
             Forward_iterator<Iter2> &&
             Predicate<Eq, Value_type<Iter1>, Value_type<Iter2>>)
      Iter1 search(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Eq eq)
      {
        Iter1 limit = next(f1, 1 + distance(f1, l1) - distance(f2, l2));
        while(f1 != limit) {
          // NOTE: Need to reorder the arguments to preserve the calling
          // order required Predicate.
          if(equal(f2, l2, f1, reorder_arguments(eq)))
            return f1;
          f1 = next(f1);
        }
        return l1;
      }
    
  // Iter1 and Iter2 are required to be Forward_iterators whose value types
  // share a Common type that is Equality_comparable.
  template<typename Iter1, typename Iter2>
    requires(Forward_iterator<Iter1> &&
             Forward_iterator<Iter2> &&
             Equality_comparable<Common_type<Value_type<Iter1>, Value_type<Iter2>>)
      inline Iter1 search(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2)
      {
        typedef typename value_type<Iter1>::type Value;
        return search(f1, l1, f2, l2, std::equal_to<Value>{});
      }

  // NOTE: Here, the _n indicates that the subsequence is implied by the
  // n elements of the value type rather than, "search for the nth" subsequence.
  //
  // Iter is required to be a Forward_iterator.
  //
  // Size is required to be an nonnegative Integer value.
  //
  // Eq is a Predicate that takes an argument of Iter's value type and T,
  // in that order. T is otherwise unconstrained.
  template<typename Iter, typename Size, typename T, typename Eq>
    requires(Forward_iterator<Iter> &&
             Integer<Iter> &&
             Predicate<Eq, Value_type<Iter>, T>)
      Iter search_n(Iter f, Iter l, Size n, T const& x, Eq eq)
      {
        Iter limit = next(f, distance(f, l) - count);
        while(f != limit) {
          // Are there c objects in [i, limit) that are equal to x?
          Iter i = f;
          Size c = n;
          while(eq(get(i), x) && c != Size{0}) {
            --c;
            i = next(i);
          }
          
          // If we got to zero, then we've found a match.
          if(!c)
            return f;

          f = next(f);
        }
      }
    
  // Iter is required to be a Forward_iterator whose valuie type and T share
  // a common type, which is required to be Equality_comparable.
  //
  // Size is required to be an nonnegative Integer value.
  template<typename Iter, typename Size, typename T>
    requires(Forward_iterator<Iter> &&
             Integer<Size> &&
             Equality_comparable<Common_type<Value_type<Iter>, T>>)
      Iter search_n(Iter f, Iter l, Size n, T const& x)
      {
        typedef typename value_type<Iter>::type Value;
        return search_n(f, l, n, x, std::equal_to<Value>{});
      }
    
  // Mutating algorithms.
  
  // Iter is required to be an Input_iterator. The get operation must not be
  // destructive.
  //
  // Out is required to be an Output_iterator, taking Iter's value type as
  // an argument. The put operation must not move Iter's referenced object,
  // but it may move a local copy.
  //
  // NOTE: In the copy algorithms, the input iterator Iter must not be a 
  // strict Move_iterator, and the output iterator must not be a Give_iterator.
  // However, if get() returns a copy, then Out could conceivably be a Give 
  // iterator since the operation would only devalue the local copy. 
  //
  // If we unify the take/get and give/put interfaces, then we can optimize
  // cases where where get() returns a copy, since the output iterator may
  // overload put for both rvalue and lvalue references.
  template<typename Iter, typename Out>
    requires(Input_iterator<Iter> && Output_iterator<Out, Value_type<Iter>>)
      Out copy(Iter f, Iter l, Out r)
      {
        while(f != l) {
          put(r, get(f));
          f = next(f);
          r = next(r);
        }
        return r;
      }

  // Iter is required to be an Input_iterator.
  //
  // Size is required to be a nonnegative Integer.
  //
  // Out is required to be an Output_iterator, taking Iter's value type as
  // an argument.
  //
  // SEE: Notes on copy().
  template<typename Iter, typename Size, typename Out>
    requires(Input_iterator<Iter> && 
             Integer<Size> && 
             Output_iterator<Out, Value_type<Iter>>)
      Out copy_n(Iter f, Size n, Out r)
      {
        while(n != Size{0}) {
          put(r, get(f));
          f = next(f);
          r = next(r);
          --n;
        }
        return r;
      }
  
  // Iter is required to be an Input_iterator.
  //
  // Out is required to be an Output_iterator, taking Iter's value type as
  // an argument.
  //
  // Pred is a unary Predicate taking an argument of Iter's value type.
  //
  // SEE: Notes on copy(). Furthermore, Move_iterators are explicitly forbidden
  // since there are multiple subsequent reads on the same iterator.
  template<typename Iter, typename Out, typename Pred>
    requires(Input_iterator<Iter> && 
             Output_iterator<Out, Value_type<Iter>> &&
             Pred<Pred, Value_type<Iter>>)
      Out copy_if(Iter f, Iter l, Out r, Pred p)
      {
        while(f != l) {
          if(get(f))
            put(r, get(f));
          f = next(f);
          r = next(r);
        }
        return r;
      }
  
  // Iter is required to be a Bidirectional_iterator.
  //
  // Out is requireeed to be a Bidirectional_iterator and an Output_iterator 
  // taking Iter's value type as an argument.
  //
  // SEE: Notes on copy().
  template<typename Iter, typename Out>
    requires(Bidirectional_iterator<Iter> &&
             Bidirectional_iterator<Out> &&
             Output_iterator<Out, Value_type<Iter>>)
    Out copy_backward(Iter f, Iter l, Out r)
    {
      while(f != l) {
        l = prev(l);
        r = prev(r);
        put(r, get(l));
      }
      return r;
    }

  // Iter is required to be a Move_iterator. 
  //
  // Out is required to be a Output_iterator, taking an argument of Iter's
  // value_type.
  //
  // NOTE: Reads may not be destructive, but we must not read twice (because
  // they could be).
  //
  // NOTE: The give iterator requirement is constrained over the result of 
  // take() rather than the value type. In other words, if the output iterator 
  // provides a give() that accepts the result of read(), the algorithm will
  // still be valid (although no actual moving may occur).
  template<typename Iter, typename Out>
    requires(Move_iterator<Iter> && 
             Output_iterator<Out, Reference_type<Iter>>)
      Out move(Iter f, Iter l, Out r)
      {
        while(f != l) {
          put(r, std::move(get(f)));
          f = next(f);
          r = next(r);
        }
        return r;
      }
    
  template<typename Iter, typename Out>
    requires(Bidirectional_iterator<Iter> &&
             Bidirectional_iterator<Out> &&
             Output_iterator<Out, Reference_type<Iter>>)
    Out move_backward(Iter f, Iter l, Out r)
    {
      while(f != l) {
        l = prev(l);
        r = prev(r);
        put(r, std::move(get(l)));
      }
      return r;
    }

  // Swap the values of the given iterators. Here, we require the value types
  // to be the same, not simply Common or mutally move-assignable.
  //
  // Iter1 and Iter2 are required to be Mutable_iterators with the same value
  // type.
  template<typename Iter1, typename Iter2>
    requires(Mutable_iterator<Iter1> && 
             Mutable_iterator<Iter2> && 
             Same<Value_type<Iter1>, Value_type<Iter2>>)
      void iter_swap(Iter1 i, Iter2 j)
      {
        typedef typename value_type<Iter1>::type Value;
        Value x = std::move(get(i));
        put(i, std::move(get(j)));
        put(j, std::move(x));
      }
    
  // Iter1 and Iter2 are required to be Mutable_iterators with the same value
  // type: the same requirements as iter_swap.
  template<typename Iter1, typename Iter2>
    requires(Mutable_iterator<Iter1> &&
             Mutable_iterator<Iter2> && 
             Same<Value_type<Iter1>, Value_type<Iter2>>)
      Iter2 swap_ranges(Iter1 f1, Iter1 l1, Iter2 f2)
      {
        while(f1 != l1) {
          iter_swap(f1, f2);
          f1 = next(f1);
          f2 = next(f2);
        }
        return f2;
      }

  // Iter is required to be an Input_iterator.
  //
  // Out is required to be an Output_iterator taking the result of Op as the
  // output value.
  //
  // Op is required to be a unary Operation taking the value type of Iter as
  // an argument.
  template<typename Iter, typename Out, typename Op>
    requires(Input_iterator<Iter> &&
             Operation<Op, Value_type<Iter>> &&
             Output_iterator<Iter, Result_type<Op, Value_type<Iter>>>)
      Out transform(Iter f, Iter l, Out r, Op op) 
      {
        while(f != l) {
          // NOTE: Iter is an input iterator so move(op(...)) must not modify
          // the original value.
          put(r, std::move(op(get(f))));
          f = next(f);
          r = next(r);
        }
        return r;
      }
    
  // Iter1 and Iter2 are required to be Input_iterators.
  //
  // Out is required to be an Output_iterator taking the result of Op as
  // the output value.
  //
  // Op is required to be a binary Operation taking argumnents of the value
  // types of Iter1 and Iter in that order.
  template<typename Iter1, typename Iter2, typename Out, typename Op>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Operation<Op, Value_type<Iter1>, Value_type<Iter2>> &&
             Output_iterator<Out, Result_type<Op, Value_type<Iter1>, Value_type<Iter2>>>)
      Out transform(Iter1 f1, Iter1 l1, Iter2 f2, Out r, Op op)
      {
        while(f1 != l1) {
          put(r, op(get(f1), get(f2)));
          f1 = next(f1);
          f2 = next(f2);
          r = next(r);
        }
        return r;
      }

  template<typename Iter, typename T>
    requires(Mutable_iterator<Iter> && 
             Same<Value_type<Iter>, T>)
      void replace(Iter f, Iter l, T const& old, T const& x)
      {
        while(f != l) {
          if(get(f) == old)
            put(f, x);
          f = next(f);
        }
      }

  template<typename Iter, typename Pred, typename T>
    requires(Mutable_iterator<Iter> && 
             Predicate<Pred, Value_type<Iter>> &&
             Same<Value_type<Iter>, T>)
      void replace_if(Iter f, Iter l, Pred p, T const& x) 
      {
        while(f != l) {
          if(p(get(f)))
            put(f, x);
          f = next(f);
        }
      }

  template<typename Iter, typename Out, typename T>
    requires(Input_iterator<Iter> &&
             Output_iterator<Out, T> &&
             Same<Value_type<Iter1, T>)
      Out replace_copy(Iter f, Iter l, Out r, T const& old, T const& x)
      {
        while(f != l) {
          if(get(f) == old)
            put(r, x);
          f = next(f);
          r = next(r);
        }
        return r;
      }

  template<typename Iter, typename Out, typename Pred, typename T>
    requires(Mutable_iterator<Iter> &&
             Predicate<Pred, Value_type<Iter>> &&
             Same<Value_type<Iter>, T>)
      void replace_copy_if(Iter f, Iter l, Out r, Pred p, T const& x)
      {
        while(f != l) {
          if(p(get(f)))
            put(r, x);
          f = next(f);
          r = next(r);
        }
        return r;
      }
    
  // Iter is required to be a Forward_iterator and Mutable_iterator whose
  // value type is the same as T.
  template<typename Iter, typename T>
    requires(Output_iterator<Iter, T>)
      void fill(Iter f, Iter l, T const& x)
      {
        while(f != l) {
          put(f, x);
          f = next(f);
        }
      }
    
  template<typename Out, typename Size, typename T>
    requires(Output_iterator<Iter, T> && Integer<Size>)
      Out fill_n(Out f, Size n, T const& x) 
      {
        while(n != Size{0}) {
          put(f, x);
          f = next(f);
          --n;
        }
        return f;
      }
  
  template<typename Iter, typename Gen>
    requires(Generator<Gen> && Output_iterator<Out, Result_type<Gen>>)
      void generate(Iter f, Iter l, Gen gen)
      {
        while(f != l) {
          put(f, gen());
          f = next(f);
        }
      }
    
  template<typename Out, typename Size, typename Gen>
    requires(Generator<Gen> && 
             Output_iterator<Out, Result_type<Gen>> &&
             Integer<Size)
      void generate(Out f, Size n, Gen gen)
      {
        while(n != Size{0}) {
          put(f, gen());
          f = next(f);
          --n;
        }
        return f;
      }
    
  // Removal of elements is achieved by "condensing" the values in the range.
  // This is done by moving objects in the range so that [f, r) will consist
  // of objects not equal to x. Here, r is past the last element that is equal
  // to x. The range [r, l) consists of objects in a valid but unspecified
  // state (even though some objects may actually still be valid).
  //
  // Iter is required to be a Mutable_iterator whose value type shares a common
  // type with T and is Equality_comparable.
  //
  // NOTE: The Forward_iterator requirement is implied because of the 
  // persistence of the original range (just not its values).
  template<typename Iter, typename T>
    requires(Mutable_iterator<Iter> &&
             Equality_comparable<Common_type<Value_type<Iter>, T>>)
      Iter remove(Iter f, Iter l, T const& x)
      {
        Iter r = f;
        while(f != l) {
          if(get(f) != x) {
            put(r, std::move(get(f)));
            r = next(r);
          }
          f = next(f);
        }
        return r;
      }
  
  // Iter is requried to be a Mutable_iterator.
  //
  // Pred is required to be a unary Predicate taking a single argument of 
  // Iter's value_type.
  template<typename Iter, typename Pred>
    requires(Mutable_iterator<Iter> && Predicate<Pred, Value_type<Iter>>)
      Iter remove_if(Iter f, Iter l, Pred p)
      {
        Iter r = f;
        while(f != l) {
          if(!p(get(f))) {
            put(r, std::move(get(f)));
            r = next(r);
          }
          f = next(f);
        }
        return r;
      }
  
  // NOTE: On the surface, it looks like remove could be implemented in terms
  // of remove_copy. However, remove actually moves the non-removed values,
  // while copies them.
  template<typename Iter, typename Out, typename T>
    requires(Input_iterator<Iter> && 
             Output_iterator<Out, Value_type<Iter>> &&
             Equality_comparable<Common_type<Value_type<Iter>, T>>)
      Out remove_copy(Iter f, Iter l, Out r, T const& x)
      {
        while(f != l) {
          if(get(f) != x) {
            put(r, get(f));
            r = next(r);
          }
          f = next(f);
        }
        return r;
      }
  
  template<typename Iter, typename Out, typename Pred>
    requires(Input_iterator<Iter> &&
             Output_iterator<Out, Value_type<Iter>> &&
             Predicate<Pred, Value_type<Iter>>)
      Out remove_copy_if(Iter f, Iter l, Out r, Pred p)
      {
        while(f != l) {
          if(!p(get(f))) {
            put(r, get(f));
            r = next(r);
          }
          f = next(f);
        }
        return r;
      }
    
  // NOTE: As with removal, the elements in the range [r, l) are in valid
  // but unspecified state after execution.
  //
  // NOTE: Pred is not required to be anything stronger than a binary predicate
  // since the added generality does not affect the result of the computation.
  template<typename Iter, typename Pred>
    requires(Mutable_iterator<Iter> && Predicate<Pred, Value_type<Iter>>)
      Iter unique(Iter f, Iter l, Pred p)
      {
        f = adjacent_find(f, l, p);
        if(f != l) {
          // We know that f == next(f) because of adjacent find. Start past
          // next(f) so we don't compare redundantly.
          Iter i = next(f, 2);
          while(i != l) {
            // If the ith element is not the same as the fth element, move it
            // into the next element past f.
            if(!p(get(f), get(i))) {
              f = next(f);
              put(f, std::move(get(i)));
            }
            i = next(i);
          }
          
          // Advance f so that it's past the last unique element.
          f = next(f);
        }
        return f;
      }
    
  template<typename Iter>
    requires(Mutable_iterator<Iter> && Equality_comparable<Value_type<Iter>>)
      Iter unique(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return unique(f, l, std::equal_to<Value>{});
      }

  template<typename Iter, typename Out, typename Pred>
    requires(Input_iterator<Iter> && 
             Output_iterator<Out, Value_type<Iter>> &&
             Predicate<Pred, Value_type<Iter>, Value_type<Iter>>)
      Out unique_copy(Iter f, Iter l, Out r, Pred p)
      {
        // FIXME: Could refactor the put and increment steps. It's probably
        // a pretty common operation.
        typedef typename value_type<Iter>::type Value;
        if(f != l) {
          Value x = get(f);
          put(r, x);
          r = next(r);
          f = next(f);

          while(f != l) {
            if(!p(get(f), x)) {
              x = get(f);
              put(r, x);
              r = next(r);
              f = next(f);
            }
          }
        }
        return r;
      }
    
  template<typename Iter, typename Out>
    requires(Input_iterator<Iter> &&
             Output_iterator<Iter, Value_type<Iter>> &&
             Equality_comparable<Value_type<Iter>>)
      Out unique_copy(Iter f, Iter l, Out r)
      {
        typedef typename value_type<Iter>::type Value;
        return unique_coy(f, l, r, std::equal_to<Value>{});
      }
    
  // FIXME: This can be specialized for random access iterators by using
  // f < l rather than two comparisons.
  template<typename Iter>
    requires(Mutable_iterator<Iter>)
      void reverse(Iter f, Iter l)
      {
        while(f != l) {
          l = prev(l);
          if(f == l)
            break;
          iter_swap(f, l);
          f = next(f);
        }
      }
    
  template<typename Iter, typename Out>
    requires(Input_iterator<Iter> && Output_iterator<Out, Value_type<Iter>>)
      void reverse_copy(Iter f, Iter l, Out r)
      {
        while(f != l) {
          l = prev(l);
          put(r, get(l));
          r = next(r);
        }
      }
    
  // FIXME: Not sure that this implementation is correct. There's an entire
  // section about this algorithm in EoP that explores different implementation
  // options, and some seem fairly interesting.
  template<typename Iter>
    requires(Mutable_iterator<Iter>)
      Iter rotate(Iter f, Iter m, Iter l)
      {
        Iter n = m;
        while(f != n) {
          iter_swap(f, n);
          f = next(f);
          n = next(n);
          if(n == l) 
            n = m;
          else if(f == m)
            m = n;
        }
      }
    
  template<typename Iter, typename Out>
    requires(Input_iterator<Iter> && Output_iterator<Out, Value_type<Iter>>)
      Out rotate_copy(Iter f, Iter m, Iter l, Out r)
      {
        return copy(f, m, copy(m, l, r));
      }
    
  // NOTE: This version of the algorithm is inconsistent with newer random
  // number generating requirements. Here, Gen is a uniform ranodom integer 
  // distribution from [0, n) where n is the argument passed to rand. Note that 
  // we could conceivably wrap the std uniform_int_distribution to satisfy
  // these requirements.

  // The algorithm proceeds by iterating over each element and swapping with
  // a randomly selected element from the remainder of the elements in the
  // range. The algorithm requires random access iterators.
  //
  // NOTE: The Random_modular_integer_generator is just a guess at a concept 
  // name. Here, we're describing a unary Procedure on a single integer 
  // argument n that returns integer values in the range [0, n): integers mod n.
  template<typename Iter, typename Gen>
    requires(Mutable_iterator<Iter> && Uniform_modular_integer_generator<Gen>)
      void random_shuffle(Iter f, Iter l, Gen&& rand)
      {
      typedef typename difference_type<Iter>::type Distance;
        Distance d = distance(f, l);
        if(d > 1) {
          l = prev(l);
          while(f < l) {
            Distance n = rand(d);
            iter_swap(f, next(f, n));
            f = next(f);
            --d;
          }
        }
      }
    
  template<typename Iter>
    requires(Mutable_iterator<Iter>)
      void random_shuffle(Iter f, Iter l)
      {
        std::default_random_engine g;
        shuffle(f, l, g);
      }
    
  // This variant of the algorithm takes a uniform random number generator
  // rather than a distribution. The algorithm internally "shapes" the
  // generated numbers with its own distribution.
  template<typename Iter, typename Gen>
    requires(Mutable_iterator<Iter> && Uniform_random_number_generator<Gen>)
      void shuffle(Iter f, Iter l, Gen&& gen)
      {
        typedef typename difference_type<Iter>::type Distance;
        typedef std::uniform_int_distribution<Distance> Dist;
        typedef typename Dist::param_type Param;
        Distance d = distance(f, l);
        if(d > 1) {
          Dist rand;
          l = prev(l);
          while(f < l) {
            Distance n = rand(gen, Param{0, d}); 
            iter_swap(f, next(f, n));
            f = next(f);
            --d;
          }
        }
      }
    

  // FIXME: This should be included with the predicate functions.
  // NOTE: The version required by the standard disagrees with the version
  // presented in EoP, but only in respect to the order of the two equivalence
  // classes.
  template<typename Iter, typename Pred>
    requires(Input_iterator<Iter> && Predicate<Pred, Value_type<Input_iterator>>)
      void is_partitioned(Iter f, Iter l, Pred p)
      {
        return find_if(find_if_not(f, l, p), l, p) == l;
      }
    
  // FIXME: This can be specialized for bidirectional iterators.
  template<typename Iter, typename Pred>
    requires(Mutable_iterator<Iter> && Predicate<Pred, Value_type<Iter>>)
      Iter partition(Iter f, Iter l, Pred p)
      {
        f = find_if_not(f, l, p);
        if(f != l) {
          for(Iter i = next(f); i != l; i = next(i)) {
            if(p(get(i))) {
              iter_swap(f, i);
              f = next(f);
            }
          }
        }
        return f;
      }
    
  // FIXME: This is a non-trivial algorithm with a number of specializations.
  // Implement it.
  template<typename Iter, typename Pred>
    requires(Mutable_iterator<Iter> && Predicate<Pred, Value_type<Iter>>)
      void stable_partition(Iter f, Iter l, Pred p)
      {
        assert(( false ));
      }

  template<typename Iter, typename Out1, typename Out2, typename Pred>
    requires(Input_iterator<Iter> &&
             Output_iterator<Out1, Value_type<Iter1> &&
             Output_iterator<Out2, Value_type<Iter2> &&
             Predicate<Pred, Value_type<Iter>>)
      std::pair<Out1, Out2> partition_copy(Iter f, Iter l, Out1 rt, Out2 rf, Pred p)
      {
        while(f != l) {
          if(p(get(f))) {
            put(rt, get(f));
            rt = next(rt);
          } else {
            put(rf, get(f));
            rf = next(rf);
          }
          f = next(f);
        }
      }
  
  template<typename Iter, typename Pred>
    requires(Forward_iterator<Iter> && Predicate<Pred, Value_type<Iter>>)
      Iter partition_point(Iter f, Iter l, Pred p)
      {
        typedef typename difference_type<Iter>::type Distance;
        Distance n = distance(f, l);
        while(n != Distance{0}) {
          Distance h = n / 2;
          Iter m = next(f, h);
          if(p(get(m))) {
            m = next(m);
            f = m;
            n -= h + Distance{1};
          } else {
            n = h;
          }
        }
        return f;
      }

  // Min/Max algorithms
  
  template<typename T, typename Comp>
    requires(Strict_weak_order<Comp, T>)
      inline T const& min(T const& x, T const& y, Comp comp)
      {
        if(comp(y, x))
          return y;
        else
          return x;
      }

  template<typename T>
    requires(Ordered<T>)
      inline T const& min(T const& x, T const& y)
      {
        return min(x, y, std::less<T>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Forward_iterator<Iter> && Strict_weak_order<Comp, Value_type<Iter>>)
      Iter min_element(Iter f, Iter l, Comp comp)
      {
        if(f == l)
          return f;
        
        Iter min = f;
        f = next(f);
        while(f != l) {
          if(comp(get(f), get(min)))
            min = f;
          f = next(f);
        }
        return min;
      }
    
  template<typename Iter>
    requires(Forward_iterator<Iter> && Ordered<Value_type<Iter>>)
      inline Iter min_element(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return min_element(f, l, std::less<Value>{});
      }
  
  // FIXME: We could actually make this return a reference by storing the
  // minimum element indirectly (i.e., via an iterator). This may also apply to 
  // the min_element algorithm as well.
  template<typename T, typename Comp>
    requires(Strict_weak_order<Comp, T>)
      inline T min(std::initializer_list<T> list, Comp comp)
      {
        return get(min_element(list.begin(), list.end(), comp));
      }
    
  template<typename T>
    requires(Ordered<T>)
      inline T min(std::initializer_list<T> list)
      {
        return min(list, std::less<T>{});
      }

  template<typename T, typename Comp>
    requires(Strict_weak_order<Comp, T>)
      inline T const& max(T const& x, T const& y, Comp comp)
      {
        if(comp(x, y))
          return y;
        else
          return x;
      }
    
  template<typename T>
    requires(Ordered<T>)
      inline T const& max(T const& x, T const& y)
      {
        return max(x, y, std::less<T>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Forward_iterator<Iter> && Strict_weak_order<Value_type<Iter>>)
      Iter max_element(Iter f, Iter l, Comp comp)
      {
        if(f == l)
          return f;

        Iter max = f;
        f = next(f);
        while(f != l) {
          if(comp(get(max), get(f)))
            max = f;
          f = next(f);
        }
        return max;
      }
    
  template<typename Iter>
    requires(Forward_iterator<Iter> && Ordered<Value_type<Iter>>)
      inline Iter max_element(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return max_element(f, l, std::less<Value>{});
      }
  
  template<typename T, typename Comp>
    requires(Strict_weak_order<Comp, T>)
      inline T max(std::initializer_list<T> list, Comp comp)
      {
        return get(max_element(list.begin(), list.end(), comp));
      }
    
  template<typename T>
    requires(Ordered<T>)
      inline T max(std::initializer_list<T> list)
      {
        return max(list, std::less<T>{});
      }
    
  template<typename T, typename Comp>
    requires(Strict_weak_order<Comp, T>)
      inline std::pair<T const&, T const&> minmax(T const& x, T const& y)
      {
        if(comp(y, x))
          return {y, x};
        else
          return {x, y};
      }
    
  template<typename T>
    requires(Ordered<T>)
      inline std::pair<T const&, T const&> minmax(T const& x, T const& y)
      {
        return minmax(x, y, std::less<T>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Forward_iterator<Iter> && Strict_weak_order<Comp, Value_type<Comp>>)
      std::pair<Iter, Iter> minmax_element(Iter f, Iter l, Comp comp)
      {
        if(f == l)
          return {f, f};
        
        // FIXME: The libc++ and libstdc++ implementations are much more
        // complicated. I don't really understand why.

        Iter min = f;
        Iter max = f;
        f = next(f);
        while(f != l) {
          if(comp(get(f), get(min)))
            min = f;
          if(!comp(get(f), get(max)))
            max = f;
          f = next(f);
        }
        return {min, max};
      }
      
  template<typename Iter>
    requires(Forward_iterator<Iter> && Ordered<Value_type<Iter>>)
      inline std::pair<Iter, Iter> minmax_element(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return minmax(f, l, std::less<Value>{});
      }
    
  // Lexicographical comparison
  template<typename Iter1, typename Iter2, typename Comp>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      bool lexicographical_compare(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Comp comp)
      {
        while(f1 != l1 && f2 != l2) {
          if(comp(get(f1), get(f2)))
            return true;
          if(comp(get(f2), get(f1)))
            return false;
          f1 = next(f1);
          f2 = next(f2);
        }
        return f1 == l1 && f2 != l2;
      }

  template<typename Iter1, typename Iter2>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Ordered<Common_type<Value_type<Iter1> && Value_type<Iter2>>>)
      bool lexicographical_compare(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2)
      {
        typedef typename value_type<Iter1>::type Value;
        return lexicographical_compare(f1, l1, f2, l2, std::less<Value>{});
      }

  // Search algorithms
  template<typename Iter, typename T, typename Comp>
    requires(Forward_iterator<Iter> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter>, T>>)
      void lower_bound(Iter f, Iter l, T const& x, Comp comp)
      {
        typedef typename difference_type<Iter>::type Distance;
        Distance n = distance(f, l);
        while(n > Distance{0}) {
          Distance h = n / Distance{2};
          Iter m = next(f, n);
          if(comp(get(m), x)) {
            f = next(m);
            n -= h - Distance{1};
          } else {
            n = h;
          }
        }
        return f;
      }
    
  template<typename Iter, typename T>
    requires(Forward_iterator<Iter> && Ordered<Common_type<Value_type<Iter>, T>>)
      inline void lower_bound(Iter f, Iter l, T const& x)
      {
        typedef typename value_type<Iter>::type Value;
        return lower_bound(f, l, x, std::less<Value>{});
      }
    
  template<typename Iter, typename T, typename Comp>
    requires(Forward_iterator<Iter> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter>, T>>)
      void upper_bound(Iter f, Iter l, T const& x)
      {
        typedef typename difference_type<Iter>::type Distance;
        Distance n = distance(f, l);
        while(n > Distance{0}) {
          Distance h = n / Distance{2};
          Iter m = next(f, n);
          if(comp(x, get(m))) {
            n = h;
          } else {
            f = next(m);
            n -= h - Distance{1};
          }
        }
        return f;
      }
  
  template<typename Iter, typename T>
    requires(Forward_iterator<Iter> && Ordered<Common_type<Value_type<Iter>, T>>)
      inline void upper_bound(Iter f, Iter l, T const& x)
      {
        typedef typename value_type<Iter>::type Value;
        return upper_bound(f, l, x, std::less<Value>{});
      }
    
  template<typename Iter, typename T, typename Comp>
    requires(Forward_iterator<Iter> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter>, T>>)
      inline std::pair<Iter, Iter> equal_range(Iter f, Iter l, T const& x, Comp comp)
      {
        // FIXME: This could be made much more efficient.
        return {lower_bound(f, l, x, comp), upper_bound(f, l, x, comp)};
      }
    
  template<typename Iter, typename T>
    requires(Forward_iterator<Iter> && Ordered<Common_type<Value_type<Iter>, T>>)
      inline std::pair<Iter, Iter> equal_range(Iter f, Iter l, T const& x)
      {
        typedef typename value_type<Iter>::type Value;
        return equal_range(f, l, x, std::less<Value>{});
      }
    
  template<typename Iter, typename T, typename Comp>
    requires(Forward_iterator<Iter> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter>, T>>)
      inline void binary_search(Iter f, Iter l, T const& x, Comp comp)
      {
        Iter i = lower_bound(f, l, x, comp);
        return i != l && !(comp(x, get(i)));
      }

  template<typename Iter, typename T>
    requires(Forward_iterator<Iter> && Ordered<Common_type<Value_type<Iter>, T>>)
      inline void binary_search(Iter f, Iter l, T const& x)
      {
        typedef typename value_type<Iter>::type Value;
        return binary_search(f, l, x, std::less<Value>{});
    }

  // Merge operations

  // NOTE: This can be rewritten to support Move_iterators such that the
  // values in the input ranges are moved into the output range. It may be a
  // better choice for that to be a different algorithm since using the same
  // name would create an abstraction in which the caller may not know whether
  // or not their original sequences are preserved.
  template<typename Iter1, typename Iter2, typename Out, typename Comp>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
    Out merge(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r, Comp comp)
    {
      while(f1 != l1 && f2 != l2) {
        if(comp(get(f1), get(f2))) {
          put(r, get(f1));
          f1 = next(f1);
        } else {
          put(r, get(f2));
          f2 = next(f2);
        }
        r = next(r);
      }
      
      // If there are any leftovers, copy then into the output range.
      if(f1 == l1)
        copy(f2, l2, r);
      else if(f2 == l2)
        copy(f1, l1, r);
      
      return r;
    }
    
  template<typename Iter1, typename Iter2, typename Out>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Ordered<Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out merge(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r)
      {
        typedef typename value_type<Iter1>::type Value;
        return merge(f1, l1, f2, l2, r, std::less<Value>{});
      }
    
  // NOTE: The value type is not required to be regular despite the fact that
  // I am allocating a temporary buffer. I can dynamically allocate Resources
  // just as easily as Regular value types.
  //
  // SEE: The notes on merge() about movability.
  template<typename Iter, typename Comp>
    requires(Mutable_iterator<Iter> && 
             Strict_weak_order<Comp, Value_type<Iter>>)
      void inplace_merge(Iter f, Iter m, Iter l, Comp comp)
      {
        typedef typename value_type<Iter>::type Value;
        typedef typename difference_type<Iter>::type Distance;

        // FIXME: This clearly isn't the best implementation.
        // Create a temporary buffer.
        Distance n = distance(f, l);
        std::unique_ptr<Value[]> buf(new Value[n]);
        Value* p = buf.get();

        // Merge into the temporary buffer and then move the results back onto
        // the original range [f, l).
        // FIXME: Use move_merge?
        merge(f, m, m, l, p, comp);
        move(p, p + n, f);
      }
    
  template<typename Iter>
    requires(Mutable_iterator<Iter> && Ordered<Value_type<Iter>>)
      void inplace_merge(Iter f, Iter m, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return inplace_merge(f, m, l, std::less<Value>{});
      }
    
  // Set operations
  // NOTE: These algorithms help adapt sorted sequences to Set concept.
  
  template<typename Iter1, typename Iter2, typename Comp>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      bool includes(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Comp comp)
      {
        while(f1 != l1) {
          if(comp(get(f2), get(f1)))
            return false;
          else if(comp(get(f1), get(f2)))
            f1 = next(f1);
          else {
            f1 = next(f1);
            f2 = next(f2);
          }
        }
        return f2 == l2;
      }
    
  template<typename Iter1, typename Iter2>
    requires(Input_iterator<Iter1> && 
             Input_iterator<Iter2> &&
             Ordered<Comp, Common_type<Iter1, Iter2>>)
      bool includes(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2)
      {
        typedef typename value_type<Iter1>::type Value;
        return includes(f1, l1, f2, l2, std::less<Value>{});
      }
    
  template<typename Iter1, typename Iter2, typename Out, typename Comp>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_union(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r, Comp comp)
      {
        while(f1 != l1 && f2 != l2) {
          if(comp(get(f1), get(f2))) {
            put(r, get(f1));
            f1 = next(f1);
          } else if(comp(get(f2), get(f1))) {
            put(r, get(f2));
            f2 = next(f2);
          } else {
            // FIXME: If incomparability under comp defines an equivalence 
            // relation, but not equality, does this algorithm generate an
            // incomplete union? Sort of. Maybe.
            put(r, get(f1));
            f1 = next(f1);
            f2 = next(f2);
          }
          r = next(r);
        }
        return r;
      }

  template<typename Iter1, typename Iter2, typename Out>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Ordered<Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_union(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return set_union(f1, l1, f2, l2, r, std::less<Value>{});
      }

  template<typename Iter1, typename Iter2, typename Out, typename Comp>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_intersection(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r, Comp comp)
      {
        while(f1 != l1 && f2 != l2) {
          if(comp(get(f1), get(f2)))
            f1 = next(f1);
          else if(comp(get(f2), get(f1)))
            f2 = next(f2);
          else {
            put(r, get(f1));
            f1 = next(f1);
            f2 = next(f2);
            r = next(r);
          }
        }
        return r;
      }

  template<typename Iter1, typename Iter2, typename Out>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Ordered<Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_intersection(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return set_intersection(f1, l1, f2, l2, r, std::less<Value>{});
      }

  template<typename Iter1, typename Iter2, typename Out, typename Comp>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_difference(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r, Comp comp)
      {
        while(f1 != l1 && f2 != l2) {
          if(comp(get(f1), get(f2))) {
            put(r, get(f1));
            f1 = next(f1);
            r = next(r);
          }
          else if(comp(get(f2), get(f1)))
            f2 = next(f2);
          else {
            f1 = next(f1);
            f2 = next(f2);
          }
        }
        return r;
      }
    
  template<typename Iter1, typename Iter2, typename Out>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Ordered<Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_difference(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return set_difference(f1, l1, f2, l2, r, std::less<Value>{});
      }

  template<typename Iter1, typename Iter2, typename Out, typename Comp>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Strict_weak_order<Comp, Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_symmetridifference(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r, Comp comp)
      {
        while(f1 != l1 && f2 != l2) {
          if(comp(get(f1), get(f2))) {
            put(r, get(f1));
            f1 = next(f1);
            r = next(r);
          }
          else if(comp(get(f2), get(f1))) {
            put(r, get(f2));
            f2 = next(f2);
            r = next(r);
          }
          else {
            f1 = next(f1);
            f2 = next(f2);
          }
        }
        return r;
      }
  
  template<typename Iter1, typename Iter2, typename Out>
    requires(Input_iterator<Iter1> &&
             Input_iterator<Iter2> &&
             Output_iterator<Out, Common_type<Value_type<Iter1>, Value_type<Iter2>>> &&
             Ordered<Common_type<Value_type<Iter1>, Value_type<Iter2>>>)
      Out set_symmetridifference(Iter1 f1, Iter1 l1, Iter2 f2, Iter2 l2, Out r)
      {
        typedef typename std::common_type<
          typename value_type<Iter1>::type, typename value_type<Iter2>::type
        >::type Value;
        return set_symmetridifference(f1, l1, f2, l2, r, std::less<Value>{});
      }

  // Heap operations
  // FIXME: These need to be rewritten a little more carefully. There actually
  // a suite of helper functions involved in the heap operations.

  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Strict_weak_order<Value_type<Iter>>)
      void push_heap(Iter f, Iter l, Comp comp)
      {
        typedef typename value_type<Iter>::type Value;
        typedef typename difference_type<Iter>::type Distance;
        
        // Take the last element (the most recently pushed), and bubble it
        // upwards, swapping with parents if they violate the heap order.
        l = prev(l);
        Value v = std::move(get(l));
        Distance h = distance(f, l);  // The hole index (starts at the back)
        Distance p = (h - 1) / 2;     // The parent index
        while(h > Distance{0} && comp(get(f, p), v)) {
          put(f, h, std::move((f, p)));
          h = p;
          p = (h - 1) / 2;
        }
        put(f, h, std::move((v)));
      }
    
  template<typename Iter>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Ordered<Value_type<Iter>>)
      void push_heap(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return push_heap(f, l, std::less<Value>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Strict_weak_order<Value_type<Iter>>)
      void pop_heap(Iter f, Iter l, Comp comp)
      {
        typedef typename value_type<Iter>::type Value;
        typedef typename difference_type<Iter>::type Distance;

        // Exchange the last element with the first and then bubble it downward.
        l = prev(l);
        Value v = std::move(get(l));
        put(l, std::move(get(f)));

        Distance n = distance(f, l);
        Distance h = 0;
        Distance c = h;

        // Bubble the value down
        while(c < (n - 1) / 2) {
          c = 2 * (c + 1); 
          if(get(f, c) < get(f, c - 1))
            --c;
          give(f, h, take(f, c));
          h = c;
        }
        
        // FIXME: I don't know what this does. It seems to me like the algorithm
        // should terminate after the loop above. Obviously, fixe this.
        if((n & 1) == 0 && c == (n - 2) / 2) {
          c = 2 * (c + 1); 
          give(f, h, take(f, c - 1));
          h = c - 1;
        }
        // FIXME: What do I have to re-push the value?
        // push_heap(f, h, Distance{0}, value);
      }
      
  template<typename Iter>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Ordered<Value_type<Iter>>)
      void pop_heap(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return pop_heap(f, l, std::less<Value>{});
      }
    
  // FIXME: Implement me.
  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Strict_weak_order<Value_type<Iter>>)
      void make_heap(Iter f, Iter l, Comp comp)
      {
        assert(( false ));
      }
    
  template<typename Iter>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Ordered<Value_type<Iter>>)
      void make_heap(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        make_heap(f, l, std::less<Value>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Strict_weak_order<Value_type<Iter>>)
      void sort_heap(Iter f, Iter l, Comp comp)
      {
        l = prev(l);
        while(f != l) {
          // FIXME: Need to use an internal pop function to support this
          // algorithm.
          // pop_heap(f, l, next(l));
          l = prev(l);
        }
      }
    
  template<typename Iter>
    requires(Random_access_iterator<Iter> && 
             Mutable_iterator<Iter> &&
             Ordered<Value_type<Iter>>)
      inline void sort_heap(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return sort_heap(f, l, std::less<Value>{});
      }
    
  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> && Strict_weak_order<Value_type<Iter>>)
      void is_heap_until(Iter f, Iter l, Comp comp)
      {
        assert(( false ));
      }
    
  template<typename Iter>
    requires(Random_access_iterator<Iter> && Ordered<Value_type<Iter>>)
      void is_heap_until(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return is_heap_until(f, l, std::less<Value>{});
      }

  template<typename Iter, typename Comp>
    requires(Random_access_iterator<Iter> &&  Strict_weak_order<Value_type<Iter>>)
      bool is_heap(Iter f, Iter l, Comp comp)
      {
        return is_heap_until(f, l, comp) == l;
      }
    
  template<typename Iter>
    requires(Random_access_iterator<Iter> && Ordered<Value_type<Iter>>)
      bool is_heap(Iter f, Iter l)
      {
        typedef typename value_type<Iter>::type Value;
        return is_heap(f, l, std::less<Value>{});
      }
    
  // Sort operations
  // FIXME: I could write a bunch of other well-known sort algorithms here.
  
  // FIXME: This is an insertion sort, but there's probably a more efficient
  // implementation out there somewhere.
  template<typename Iter, typename Comp>
    void stable_sort(Iter f, Iter l, Comp comp)
    {
      typedef typename value_type<Iter>::type Value;
      typedef typename difference_type<Iter>::type Distance;

      Distance n = distance(f, l);
      for(Distance j = 1; j < n; j++) {
        Value tmp = get(f, j);
        Distance i = j - Distance{1};
        while(i >= Distance{0} && !comp(get(f, i), tmp)) {
          put(f, i + Distance{1}, get(f, i));
          --i;
        }
        put(f, i + Distance{1}, tmp);
      }
    }
    
  template<typename Iter>
  void stable_sort(Iter f, Iter l)
  {
    typedef typename value_type<Iter>::type Value;
    return stable_sort(f, l, std::less<Value>{});
  }
  
  // FIXME: Implement a real sort algorithm.
  template<typename Iter, typename Comp>
    void sort(Iter f, Iter l, Comp comp)
    {
      stable_sort(f, l, comp);
    }

  template<typename Iter>
    void sort(Iter f, Iter l)
    {
      typedef typename value_type<Iter>::type Value;
      return sort(f, l, std::less<Value>{});
    }
    
  template<typename Iter, typename Comp>
    void partial_sort(Iter f, Iter m, Iter l, Comp comp)
    {
      // FIXME: Apparently, this is done using heap sort.
      assert(( false ));
    }

  template<typename Iter>
    void partial_sort(Iter f, Iter m, Iter l)
    {
      typedef typename value_type<Iter>::type Value;
      return partial_sort(f, m, l, std::less<Value>{});
    }
    
  template<typename Iter, typename Comp>
    void nth_element(Iter f, Iter nth, Iter l, Comp comp)
    {
      assert(( false ));
    }
  
  template<typename Iter>
    inline void nth_element(Iter f, Iter nth, Iter l)
    {
      typedef typename value_type<Iter>::type Value;
      nth_element(f, nth, l, std::less<Value>{});
    }
    
  // Permutation algorithms.
  template<typename Iter, typename Comp>
    bool next_permutation(Iter f, Iter l, Comp comp)
    {
      // If there are 0 or 1 elements, then no permutations are possible.
      if(f == l)
        return false;
      if(next(f) == l)
        return false;
      
      // Permute the sequence.
      Iter i = prev(l);
      while(true) {
        Iter j = i;
        i = prev(i);
        if(comp(get(i), get(j))) {
          Iter k = prev(l);
          while(!comp(get(i), get(k)))
            k = prev(k);
          iter_swap(i, k);
          reverse(j, l);
          return true;
        }
        
        if(i == f) {
          reverse(f, l);
          return false;
        }
      }
    }
    
  template<typename Iter>
    bool next_permutation(Iter f, Iter l)
    {
      typedef typename value_type<Iter>::type Value;
      return next_permutation(f, l, std::less<Value>{});
    }

  template<typename Iter, typename Comp>
    bool prev_permutation(Iter f, Iter l, Comp comp)
    {
      // If there are 0 or 1 elements, then no permutations are possible.
      if(f == l)
        return false;
      if(next(f) == l)
        return false;
      
      // Permute the sequence.
      Iter i = prev(l);
      while(true) {
        Iter j = i;
        i = prev(i);
        if(comp(get(j), get(i))) {
          Iter k = prev(l);
          while(!comp(get(k), get(i)))
            k = prev(k);
          iter_swap(i, k);
          reverse(j, l);
          return true;
        }
        
        if(i == f) {
          reverse(f, l);
          return false;
        }
      }
    }

  template<typename Iter>
    bool prev_permutation(Iter f, Iter l)
    {
      typedef typename value_type<Iter>::type Value;
      return prev_permutation(f, l, std::less<Value>{});
    }
}

#endif
