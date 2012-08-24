// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_CONCEPTS_HPP
#define ORIGIN_CONCEPTS_HPP

#include <origin/type/traits.hpp>

namespace origin
{
  // Implementation
  namespace concepts 
  {
    template <typename T, typename U> struct is_equality_comparable;
    template <typename T, typename U> struct is_weakly_ordered;
    template <typename R, typename T, typename U> struct is_relation;
    template <typename O, typename T, typename U> struct is_binary_operation;
  }

  // Declarations
  template <typename T> constexpr Boolean();



  //////////////////////////////////////////////////////////////////////////////
  // Boolean                                                    concepts.boolean
  //
  // A type T is Boolean if it can be used in a Boolean context; That is, T
  // must be convertible to bool. For example:
  //
  //    Boolean<int>()    // returns true
  //    Boolean<void*>()  // returns true
  //    Boolean<string>() // returns false.
  // 
  // This concept is primarily defined as a more concise way of writing
  // Convertible<T, bool>.
  //
  // Template parameters:
  //    T -- The type being tested
  //
  // Returns:
  //    True if and only if T can be converted to bool.
  template <typename T>
    constexpr bool Boolean() { return Convertible<T, bool>(); }



  //////////////////////////////////////////////////////////////////////////////
  // Equality                                                        concepts.eq
  //
  // TODO: Give a semantic description of notions of equality.
  //
  // Note that it is inusfficient to define == as only an equivalence relation.
  // Two equivalent values may be substituted in programs, yielding different
  // results.



  //////////////////////////////////////////////////////////////////////////////
  // Equality Comparable                         concepts.eq.equality_comparable
  //
  // The equality comparable determines if objects of a given type (or set of
  // types) can be compared for equality (==) or differentiated (!=). There are
  // two overloads of this concept.
  //
  //    Equality_comparable<T>()
  //    Equality_comparable<T, U>()
  // 
  // The unary concept determines if objects of type T can be compared using
  // == and !=. The semantics of the == operator is that it implement the
  // equality relation (see [concepts.eq]); it must be an equivalence relation
  // that returns true when one argument can be substituted for the other in
  // regular programs. The != operator must be the complement of the equality
  // relation.
  //
  // The first overload returns true when T satisfies the syntactic requirements
  // of equality comparison. For example:
  //
  //    Equality_comparable<int>()           // returns true
  //    Equality_comparable<runtime_error>() // returns false
  //
  // The binary concept defines cross-type equality comparison. If T and U are
  // different types, then T and U must be individually equality comparable, T
  // and U must have a common type, C, that is also equality comparable, and
  // objects of type T and U must be symmetrically equality comparable using ==
  // and !=. The results of those comparisons must be the same as converting
  // each argument to the common type before comparing. See Common_type for more
  // information about cross-type concepts.
  //
  // This overload returns true when T and U satisfy the syntactic requirements
  // of cross-type equality comparison. For example:
  //
  //    Equality_comparable<int, long>()           // returns true
  //    Equality_comparable<string, const char*>() // returns true
  //    Equality_comparable<string, int>()         // returns false
  //
  // When the arguments T and U are the same, the binary concept is equivalent
  // to the unary concept. That is:
  //
  //    Equality_comparable<T, T>() <=> Equality_comparable<T>()
  //
  template <typename T, typename U = T>
    constexpr bool Equality_comparable()
    {
      return Equality_comparable_concept<T, U>::value;
    }



  //////////////////////////////////////////////////////////////////////////////
  // Ordering                                                       concepts.ord
  //
  // There are two ordering concepts in Origin: weak ordering and total
  // ordering. Both require the definition of the relational operators <, >,
  // <=, and >=. Additionally, the incomparability relation is also associated
  // with ordering. Two ordered values a and b are incomparable when
  //
  //    !(a < b) && !(a < b)
  //
  // The semantics of the ordering are primarily associated with the < operator.
  // For specifics, refer to [concepts.weakly_ordered] and
  // [concepts.totally_ordered]. The definitions of >, <=, and >= can be derived
  // from < in the usual way:
  //
  //    a > b  <=> b < a
  //    a <= b <=> !(b < a)
  //    a >= b <=> !(a < b)
  //
  // Note that there is no partially ordered concept for types. The reason for
  // this is that we expect all relational operators to be total operations in
  // the sense that all well-formed values of the type can be compared. There
  // is, however, a partial ordering property that can be used to describe 
  // specific relations.



  //////////////////////////////////////////////////////////////////////////////
  // Weakly Ordered                                            concepts.ord.weak
  //
  // The weakly ordered concept determines if a type (or pair of types) can be
  // compared using the relational operators <, >, <=, and >=. There are two
  // overloads of the weakly ordered concept.
  //
  //    Weakly_ordered<T>()
  //    Weakly_ordered<T, U>()
  //
  // The unary overload determines if a type T is is weakly ordered. A weakly
  // ordered type defines the standard relational operators <, >, <=, and >=
  // where < defines a strict weak order and the other operators have the usual
  // meaning. A strict weak order is a relation that is irreflexive, asymmetric,
  // and transitive, and incomparability is an equivalence relation.
  //
  // This overload returns true when T satisfies the syntactic requirements of
  // weakly ordered types. For example:
  //
  //    Weakly_ordered<int>()       // returns true
  //    Weakly_ordered<type_info>() // returns false (no operator <)
  //
  // Note that weakly ordered types are rarely defined in practice. If one can
  // define an ordering over values, then one can generally define a reasonable
  // equality operator. The weakly ordered concept is 
  //
  // The binary overload checks if T and U are cross-type weakly ordered. That
  // is, T and U must be individually weakly or totally ordered, T and U must
  // share a common type C, which is also weakly or totally ordered, and objects
  // of type T and U must be symmetrically comparable using <, >, <= and >=. See
  // Common_type for more information cross-type concepts.
  //
  // This overload returns true when the syntactic requirements for cross-type
  // weak ordering are satisfied. For example:
  //
  //    Weakly_ordered<int, long>()           // returns true
  //    Weakly_ordered<string, const char*>() // returns true
  //    Weakly_ordered<string, int>()         // returns false
  //
  // When the type areguments T and U are the same, the binary concept is
  // equivalent to the unary concept:
  //
  //    Weakly_ordered<T, T>() <=> Weakly_ordered<T>()
  //
  template <typename T, typename U = T>
    constexpr bool Weakly_ordered()
    {
      return is_weakly_ordered<T, U>::value;
    }



  //////////////////////////////////////////////////////////////////////////////
  // Totally Ordered                                          concepts.ord.total
  //
  // There are two overloads of the totally ordered concept.
  //
  //    Totally_ordered<T>
  //    Totally_ordered<T, U>
  //
  // The unary overload determines if a type T is is totally ordered. A totally
  // ordered type is both weakly ordered and equality comparable where the
  // incomparability relation is the same as the equality operator. That is:
  //
  //    !(a < b) && !(b < a) <=> a == b
  //
  // This overload returns true when T satisfies the syntactic requirements of
  // totally ordered types. For example:
  //
  //    Totally_ordered<int>()       // returns true
  //    Totally_ordered<type_info>() // returns false (no operator <)
  //
  // The binary overload checks if T and U are cross-type weakly ordered. That
  // is, T and U must be individually weakly or totally ordered, T and U must
  // share a common type C, which is also weakly or totally ordered, and objects
  // of type T and U must be symmetrically comparable using <, >, <= and >=. See
  // Common_type for more information cross-type concepts.
  //
  // This overload returns true when the syntactic requirements for cross-type
  // weak ordering are satisfied. For example:
  //
  //    Weakly_ordered<int, long>()           // returns true
  //    Weakly_ordered<string, const char*>() // returns true
  //    Weakly_ordered<string, int>()         // returns false
  template <typename T, typename U>
    constexpr bool Totally_ordered()
    {
      return Equality_comparable<T, U>() && Weakly_ordered<T, U>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Object concepts                                                concepts.obj
  //
  // The concepts in this section describe properties of types that define
  // objects. This is a small hierarchy of concepts that builds on a number of
  // initialization and assignment type traits. It includes:
  //
  //    Movable
  //    Copyable
  //    Semiregular
  //    Regular
  //
  // The reason that we refer to these as "object" concepts is that they 
  // describe properties of types used to initialize (and destroy and compare)
  // objects.
  //
  // Note that the "trivial" type traits can be seen as semantic refinements
  // of these concepts. That is, a trivially movable is a refinement of movable,
  // trivially copyable is a refinement of copyable, and trivial is a refinement
  // of semiregular.



  //////////////////////////////////////////////////////////////////////////////
  // Movable                                                concepts.obj.movable
  //
  // A type T is movable if it is desctructible and can be both move constructed
  // and move assigned. Note that a type is still movable even if the
  // constructor and assignment operator actually perform copies. 
  //
  // The function returns true if T satisfies the syntactic requirements of
  // movability.
  //
  //    Movable<int>()          // returns true
  //    Movable<string>()       // returns true 
  //    Movable<decltype([]{})> // returns false (not move assignable)
  //
  template <typename T>
    constexpr bool Movable()
    {
      return Destructible<T>() 
          && Move_constructible<T>() 
          && Move_assignable<T>();
    };
    
  
    
  //////////////////////////////////////////////////////////////////////////////
  // Copyable                                              concepts.obj.copyable
  //
  // A type T is copyable if it is movable and can be both copy constructed and 
  // copy assigned.
  //
  // The function returns true if T satisfies the syntactic requirements of
  // copyability.
  //
  //    Copyable<int>()          // returns true
  //    Copyable<string>()       // returns true 
  //    Copyable<decltype([]{})> // returns false (not copy assignable)
  //
  // Template parameters:
  //    T -- The type being tested
  //
  // Returns:
  //    True if and only if T satisfies the syntactic requirements of the
  //    Copyable concept.
  template <typename T>
    constexpr bool Copyable()
    {
      return Movable<T>() 
          && Copy_constructible<T>() 
          && Copy_assignable<T>();
    }
    
    
  
  //////////////////////////////////////////////////////////////////////////////
  // Semiregular                                        concepts.obj.semiregular
  //
  // A semiregular type T is copyable and default constructible. Semiregular
  // types are those that can be used in general ways: we can use them to 
  // declare variables or arrays and pass them by value (i.e., copy) or move
  // them. For example, the following class is semiregular:
  //
  //    struct Person 
  //    {
  //      string first;
  //      string last;
  //      int id;
  //    };
  //
  // The destructor, default constructor, move constructor, copy constructor,
  // and move assignment oeprator, and copy assignment operator are all
  // implicitly generated, but are non-trivial. Note that the class would still
  // be Semiregular if the members were private, as long as the required
  // operations were still publicly accessible.
  //
  // The notion of semiregularity of a type generalizes the notion of trivial
  // types in the language. A trivial type is one that trivially copyable (has
  // a trivial copy constructor and copy assignment operator) and trivially
  // default constructible.
  //
  // Template parameters:
  //    T -- the type being tested for semiregularity.
  //
  // Returns:
  //    True if and only if T satisfies the syntactic requirements of the
  //    Semiregular concept.
  template <typename T>
    constexpr bool Semiregular()
    {
      return Copyable<T>() && Default_constructible<T>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Regular                                                concepts.obj.regular
  //
  // A regular type T is semiregular and equality comparable. Regular types
  // are the basis of a large number of abstractions - in particular, those that
  // can be used as (if they represented) values. For example, every built-in
  // type is regular, and so are all standard container types (assuming that
  // their contained value types are also regular).
  //
  // Template parameters:
  //    T -- the type being tested for regularity.
  //
  // Returns:
  //    True if and only if T satisfies the syntactic requirements of the
  //    Regular concept.
  template <typename T>
    constexpr bool Regular()
    {
      return Semiregular<T>() && Equality_comparable<T>();
    }
    
  
  //////////////////////////////////////////////////////////////////////////////
  // Ordered                                                concepts.obj.ordered
  //
  // A type T is ordered if it is regular and totally ordered. This concept
  // is primarily provided as a convenience for describing higher level 
  // abstractions. 
  //
  // Template parameters:
  //    T -- The type being tested
  //
  // Returns:
  //    True if and only if T satisfies the syntactic requirements of the
  //    Ordered concept.
  template <typename T>
    constexpr bool Ordered()
    {
      return Regular<T>() && Totally_ordered<T>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Functions                                                     concepts.func
  //
  // A function is an object that can be called, taking a sequence of arguments
  // and returning some result (possibly none). There are a number of concepts
  // describing the kinds of functions in the STL.
  //
  // - Function
  // - Regular function
  // - Predicate
  // - Relation
  // - Unary operation
  // - Binary operation
      


  //////////////////////////////////////////////////////////////////////////////
  // Function                                             concepts.func.function
  //
  // A function type, F, is one that can be called with a sequence of arguments,
  // Args. Every function has an associated result type that can be deduced
  // using Result_of<F(Args...)>.
  //
  // Note that functions are not required to be equality preserving. There are
  // no constraints on the effects of the function.
  //
  // Template parameters:
  //    F -- The function being tested
  //    Args -- A sequence of argument types over which F might be called
  //
  // Returns:
  //    True if and only if F can be called with a sequence of arguments whose
  //    types are given by the type parameter pack Args.
  template <typename F, typename... Args>
    constexpr bool Function()
    {
      return Copy_constructible<F>() && Has_call<F, Args...>();
    }
    


  //////////////////////////////////////////////////////////////////////////////
  // Regular Function                                      concepts.func.regular
  //
  // A regular function is an equality preserving function. That is, equal
  // inputs reliably yield equal outputs. More formally: if a and b are
  // equivalent expressions (have the same observable effects), then:
  //
  //    a <=> b => f(a) <=> f(b)
  //
  // Evaluating a regular function, f, over the equivalent expressions, a and b,
  // will yield equivalent computations.
  //
  // The result of a regular function cannot be void.
  //
  // Template parameters:
  //    - F    -- The function being tested
  //    - Args -- A sequence of argument types over which F might be called
  //
  // Returns:
  //    True if and only if F can be called with a sequence of arguments whose
  //    types are given by the type parameter pack Args.
  template <typename F, typename... Args>
    constexpr bool Regular_function()
    {
      return Function<F, Args...>() && !Same<Result_of<F(Args...)>, void>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Predicate                                           concepts.func.predicate
  //
  // A predicate is a regular function whose result type is convertible to
  // bool.
  //
  // Template parameters:
  //    - P    -- The predicate being tested
  //    - Args -- A sequence of argument types over which P might be called
  //
  // Returns:
  //    True if and only if P can be called with a sequence of arguments whose
  //    types are given by the type parameter pack Args, and if the result of 
  //    of the function is Boolean.
  template <typename P, typename... Args>
    constexpr bool Predicate()
    {
      return Function<P, Args...>() && Boolean<Result_of<P(Args...)>>();
    }
    
    
    
  //////////////////////////////////////////////////////////////////////////////
  // Relation                                             concepts.func.relation
  //
  // A relation is a binary bredicate over a set of values. There are two
  // overloads of this concept:
  //
  //    Relation<R, T>()
  //    Relation<R, T, U>()
  //
  // The first overload determines if R is a relation with the homogenous
  // domain T. The second is a generalization, allowing the domain to be
  // heterogenous. However, when the domain is heterogeneous, the types t and
  // U must share a common type.
  //
  // Template parameters:
  //    R -- The relation being tested
  //    T -- The domain of the relation
  //    U -- If given, the heterogeneous domain type along with T
  //
  // Returns:
  //    True if R is a relation taking arguments arguments of type T (and U, if
  //    specified).
  template <typename R, typename T, typename U = T>
    constexpr bool Relation()
    {
      return is_relation<R, T, U>::value;
    }



  //////////////////////////////////////////////////////////////////////////////
  // Relational Properties                              properties.func.relation
  //
  // The followwing predicates are used to write preconditions in generic
  // algorithms or other data structures. Note that because these properties
  // cannot be evaluated at runtime, they simply return true. They are primarily
  // used to indicate a symbolic property.

  // Evaluates whether r is an equivalence relation. Always returns true.
  template <typename R>
    constexpr bool is_equivalence_relation(R r) { return true; }

  // Determines whether r is a strict partial order. Always returns true.
  template <typename R>
    constexpr bool is_strict_partial_order(R r) { return true; }

  // Determines whether r is a strict weak order. Always returns true.
  template <typename R>
    constexpr bool is_strict_weak_order(R r) { return true; }

  // Determines whether r is a total order. Always returns true.
  template <typename R>
    constexpr bool is_total_order(R r) { return true; }



  //////////////////////////////////////////////////////////////////////////////
  // Generator                                           concepts.func.generator
  //
  // a generator is a nullary function (taking no arguments) that returns values
  // of a specified type. Generators are typically non-regular functions, since
  // they are often used to generate different values on subsequent calls.
  // Random value generators are examples of generating functions. 
  //
  // Template parameters:
  //    G -- The function being tested
  //
  // Returns:
  //    True if and only if G can be called with no arguments, and has a 
  //    non-void result type.
  template <typename G>
    constexpr bool Generator()
    {
      return Function<G>() && !Void<Result_of<G()>>();
    }



  //////////////////////////////////////////////////////////////////////////////
  // Operations                                                     concepts.ops
  //
  // An operation is a regular function that takes one or more arguments of
  // its domain type and returns a value of the same type. The domain and
  // codomain types are the same.
  //
  // Operation concepts are prefixed with their arity because of overloads of
  // the binary operation concept.



  //////////////////////////////////////////////////////////////////////////////
  // Unary Operation                                          concepts.ops.unary
  //
  // Template parameters:
  //    Op -- The operation being tested
  //    T  -- The domain type of Op
  //
  // Returns:
  //    True if and only if Op can be called with a single argument of type T
  //    and the result type of Op is also T.
  //
  // TODO: Describe this concept.
  template <typename Op, typename T>
    constexpr bool Unary_operation()
    {
      return Regular_function<Op, T>() && Same<Result_of<Op(T)>, T>();
    }


  //////////////////////////////////////////////////////////////////////////////
  // Binary Operations                                       concepts.ops.binary
  //
  // Template parameters:
  //    Op -- The operation being tested
  //    T  -- The domain type of Op
  //    U  -- If given, the heteregenous domain type (along with T) of Op
  //
  // Returns:
  //    True if and only if Op can be called with arguments of type T and U,
  //    and the result type of Op is Common_type<T, U>.
  //
  // TODO: Describe this concept.
  template <typename Op, typename T, typename U = T>
    constexpr bool Binary_operation()
    {
      return concepts::is_binary_operation<Op, T, U>::value;
    }


  // Concepts for common associated types

  
  // Value type.
  //
  // A value type is the type of a value contained or referred to by another
  // object. For example, the value type of a container is the type of objects
  // it contains. The value type of an iterator is the type of object it
  // refers to.
  //
  // Note that a value type is never just the result type of an accessor
  // function -- it is never a cv-qualified or reference type.
  //
  // In general 

  namespace traits
  {
    // Type deduction hook.
    subst_failure deduce_value_type(...);

    // If T can be dereferenced, then the value type is the result of that
    // operation minus reference and const-qualifiers.
    template <typename T>
      auto deduce_value_type(default_t, const T&) 
        -> Requires<Has_dereference<T>(), Unqualified<Dereference_result<T>>>;


    // Deduce the value type associated with T. We first look for an 
    // associated member type, then default to the deduction overloads.
    template <typename T, bool = Has_associated_value_type<T>()>
      struct get_value_type;

    template <typename T>
      struct get_value_type<T, true>
      {
        using type = typename T::value_type;
      };

    template <typename T>
      struct get_value_type<T, false>
      {
        using type = decltype(deduce_value_type(default_t {}, std::declval<T>()));
      };

  } // namespace traits
  


  // Value traits (traits class)
  // The value traits class can be used to specialize the associated value type
  // of T. This overrides the usual deduction protocol for specific sets of
  // types.
  template <typename T>
    struct value_traits
    {
      using type = typename traits::get_value_type<T>::type;
    };

  template <typename T>
    struct value_traits<T&> : value_traits<T> { };

  template <typename T>
    struct value_traits<T&&> : value_traits<T> { };

  template <typename T, std::size_t N>
    struct value_traits<T[N]>
    {
      using type = T;
    };



  // Value type (alias)
  // The Value_type alias associates a value type with another type, usually
  // the type of a sub-object or contained object.
  template <typename T>
    using Value_type = typename value_traits<T>::type;



  // Value type (trait)
  // Return true if T has an associated value type.
  template <typename T>
    constexpr bool Has_value_type()
    {
      return Subst_succeeded<Value_type<T>>();
    }

    

  // Distance Type
  // The distance type is a type that encodes the distance between two 
  // objects (e.g., iterators, pointers, and integral types in general).
  //
  // This is called difference type in the standard library. We rename it
  // to be an accordance with the Palo Alto TR. Distance is also easier to
  // write than difference.
  //
  // User-defined types model this concepts by containing a nested member
  // called the difference_type (i.e., T::difference_type must be a valid
  // type name). The distance type is implicitly defined for integral types
  // pointers, and statically sized arrays.
  //
  // The notion of "distance" as used in most concept designs is somewhat
  // artificial in that it generally required to be explicitly specified. A
  // more natural approach would be to deduce the distance type as a result
  // of the distance() function as if it were required by all differentiable
  // values.

  namespace traits {
    // Type deduction hook. 
    subst_failure deduce_distance_type(...);

    template <typename T>
      auto deduce_distance_type(default_t, T) -> Requires<Arithmetic<T>(), T>;

    // We can provide a reasonable guess for all other incrementable 
    // user-defined types as ptrdiff_t. This is basically a default guess for
    // all iterator-like types.
    template <typename T>
      auto deduce_distance_type(default_t, T) 
        -> Requires<Class<T>() && Has_pre_increment<T>(), std::ptrdiff_t>;


    // Get the distance type associated with T.
    template <typename T, bool = Has_associated_difference_type<T>()>
      struct get_distance_type;

    template <typename T>
      struct get_distance_type<T, true>
      {
        using type = typename T::difference_type;
      };

    template <typename T>
      struct get_distance_type<T, false>
      {
        using type = decltype(deduce_distance_type(default_t {}, std::declval<T>()));
      };
  } // namespace traits



  // Distance traits (traits class)
  // The distance traits class can be specialized to associated a distance
  // type with a specific set of types. This traits class is specialized for
  // pointers, arrays, and initializer lists, by default.
  template <typename T>
    struct distance_traits
    {
      using type = typename traits::get_distance_type<T>::type;
    };

  template <typename T>
    struct distance_traits<T&> : distance_traits<T> { };

  template <typename T>
    struct distance_traits<T&&> : distance_traits<T> { };

  template <typename T>
    struct distance_traits<T*>
    {
      using type = std::ptrdiff_t;
    };
  
  template <typename T, std::size_t N>
    struct distance_traits<T[N]>
    {
      using type = std::ptrdiff_t;
    };

  template <typename T>
    struct distance_traits<std::initializer_list<T>>
    {
      using type = std::ptrdiff_t;
    };



  // Distance type (alias)
  // An alias to the associated distance type, if supported.
  template <typename T>
    using Distance_type = typename distance_traits<T>::type;
    


  // Has distance type (trait)
  // Returns true if T has an associated difference type.
  template <typename T>
    constexpr bool Has_distance_type()
    {
      return Subst_succeeded<Distance_type<T>>();
    }



  // Size type
  // An associated size type encodes the largest possible size of a data
  // structure. For example, the number of elemnts in a container, or the
  // number of rows and columns in a matrix. 


  // Infrastructrue for deducing a size type.
  namespace traits
  {
    // This function is called to deduce the size type within some conceptual
    // domain. This declaration depends on ADL to find overloads for differnt
    // conceptual domains.
    subst_failure deduce_size_type(...);

    // Deduce the associate size type of T. If T has an explicitly associated
    // size type, then that is taken to be T's size type. Otherwise, we expect
    // the associated size type be deduced from the member function size().
    template <typename T, bool = Has_associated_size_type<T>()>
      struct get_size_type;

    template <typename T>
      struct get_size_type<T, true>
      {
        using type = typename T::size_type;
      };

    template <typename T>
      struct get_size_type<T, false> 
      { 
        using type = decltype(deduce_size_type(default_t {}, std::declval<T>()));
      };
  } // namespace traits



  // Size traits (traits class)
  // The size_traits class can be used to specialize the associated size type 
  // for a specific type or set of types. Traits class specialization should
  // only be used when T is not a mode of a "sized" concept.
  template <typename T>
    struct size_traits
    {
      using type = typename traits::get_size_type<T>::type;
    };

  template <typename T>
    struct size_traits<T&> : size_traits<T> { };

  template <typename T>
    struct size_traits<T&&> : size_traits<T> { };



  // Size type (alias)
  // Refers to the associated size type of T.
  template <typename T>
    using Size_type = typename size_traits<T>::type;



  // Has size type (trait)
  // Returns true if T hwas an associated size type.
  template<typename T>
    constexpr bool Has_size_type()
    {
      return Subst_succeeded<Size_type<T>>();
    }


  
  // Streaming concepts
  // There are 3 concepts describing streaming operations.
  // - Input_streamable<T>
  // - Output_streamable<T>
  // - Streamable<T>



  // Input streamable
  // Describes types that can be read from a stream using the input stream
  // operator (>>). There are two overloads of this concept:
  // - Input_streamable<T>
  // - Input_streamable<S, T>
  //
  // The first determines if T can be read from a stream derived from either 
  // istream or wistream. Note that the two are equivalent are equivalent if 
  // the >> overloads are defined in terms of basic_istream.
  //
  // The second determines if T can be read from the stream S.
  
  // Safely deduce the result type of an input streaming operator, s >> x, for
  // any Stream s.
  template <typename S, typename T>
    struct get_input_stream_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y& y) -> decltype(x >> y);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<S&>(), std::declval<T&>()));
    };

  // An alias for the result type of the expression, s >> x, for any Stream s.
  template <typename S, typename T>
    using Input_stream_result = typename get_input_stream_result<S, T>::type;

  // The Input_streamable concept.
  template <typename S, typename T>
    struct Input_streamable_concept
    {
      static constexpr bool check()
      {
        return Subst_succeeded<Input_stream_result<S, T>>();
      }
    };
    
  template <typename T>
    struct Input_streamable_concept<T, default_t>
    {
      static constexpr bool check()
      {
        return Subst_succeeded<Input_stream_result<std::istream, T>>();
      }
    };

  // Return true if values of type T can be read from an input stream.
  template <typename T, typename U = default_t>
    constexpr bool Input_streamable()
    {
      return Input_streamable_concept<T, U>::check();
    }



  // Output streamable
  // Describes types that can be written to a stream using output stream
  // operator (<<). There are two overloads of this concept:
  // - Output_streamable<T>
  // - Output_streamable<S, T>
  //
  // The first determines if T can be written to a stream derived from either 
  // ostream or wostream. Note that the two are equivalent are equivalent if 
  // the >> overloads are defined in terms of basic_ostream.
  //
  // The second determines if T can be written to the stream S.
  
  // Safely deduce the result type of an input streaming operator, s << x, for
  // any Stream s.
  template <typename S, typename T>
    struct get_output_stream_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y const& y) -> decltype(x << y);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<S&>(), std::declval<T const&>()));
    };

  // An alias for the result type of the expression, s >> x, for any Stream s.
  template <typename S, typename T>
    using Output_stream_result = typename get_output_stream_result<S, T>::type;

  // The Input_streamable concept.
  template <typename S, typename T>
    struct Output_streamable_concept
    {
      static constexpr bool check()
      {
        return Subst_succeeded<Output_stream_result<S, T>>();
      }
    };
    
  template <typename T>
    struct Output_streamable_concept<T, default_t>
    {
      static constexpr bool check()
      {
        return Subst_succeeded<Output_stream_result<std::ostream, T>>();
      }
    };

  // Return true if values of type T can be read from an input stream.
  template <typename T, typename U = default_t>
    constexpr bool Output_streamable()
    {
      return Output_streamable_concept<T, U>::check();
    }



  // Return true if the values of T can be streamed: read from and written to
  // a stream.
  template <typename T, typename U = default_t>
    constexpr bool Streamable()
    {
      return Input_streamable<T, U>() && Output_streamable<T, U>();
    }


  // Implementation
  namespace concepts
  {
    // Returns true if T and U are cross-type equality comparable.
    template <typename T, typename U>
      struct is_equality_comparable
        : boolean_constant<
              Common<T, U>() 
           && Equality_comparable<T>()
           && Equality_comparable<U>()
           && Equality_comparable<Common_type<T, U>>()
           && Has_equal<T, U>()     && Boolean<Equal_result<T, U>>()
           && Has_equal<U, T>()     && Boolean<Equal_result<U, T>>()
           && Has_not_equal<T, U>() && Boolean<Equal_result<T, U>>()
           && Has_not_equal<U, T>() && Boolean<Equal_result<T, U>>()
          >
      { };
    
    // Returns true if T is equality comparable.
    template <typename T>
      struct is_equality_comparable<T, T>
        : boolean_constant<
              Has_equal<T>()     && Boolean<Equal_result<T>>()
           && Has_not_equal<T>() && Boolean<Not_equal_result<T>>()
          >
      { };



    // Returns true if T and U are cross-type weakly ordered.
    template <typename T, typename U>
      struct is_weakly_ordered
        : boolean_constant<
              Common<T, U>()
           && Totally_ordered<T>()
           && Totally_ordered<T>()
           && Totally_ordered<Common_type<T, U>>()
           && Has_less<T, U>()          && Boolean<Less_result<T, U>>()
           && Has_less<U, T>()          && Boolean<Less_result<U, T>>()
           && Has_greater<T, U>()       && Boolean<Greater_result<T, U>>()
           && Has_greater<U, T>()       && Boolean<Greater_result<U, T>>()
           && Has_less_equal<T, U>()    && Boolean<Less_equal_result<T, U>>()
           && Has_less_equal<U, T>()    && Boolean<Less_equal_result<U, T>>()
           && Has_greater_equal<T, U>() && Boolean<Greater_equal_result<T, U>>()
           && Has_greater_equal<U, T>() && Boolean<Greater_equal_result<U, T>>()
          >
      { };

    // Returns true if T is weakly ordered.
    template <typename T>
      struct is_weakly_ordered<T, T>
        : boolean_constant<
              Has_less<T>()          && Boolean<Less_result<T>>()
           && Has_greater<T>()       && Boolean<Greater_result<T>>()
           && Has_less_equal<T>()    && Boolean<Less_equal_result<T>>()
           && Has_greater_equal<T>() && Boolean<Greater_equal_result<T>>()
          >
      { };


    // Returns true if R is a cross-type relation over (T, U).
    template <typename R, typename T, typename U>
      struct is_relation
        : boolean_constant<
              Common<T, U>()
           && Relation<R, T>()
           && Relation<R, U>()
           && Relation<R, Common_type<T, U>>()
           && Predicate<R, T, U>()
           && Predicate<R, U, T>()
          >
      { };

    // Returns true if R is a relation over T.
    template <typename R, typename T>
      struct is_relation<R, T, T>
        : boolean_constant<Predicate<R, T, T>()>
      { };


    // Returns true if Op is a binary operation over T and U.
    template <typename R, typename T, typename U>
      struct is_binary_operation
        : boolean_constant<
              Common<T, U>()
           && Relation<R, T>()
           && Relation<R, U>()
           && Relation<R, Common_type<T, U>>()
           && Regular_function<R, T, U>() 
           && Same<Result_of<R(T, U)>, Common_type<T, U>>()
           && Regular_function<R, U, T>()
           && Same<Result_of<R(U, T)>, Common_type<U, T>>()
          >
      { };

  } // namespace concepts

} // namespace origin

#endif
