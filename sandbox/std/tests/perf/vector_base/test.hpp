// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and condit

#ifndef TEST_HPP
#define TEST_HPP

template <typename T>
  std::size_t capacity(const origin::vector_base<T>& v)
  {
    return v.capacity();
  }

template <typename T, typename A>
  std::size_t capacity(const std::_Vector_base<T, A>& v)
  {
    return v._M_impl._M_end_of_storage - v._M_impl._M_start;
  }

template <typename Vec>
  struct default_alloc_test
  {
    template <typename Env>
      void operator()(Env& env, std::size_t n, std::size_t r) const
      {
        Vec v(n);
      }
  };

#endif
