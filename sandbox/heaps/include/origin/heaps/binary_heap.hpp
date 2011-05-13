// Copyright (c) 2008-2010 Kent State University
// Copyright (c) 2011 Texas A&M University
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef ORIGIN_HEAPS_BINARY_HEAP_HPP
#define ORIGIN_HEAPS_BINARY_HEAP_HPP

#include <cassert>
#include <iosfwd>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <origin/utility/meta.hpp>

namespace origin
{
  /**
   * A mutable binary heap is a binary heap that allows the values in the to
   * be modified by an external program. The heap is updated to adjust the
   * location of the element within the data structure and preserve the heap
   * invariant.
   *
   * @tparam T      The value type.
   * @tparam Comp   A Strict_weak_order on T
   * @tparam Map    A Mapping between T and an index
   * @tparam Alloc  An Allocator type
   */
  template<typename T, 
           typename Comp = std::less<T>, 
           typename Map = default_t, 
           typename Alloc = std::allocator<T>>
    class mutable_binary_heap
    {
    public:
      typedef Alloc allocator_type;
      typedef T value_type;
      typedef Comp value_compare;
      typedef typename Alloc::size_type size_type;
    private:
      typedef std::vector<T> container_type;
      typedef std::unordered_map<value_type, size_t> map_type; //FIXME: Use traits class
    public:
      

    public:
      /** @name Initialization */
      //@{
      /**
       * @brief Default constructor
       * Initialize an empty heap. The value compare function may be optionally 
       * given.
       *
       * @param comp  A value_compare function object
       */
      mutable_binary_heap(value_compare const& comp = value_compare{})
        : compare_{comp}
      { }

      // FIXME: Generalize this operation for input iterators.
      /**
       * @brief Range constructor
       * Initialize the heap with the elements in the range [first, last).
       * 
       * @tparam Iter   A Forward_Iterator whose reference type is convertible
       * the the value type of the heap.
       * 
       * @param first   An iterator referencing the first object in the range
       * @param last    An iterator past the end of the the range
       * @param comp    A value_compare function object
       */
      template<typename Iter>
        mutable_binary_heap(Iter first,
                            Iter last,
                            value_compare const& comp = value_compare{}) 
        : compare_{comp}
      {
        reserve(std::distance(first, last));
        while(first != last) {
          elements_.push_back(*first);
          index_[*first] = elements_.size() - 1;
          ++first;
        }
        size_type index = elements_.size() / 2;
        while (index > 0){
          index--;
          heapify(index);
        }
      }

      // FIXME: Implement me!
      /**
       * @brief Initializer list constructor
       * Initialize the heap with the elements from the given initializer list.
       * 
       * @param list  An initializer list of 
       * @param comp  A value_compare function object
       */
      mutable_binary_heap(std::initializer_list<T> list,
                          value_compare const& comp = value_compare{})
      { }
      //@}


      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.empty();
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison function.
       */
      value_compare value_comp() const
      {
        return compare_;
      }

      // FIXME: I think that this should probably go away.
      /**
       * Return a reference to the underlying container.
       */
      container_type const& data() const
      {
        return elements_;
      }
      //@}

      /** @name Capacity */
      //@{
      /**
       * Return the capacity allocated to the heap.
       */
      size_type capacity() const
      {
          return elements_.capacity();
      }

      /**
       * Reserve additional capacity for the heap.
       */
      void reserve(size_type n)
      {
          elements_.reserve(n);
      }
      //@}

      /** @name Heap operations */
      //@{
      /**
       * Return the element on the top of the heap.
       */
      value_type const& top() const
      {
          return elements_[0];
      }

      /**
       * Update the given key within the heap.
       */
      void update(const value_type& x);

      /**
       * Push the given element onto the heap.
       */
      void push(const value_type& x);

      /**
       * Pop the top element from the heap. This operation completes in
       * O(lg n) time.
       */
      void pop();
      //@}

      // FIXME: Go away?
      template<typename Char, typename Traits>
        void print(std::basic_ostream<Char, Traits>& os);

    private:
      // Return the object at the given index.
      value_type& get(size_type n)
      {
        assert(( !empty() && n < size() ));
        return elements_[n];
      }
      
      value_type const& get(size_type n) const
      {
        assert(( !empty() && n < size() ));
        return elements_[n];
      }
      
      // Return the index of the given value. The value must be in the heap.
      size_type index(value_type const& x) const
      {
        assert(( index_.find(x) != index_.end() ));
        return index_[x];
      }
      
      // Comparing the value of the mth element with the nth element.
      bool compare(size_type m, size_type n) const
      {
        return compare_(get(m), get(n));
      }

      // Swap two elements in the heap and exchange their indexes.
      void exchange(size_type m, size_type n)
      {
        std::swap(get(m), get(n));
        index_[get(m)] = m;
        index_[get(n)] = n;
      }

      void up_heap(size_type n);
      void down_heap(size_type n);

      // Recursively print the heap as a tree
      // FIXME: Write this as an out-of body function.
      template<typename Char, typename Traits>
        void print_recur(size_type x, std::basic_ostream<Char, Traits>& os);


    private:
      container_type elements_;
      value_compare compare_;
      map_type index_;
    };

  // Bubble the object at the given index up the heap.
  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::up_heap(size_type n)
    {
      while(n > 0) {
        size_type p = (n - 1) / 2;
        if(!compare(n, p)) {
          exchange(n, p);
          n = p;
        } else
          break;
      }
    }
  
  // Bubble the object at the given index down the heap.
  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::down_heap(size_type n)
    {
      auto parent = [](size_t x) { return x == 0 ? 0 : (x - 1) / 2; }
      auto left = [](size_t x) { return x * 2 + 1; }
      
      size_t c = left(n);
      // Elem Item    = Data[Current];
      
      while(c < size()) {
        // Find the right child to compare against
        if(c < (size() - 1) && compare(c, c + 1))
          ++c;
        
        if(compare(n, c)) {
          exchange(n, c);
          n = c;
          c = left(c);
        } else
          break;
      }

      // FIXME: What the fuck does this do?
      Data[Current] = Item; 
      /*
      size_type sz = elements_.size();
      size_type p = n;
 
      do {
        exchange(n, p);
        n = p;

        size_type left = 2 * n + 1;
        size_type right = 2 * n + 2;

        if((left < sz) && compare(left, n))
          p = left;
        if((right < sz) && compare(right, p))
          p = right;
      } while(n != p);
      */
    }
    
  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::push(value_type const& x)
    {
      // Push element into the heap structure
      size_type n = elements_.size();
      elements_.push_back(x);
      index_[x] = n;
      
      // Adjust the heap.
      up_heap(n);
    }


  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::pop()
    {
      //swap root with last element and delete old root
      exchange(0, elements_.size() - 1);
      elements_.pop_back();

      // Preserve the heap invariant.
      if(!empty()) {
        down_heap(0);
      }
    }

  template<typename T, typename Comp, typename Map, typename Alloc>
    void mutable_binary_heap<T, Comp, Map, Alloc>::update(value_type const& x)
    {
      assert(( get(index(x)) == x ));

      //update the element with the new value
      size_type n = index(x);

      // FIXME: This seems wrong. The heap should always observe the
      // precondition above. The object at the index associated with the key
      // x must be equal to x. Re-inserting the value should be a no-op.
      get(n) = x;
      size_type parent = (n - 1) / 2;

      // FIXME: This entirely wrong.
      if(n == 0)
        return; 
      else if(compare(n, parent))
        up_heap(n);
      else
        down_heap(n);
    }


    // Recursively print the heap as a tree
    template<typename T, typename Comp, typename Map, typename Alloc>
      template<typename Char, typename Traits>
        void mutable_binary_heap<T, Comp, Map, Alloc>::
          print_recur(size_type x, std::basic_ostream<Char, Traits>& os)
        {
          size_type sz = elements_.size();

          os << get(x);
          size_type i = 2 * x + 1;

          if (i < sz) {
            os << "(";
            print_recur (i, os);

            os << " ";
            i = i + 1;
            if (i < sz) {
              print_recur(i, os);
            }
            os << ")";
          }
        }

  template<typename T, typename Comp, typename Map, typename Alloc>
    template<typename Char, typename Traits>
      void 
      mutable_binary_heap<T, Comp, Map, Alloc>::
        print(std::basic_ostream<Char, Traits>& os)
      {
        if(elements_.size() > 0)
            print_recur(0, os);
      }


  // FIXME: Find a nice data structure reference for the documentation (CLR?).
  // FIXME: This is basically std::priority_queue. Why not reuse that
  // implementation?
  /**
   * The binary heap data structure implements a classic, generic binary heap.
   * The order (min or max) of the heap is determined by a compare operation on
   * the value type. This defaults to std::less, resulting in max-heap.
   *
   * @tparam T      Type of data to be stored in the heap
   * @tparam Comp   A Strict Weak Ordering over T
   */
  template<typename T, typename Comp = std::less<T>>
    class binary_heap
    {
      typedef std::vector<T> container_type;
    public:
      typedef T value_type;
      typedef Comp value_compare;
      typedef typename container_type::size_type size_type;

      /**
       * @brief Default constructor
       * Initialize an empty heap. A comparison function may optionally be given.
       *
       * @param comp  A value comparison Operation
       */
      binary_heap(value_compare const& comp = value_compare{})
        : elements_{}, compare_{comp}
      { }

      // FIXME: This needs to be specialized for input and forwrad iterators.
      /**
       * @brief Range constructor
       * Initialize the heap over the sequence of elements in the range
       * [first, last). A comparison function may be optional given.
       *
       *
       * @param first   The first iterator in a range of objects
       * @param last    An iterator past the end of a range of objects
       * @param comp    A Comp function object.
       */
      template<typename ForwardIterator>
        binary_heap(ForwardIterator first,
                    ForwardIterator last,
                    value_compare const& comp = value_compare{})
          : elements_{first, last}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /**
       * @brief Initializer list constructor
       * Initialize the heap from a seuqence of objects. A comparison function
       * may be optionally given.
       *
       * @param list  An initializer list of objects of type T
       * @param comp  A Comp function object.
       */
      binary_heap(std::initializer_list<T> list, value_compare const& comp)
        : elements_{list.begin(), list.end()}, compare_{comp}
        {
          std::make_heap(elements_.begin(), elements_.end(), compare_);
        }

      /** @name Properties */
      //@{
      /**
       * Return true if the heap has no elements.
       */
      bool empty() const
      {
        return elements_.size() == 0;
      }

      /**
       * Return the number of elements in the heap.
       */
      size_type size() const
      {
        return elements_.size();
      }

      /**
       * Return the value comparison operation.
       */
      value_compare value_comp() const
      {
        return compare_;
      }
      
      
      container_type const& data() const
      {
        return elements_;
      }
      //@}

      /** @name Capacity */
      //@{
      /**
       * Return the current capacity of the heap. This is the number of objects
       * for which memory has been allocated, but not initialized.
       */
      size_type capacity() const
      {
        return elements_.capacity();
      }

      // FIXME: What if n < size()? Do you truncate the heap? Is the heap still
      // valid after truncation? Should you never give up capacity once required?
      /**
       * Reserve additional memory for the heap.
       */
      void reserve(size_type n)
      {
        elements_.reserve(n);
      }
      //@}


      /** @name Operations */
      //@{
      /**
       * Return the object on the top of the heap.
       */
      const value_type& top() const
      {
          return elements_[0];
      }

      /**
       * Push an object onto the heap. This operation completes in O(lg n) time.
       */
      void push(value_type const& x)
      {
        elements_.push_back(x);
        std::push_heap(elements_.begin(), elements_.end(), compare_);
      }

      /**
       * Pop the top object off the heap. This operation completes in O(lg n)
       * time.
       */
      void pop()
      {
        std::pop_heap(elements_.begin(), elements_.end(), compare_);
        elements_.pop_back();
      }
      //@}

      // FIXME: Go away?
      /**
       * Write the contents of the heap to the given output stream.
       */
      template<typename Char, typename Traits>
        void print(std::basic_ostream<Char, Traits>& os);

    private:

      // FIXME: Go away?
      // Helper function for displaying the binary heap
      template<typename Char, typename Traits>
        void print_recur(size_type n, std::basic_ostream<Char, Traits>& os);

    private:
      // FIXME: Use EBO to remove the overhead of the value compare function.
      container_type elements_;
      value_compare compare_;
    };


  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print(std::basic_ostream<Char, Traits>& os)
      {
        if(!empty()) {
          print_recur(0, os);
        }
      }

  template<typename T, typename Comp>
    template<typename Char, typename Traits>
      void binary_heap<T, Comp>::print_recur(size_type n,
                                             std::basic_ostream<Char, Traits>& os)
      {
        os << elements_[n];
        size_type i = 2 * n + 1;

        size_type sz = elements_.size();
        if(i < sz) {
          os << "(";
          print_recur(i, os);

          os << " ";
          i = i + 1;
          if(i < sz) {
            print_recur(i, os);
          }
          os << ")";
        }
      }

} // namespace origin


#endif // ORIGIN_HEAPS_BINARY_HEAP_HPP
