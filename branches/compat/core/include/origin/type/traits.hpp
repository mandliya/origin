// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_TRAITS_HPP
#define ORIGIN_TYPE_TRAITS_HPP

#include <type_traits>
#include <utility>
#include <iosfwd>

namespace std
{
  template<typename... > class tuple;
} // namespace std


namespace origin
{
  //////////////////////////////////////////////////////////////////////////////
  // Miscellaneous types
  //
  // Various types used throughout Origin.
  //////////////////////////////////////////////////////////////////////////////

  // Default type
  //
  // The default type is a tag class used to indicate the selection of a default
  // value. This is only used to support class template specialization.
  struct default_t { };
  

  // Unspecified type
  //
  // The unspecified type is a tag class used to indicate that an argument for a
  // template parameter has not been specified.
  struct unspecified_t { };
  

  // Empty type
  //
  // The empty type is an empty, trivial type that is meant to be used as a
  // placeholder for unspecified types in containers or other data structures.
  struct empty_t { };


  // Streamable<empty_t>
  //
  // TODO: Should we actually read and/or write something for this type? It
  // might be kind of nice.
  template <typename Char, typename Traits>
    inline std::basic_ostream<Char, Traits>& 
    operator<<(std::basic_ostream<Char, Traits>& os, empty_t)
    { 
      return os; 
    }

  template <typename Char, typename Traits>
    inline std::basic_istream<Char, Traits>& 
    operator>>(std::basic_istream<Char, Traits>& is, empty_t&)
    { 
      return is; 
    }
    

  //////////////////////////////////////////////////////////////////////////////
  // Metaprogramming support
  //
  // The following construccts extend the std type traits, providing new 
  // features, aliases, and functiohns.
  //////////////////////////////////////////////////////////////////////////////

  // The bool constant is an alias for the type integral_constant<bool, X>.
  // This type is provided purely for convenience.
  template <bool B>
    using boolean_constant = std::integral_constant<bool, B>;


  // The requires template is a conditional alias for T, if B is true. If B is
  // false, the alias results in a substitution failure.
  template <bool B, typename T = void>
    using Requires = typename std::enable_if<B, T>::type;


  // An alias for T if B is true and F is B false.
  template <bool B, typename T, typename F>
    using If = typename std::conditional<B, T, F>::type;
  

  // Returns true if each boolean argument is true or if no arguments are given.
  // This function can be used to evaluate a concept or type constraint for all
  // types in a template parameter pack. For example:
  //
  //    static_assert(All(Equality_comparable<Args>()...), "")
  //
  // where Equality_comparable is a concept taking a single argument.
  //
  // Note that all arguments must be convertible to bool.
  constexpr bool All() { return true; }
    

  template <typename... Args>
    constexpr bool All(bool b, Args... args)
    {
      return b && All(args...);
    }


  // Infrastructure for implementing type traits.
  namespace traits
  {
    // Return the first type in the given sequence of arguments. This is
    // undefined when Args... is empty.
    template <typename... Args> struct front_type;

    template <typename T, typename... Args>
      struct front_type<T, Args...>
      { 
        using type = T; 
      };



    // Returns the last type in a sequence of type arguments. This is
    // undefined when Args... is empty.
    template <typename... Args> struct back_type;

    template <typename T>
      struct back_type<T>
      { 
        using type = T;
      };

    template <typename T, typename... Args>
      struct back_type<T, Args...> : back_type<Args...>
      { };
  } // namespace traits


  // An alias to the first type in a non-empty sequence of type arguments.
  template <typename... Args>
    using Front_type = typename traits::front_type<Args...>::type;


  // An alias to the last type in a non-empty sequence of type arguments.
  template <typename T, typename... Args>
    using Back_type = typename traits::back_type<T, Args...>::type;
    


  //////////////////////////////////////////////////////////////////////////////
  // SFINAE support
  //
  // The following types and functions are used to support safe type deduction
  // and type trait implementations in the Origin library.
  //////////////////////////////////////////////////////////////////////////////

  template <typename... Args> constexpr bool Same();
  template <typename T, typename U> constexpr bool Different();

  // The substitution failure type represents the result of failed name lookup
  // and is used to support queries about the existence or state of expressions
  // that might fail.
  struct subst_failure { };


  // Returns true if T indicates a substitution failure.
  template <typename T>
    constexpr bool Subst_failed() { return Same<T, subst_failure>(); }


  // Returns true if T does not indicate a substitution failure.
  template <typename T>
    constexpr bool Subst_succeeded() { return Different<T, subst_failure>(); }



  //////////////////////////////////////////////////////////////////////////////
  // Type relations
  //
  // The following type constraints describe relations on types. There are
  // four:
  //
  //    - Same
  //    - Common
  //    - Convertible
  //    - Derived
  //////////////////////////////////////////////////////////////////////////////


  // Infrastructure for defining the Same() constraint.
  namespace traits
  {
    // Returns true if all of the types are the same, or if Args... is an
    // empty sequence of types. Evaluation is of type equality is performed
    // left-to-right, and the operation will not instantiate arguments after
    // a non-equal pair is found.
    template <typename... Args> struct are_same;

    // True for an empty sequence
    template <>
      struct are_same<> : std::true_type
      { };

    // For a single type, this is trivially true.
    template <typename T> 
      struct are_same<T> : std::true_type 
      { };

    // Recursively apply are_same (is_same) to T and Args...
    // FIXME: Does && properly short-circuit the instantiation, or do I need to
    // use std::conditional to make sure thta it's done correctly. How do you
    // test this?
    template <typename T, typename... Args>
      struct are_same<T, Args...>
        : boolean_constant<
            std::is_same<T, typename front_type<Args...>::type>::value &&
            are_same<Args...>::value
          >
      { };
  } // namespace traits


  // Returns true if T is the same as U for all pairs of types T and U in
  // Args or if Args is an empty type sequence.
  template <typename... Args>
    constexpr bool Same()
    {
      return traits::are_same<Args...>::value;
    }
    

  // Returns true if T is different than U. This expression Different<T, U>()
  // is equivalent to !Same<T, U>().
  template <typename T, typename U>
    constexpr bool Different()
    {
      return !Same<T, U>();
    }


  // The common_type trait yields the common type of T and U. 
  //
  // NOTE: This supercedes the std implementation of common type, which will
  // can result in compilation failures when the common type of T and U is not
  // defined. Also note tht this is not in the traits namespace to allow for
  // easier customization.
  //
  // FIXME: This needs to be explicitly specialized for std::duration.
  template <typename T, typename U>
    struct common_type
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& a, Y&& b) -> decltype(true ? a : b);

      static subst_failure check(...);

      using C = decltype(check(std::declval<T>(), std::declval<U>()));
    public:
      using type = typename std::remove_const<
        typename std::remove_reference<C>::type
      >::type;
    };

  // When the type arguments are the same, we can bypass the deduction on
  // the conditional operator. This should make compilation slightly faster
  // in those cases.
  template <typename T>
    struct common_type<T, T>
    {
      using type = typename std::remove_const<
        typename std::remove_reference<T>::type
      >::type;
    };


  // An alias to the common type of T and U if it exists. Note that the common
  // type of T and U may be neither T nor U.
  template <typename T, typename U>
    using Common_type = typename common_type<T, U>::type;


  // Returns true if the common type of T and U exists.
  template <typename T, typename U>
    constexpr bool Common() { return Subst_succeeded<Common_type<T, U>>(); }

    

  // Returns true if T is convertible to U.
  //
  // FIXME: Convertibility is not particularly well-defined in the abstract
  // sense. It convers some forms of construction and casting, but does not
  // include the notion of assignment. Convertible should be analogous to
  // Copyable, in that the a value of type U can be constructed over or assigned
  // to a value of type T and the result of that operation should preserve the
  // value of the original.
  template <typename T, typename U>
    constexpr bool Convertible() { return std::is_convertible<T, U>::value; }


  // Returns true if T is derived from U.
  template <typename T, typename U>
    constexpr bool Derived() { return std::is_base_of<U, T>::value; }    

    
    
    
  //////////////////////////////////////////////////////////////////////////////   
  // Type queries and transformations
  //
  // The following type traits query a type for classification or property.
  // Type transformations return a type name.
  //////////////////////////////////////////////////////////////////////////////


  // Infrastructure for defining the identity alias.
  namespace traits
  {
    template <typename T>
      struct identity
      {
        using type = T;
      };
  };


  // An alias to the type argument. One use of the Identity alias is inhibit
  // perfect forwarding for rvalue parameters. For example:
  //
  //    template <typename T>
  //      void f(Identity<T>&& x) { ... }
  //
  //    A a = ... // some value
  //    f(a)
  //
  // The compiler will deduce T as the type A (resulting in an rvalue reference
  // to A) instead of forwarding the function argument x as A&.
  template <typename T>
    using Identity = typename traits::identity<T>::type;

    
  // Returns true if T is void, and false otherwise.
  template <typename T>
    constexpr bool Void() { return std::is_void<T>::value; }


  //////////////////////////////////////////////////////////////////////////////
  // Integer types
  //
  // The following traits apply to integer types.
  //  
  //
  // FIXME: There is an issue with the classification of bool as an integer type
  // in that it is neither signed nor unsigned. This can lead to compilation
  // failures when using the Make_unsigned or Make_signed type traits when bool
  // is accepted as an argument. A reasonable solution would be to abstract the
  // language's characterization of Integer types and exclude bool.
  //////////////////////////////////////////////////////////////////////////////


  // Returns true if T is a signed or unsigned, possibly cv-qualified, bool,
  // char, short, int, long or long long.
  template <typename T>
    constexpr bool Integer() { return std::is_integral<T>::value; }
  

  // Returns true if T is a signed type. Note that floating point values
  // are signed.
  template <typename T>
    constexpr bool Signed() { return std::is_signed<T>::value; }


  // Returns true if T is an unsigned type.
  template <typename T>
    constexpr bool Unsigned() { return std::is_unsigned<T>::value; }


  // An alias for the unsigned integral type with the same width as T.
  template <typename T>
    using Make_unsigned = typename std::make_unsigned<T>::type;


  // An alias for the signed integral type with the same width as T.
  template <typename T>
    using Make_signed = typename std::make_signed<T>::type;
    
  

  //////////////////////////////////////////////////////////////////////////////
  // Integer types
  //
  // The following traits apply to floating types.
  //////////////////////////////////////////////////////////////////////////////
    
  
  // Returns true if T is a float, double, or long double.
  template <typename T>
    constexpr bool Floating_point() { return std::is_floating_point<T>::value; }



  //////////////////////////////////////////////////////////////////////////////
  // Array types
  //
  // The following traits apply to array types.
  //////////////////////////////////////////////////////////////////////////////
  

  // Return true if T is an array type of type U[N].
  template <typename T>
    constexpr bool Array() { return std::is_array<T>::value; }


  // Returns the number of dimensions of the array type T. If T is not an
  // Array, it has 0 dimensions. For example:
  //
  //    using A1 = int[3][2];
  //    static_assert(Rank<A1>() == 2);
  //  
  //    using A2 = int[5];
  //    static_assert(Rank<A2>() == 1);
  //
  //    using A3 = int;
  //    static_assert(Rank<A3>() == 0);
  //
  template <typename T>
    constexpr unsigned Rank() { return std::rank<T>::value; }
    

  // Returns the extent of the array type T in the Ith dimension. The extent
  // of an array is the number of subobjects allocated in that dimensions. For
  // example:
  //
  //    using A = int[3][2];
  //    static_assert(Extent<A, 0> == 3, "");
  //    static_assert(Extent<A, 1> == 2, "");
  //
  // If the dimension I is not given, it defaults to 0.
  template <typename T, unsigned I = 0>
    constexpr std::size_t Extent() { return std::extent<T, I>::value; }
  

  // An alias to the array type T with its outermost extent removed. For
  // example:
  //
  //    using A = int[3][2];
  //    using B = Remove_extent<A>;
  //    static_assert(Same<B, int[3]>(), "");
  template <typename T>
    using Remove_extent = typename std::remove_extent<T>::type;
    
  // An alias to the underlying value type of the array type T. For example:
  //
  //    using A = int[3][2];
  //    using B = Remove_all_extents<A>;
  //    static_assert(Same<B, int>(), "");
  template <typename T>
    using Remove_all_extents = typename std::remove_all_extents<T>::type;



  //////////////////////////////////////////////////////////////////////////////
  // Pointer types
  //
  // The following traits apply to pointer types.
  //////////////////////////////////////////////////////////////////////////////
    
  
  // Returns true if T is a pointer.
  template <typename T>
    constexpr bool Pointer() { return std::is_pointer<T>::value; }
  

  // An alias to a pointer-to-T.
  template <typename T>
    using Add_pointer = typename std::add_pointer<T>::type;
    

  // An alias to the underlying type U if T is a pointer-to-U.
  template <typename T>
    using Remove_pointer = typename std::remove_pointer<T>::type;
    
  
  //////////////////////////////////////////////////////////////////////////////
  // Reference types
  //
  // The following traits apply to pointer types.
  //////////////////////////////////////////////////////////////////////////////
    

  // Returns true if T is an lvalue reference to some type U.
  template <typename T>
    constexpr bool Lvalue_reference() { return std::is_lvalue_reference<T>::value; }
    

  // Returns true if T is rvalue reference to some type U.
  template <typename T>
    constexpr bool Rvalue_reference() { return std::is_rvalue_reference<T>::value; }
    

  // Returns true if T is either an lvalue or rvalue reference to some type U.
  template <typename T>
    constexpr bool Reference() { return std::is_reference<T>::value; }
    

  // An alias to an lvalue-reference-to-T.
  template <typename T>
    using Add_lvalue_reference = typename std::add_lvalue_reference<T>::type;
    

  // An alias to an rvalue-reference-to-T.
  template <typename T>
    using Add_rvalue_reference = typename std::add_rvalue_reference<T>::type;
    

  // An alias to a type U if T is a reference-to-T.
  template <typename T>
    using Remove_reference = typename std::remove_reference<T>::type;

    

  // Member data

  // Returns true if T is a pointer to a member variable.
  template <typename T>
    constexpr bool Member_object_pointer()
    {
      return std::is_member_object_pointer<T>::value;
    }

  // Returns true if T is a member function pointer.
  template <typename T>
    constexpr bool Member_function_pointer()
    {
      return std::is_member_function_pointer<T>::value;
    }



  // Infrastructure for accessing a member pointer's pointee and class types.
  // FIXME: Extend these traits with accessors for this pointers of member
  // functions and (perhaps), their argument tuples.
  namespace traits
  {
    template <typename Memptr>
      struct member_pointer_traits
      {
        using result_type = subst_failure;
        using class_type = subst_failure;
      };

    template <typename T, typename C>
      struct member_pointer_traits<T(C::*)>
      {
        using result_type = T;
        using class_type = C;
      };

    template <typename T, typename C, typename... Args>
      struct member_pointer_traits<T(C::*)(Args...)>
      {
        using result_type = T;
        using class_type = C;
      };

    template <typename T, typename C, typename... Args>
      struct member_pointer_traits<T(C::*)(Args...) const>
      {
        using result_type = T;
        using class_type = C;
      };

    template <typename T, typename C, typename... Args>
      struct member_pointer_traits<T(C::*)(Args...) volatile>
      {
        using result_type = T;
        using class_type = C;
      };

    template <typename T, typename C, typename... Args>
      struct member_pointer_traits<T(C::*)(Args...) const volatile>
      {
        using result_type = T;
        using class_type = C;
      };
  } // namespace traits



  // Member pointee type (alias)
  // An alias to a member pointer's pointee type.
  template <typename Ptr>
    using Member_result_type = 
      typename traits::member_pointer_traits<Ptr>::result_type;



  // Member class type (alias)
  // An alias to a member pointer's class type.
  template <typename Ptr>
    using Member_class_type = 
      typename traits::member_pointer_traits<Ptr>::class_type;



  // Composite ctaegories
    
  // Returns true if T is an object type. References and function types are
  // not object types.
  template <typename T>
    constexpr bool Object() { return std::is_object<T>::value; }
    
  // Returns true if T is a scalar type (i.e,. not compound).
  template <typename T>
    constexpr bool Scalar() { return std::is_scalar<T>::value; }
    
  // Returns true if T is a compound type (i.e., is comprised of 1 or more 
  // sub-objects).
  template <typename T>
    constexpr bool Compound() { return std::is_compound<T>::value; }
    
  // Returns true if T is a built-in type.
  template <typename T>
    constexpr bool Fundamental() { return std::is_fundamental<T>::value; }
    
  // Returns true if T is an arithmetic type (Integral() || Floating_point()).
  template <typename T>
    constexpr bool Arithmetic() { return std::is_arithmetic<T>::value; }
    

    
  // Qualifiers
  
  // Returns true if T is const-qualified
  template <typename T>
    constexpr bool Const() { return std::is_const<T>::value; }
    
  // Returns true if T is volatile-qualified
  template <typename T>
    constexpr bool Volatile() { return std::is_volatile<T>::value; }
    
  // Returns true if T is cv-qualified.
  template <typename T>
    constexpr bool Qualified() { return Const<T>() || Volatile<T>(); }

  template <typename T>
    using Add_const = typename std::add_const<T>::type;
    
  template <typename T>
    using Add_volatile = typename std::add_volatile<T>::type;
    
  template <typename T>
    using Add_cv = typename std::add_cv<T>::type;
    
  template <typename T>
    using Remove_const = typename std::remove_const<T>::type;
    
  template <typename T>
    using Remove_volatile = typename std::remove_volatile<T>::type;
    
  template <typename T>
    using Remove_cv = typename std::remove_cv<T>::type;
    
    

  // Remove all qualifiers and reference types and decay arrays and functions
  // to pointers and function pointers.
  template <typename T>
    using Decay = typename std::decay<T>::type;



  // Aligned storage
  template <std::size_t Size, std::size_t Align>
    using Aligned_storage = typename std::aligned_storage<Size, Align>::type;


  // Return an unqualified a type name by removing any references and 
  // CV-qualifiers. This is helpful when concept checking template arguments 
  // of types that are known to be non-value (i.e., qualified) types, 
  // especially overloads that rely on rvalue references for forwarding.
  template <typename T>
    using Unqualified = Remove_cv<Remove_reference<T>>;
    
  
    
  // The Forwarded trait returns the value type of a forwarded template
  // argument. This must be used to remove qualifiers from template arguments
  // when the function argument is forwarded.
  template <typename T>
    using Forwarded = Unqualified<T>;
    

  // Layout and initialization

  // Returns true if T is a standard layout type.
  template <typename T>
    constexpr bool Standard_layout() { return std::is_standard_layout<T>::value; }
    
  // Returns true if T is a trivially copyable type.
//   template <typename T>
//     constexpr bool Trivially_copyable() { return std::is_trivially_copyable<T>::value; }
    
    
  // Returns true if T is a trivial type.
  template <typename T>
    constexpr bool Trivial() { return std::is_trivial<T>::value; }
    
  // Returns true if T is a plain old data type.
  template <typename T>
    constexpr bool Pod() { return std::is_pod<T>::value; }
    
  // Returns true if an array of T eleemnts can be compared using the memcmp
  // algorithm. This is the case when T has a standard layout and is not
  // aligned to a larger address boundary.
  template <typename T>
    constexpr bool Memory_comparable()
    {
      return Standard_layout<T>() && sizeof(T) == alignof(T);
    }
    
    
    
  // Classes and unions
    
  // Return true if T is a class or struct.
  template <typename T>
    constexpr bool Class() { return std::is_class<T>::value; };
    
  // Return true if T is a union.
  template <typename T>
    constexpr bool Union() { return std::is_union<T>::value; }
    
  // Return true if T has no member variables.
  template <typename T>
    constexpr bool Empty() { return std::is_empty<T>::value; }
    
  // Return true if T has at least one virtual method.
  template <typename T>
    constexpr bool Polymorphic() { return std::is_polymorphic<T>::value; }
    
  // Return true if T has at least one pure virtual method.
  template <typename T>
    constexpr bool Abstract() { return std::is_abstract<T>::value; }
 

  // Class destruction and construction.
  
  // Return true if T is destructible.
  template <typename T>
    constexpr bool Destructible() { return std::is_destructible<T>::value; }
    
//   template <typename T>
//     constexpr bool Trivially_destructible()
//     {
//       return std::is_trivially_destructible<T>::value;
//     }
    
//   template <typename T>
//     constexpr bool Nothrow_destructible()
//     {
//       return std::is_nothrow_destructible<T>::value;
//     }
    
  // Return true if T has a virtual destructor
  template <typename T>
    constexpr bool Has_virtual_destructor() 
    { 
      return std::has_virtual_destructor<T>::value; 
    }
    

  // Return trie if T can be constructed over Args...
  template <typename T, typename... Args>
    constexpr bool Constructible() 
    { 
      return std::is_constructible<T, Args...>::value; 
    }
    
//   template <typename T, typename... Args>
//     constexpr bool Trivially_constructible()
//     {
//       return std::is_trivially_constructible<T, Args...>::value;
//     }
    
  // Return true if T can be constructed over Args... and does not throw
  // an exception.
  template <typename T, typename... Args>
    constexpr bool Nothrow_constructible() 
    { 
      return std::is_nothrow_constructible<T, Args...>::vlaue; 
    }
    
    
  // Return true if T has a default constructor.
  template <typename T>
    constexpr bool Default_constructible()
    {
      return std::is_default_constructible<T>::value;
    }
    
//   template <typename T>
//     constexpr bool Trivially_default_constructible()
//     {
//       return std::is_trivially_default_constructible<T>::value;
//     }
    
  template <typename T>
    constexpr bool Nothrow_default_constructible()
    {
      return std::is_nothrow_default_constructible<T>::value;
    }
    
    
  template <typename T>
    constexpr bool Copy_constructible()
    {
      return std::is_copy_constructible<T>::value;
    }
    
//   template <typename T>
//     constexpr bool Trivially_copy_constructible()
//     {
//       return std::is_trivially_copy_constructible<T>::value;
//     }

  template <typename T>
    constexpr bool Nothrow_copy_constructible()
    {
      return std::is_nothrow_copy_constructible<T>::value;
    }

    
  template <typename T>
    constexpr bool Move_constructible()
    {
      return std::is_move_constructible<T>::value;
    }

//   template <typename T>
//     constexpr bool Trivially_move_constructible()
//     {
//       return std::is_trivially_move_constructible<T>::value;
//     }

  template <typename T>
    constexpr bool Nothrow_move_constructible()
    {
      return std::is_nothrow_move_constructible<T>::value;
    }
    
    
  // Assignment
  
  template <typename T, typename U>
    constexpr bool Assignable()
    {
      return std::is_assignable<T, U>::value;
    }
    
//   template <typename T, typename U>
//     constexpr bool Trivially_assignable()
//     {
//       return std::is_trivially_assignable<T>::value;
//     }
    
  template <typename T, typename U>
    constexpr bool Nothrow_move_assignable()
    {
      return std::is_nothrow_move_assignable<T>::value;
    }
    
    
  template <typename T>
    constexpr bool Copy_assignable()
    {
      return std::is_copy_assignable<T>::value;
    }
    
//   template <typename T>
//     constexpr bool Trivially_copy_assignable()
//     {
//       return std::is_trivially_copy_assignable<T>::value;
//     }
    
  template <typename T>
    constexpr bool Nothrow_copy_assignable()
    {
      return std::is_nothrow_copy_assignable<T>::value;
    }

    
  template <typename T>
    constexpr bool Move_assignable()
    {
      return std::is_move_assignable<T>::value;
    }
    
//   template <typename T>
//     constexpr bool Trivially_move_assignable()
//     {
//       return std::is_trivially_move_assignable<T>::value;
//     }
    
  template <typename T>
    constexpr bool Nothrow_move_assignable()
    {
      return std::is_nothrow_move_assignable<T>::value;
    }
    
    
  // Enums
  
  template <typename T>
    constexpr bool Enum()
    {
      return std::is_enum<T>::value;
    }
    
  template <typename T>
    using Underlying_type = typename std::underlying_type<T>::type;
 
    
  // Functions
    
  // Returns true if T is a function.
  //
  // Note that the name of this trait differs from the standard to avoid 
  // collision with the Function concept.
  template <typename T>
    constexpr bool Function_type()
    {
      return std::is_function<T>();
    }
    
    

  // Relational operator traits
  
  // Safely deduce the result type of the expression t == u.
  template <typename T, typename U>
    struct equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x == y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t == u.
  template <typename T, typename U = T>
    using Equal_result = typename equal_result<T, U>::type;
    
  // Return true if the expression t == u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_equal()
    {
      return Subst_succeeded<Equal_result<T, U>>();
    }
    

  // Safely deduce the result type of the expression t != u.
  template <typename T, typename U>
    struct not_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y const&& y) -> decltype(x != y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t != u.
  template <typename T, typename U = T>
    using Not_equal_result = typename not_equal_result<T, U>::type;
    
  // Return true if the expression t != u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_not_equal()
    {
      return Subst_succeeded<Not_equal_result<T, U>>();
    }

    
  // Safely deduce the result type of the expression t < u.
  template <typename T, typename U>
    struct less_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x < y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t < u.
  template <typename T, typename U = T>
    using Less_result = typename less_result<T, U>::type;
    
  // Return true if the expression t < u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_less()
    {
      return Subst_succeeded<Less_result<T, U>>();
    }


  // Safely deduce the result type of the expression t > u.
  template <typename T, typename U>
    struct greater_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x > y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t > u.
  template <typename T, typename U = T>
    using Greater_result = typename greater_result<T, U>::type;
    
  // Return true if the expression t > u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_greater()
    {
      return Subst_succeeded<Greater_result<T, U>>();
    }

    
  // Safely deduce the result type of the expression t <= u.
  template <typename T, typename U>
    struct less_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x <= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t <= u.
  template <typename T, typename U = T>
    using Less_equal_result = typename less_equal_result<T, U>::type;
    
  // Return true if the expression t <= u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_less_equal()
    {
      return Subst_succeeded<Less_equal_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression t >= u.
  template <typename T, typename U>
    struct greater_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x >= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t >= u.
  template <typename T, typename U = T>
    using Greater_equal_result = typename greater_equal_result<T, U>::type;
    
  // Return true if the expression t >= u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_greater_equal()
    {
      return Subst_succeeded<Greater_equal_result<T, U>>();
    }    
    
    
  // Arithmetic operator traits

  // Safely deduce the result type of the expression t + u.
  template <typename T, typename U>
    struct plus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x + y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t + u.
  template <typename T, typename U = T>
    using Plus_result = typename plus_result<T, U>::type;
    
  // Return true if the expression t + u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_plus()
    {
      return Subst_succeeded<Plus_result<T, U>>();
    }

    
  // Safely deduce the result type of the expression t - u.
  template <typename T, typename U>
    struct minus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x - y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t - u.
  template <typename T, typename U = T>
    using Minus_result = typename minus_result<T, U>::type;
    
  // Return true if the expression t - u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_minus()
    {
      return Subst_succeeded<Minus_result<T, U>>();
    }
  

  // Safely deduce the result type of the expression t * u.
  template <typename T, typename U>
    struct multiplies_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x * y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t * u.
  template <typename T, typename U = T>
    using Multiplies_result = typename multiplies_result<T, U>::type;
    
  // Return true if the expression t * u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_multiplies()
    {
      return Subst_succeeded<Multiplies_result<T, U>>();
    }

    
  // Safely deduce the result type of the expression t / u.
  template <typename T, typename U>
    struct divides_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x / y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t / u.
  template <typename T, typename U = T>
    using Divides_result = typename divides_result<T, U>::type;
    
  // Return true if the expression t / u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_divides()
    {
      return Subst_succeeded<Divides_result<T, U>>();
    }

    
  // Safely deduce the result type of the expression t % u.
  template <typename T, typename U>
    struct modulus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x % y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t % u.
  template <typename T, typename U = T>
    using Modulus_result = typename modulus_result<T, U>::type;
    
  // Return true if the expression t % u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_modulus()
    {
      return Subst_succeeded<Modulus_result<T, U>>();
    }
    

  // Safely deduce the result of +t.
  template <typename T>
    struct unary_plus_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(+a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias for the result of +t.
  template <typename T>
    using Unary_plus_result = typename unary_plus_result<T>::type;
    
  // Return true if the expression +t is valid.
  template <typename T>
    constexpr bool Has_unary_plus()
    {
      return Subst_succeeded<Unary_plus_result<T>>();
    }

  
  // Safely deduce the result of -t.
  template <typename T>
    struct unary_minus_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(-a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias for the result of -t.
  template <typename T>
    using Unary_minus_result = typename unary_minus_result<T>::type;
    
  // Return true if the expression -t is valid.
  template <typename T>
    constexpr bool Has_unary_minus()
    {
      return Subst_succeeded<Unary_plus_result<T>>();
    }
    
    
  
  // Infrastructure for deducing the results of pre- and post- increment and
  // decrement.
  namespace traits
  {
    // Safely deduce the result of ++t;
    template <typename T>
      struct pre_increment_result
      {
      private:
        template <typename X>
          static auto check(X& x) -> decltype(++x);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T&>()));
      };

    // Safely deduce the result of t++;
    template <typename T>
      struct post_increment_result
      {
      private:
        template <typename X>
          static auto check(X& x) -> decltype(x++);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T&>()));
      };

    // Safely deduce the result of --t;
    template <typename T>
      struct pre_decrement_result
      {
      private:
        template <typename X>
          static auto check(X& x) -> decltype(--x);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T&>()));
      };
      
    
    // Safely deduce the result of t--;
    template <typename T>
      struct post_decrement_result
      {
      private:
        template <typename X>
          static auto check(X& x) -> decltype(x--);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T&>()));
      };
  } // namespace traits


    
  // Post-increment result (alias)
  // An alias for the result of ++t or subst_failure if the expression is
  // not valid.
  template <typename T>
    using Pre_increment_result = typename traits::pre_increment_result<T>::type;



  // Has pre-increment (trait)
  // Returns true if the expression ++t is valid.
  template <typename T>
    constexpr bool Has_pre_increment()
    {
      return Subst_succeeded<Pre_increment_result<T>>();
    }



  // Post-increment result (alias)
  // An alias for the result of t++ or subt_failure if the expression is
  // not valid.
  template <typename T>
    using Post_increment_result = typename traits::post_increment_result<T>::type;

  

  // Has post-increment (trait)
  // Returns true if the expostssion t++ is valid.
  template <typename T>
    constexpr bool Has_post_increment()
    {
      return Subst_succeeded<Post_increment_result<T>>();
    }

    
    
  // Pre-decrement result (alias)
  // An alias for the result of --t or subst_failure if the expression is
  // invalid.
  template <typename T>
    using Pre_decrement_result = typename traits::pre_decrement_result<T>::type;



  // Has pre-decrement (trait)
  // Returns true if the expression --t is valid.
  template <typename T>
    constexpr bool Has_pre_decrement()
    {
      return Subst_succeeded<Pre_decrement_result<T>>();
    }



  // Post-decrement result
  // An alias for the result of t-- or subst_failure if the expression is
  // not valid.
  template <typename T>
    using Post_decrement_result = typename traits::post_decrement_result<T>::type;



  // Has post-decrement
  // Returns true if the expostssion t-- is valid.
  template <typename T>
    constexpr bool Has_post_decrement()
    {
      return Subst_succeeded<Post_decrement_result<T>>();
    }
    
    
    
  // Logical operators
  
  // Safely deduce the result type of the expression t && u.
  template <typename T, typename U>
    struct and_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x && y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t && u.
  template <typename T, typename U = T>
    using And_result = typename and_result<T, U>::type;
    
  // Return true if the expression t && u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_and()
    {
      return Subst_succeeded<And_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression t || u.
  template <typename T, typename U>
    struct or_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x || y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t || u.
  template <typename T, typename U = T>
    using Or_result = typename or_result<T, U>::type;
    
  // Return true if the expression t || u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_or()
    {
      return Subst_succeeded<Or_result<T, U>>();
    }
    
    
   // Safely deduce the result of !t.
  template <typename T>
    struct not_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(!a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias for the result of !t.
  template <typename T>
    using Not_result = typename not_result<T>::type;
    
  // Return true if the expression !t is valid.
  template <typename T>
    constexpr bool Has_not()
    {
      return Subst_succeeded<Not_result<T>>();
    }   

    
  // Binary operations
  
  // Safely deduce the result type of the expression t & u.
  template <typename T, typename U>
    struct bit_and_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x & y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for the result of t & u.
  template <typename T, typename U = T>
    using Bit_and_result = typename bit_and_result<T, U>::type;
    
  // Return true if the expression t & u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_bit_and()
    {
      return Subst_succeeded<Bit_and_result<T, U>>();
    }
    
  // Safely deduce the result type of the expression t | u.
  template <typename T, typename U>
    struct bit_or_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x | y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias for bit_or the result of t | u.
  template <typename T, typename U = T>
    using Bit_or_result = typename bit_or_result<T, U>::type;
    
  // Return true if the expression t | u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_bit_or()
    {
      return Subst_succeeded<Bit_or_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression t ^ u.
  template <typename T, typename U>
    struct bit_xor_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x ^ y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias fbit_xor the result of t ^ u.
  template <typename T, typename U = T>
    using Bit_xor_result = typename bit_xor_result<T, U>::type;
    
  // Return true if the expression t ^ u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_bit_xor()
    {
      return Subst_succeeded<Bit_xor_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression t << u.
  template <typename T, typename U>
    struct left_shift_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x << y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias fleft_shift the result of t << u.
  template <typename T, typename U = T>
    using Left_shift_result = typename left_shift_result<T, U>::type;
    
  // Return true if the expression t << u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_left_shift()
    {
      return Subst_succeeded<Left_shift_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression t >> u.
  template <typename T, typename U>
    struct right_shift_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x >> y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias fright_shift the result of t >> u.
  template <typename T, typename U = T>
    using Right_shift_result = typename right_shift_result<T, U>::type;
    
  // Return true if the expression t >> u is valid.
  template <typename T, typename U = T>
    constexpr bool Has_right_shift()
    {
      return Subst_succeeded<Right_shift_result<T, U>>();
    }
    
    
   // Safely deduce the result of ~t.
  template <typename T>
    struct complement_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(~a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias for the result of ~t.
  template <typename T>
    using Complement_result = typename complement_result<T>::type;
    
  // Return true if the expression ~t is valid.
  template <typename T>
    constexpr bool Has_complement()
    {
      return Subst_succeeded<Complement_result<T>>();
    }   
  

  // Address, dereference, subscript, and function call

   // Safely deduce the result of &t.
  template <typename T>
    struct address_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(&a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias for the result of &t.
  template <typename T>
    using Address_result = typename address_result<T>::type;
    
  // Return true if the expression &t is valid.
  template <typename T>
    constexpr bool Has_address()
    {
      return Subst_succeeded<Address_result<T>>();
    }   
    
    

  // Infrastructure for safely deducing the result of *t.
  namespace traits
  {
    template <typename T>
      struct dereference_result
      {
      private:
        template <typename X>
          static auto check(X&& a) -> decltype(*a);
        
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  } // namespace traits

  

  // Dereference result (alias)
  // An alias for the result of *t.
  template <typename T>
    using Dereference_result = typename traits::dereference_result<T>::type;
    


  // Dereference (trait)
  // Return true if the expression *t is valid.
  template <typename T>
    constexpr bool Has_dereference()
    {
      return Subst_succeeded<Dereference_result<T>>();
    }
    


  // Safely deduce the result type of the expression t[u].
  template <typename T, typename U>
    struct subscript_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x[y]);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // An alias fsubscript the result of t[u].
  template <typename T, typename U>
    using Subscript_result = typename subscript_result<T, U>::type;
    
  // Return true if the expression t[u] is valid.
  template <typename T, typename U>
    constexpr bool Has_subscript()
    {
      return Subst_succeeded<Subscript_result<T, U>>();
    }
    
    
  // Safely deduce the result type of the expression f(args...).
  //
  // NOTE: This is not simply the same as result_of. That can fail noisily,
  // whereas this does not.
  template <typename F, typename... Args>
    struct call_result
    {
    private:
      template <typename X, typename... Y>
        static auto check(X&& x, Y&&... y) -> decltype(x(std::forward<Y>(y)...));
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<F>(), std::declval<Args>()...));
    };

  // Specialization when F is known to be void.
  template <typename... Args>
    struct call_result<void, Args...>
    {
      using type = void;
    };

  // An alias fcall the result of t >> u.
  template <typename F, typename... Args>
    using Call_result = typename call_result<F, Args...>::type;
    
  // Return true if the expression t >> u is valid.
  template <typename F, typename... Args>
    constexpr bool Has_call()
    {
      return Subst_succeeded<Call_result<F, Args...>>();
    }

    
  
  // A helper class for computing the result type of a function.
  template <typename X>
    struct result_of_impl
    {
      using type = subst_failure;
    };
    
  template <typename F, typename... Args>
    struct result_of_impl<F(Args...)>
    {
      using type = Call_result<F, Args...>;
    };
    
    
  
  // Result_of (type trait)
  // An alias for the result of the function type F. Note that F must have
  // the form G(Args...) where G is a callable type and Args... is a sequence
  // of arguments.
  template <typename F>
    using Result_of = typename result_of_impl<F>::type;



  // Infrastracture for deducing the argument types from a function type (e.g.,
  // F(Args...) or an adapted function object (deriving from a function
  // adaptor.)
  //
  // TODO: Extend this trait so that we can deduce argument types from a
  // class that declares them as associatd type (i.e., first_argument_type,
  // second_argument_type, etc.).
  namespace traits
  {
    template <typename Sig>
      struct get_argument_types
      {
        using type = subst_failure;
      };

    template <typename Result, typename... Args>
      struct get_argument_types<Result(Args...)>
      {
        using type = std::tuple<Args...>;
      };
  } // namespace traits



  // Argument types (alias)
  // Return the function argument types as a type list.
  template <typename Sig>
    using Argument_types = typename traits::get_argument_types<Sig>::type;



  // Compound assignment
    
  // Safely deduce the result type of the expression t += u.
  template <typename T, typename U>
    struct plus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x += y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fplus_assign the result of t += u.
  template <typename T, typename U>
    using Plus_assign_result = typename plus_assign_result<T, U>::type;
    
  // Return true if the expression t += u is valid.
  template <typename T, typename U>
    constexpr bool Has_plus_assign()
    {
      return Subst_succeeded<Plus_assign_result<T, U>>();
    }    

    
  // Safely deduce the result type of the expression t -= u.
  template <typename T, typename U>
    struct minus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x -= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fminus_assign the result of t -= u.
  template <typename T, typename U>
    using Minus_assign_result = typename minus_assign_result<T, U>::type;
    
  // Return true if the expression t -= u is valid.
  template <typename T, typename U>
    constexpr bool Has_minus_assign()
    {
      return Subst_succeeded<Minus_assign_result<T, U>>();
    }    
    
    
  // Safely deduce the result type of the expression t *= u.
  template <typename T, typename U>
    struct multiplies_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x *= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fmultiplies_assign the result of t *= u.
  template <typename T, typename U>
    using Multiplies_assign_result = typename multiplies_assign_result<T, U>::type;
    
  // Return true if the expression t *= u is valid.
  template <typename T, typename U>
    constexpr bool Has_multiplies_assign()
    {
      return Subst_succeeded<Multiplies_assign_result<T, U>>();
    }    
    
    
  // Safely deduce the result type of the expression t /= u.
  template <typename T, typename U>
    struct divides_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x /= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fdivides_assign the result of t /= u.
  template <typename T, typename U>
    using Divides_assign_result = typename divides_assign_result<T, U>::type;
    
  // Return true if the expression t /= u is valid.
  template <typename T, typename U>
    constexpr bool Has_divides_assign()
    {
      return Subst_succeeded<Divides_assign_result<T, U>>();
    }    
    
    
  // Safely deduce the result type of the expression t %= u.
  template <typename T, typename U>
    struct modulus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x %= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fmodulus_assign the result of t %= u.
  template <typename T, typename U>
    using Modulus_assign_result = typename modulus_assign_result<T, U>::type;
    
  // Return true if the expression t %= u is valid.
  template <typename T, typename U>
    constexpr bool Has_modulus_assign()
    {
      return Subst_succeeded<Modulus_assign_result<T, U>>();
    }        
    

  // Safely deduce the result type of the expression t &= u.
  template <typename T, typename U>
    struct bit_and_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x &= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fbit_and_assign the result of t &= u.
  template <typename T, typename U>
    using Bit_and_assign_result = typename bit_and_assign_result<T, U>::type;
    
  // Return true if the expression t &= u is valid.
  template <typename T, typename U>
    constexpr bool Has_bit_and_assign()
    {
      return Subst_succeeded<Bit_and_assign_result<T, U>>();
    }        

    
  // Safely deduce the result type of the expression t |= u.
  template <typename T, typename U>
    struct bit_or_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x |= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fbit_or_assign the result of t |= u.
  template <typename T, typename U>
    using Bit_or_assign_result = typename bit_or_assign_result<T, U>::type;
    
  // Return true if the expression t |= u is valid.
  template <typename T, typename U>
    constexpr bool Has_bit_or_assign()
    {
      return Subst_succeeded<Bit_or_assign_result<T, U>>();
    }        

    
  // Safely deduce the result type of the expression t ^= u.
  template <typename T, typename U>
    struct bit_xor_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x ^= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fbit_xor_assign the result of t ^= u.
  template <typename T, typename U>
    using Bit_xor_assign_result = typename bit_xor_assign_result<T, U>::type;
    
  // Return true if the expression t ^= u is valid.
  template <typename T, typename U>
    constexpr bool Has_bit_xor_assign()
    {
      return Subst_succeeded<Bit_xor_assign_result<T, U>>();
    }        

    
    
  // Safely deduce the result type of the expression t <<= u.
  template <typename T, typename U>
    struct left_shift_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x <<= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fleft_shift_assign the result of t <<= u.
  template <typename T, typename U>
    using Left_shift_assign_result = typename left_shift_assign_result<T, U>::type;
    
  // Return true if the expression t <<= u is valid.
  template <typename T, typename U>
    constexpr bool Has_left_shift_assign()
    {
      return Subst_succeeded<Left_shift_assign_result<T, U>>();
    }        

    
    
  // Safely deduce the result type of the expression t >>= u.
  template <typename T, typename U>
    struct right_shift_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x >>= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // An alias fright_shift_assign the result of t >>= u.
  template <typename T, typename U>
    using Right_shift_assign_result = typename right_shift_assign_result<T, U>::type;
    
  // Return true if the expression t >>= u is valid.
  template <typename T, typename U>
    constexpr bool Has_right_shift_assign()
    {
      return Subst_succeeded<Right_shift_assign_result<T, U>>();
    }        



  // Common member traits
  // There are number of common operations foundin the STL and beyond. Members
  // like x.size(), x.empty(), x.find(), and x.swap() typically optimize
  // algorithms or properties.



  // Associated value type
  // A type T has an associated value type if T::value_type is a valid 
  // expression. 

  // Infrastructure for accessing an associated size type.
  namespace traits
  {
    // Safely deduce the type expression T::size_type.
    template <typename T>
      struct get_associated_value_type
      {
      private:
        template <typename X>
          static typename X::value_type check(const X&);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  } // namespace traits



  // Associated value type (alias)
  // An alias to the associated value type.
  template <typename T>
    using Associated_value_type = 
      typename traits::get_associated_value_type<T>::type;



  // Associated value type (trait)
  // Returns true if T has an associated value type.
  template <typename T>
    constexpr bool Has_associated_value_type()
    {
      return Subst_succeeded<Associated_value_type<T>>();
    }



  // Associated difference type
  // A type T has an associated difference type if T::difference_type is a
  // valid expression. Note that the name "difference" is a hold-over from the
  // standard library. The abstraction more closely associated with this
  // associated type is "distance".

  // Infrastructure for accessing an associated size type.
  namespace traits
  {
    // Safely deduce the type expression T::size_type.
    template <typename T>
      struct get_associated_difference_type
      {
      private:
        template <typename X>
          static typename X::difference_type check(const X&);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  } // namespace traits



  // Associated difference type (alias)
  // An alias to the associated difference type.
  template <typename T>
    using Associated_difference_type = 
      typename traits::get_associated_difference_type<T>::type;



  // Associated difference type (trait)
  // Returns true if T has an associated difference type.
  template <typename T>
    constexpr bool Has_associated_difference_type()
    {
      return Subst_succeeded<Associated_difference_type<T>>();
    }



  // Associated size type
  // A type T has an associated size type if T::size_type is a valid 
  // expression.

  // Infrastructure for accessing an associated size type.
  namespace traits
  {
    // Safely deduce the type expression T::size_type.
    template <typename T>
      struct get_associated_size_type
      {
      private:
        template <typename X>
          static typename X::size_type check(const X&);
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  } // namespace traits

  

  // Associated size type (alias)
  // An alias to the associated size type or subst failure if the expression
  // T::size_type is invalid.
  template <typename T>
    using Associated_size_type = typename traits::get_associated_size_type<T>::type;

  

  // Associated size type (trait)
  // Returns true if T has an associated size type.
  template <typename T>
    constexpr bool Has_associated_size_type()
    {
      return Subst_succeeded<Associated_size_type<T>>();
    }



  // Member size
  // These traits determine if x.size() is a valid expression. Classes 
  // implementing this member typically cache their size rather than compute
  // it (e.g., Containers).

  // Infrastructure for deducing an associated size type.
  namespace traits
  {
    // Safely get the type returned by the member function x.size().
    template <typename T>
      struct member_size_result
      {
      private:
        template <typename X>
          static auto check(X const& x) -> decltype(x.size());
        static subst_failure check(...);
      public:
        using type = decltype(check(std::declval<T>()));
      };
  } // namespace traits
    
  // An alias to the result of the expression empty(x).
  template <typename T>
    using Member_size_result = typename traits::member_size_result<T>::type;
    
  // Return true if empty(t) is a valid expression.
  template <typename T>
    bool constexpr Has_member_size()
    {
      return Subst_succeeded<Member_size_result<T>>();
    }



  // Member empty
  // These traits determine if x.empty() is a valid expression. Usually if
  // x.size() is a valid expression, x.empty() will also be a valid expression.
  // This is not always the case (e.g., counted_range).

  // Safely get the type returned by the member function x.empty().
  template <typename T>
    struct member_empty_result
    {
    private:
      template <typename X>
        static auto check(X const& x) -> decltype(x.empty());
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // An alias to the result of the expression empty(x).
  template <typename T>
    using Member_empty_result = typename member_empty_result<T>::type;
    
  // Return true if empty(t) is a valid expression.
  template <typename T>
    bool constexpr Has_member_empty()
    {
      return Subst_succeeded<Member_empty_result<T>>();
    }
  


  // Member find
  // These traits determine if the type supports finding a value based on a
  // key. Typically, containers implementing a member find() will implement
  // more efficient search techniques than the usual linear search.
  
  // Safely get the type associated with the result of t.find(x).
  template <typename C, typename K>
    struct member_find_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x.find(y));
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<C>(), std::declval<K>()));
    };
    
  // An alias to the result of the expression empty(x).
  template <typename C, typename K>
    using Member_find_result = typename member_find_result<C, K>::type;
    
  // Return true if empty(t) is a valid expression.
  template <typename C, typename K>
    bool constexpr Has_member_find()
    {
      return Subst_succeeded<Member_find_result <C, K>>();
    }


} // namespace origin

#endif
