// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_ITERATOR_ITERATOR_HPP
#define ORIGIN_ITERATOR_ITERATOR_HPP

#include <cstddef>
#include <origin/utility/meta.hpp>

#include <origin/iterator/concepts.hpp>

// This file contains utilities supporting the iterator abstraction and its
// associated operations.

namespace origin
{
  // Iterator categories. There are two components to the hierarchy. The
  // traversal component includes the forward, bidirectional, random access 
  // tags. The access category contains the input, output, and mutable
  // iterator tags.
  struct input_iterator_tag { };
  struct output_iterator_tag { };
  struct mutable_iterator_tag { };
  struct forward_iterator_tag : input_iterator_tag { };
  struct bidirectional_iterator_tag : forward_iterator_tag { };
  struct random_access_iterator_tag : bidirectional_iterator_tag { };
  
  // Some traits...
  template<typename Iter> struct reference;
  template<typename Iter> struct value_type;
  template<typename Iter> struct difference_type;

  // Iterator concepts. Note that unlike the conventional Origin concept
  // checking classes, these are not meant to be instantiated. They are purely
  // documentary (for now).

  template<typename Iter> 
    struct c_Iterator : c_Regular<Iter> 
    {
      typedef auto_type difference_type;
      typedef auto_type reference;
      
      requires(Integer<difference_type>);
      requires(Movable<reference>);

      // Increment the iterator.
      Iter next(Iter i);

      // Increment the iterator n steps. This operation introduces the
      // difference type.
      void advance(Iter& i, difference_type n);
      
      // Return the reference to the refernced object. This could be an lvalue
      // or rvalue reference (possibly const qualified), a temporary objet
      // (e.g., a proxy), or even the iterator itself.
      //
      // FIXME: Is it meaningful to introduce a requirement on an operation 
      // that has incomplete semantics? This is kind of a partial requirement
      // that says, "there's something under here".
      reference ref(Iter i);
    };
  
  // A Move_iterator is an Iterator that exposes its referenced object through
  // a get() operation. Reading the associated value using get() may be a
  // destructive operation. This is the case if the return type of get() is
  // an rvlaue reference or move() is applied to the result. The get() 
  // operation is not required to be regular.
  //
  // NOTE: You can always take a value from an iterator, including (and
  // somewhat surprisingly) iterators with const references. Of course, you
  // can't plug a const rvalue reference into a non-const rvalue reference. You
  // can only copy the results. This is basically what happens for fundamental
  // types.
  //
  // NOTE: This concept does not exist because there are a large number of
  // algorithms that require it. The concept exists because most algorithms 
  // taking input iterators cannot be correct with move iterators.
  template<typename Iter> 
    struct c_Move_iterator : c_Iterator<Iter>
    {
      typedef typename value_type<Iter>::value_type value_type;
      
      // The value type can be moved.
      requires(Movable<value_type>);
      
      // Read the referenced object. Like ref(), the result can be a reference
      // (lvalue or rvalue, possibly const) or temporary. However, the deduced
      // value type must not actually be a reference. The value type must also
      // not be a proxy class (e.g., bit_reference for bit_iterators); the type
      // must represent the underlying object.
      //
      // NOTE: Allowing the result type to be references allows us to avoid
      // unneeded copies. Note that the deduced value type must not be a
      // reference.
      value_type get(Iter i);
      
      // FIXME: Semantics here?
    };
    
  // An Input_iterator is a Move_iterator whose get() operation is regular.
  // That is, multiple reads from the same iterator yield the same result.
  template<typename Iter>
    struct c_Input_iterator : c_Move_iterator<Iter>
    {
      typedef typename c_Move_iterator<Iter>::value_type value_type;
      
      // The function get() is regular. 
      //
      // Note that even in cases where the reference type is an rvalue 
      // reference, we can still call get multiple times unless the result is 
      // connected to a move sink (an object that will take the value).
      requires(Function<decltype([](Iter i) { return get(i); )>{});
      
      // If the value type is Copyable, then we can get the value of the
      // referenced object in order to create a copy.
      requires(Copyable<Iter>)
        bool Read_copy(Iter i)
        {
          return value_type{get(i)} == get(i);
        }
    };

  // An output iterator is an iterator that accepts a value of the specified
  // type (rvalue references imply movement) and assigns it to the referenced
  // object.
  //
  // NOTE: Output iterators do not have a value type.
  template<typename Iter, typename Value>
    struct c_Output_iterator : c_Iterator<Iter>
    {
      void put(Iter& i, Value x);
    };

  // A Forward_iterator is an Input_iterator whose next operation is regular.
  // Advancing the iterator does not destroy the referenced object, as it may
  // with a Move_iterator.
  template<typename Iter>
    struct c_Forward_iterator : c_Input_iterator<Iter>
    {
      requires(c_Function<decltype([](Iter i) { return next(i); )>{});
    };
    
  // A Mutable_iterator is both an Forward_iterator and an Output_iterator
  // whose required put() operation can accept an rvalue reference to an
  // object of the value type. 
  //
  // Mutable iterators are required to be forward iterators and not just input
  // iterators because algorithms that mutate those sequences generally 
  // require the underlying sequence to be preserved during and after 
  // execution.
  template<typename Iter>
    struct c_Mutable_iterator
      : c_Move_iterator<Iter>
      , c_Output_iterator<Iter, typename value_type<Iter>::type&&>
    {
      typedef typename c_Move_iterator<Iter>::value_type value_type;

      // FIXME: Write semantics.
    };
    
  // A Bidirectional_iterator is a Forward_iterator that can move forwards
  // and backwards.
  template<typename Iter>
    struct c_Bidirectional_iterator : c_Forward_iterator<Iter>
    {
      Iter prev(Iter const&);
    };

  // A Random_access_iterator is an Ordered, Bidirectional_iterator that can 
  // move in constant time. The distance between iterators can also be computed
  // in constant time.
  template<typename Iter>
    struct c_Random_access_iterator : c_Ordered<Iter>, c_Bidirectional_iterator<Iter>
    {
      typedef typename c_Iterator<Iter>::difference_type difference_type;
      
      Iter next(Iter i, difference_type n);
      Iter prev(Iter i, difference_type n);
      difference_type distance(Iter i, Iter j);
      
      bool Move_performance(Iter i)
      {
        return performance(next(i)) == O(1) && performance(prev(i)) == O(1);
      }
      
      bool Distance_performance(Iter i, Iter j)
      {
        return performance(distance(i, j)) == O(1);
      }
    };
 
  //
  // The default pointer model. Pointers are random access iterarors.
  //
 
  // Iterator tags specific to pointers
  struct pointer_iterator_tag 
    : random_access_iterator_tag 
  { };

  struct mutable_pointer_iterator_tag 
    : random_access_iterator_tag, mutable_iterator_tag 
  { };
  
  template<typename T>
    inline pointer_iterator_tag category(T const*)
    {
      return pointer_iterator_tag{};
    }  
  
  template<typename T>
    inline mutable_pointer_iterator_tag category(T*)
    {
      return mutable_pointer_iterator_tag{};
    }

  // The only default model is pointers.
  template<typename T>
    inline T* next(T* p)
    {
      return p + 1;
    }
    
  template<typename T>
    inline T* prev(T* p)
    {
      return p - 1;
    }
    
  template<typename T>
    inline void advance(T*& p, std::ptrdiff_t n)
    {
      p += n;
    }
    
  // This is an artifact of language limitations. We can't generally observe
  // function arguments.
  template<typename T>
    inline std::ptrdiff_t zero(T const* p)
    {
      return 0;
    }
    
  template<typename T>
    inline std::ptrdiff_t distance(T const* p, T const* q)
    {
      return q - p;
    }

  // The ref operations return a reference to the referenced object. Note that
  // get() is trivially satisfied by the default implementation.
  template<typename T>
    inline T& ref(T* p)
    {
      return *p;
    }
    
  template<typename T>
    inline T const& ref(T const* p)
    {
      return *p;
    }

  // FIXME: These can go away if we replace all gets() with refs() and puts()
  // with assignment to the referencef.
  template<typename T>
    inline void put(T* p, T&& x)
    {
      *p = std::move(x);
    }

  template<typename T>
    inline void put(T* p, T const& x)
    {
      *p = x;
    }

  // Iterator traits

  // FIXME: The reference, value_type, difference_type, and iterator_category 
  // should all be template aliases.
  template<typename Iter>
    struct reference
    {
      typedef decltype(ref(std::declval<Iter>())) type;
    };
  
  
  // The value type is the deduced type of the Iterator get operation. Note 
  // that auto deduction removes references and const qualifiers. The standard
  // decay type trait also gives us this result.
  template<typename Iter>
    struct value_type
    {
      typedef typename std::decay<typename reference<Iter>::type>::type type;
    };
    
  // By default, the difference type is the result type of the zero
  // operation. That means either that you either have to write distance or
  // specialize the trait.
  template<typename Iter>
    struct difference_type
    {
      typedef decltype(zero(std::declval<Iter>())) type;
    };
    
  template<typename Iter>
    struct iterator_category
    {
      typedef decltype(category(std::declval<Iter>())) type;
    };
    
  // Return true if the given iterator is an input iterator.
  template<typename Iter>
    struct is_input_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, input_iterator_tag
          >::value
        >
    { };
  
  // Return true if the given iterator is an output iterator.
  template<typename Iter>
    struct is_output_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, output_iterator_tag
          >::value
        >
    { };

  // Return true if the given iterator is a mutable iterator.
  template<typename Iter>
    struct is_mutable_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, mutable_iterator_tag
          >::value
        >
    { };
    
  // Return true if the given iterator is a forward iterator.
  template<typename Iter>
    struct is_forward_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, forward_iterator_tag
          >::value
        >
    { };

  // Return true if the given iterator is a bidirectional iterator.
  template<typename Iter>
    struct is_bidirectional_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, bidirectional_iterator_tag
          >::value
        >
    { };
    
  // Return true if the given iterator is a random access iterator.
  template<typename Iter>
    struct is_random_access_iterator
      : bool_constant<
          std::is_convertible<
            typename iterator_category<Iter>::type, random_access_iterator_tag
          >::value
        >
    { };
  
  // Operations
  
  // Return the distance between the first and last iterators. This is only
  // defined for non-random access iterators since random access iterators have
  // the algorithm as a requirement.
  // NOTE: Concept overloading should support the definition of this algorithm
  // in an open way. Because distance() is required by random access iterators,
  // those definitions must be more specialized than this algorithm.
  template<typename Iter>
    typename std::enable_if<
      !is_random_access_iterator<Iter>::value,
      typename difference_type<Iter>::type  
    >::type
    distance(Iter f, Iter l, input_iterator_tag)
    {
      typedef typename difference_type<Iter>::type Diff;
      Diff d = 0;
      while(f != l) {
        f = next(f);
        ++d;
      }
      return d;
    }
    
  // Invoke next n times. Requires Iterator
  template<typename Iter>
    inline typename std::enable_if<
      !is_random_access_iterator<Iter>::value, Iter
    >::type
    next(Iter i, typename difference_type<Iter>::type n) 
    {
      assert(( n >= 0 ));
      while(n--)
        ++i;
    }
  
  // Specialization for random access iterators.
  template<typename Iter>
    inline typename std::enable_if<
      is_random_access_iterator<Iter>::value, Iter
    >::type
    next(Iter i, typename difference_type<Iter>::type n) 
    {
      advance(i, n);
      return i;
    }

  // Invoke prev n times. Requires Bidirectional_iterator
  template<typename Iter>
    inline typename std::enable_if<
      !is_random_access_iterator<Iter>::value, Iter
    >::type
    prev(Iter i, typename difference_type<Iter>::type n) 
    {
      assert(( n >= 0 ));
      while(n--)
        --i;
    }

  // Specialization for random access iterators.
  template<typename Iter>
    inline typename std::enable_if<
      is_random_access_iterator<Iter>::value, Iter
    >::type
    prev(Iter i, typename difference_type<Iter>::type n) 
    {
      advance(i, -n);
      return i;
    }
  
  // A default implementation of get() is defined for every iterator whose
  // reference type is actually a Reference type. The result of the operation 
  // is the same as the reference, which means that the value type is deduced
  // as the value type with cv-qualifiers and references removed..
  template<typename Iter>
    requires(Iterator<Iter>)
      auto get(Iter&& i) ->
        typename std::enable_if<
          std::is_reference<typename reference<Iter>::type>::value,
          decltype(ref(std::forward<Iter>(i)))
        >::type
      {
        return ref(std::forward<Iter>(i));
      }

  // Return the nth element from the random access iterator.
  template<typename Iter>
    inline auto
    get(Iter&& i, typename difference_type<Iter>::type n)
      -> typename std::enable_if<
          is_random_access_iterator<Iter>::value, 
          decltype(get(next(std::forward<Iter>(i), n)))
        >::type
    {
      return get(next(std::forward<Iter>(i), n));
    }

  // Copy the given value into the nth position past the random access 
  // iterator.
  template<typename Iter>
    inline typename std::enable_if<
      is_random_access_iterator<Iter>::value, void
    >::type
    put(Iter& i, 
        typename difference_type<Iter>::type n, 
        typename value_type<Iter>::type const& x)
    {
      put(next(i, n), x);
    }
}

#endif

