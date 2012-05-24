// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_VECTOR_HPP
#define ORIGIN_VECTOR_HPP

#include <vector>

#include <origin/range.hpp>

namespace origin
{
  // Vector (container)
  // A vector is a dynamically resizable sequence of contiguously allocated
  // elements.
  template<typename T, typename Alloc = std::allocator<T>>
    class o_vector
    {
      using base_type = std::vector<T, Alloc>;
    public:
      using allocator_type = typename base_type::allocator_type;

      using value_type      = typename base_type::value_type;
      using reference       = typename base_type::reference;
      using const_reference = typename base_type::const_reference;
      using pointer         = typename base_type::pointer;
      using const_pointer   = typename base_type::const_pointer;
      
      using size_type       = typename base_type::size_type;
      using difference_type = typename base_type::difference_type;
      
      using iterator       = typename base_type::iterator;
      using const_iterator = typename base_type::const_iterator;
      using reverse_iterator       = typename base_type::reverse_iterator;
      using const_reverse_iterator = typename base_type::const_reverse_iterator;
      
      
      // Default constructible
      explicit o_vector(const allocator_type& alloc = {}) 
        : impl(alloc) 
      { }
      
      
      
      // Movable
      o_vector(o_vector&& x, const allocator_type& alloc = {})
        : impl(std::move(x.impl), alloc)
      { }
       
      o_vector& operator=(o_vector&& x) 
      { 
        impl = std::move(x.impl);
        return *this;
      }
      
      
      
      // Copyable
      o_vector(const o_vector& x, const allocator_type& alloc = {})
        : impl(x.impl, alloc)
      { }
      
      o_vector& operator=(const o_vector& x) 
      { 
        impl = x.impl;
        return *this;
      }
      
      
      
      // Fill initialization
      explicit o_vector(size_type n, 
                        const value_type& value = {}, 
                        const allocator_type& alloc = {})
        : impl(n, value, alloc)
      { }
      
      void assign(size_type n, const value_type value)
      {
        impl.assign(n, value);
      }
      
      
      
      // Iterator range initialization
      // Initialize or assign the vector by copying the elements in the range 
      // [first, last).
      template<typename I>
        o_vector(I first, I last, const allocator_type& alloc = {})
          : impl(first, last, alloc)
        { }
        
      template<typename I>
        void assign(I first, I last) 
        { 
          impl.assign(first, last); 
        }
      
      
      
      // Range initialization
      // Initialize or assign the vector by copying the elements in range.
      template<typename R>
        explicit o_vector(const R& range, 
                          const allocator_type& alloc = {}, 
                          Requires<Input_range<R>()>* = {})
          : impl(o_begin(range), o_end(range), alloc)
        { }
      
      template<typename R>
        void assign(const R& range, Requires<Input_range<R>()>* = {}) 
        { 
          impl.assign(o_begin(range), o_end(range));
        }
      
      
      
      // Initializer list initialization
      // Initialize or assign the vector by copying the elements in list.
      o_vector(std::initializer_list<value_type> list, const allocator_type& alloc = {})
        : impl(list.begin(), list.end(), alloc)
      { }
      
      o_vector& operator=(std::initializer_list<value_type> list)
      {
        impl = list;
        return *this;
      }
      
      void assign(std::initializer_list<value_type> list)
      {
        impl.assign(list);
      }
      
      
      
      // Equality comparable
      bool operator==(const o_vector& v) const { return impl == v.impl; }
      bool operator!=(const o_vector& v) const { return impl != v.impl; }
      
      
      // Totally ordered
      bool operator<(const o_vector& v) const  { return impl < v.impl; }
      bool operator>(const o_vector& v) const  { return impl > v.impl; }
      bool operator<=(const o_vector& v) const { return impl <= v.impl; }
      bool operator>=(const o_vector& v) const { return impl >= v.impl; }
      
      
      
      // Size and capacity
      bool empty() const { return impl.empty(); }
      size_type size() const { return impl.size(); }
      size_type capacity() const { return impl.capacity(); }
      
      void resize(size_type n, const value_type& value = {}) 
      { 
        impl.resize(n, value); 
      }
      
      void reserve(size_type n)
      {
        impl.reserve(n);
      }
      
      
      
      // Element access
      reference       operator[](size_type n)       { return impl[n]; }
      const_reference operator[](size_type n) const { return impl[n]; }
      
      reference       at(size_type n)       { return impl.at(n); }
      const_reference at(size_type n) const { return impl.at(n); }
      
      reference       front()       { return impl.front(); }
      const_reference front() const { return impl.front(); }
      
      reference       back()       { return impl.back(); }
      const_reference back() const { return impl.back(); }
      
      
      
      // Data access
      pointer data()             { return impl.data(); }
      const_pointer data() const { return impl.data(); }
      
      
      
      // Push and pop back
      template<typename... Args>
        void emplace_back(Args&&... args) 
        { 
          impl.emplace_back(std::forward<Args>(args)...); 
        }

      void push_back(const value_type& value) { impl.push_back(value); }
      void push_back(value_type&& value) { impl.push_back(std::move(value)); }
      void pop_back() { impl.pop_back(); }
      
      
      
      // Insert
      template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
          return impl.emplace(pos, std::forward<Args>(args)...);
        }
      
      iterator insert(const_iterator pos, const value_type& value) 
      { 
        return impl.insert(pos, value); 
      }
      
      iterator insert(const_iterator pos, value_type&& value)
      {
        return impl.insert(pos, value);
      }
      
      // Insert fill
      iterator insert(const_iterator pos, size_type n, const value_type& value)
      {
        return impl.insert(pos, n, value);
      }
      
      // Insert iterator range
      template<typename I>
        iterator insert(const_iterator pos, I first, I last)
        {
          return impl.insert(pos, first, last);
        }
        
      // Insert range
      template<typename R>
        iterator insert(const_iterator pos, const R& range, Requires<Input_range<R>()>* = {})
        {
          return impl.insert(pos, o_begin(range), o_end(range));
        }
      
      // Insert initializer list
      iterator insert(const_iterator pos, std::initializer_list<value_type> list)
      {
        return impl.insert(pos, list);
      }
      
      
      
      // Erase
      iterator erase(const_iterator pos) 
      { 
        return impl.erase(pos); 
      }
      
      iterator erase(const_iterator first, const_iterator last)
      {
        return impl.erase(first, last);
      }
      
      void clear() { impl.clear(); }
      
      
      
      // Swap
      void swap(o_vector& x) { impl.swap(x.vec); }
      
      
      
      // Iterable
      iterator begin() { return impl.begin(); }
      iterator end()   { return impl.end(); }
      
      const_iterator begin() const { return impl.begin(); }
      const_iterator end() const   { return impl.end(); }
      
      
    private:
      base_type impl;
    };
    
    
  
  // Specialization for std::swap.
  template<typename T, typename A>
    void swap(o_vector<T, A>& a, o_vector<T, A>& b) { a.swap(b); }
    
} // namespace origin


#endif

