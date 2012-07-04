// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_TYPE_IMPL_SFINAE_HPP
#define ORIGIN_TYPE_IMPL_SFINAE_HPP

#ifndef ORIGIN_TYPE_TRAITS_HPP
#  error This file cannot be included directly. Include type/traits.hpp.
#endif


namespace type_impl
{
  ////////////////////////////////////////////////////////////////////////////
  // Functions

  // Safely deduce the result of the expression f(args...).
  template <typename F, typename... Args>
    struct get_call_result
    {
    private:
      template <typename X, typename... Y>
        static auto check(X x, Y&&... y) -> decltype(x(std::forward<Y>(y)...));
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<F>(), std::declval<Args>()...));
    };

  // Actually access the result type.
  template <typename T>
    struct result_of
    {
      using type = subst_failure;
    };
    
  template <typename F, typename... Args>
    struct result_of<F(Args...)>
    {
      using type = typename get_call_result<F, Args...>::type;
    };



  // Deduce the argument types from a function type.
  //
  // TODO: Extend this trait so that we can deduce argument types from a
  // class that declares them as associatd type (i.e., first_argument_type,
  // second_argument_type, etc.).
  template <typename F>
    struct get_argument_types
    {
      using type = subst_failure;
    };

  template <typename R, typename... Args>
    struct get_argument_types<R(Args...)>
    {
      using type = std::tuple<Args...>;
    };



  ////////////////////////////////////////////////////////////////////////////
  // Member pointers

  // Type traits for pointers-to-member data and funtions. This class is
  // specialized for different kinds of member pointers. Note that the class
  // type is a value type; it is not cv-qualified as if it were a this 
  // pointer.
  //
  // TODO: Include "this" type as cv-qualified?
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



  ////////////////////////////////////////////////////////////////////////////
  // Overloadable operators

  // Deduce the result of the expression t == u.
  template <typename T, typename U>
    struct get_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x == y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t != u.
  template <typename T, typename U>
    struct get_not_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x != y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t < u.
  template <typename T, typename U>
    struct get_less_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x < y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t > u.
  template <typename T, typename U>
    struct get_greater_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x > y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t <= u.
  template <typename T, typename U>
    struct get_less_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x <= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t >= u.
  template <typename T, typename U>
    struct get_greater_equal_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x >= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t + u.
  template <typename T, typename U>
    struct get_plus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x + y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t - u.
  template <typename T, typename U>
    struct get_minus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x - y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t * u.
  template <typename T, typename U>
    struct get_multiplies_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x * y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t / u.
  template <typename T, typename U>
    struct get_divides_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x / y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };
  
  // Deduce the result type of the expression t % u.
  template <typename T, typename U>
    struct get_modulus_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x % y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result of +t.
  template <typename T>
    struct get_unary_plus_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(+a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result of -t.
  template <typename T>
    struct get_unary_minus_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(-a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result type of the expression t += u.
  template <typename T, typename U>
    struct get_plus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x += y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t -= u.
  template <typename T, typename U>
    struct get_minus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x -= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t *= u.
  template <typename T, typename U>
    struct get_multiplies_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x *= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t /= u.
  template <typename T, typename U>
    struct get_divides_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x /= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t %= u.
  template <typename T, typename U>
    struct get_modulus_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x %= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result of ++t;
  template <typename T>
    struct get_pre_increment_result
    {
    private:
      template <typename X>
        static auto check(X& x) -> decltype(++x);
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>()));
    };

  // Deduce the result of t++;
  template <typename T>
    struct get_post_increment_result
    {
    private:
      template <typename X>
        static auto check(X& x) -> decltype(x++);
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>()));
    };

  // Deduce the result of --t;
  template <typename T>
    struct get_pre_decrement_result
    {
    private:
      template <typename X>
        static auto check(X& x) -> decltype(--x);
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>()));
    };
    
  
  // Deduce the result of t--;
  template <typename T>
    struct get_post_decrement_result
    {
    private:
      template <typename X>
        static auto check(X& x) -> decltype(x--);
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>()));
    };

  // Deduce the result type of the expression t && u.
  template <typename T, typename U>
    struct get_and_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x && y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };


  // Deduce the result type of the expression t || u.
  template <typename T, typename U>
    struct get_or_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x || y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result of !t.
  template <typename T>
    struct get_not_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(!a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result type of the expression t & u.
  template <typename T, typename U>
    struct get_bit_and_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x & y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t | u.
  template <typename T, typename U>
    struct get_bit_or_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x | y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };


  // Deduce the result type of the expression t ^ u.
  template <typename T, typename U>
    struct get_bit_xor_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x ^ y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };
  
  // Deduce the result type of the expression t << u.
  template <typename T, typename U>
    struct get_left_shift_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x << y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t >> u.
  template <typename T, typename U>
    struct get_right_shift_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x >> y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

  // Deduce the result of ~t.
  template <typename T>
    struct get_complement_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(~a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result type of the expression t &= u.
  template <typename T, typename U>
    struct get_bit_and_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x &= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t |= u.
  template <typename T, typename U>
    struct get_bit_or_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x |= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t ^= u.
  template <typename T, typename U>
    struct get_bit_xor_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x ^= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t <<= u.
  template <typename T, typename U>
    struct get_left_shift_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x <<= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result type of the expression t >>= u.
  template <typename T, typename U>
    struct get_right_shift_assign_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X& x, Y&& y) -> decltype(x >>= y);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T&>(), std::declval<U>()));
    };

  // Deduce the result of the expression &t.
  template <typename T>
    struct get_address_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(&a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result of the expression *t.
  template <typename T>
    struct get_dereference_result
    {
    private:
      template <typename X>
        static auto check(X&& a) -> decltype(*a);
      
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>()));
    };

  // Deduce the result of the exprssion t[u].
  template <typename T, typename U>
    struct get_subscript_result
    {
    private:
      template <typename X, typename Y>
        static auto check(X&& x, Y&& y) -> decltype(x[y]);
        
      static subst_failure check(...);
    public:
      using type = decltype(check(std::declval<T>(), std::declval<U>()));
    };

} // namespace type_impl

#endif
