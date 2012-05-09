// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011-2012 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_MEMORY_NORMAL_POINTER_HPP
#define ORIGIN_MEMORY_NORMAL_POINTER_HPP

namespace origin
{
  // Normal pointer 
  //
  // The noraml_ptr class is the dumbest smart pointer that can be implemented.
  // It is simply a wrapper around another pointer. This is primarily used for
  // testing.
  //
  // FIXME: Add a specialization for T[], that provides array access.
  template <typename T>
    struct normal_ptr
    {
      using element_type = T;

      template <typename U> 
        using rebind = normal_ptr<U>;

      normal_ptr(T *p = nullptr) : ptr {p} { }

      // Converting constructor
      // Allow conversions to this pointer typeif U* can be converted to T*.
      // This accommodates conversions to const and base class pointers.
      template <typename U>
        normal_ptr(const normal_ptr<U>& x, Requires<Convertible<U*, T*>()>* = {}) 
          : ptr {x.ptr} 
        { }

      T&       operator*()       { return *ptr; }
      const T& operator*() const { return *ptr; }

      T* operator->() { return ptr; }
      const T* operator->() const { return ptr; }

      // Return the underlying pointer
      T*       get()       { return ptr; }
      const T* get() const { return ptr; }

      template <typename U>
        U get_as() 
        { 
          using E = Element_type<U>;
          return normal_ptr<E>(static_cast<E*>(ptr));
        }

      template <typename U>
        U get_as() const
        {
          using E = Element_type<U>;
          return normal_ptr<const E>(static_cast<const E>(ptr));
        }
      
      T *ptr;
    };

  // A specialization for void pointers allows any normal pointer to be
  // converted to this type.
  template <>
    struct normal_ptr<void>
    {
      using element_type = void;

      template <typename U> 
        using rebind = normal_ptr<U>;

      normal_ptr(void *p = nullptr) : ptr {p} { }

      template <typename U>
        normal_ptr(const normal_ptr<U>& x)
          : ptr {x.ptr}
        { }

      // Return the underlying pointer
      void*       get()       { return ptr; }
      const void* get() const { return ptr; }

      // Return this pointer, converted to the requested type.
      template <typename U>
        U get_as() 
        { 
          using E = Element_type<U>;
          return normal_ptr<E>(static_cast<E*>(ptr));
        }

      template <typename U>
        U get_as() const
        {
          using E = Element_type<U>;
          return normal_ptr<const E>(static_cast<const E>(ptr));
        }

      void *ptr;
    };



  // Equality_comparable<normal_ptr<T>, normal_ptr<U>>
  //
  // Two normal pointers compare equal when their underlying pointers compare
  // equal. 
  //
  // Note that these operations are defined for any pair of normal pointers
  // whose underlying pointers share a common type. This means that the
  // following comparisons are valid:
  //
  //    p == p // where p has type normal_ptr<T>
  //    p == vp // where vp has type normal_ptr<void>
  //    p == cp // where cp has type normal_ptr<const T>
  //    p == bp // where bp has type normal_ptr<B> and B is a base class of T
  //
  // No other comparisons are valid. For example, if p has type normal_ptr<int>
  // and q has type normal_ptr<char>, the p == q is not a valid expression.

  template <typename T, typename U>
    inline auto operator==(normal_ptr<T> a, normal_ptr<U> b) 
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() == b.get(); 
    }

  template <typename T, typename U>
    inline auto operator!=(normal_ptr<T> a, normal_ptr<U> b) 
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() != b.get(); 
    }

  // Equality comparable<normal_ptr<T>, std::nullptr_t>
  template <typename T>
    inline bool operator==(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() == b; 
    }

  template <typename T>
    inline bool operator==(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a == b.get(); 
    }

  template <typename T>
    inline bool operator!=(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() != b; 
    }

  template <typename T>
    inline bool operator!=(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a != b.get(); 
    }



  // Totally ordered
  // A normal pointer a compares less than a normal pointer b when a's 
  // underlying pointer compares less than b's.
  //
  // Note that these operations are defined for any pair of normal pointers
  // whose underlying pointers share a common type. This means that the
  // following comparisons are valid:
  //
  //    p < p // where p has type normal_ptr<T>
  //    p < vp // where vp has type normal_ptr<void>
  //    p < cp // where cp has type normal_ptr<const T>
  //    p < bp // where bp has type normal_ptr<B> and B is a base class of T
  //
  // No other comparisons are valid. For example, if p has type normal_ptr<int>
  // and q has type normal_ptr<char>, then p < q is not a valid expression.

  template <typename T, typename U>
    inline auto operator<(normal_ptr<T> a, normal_ptr<U> b)
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() < b.get(); 
    }

  template <typename T, typename U>
    inline auto operator>(normal_ptr<T> a, normal_ptr<U> b)
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() > b.get(); 
    }

  template <typename T, typename U>
    inline auto operator<=(normal_ptr<T> a, normal_ptr<U> b)
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() <= b.get(); 
    }

  template <typename T, typename U>
    inline auto operator>=(normal_ptr<T> a, normal_ptr<U> b)
      -> Requires<Common<decltype(a.get()), decltype(b.get())>(), bool>
    { 
      return a.get() >= b.get(); 
    }

  // Totally ordered<normal_ptr<T>, nullptr_t>

  template <typename T>
    inline bool operator<(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() < b; 
    }

  template <typename T>
    inline bool operator<(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a < b.get(); 
    }

  template <typename T>
    inline bool operator>(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() > b; 
    }

  template <typename T>
    inline bool operator>(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a > b.get(); 
    }

  template <typename T>
    inline bool operator<=(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() <= b; 
    }

  template <typename T>
    inline bool operator<=(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a <= b.get(); 
    }

  template <typename T>
    inline bool operator>=(normal_ptr<T> a, std::nullptr_t b) 
    { 
      return a.get() >= b; 
    }

  template <typename T>
    inline bool operator>=(std::nullptr_t a, normal_ptr<T> b) 
    { 
      return a >= b.get(); 
    }




  // Normal allocator
  // The normal allocator is essentially the same as the default allocator
  // except that it returns normal_pointer objects. The primary us of this
  // allocator is to support testing.
  //
  // TODO: Implement me...
  class normal_allocator
  {
  public:
    normal_ptr<void> allocate(std::size_t n)
    {
      return normal_ptr<void>(::operator new(n));
    }

    void deallocate(normal_ptr<void> p, std::size_t n)
    {
      ::operator delete(p.get());
    }
  };


} // namespace origin

#endif
